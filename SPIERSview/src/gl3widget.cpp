
#include "gl3widget.h"
#include "svobject.h"
#include "globals.h"
#include "spv.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector4D>
#include <QGesture>
#include <QDebug>
#include <QFile>

#include "globals.h"

GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFormat(surfaceFormat);
    LastMouseXpos = -1;
    LastMouseYpos = -1;
    setMouseTracking(true);
    cameraX = 0; cameraY = 0; cameraZ = 3;
    centerX = 0; centerY = 0; centerZ = 0;
    StereoSeparation = static_cast<float>(.04);
    defaultClipAngle = mainWindow->ui->ClipAngle->value() / 10;
    setFocusPolicy(Qt::NoFocus);
    QList<Qt::GestureType> gestures;
    gestures << Qt::PinchGesture;
    grabGestures(gestures);
    scaleGrid = new DrawGLScaleGrid(this);
    scaleBall = new DrawGLScaleBall(this);
    for (int i = 0; i < 3; i++) { shadowFBO[i] = 0; shadowDepthTexture[i] = 0; }
    dummyShadowTexture = 0;
}

GlWidget::~GlWidget()
{
    makeCurrent();
    lightingShaderProgram.release();
    lightingShaderProgramShadow.release();
    lightingShaderProgramPCF.release();
    shadowDepthShaderProgram.release();
    lightingShaderProgram.deleteLater();
    lightingShaderProgramShadow.deleteLater();
    lightingShaderProgramPCF.deleteLater();
    shadowDepthShaderProgram.deleteLater();
    for (int i = 0; i < 3; i++)
    {
        if (shadowFBO[i])          glextrafunctions->glDeleteFramebuffers(1, &shadowFBO[i]);
        if (shadowDepthTexture[i]) glDeleteTextures(1, &shadowDepthTexture[i]);
    }
    if (dummyShadowTexture) glDeleteTextures(1, &dummyShadowTexture);
#ifdef __APPLE__
    vao.release();
    vao.deleteLater();
#endif
    doneCurrent();
}

static bool addShaderWithDefines(QOpenGLShaderProgram &program,
                                  QOpenGLShader::ShaderType type,
                                  const QString &resourcePath,
                                  const QString &defines)
{
    QFile f(resourcePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Cannot open shader:" << resourcePath;
        return false;
    }
    QString source = QString::fromUtf8(f.readAll());
    QString versionLine;
    int newline = source.indexOf('\n');
    if (source.startsWith("#version") && newline != -1)
    {
        versionLine = source.left(newline + 1);
        source      = source.mid(newline + 1);
    }
    return program.addShaderFromSourceCode(type, versionLine + defines + source);
}

void GlWidget::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();
    glfunctions      = this->context()->functions();
    glextrafunctions = this->context()->extraFunctions();

#ifdef __APPLE__
    vao.create();
    vao.bind();
#endif

    const QString noShadow   = "#define SHADOWS_ENABLED 0\n#define PCF_ENABLED 0\n";
    const QString hardShadow = "#define SHADOWS_ENABLED 1\n#define PCF_ENABLED 0\n";
    const QString softShadow = "#define SHADOWS_ENABLED 1\n#define PCF_ENABLED 1\n";

    addShaderWithDefines(lightingShaderProgram,       QOpenGLShader::Vertex,   ":/lightingVertexShader.vsh", noShadow);
    addShaderWithDefines(lightingShaderProgramShadow, QOpenGLShader::Vertex,   ":/lightingVertexShader.vsh", hardShadow);
    addShaderWithDefines(lightingShaderProgramPCF,    QOpenGLShader::Vertex,   ":/lightingVertexShader.vsh", softShadow);
    addShaderWithDefines(lightingShaderProgram,       QOpenGLShader::Fragment, ":/lightingFragmentShader.fsh", noShadow);
    addShaderWithDefines(lightingShaderProgramShadow, QOpenGLShader::Fragment, ":/lightingFragmentShader.fsh", hardShadow);
    addShaderWithDefines(lightingShaderProgramPCF,    QOpenGLShader::Fragment, ":/lightingFragmentShader.fsh", softShadow);

    lightingShaderProgram.link();
    lightingShaderProgramShadow.link();
    lightingShaderProgramPCF.link();

    if (!lightingShaderProgram.isLinked())
        qDebug() << "No-shadow link failed:" << lightingShaderProgram.log();
    if (!lightingShaderProgramShadow.isLinked())
        qDebug() << "Hard shadow link failed:" << lightingShaderProgramShadow.log();
    if (!lightingShaderProgramPCF.isLinked())
        qDebug() << "PCF shadow link failed:" << lightingShaderProgramPCF.log();

    shadowDepthShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/shadowDepthVertexShader.vsh");
    shadowDepthShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shadowDepthFragmentShader.fsh");
    shadowDepthShaderProgram.link();
    if (!shadowDepthShaderProgram.isLinked())
        qDebug() << "Depth shader link failed:" << shadowDepthShaderProgram.log();

    initShadowFBOs();
    scaleGrid->initializeGL();
    scaleBall->initializeGL();
}

void GlWidget::initShadowFBOs()
{
    for (int i = 0; i < 3; i++)
    {
        glGenTextures(1, &shadowDepthTexture[i]);
        glBindTexture(GL_TEXTURE_2D, shadowDepthTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                     SHADOW_MAP_SIZE, SHADOW_MAP_SIZE,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        glextrafunctions->glGenFramebuffers(1, &shadowFBO[i]);
        glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
        glextrafunctions->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                  GL_TEXTURE_2D, shadowDepthTexture[i], 0);
        GLenum drawBufs = GL_NONE;
        glextrafunctions->glDrawBuffers(1, &drawBufs);
        glextrafunctions->glReadBuffer(GL_NONE);
        glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glGenTextures(1, &dummyShadowTexture);
    glBindTexture(GL_TEXTURE_2D, dummyShadowTexture);
    float white = 1.0f;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1, 1,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, &white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,   GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,   GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ---------------------------------------------------------------------------
// Compute RAW light-space matrix (ortho projection * light view).
// Does NOT include any object model transform — that is applied per-object
// in renderShadowPass and DrawObjects.
// ---------------------------------------------------------------------------
QMatrix4x4 GlWidget::computeLightSpaceMatrix(QVector3D lightDir)
{
    float margin = 0.2f;
    float cx = (minX + maxX) * 0.5f;
    float cy = (minY + maxY) * 0.5f;
    float cz = (minZ + maxZ) * 0.5f;
    float hx = (maxX - minX) * 0.5f + margin;
    float hy = (maxY - minY) * 0.5f + margin;
    float hz = (maxZ - minZ) * 0.5f + margin;
    float radius = sqrtf(hx*hx + hy*hy + hz*hz);

    QVector3D sceneCenter(cx, cy, cz);
    QVector3D lightPos = sceneCenter + lightDir * radius;

    QVector3D up(0, 1, 0);
    if (qAbs(QVector3D::dotProduct(lightDir.normalized(), up)) > 0.99f)
        up = QVector3D(0, 0, 1);

    QMatrix4x4 lightView;
    lightView.lookAt(lightPos, sceneCenter, up);

    QMatrix4x4 lightOrtho;
    lightOrtho.ortho(-radius, radius, -radius, radius, 0.01f, radius * 2.5f);

    return lightOrtho * lightView;
}

// ---------------------------------------------------------------------------
// Shadow pass — renders all opaque objects into a depth map for one light.
// The depth shader receives lightSpaceMatrix * objMatrix per draw call,
// so that raw vertex positions (local object space) are correctly transformed.
// lightSpaceMatrixOut stores the RAW light-space matrix for use in DrawObjects.
// ---------------------------------------------------------------------------
void GlWidget::renderShadowPass(int lightIndex, QVector3D lightDir, QMatrix4x4 &lightSpaceMatrixOut)
{
    lightSpaceMatrixOut = computeLightSpaceMatrix(lightDir);

    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[lightIndex]);
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    shadowDepthShaderProgram.bind();

    for (int i = 0; i < SVObjects.count(); i++)
    {
        if (SVObjects[i]->IsGroup || SVObjects[i]->isSurfacing) continue;
        if (!CanISee(i)) continue;
        if (SVObjects[i]->Transparency != 0) continue;

        // Per-object model matrix — no view matrix, light is independent of camera
        QMatrix4x4 objMatrix;
        objMatrix.translate(0, 0, -1);
        objMatrix *= QMatrix4x4(SVObjects[i]->matrix);
        objMatrix *= globalMatrix;

        // Bake object transform into light-space matrix for this draw call
        shadowDepthShaderProgram.setUniformValue("lightSpaceMatrix",
                                                  lightSpaceMatrixOut * objMatrix);

        for (int j = 0; j < SVObjects[i]->VertexBuffers.count(); j++)
        {
            SVObjects[i]->VertexBuffers[j]->bind();
            shadowDepthShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
            shadowDepthShaderProgram.enableAttributeArray("vertex");
            SVObjects[i]->VertexBuffers[j]->release();
            glfunctions->glDrawArrays(GL_TRIANGLES, 0, SVObjects[i]->VBOVertexCounts[j]);
        }
    }

    shadowDepthShaderProgram.release();
    glDisable(GL_CULL_FACE);
    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

QOpenGLShaderProgram *GlWidget::selectShader()
{
    bool anyHard = (mainLightShadows == ShadowMode::Hard)
                || (headlightActive     && headlightShadows     == ShadowMode::Hard)
                || (secondaryLightActive && secondaryLightShadows == ShadowMode::Hard);
    bool anySoft = (mainLightShadows == ShadowMode::Soft)
                || (headlightActive     && headlightShadows     == ShadowMode::Soft)
                || (secondaryLightActive && secondaryLightShadows == ShadowMode::Soft);
    if (anySoft) return &lightingShaderProgramPCF;
    if (anyHard) return &lightingShaderProgramShadow;
    return &lightingShaderProgram;
}

void GlWidget::resizeGL(int width, int height)
{
    if (height == 0) height = 1;
    xdim = static_cast<int>(static_cast<double>(width)  * applicationScaleX);
    ydim = static_cast<int>(static_cast<double>(height) * applicationScaleY);
    DoPMatrix(xdim, ydim);
    glViewport(0, 0, xdim, ydim);
    update();
}

void GlWidget::DoPMatrix(int width, int height)
{
    float asp   = static_cast<float>(width) / static_cast<float>(height);
    float fudge = static_cast<float>(1300)  / static_cast<float>(width);
    pMatrix.setToIdentity();
    if (mainWindow->ui->actionOrthographic_View->isChecked())
        pMatrix.ortho(-(ClipAngle / (10 * fudge)), ClipAngle / (10 * fudge),
                      -(ClipAngle / (10 * fudge)) / asp, ClipAngle / (10 * fudge) / asp,
                      ClipStart, ClipDepth);
    else
        pMatrix.perspective(ClipAngle, asp, ClipStart, ClipDepth);
}

void GlWidget::paintGL()
{
    QVector3D mainLightDirection      = GetLightDirection(mainLightXYAngle, mainLightZPos);
    QVector3D secondaryLightDirection = GetLightDirection(secondaryLightXYAngle, secondaryLightZPos);

    if (mainLightShadows != ShadowMode::None)
        renderShadowPass(0, mainLightDirection, lightSpaceMatrix[0]);
    if (headlightActive && headlightShadows != ShadowMode::None)
        renderShadowPass(1, QVector3D(0.0f, 0.0f, 1.0f), lightSpaceMatrix[1]);

    if (secondaryLightActive && secondaryLightShadows != ShadowMode::None)
        renderShadowPass(2, secondaryLightDirection, lightSpaceMatrix[2]);

    glClearColor(
        static_cast<float>(colorBackgroundRed)   / 255.0f,
        static_cast<float>(colorBackgroundGreen) / 255.0f,
        static_cast<float>(colorBackgroundBlue)  / 255.0f,
        0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (mainWindow->ui->actionSplit_Stereo->isChecked())
    {
        DoPMatrix(xdim / 2, ydim);
        glfunctions->glViewport(0, 0, xdim / 2, ydim);
        DrawObjects(false, true);
        glfunctions->glClear(GL_DEPTH_BUFFER_BIT);
        glfunctions->glViewport(xdim / 2, 0, xdim / 2, ydim);
        DrawObjects(true, true);
        return;
    }

    glViewport(0, 0, xdim, ydim);

    if (mainWindow->ui->actionAnaglyph_Stereo->isChecked())
    {
        DoPMatrix(xdim, ydim);
        glfunctions->glColorMask(true, false, false, true);
        DrawObjects(false, false);
        glfunctions->glClear(GL_DEPTH_BUFFER_BIT);
        glfunctions->glColorMask(false, true, true, true);
        DrawObjects(true, false);
        glfunctions->glColorMask(true, true, true, true);
        return;
    }

    if (mainWindow->ui->actionQuadBuffer_Stereo->isChecked())
    {
        DoPMatrix(xdim, ydim);
        DrawObjects(false, false);
        glfunctions->glClear(GL_DEPTH_BUFFER_BIT);
        DrawObjects(true, false);
        return;
    }

    DoPMatrix(xdim, ydim);

    DrawObjects(false, false);
}

void GlWidget::DrawObjects(bool rightview, bool halfsize)
{
    QVector3D mainLightDirection      = GetLightDirection(mainLightXYAngle, mainLightZPos);
    QVector3D mainLightCol            = GetLightColour(mainLightPower - 1, mainLightColour);
    QVector3D secondaryLightDirection = GetLightDirection(secondaryLightXYAngle, secondaryLightZPos);
    QVector3D secondaryLightCol       = GetLightColour(secondaryLightPower - 5, secondaryLightColour);
    QVector3D headlightCol            = GetLightColour(headlightPower - 4, headlightColour);

    Q_UNUSED(halfsize)

    QMatrix4x4 vMatrix;
    vMatrix.setToIdentity();
    QVector3D camera              = QVector3D(cameraX, cameraY, cameraZ);
    QVector3D rightcameraPosition = QVector3D(static_cast<float>(StereoSeparation) * static_cast<float>(STEREO_SEPARATION_MODIFIER) / static_cast<float>(cameraZ), 0, cameraZ);
    QVector3D cameraUpDirection   = QVector3D(0, 1, 0);
    QVector3D center              = QVector3D(centerX, centerY, centerZ);

    if (rightview)
        vMatrix.lookAt(rightcameraPosition, center, cameraUpDirection);
    else
        vMatrix.lookAt(camera, center, cameraUpDirection);

    QOpenGLShaderProgram *useshader = selectShader();
    bool shadowsActive = (useshader != &lightingShaderProgram);

    glDepthMask(true);

    for (int trans = 0; trans < 2; trans++)
    {
        if (trans == 1) { glfunctions->glDepthMask(false); glfunctions->glEnable(GL_BLEND); }
        else            { glfunctions->glDepthMask(true);  glfunctions->glDisable(GL_BLEND); }

        for (int i = 0; i < SVObjects.count(); i++)
        {
            isFileLoaded = true;

            if (!(SVObjects[i]->IsGroup))
                if (!SVObjects[i]->isSurfacing && CanISee(i))
                {
                    // Camera-space matrix for rendering
                    QMatrix4x4 mMatrix(SVObjects[i]->matrix);
                    QMatrix4x4 mvMatrix = vMatrix;
                    mvMatrix.translate(0, 0, -1);
                    mvMatrix *= mMatrix;
                    mvMatrix *= globalMatrix;
                    QMatrix3x3 normalMatrix = mvMatrix.normalMatrix();

                    // Model matrix WITHOUT view — used for shadow lookup
                    // (light-space is independent of camera)
                    QMatrix4x4 objMatrix;
                    objMatrix.translate(0, 0, -1);
                    objMatrix *= mMatrix;
                    objMatrix *= globalMatrix;

                    if ((SVObjects[i]->Transparency == 0 && trans == 0) ||
                        (SVObjects[i]->Transparency != 0 && trans == 1))
                    {
                        useshader->bind();
                        useshader->setUniformValue("mvpMatrix",    pMatrix * mvMatrix);
                        useshader->setUniformValue("mvMatrix",     mvMatrix);
                        useshader->setUniformValue("normalMatrix", normalMatrix);

                        // Build light list
                        QVector3D directions[3];
                        QVector3D colors[3];
                        int lightCount = 0;
                        directions[lightCount] = mainLightDirection;
                        colors[lightCount]     = mainLightCol;
                        lightCount++;
                        if (headlightActive)
                        {
                            directions[lightCount] = QVector3D(0.0f, 0.0f, 1.0f);
                            colors[lightCount]     = headlightCol;
                            lightCount++;
                        }
                        if (secondaryLightActive)
                        {
                            directions[lightCount] = secondaryLightDirection;
                            colors[lightCount]     = secondaryLightCol;
                            lightCount++;
                        }
                        useshader->setUniformValueArray("lightDirections", directions, lightCount);
                        useshader->setUniformValueArray("lightColors",     colors,     lightCount);
                        useshader->setUniformValue("lightCount", lightCount);

                        if (shadowsActive)
                        {
                            int castsShadow[3] = {0, 0, 0};
                            int packedIndex = 0;

                            // Main light — always first
                            castsShadow[packedIndex] = (mainLightShadows != ShadowMode::None) ? 1 : 0;
                            // Also bind its texture to unit 1+packedIndex
                            packedIndex++;

                            if (headlightActive)
                            {
                                castsShadow[packedIndex] = (headlightShadows != ShadowMode::None) ? 1 : 0;
                                packedIndex++;
                            }

                            if (secondaryLightActive)
                            {
                                castsShadow[packedIndex] = (secondaryLightShadows != ShadowMode::None) ? 1 : 0;
                                packedIndex++;
                            }


                            useshader->setUniformValueArray("lightCastsShadow", castsShadow, 3);

                            // Combine raw light-space matrices with this object's model matrix.
                            // The vertex shader applies this to raw vertex positions (local space)
                            // to get the correct shadow map UV coordinates.

                            // Build fboSlot mapping (same as for texture binding)
                            int fboSlot[3] = {0, 0, 0};
                            int p = 0;
                            fboSlot[p++] = 0;
                            if (headlightActive)      fboSlot[p++] = 1;
                            if (secondaryLightActive) fboSlot[p++] = 2;

                            // Use fboSlot to index lightSpaceMatrix correctly
                            QMatrix4x4 combinedLightMatrices[3];
                            for (int s = 0; s < 3; s++)
                                combinedLightMatrices[s] = lightSpaceMatrix[fboSlot[s]] * objMatrix;

                            useshader->setUniformValueArray("lightSpaceMatrices", combinedLightMatrices, 3);

                            useshader->setUniformValue("shadowBias",        0.005f);
                            useshader->setUniformValue("shadowMapTexelSize", 1.5f / SHADOW_MAP_SIZE);

                            for (int s = 0; s < 3; s++)
                            {
                                glextrafunctions->glActiveTexture(GL_TEXTURE1 + s);
                                bool inUse = (s < lightCount);
                                bool casts = (castsShadow[s] == 1);
                                glBindTexture(GL_TEXTURE_2D, (inUse && casts)
                                                                 ? shadowDepthTexture[fboSlot[s]]
                                                                 : dummyShadowTexture);
                                QString uniformName = QString("shadowMaps[%1]").arg(s);
                                useshader->setUniformValue(uniformName.toStdString().c_str(), s + 1);
                            }
                            glextrafunctions->glActiveTexture(GL_TEXTURE0);
                        }

                        float mcolor[3];
                        if (mainWindow->ui->actionMute_Colours->isChecked())
                        {
                            mcolor[0] = static_cast<GLfloat>(SVObjects[i]->Colour[0] / 3 + 170);
                            mcolor[1] = static_cast<GLfloat>(SVObjects[i]->Colour[1] / 3 + 170);
                            mcolor[2] = static_cast<GLfloat>(SVObjects[i]->Colour[2] / 3 + 170);
                        }
                        else
                        {
                            mcolor[0] = static_cast<GLfloat>(SVObjects[i]->Colour[0]);
                            mcolor[1] = static_cast<GLfloat>(SVObjects[i]->Colour[1]);
                            mcolor[2] = static_cast<GLfloat>(SVObjects[i]->Colour[2]);
                        }

                        if (mainWindow->ui->actionBounding_Box->isChecked())
                        {
                            if (SVObjects[i]->boundingBoxBuffer.isCreated())
                            {
                                useshader->setUniformValue("albedo", QVector3D(
                                    mcolor[0]/255.0f, mcolor[1]/255.0f, mcolor[2]/255.0f));
                                useshader->setUniformValue("roughness",       static_cast<GLfloat>(1.0f));
                                useshader->setUniformValue("metallic",        static_cast<GLfloat>(0.0f));
                                useshader->setUniformValue("alpha",           static_cast<GLfloat>(1.0f));
                                useshader->setUniformValue("ambientStrength", static_cast<GLfloat>(0.02f));
                                SVObjects[i]->boundingBoxBuffer.bind();
                                useshader->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
                                useshader->enableAttributeArray("vertex");
                                useshader->setAttributeBuffer("normal", GL_FLOAT, 12 * 6 * sizeof(GLfloat), 3, 0);
                                useshader->enableAttributeArray("normal");
                                SVObjects[i]->boundingBoxBuffer.release();
                                glfunctions->glDrawArrays(GL_LINES, 0, 24);
                            }
                        }
                        else
                        {
                            GLfloat roughness = 0.6f;
                            if      (SVObjects[i]->Shininess == 3) roughness = 0.15f;
                            else if (SVObjects[i]->Shininess == 2) roughness = 0.35f;
                            else if (SVObjects[i]->Shininess == 1) roughness = 0.55f;
                            else if (SVObjects[i]->Shininess == 0) roughness = 0.75f;
                            else if (SVObjects[i]->Shininess < 0)
                                roughness = 1.0f - (static_cast<GLfloat>(0 - SVObjects[i]->Shininess) / 100.0f);

                            float t = 1.0f;
                            if      (SVObjects[i]->Transparency == 4) t = 0.25f;
                            else if (SVObjects[i]->Transparency == 3) t = 0.45f;
                            else if (SVObjects[i]->Transparency == 2) t = 0.60f;
                            else if (SVObjects[i]->Transparency == 1) t = 0.80f;
                            else if (SVObjects[i]->Transparency < 0)
                                t = static_cast<float>(100 + SVObjects[i]->Transparency) / 100.0f;

                            useshader->setUniformValue("albedo", QVector3D(
                                mcolor[0]/255.0f, mcolor[1]/255.0f, mcolor[2]/255.0f));
                            useshader->setUniformValue("roughness",       roughness);
                            useshader->setUniformValue("metallic",        static_cast<GLfloat>(0.0f));
                            useshader->setUniformValue("alpha",           static_cast<GLfloat>(t));
                            useshader->setUniformValue("ambientStrength", static_cast<GLfloat>(0.02f));

                            for (int j = 0; j < SVObjects[i]->VertexBuffers.count(); j++)
                            {
                                SVObjects[i]->VertexBuffers[j]->bind();
                                useshader->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
                                useshader->enableAttributeArray("vertex");
                                useshader->setAttributeBuffer("normal", GL_FLOAT,
                                    3 * static_cast<int>(SVObjects[i]->VBOVertexCounts[j]) * static_cast<int>(sizeof(GLfloat)), 3, 0);
                                useshader->enableAttributeArray("normal");
                                SVObjects[i]->VertexBuffers[j]->release();
                                glfunctions->glDrawArrays(GL_TRIANGLES, 0, SVObjects[i]->VBOVertexCounts[j]);
                            }
                        }
                        useshader->release();
                    }
                }
        }
    }

    glDepthMask(true);
    updateFOV();
    scaleBall->draw(vMatrix, vMatrix * camera);
    if (mainWindow->ui->actionShow_Scale_Grid->isChecked())
        scaleGrid->draw(vMatrix, vMatrix * camera);
}

bool GlWidget::CanISee(int index)
{
    if (SVObjects[index]->Visible == false) return false;
    if (SVObjects[index]->InGroup == -1) return true;
    return CanISee(SVObjects[index]->Parent());
}

void GlWidget::SetStereoSeparation(float s) { StereoSeparation = s; }

void GlWidget::SetClip(int Start, int Depth, int Angle)
{
    ClipStart = static_cast<float>(Start + 1) / 50;
    ClipDepth = static_cast<float>(ClipStart) + 0.005f + static_cast<float>(Depth) / 100.0f;
    ClipAngle = static_cast<float>(Angle) / 10;
    update();
}

void GlWidget::Rotate(double angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 rotmatrix; rotmatrix.setToIdentity();
            rotmatrix.rotate(static_cast<float>(angle), 0.0f, 0.0f, 1.0f);
            if (i == 0) rotationZ += angle;
            QMatrix4x4 svmatrix(SVObjects[i]->matrix);
            if (!sp2Lock) { rotmatrix *= svmatrix; rotmatrix.copyDataTo(SVObjects[i]->matrix); isFileDirty = true; }
        }
    }
}

void GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    bool donesomething = false;
    bool rotmode = false;
    if (event->buttons() & Qt::RightButton
        || ((event->buttons() & Qt::LeftButton) && event->modifiers() == Qt::CTRL)
        || ((event->buttons() & Qt::LeftButton) && mainWindow->ui->actionRotate_Lock->isChecked()))
        rotmode = true;

    if ((rotmode && event->modifiers() == Qt::SHIFT) || (event->modifiers() == Qt::ALT && event->buttons() & Qt::LeftButton))
    {
        double before  = atan2(static_cast<double>(LastMouseXpos - xdim/2), static_cast<double>(LastMouseYpos - ydim/2));
        double after   = atan2(static_cast<double>(event->x()    - xdim/2), static_cast<double>(event->y()    - ydim/2));
        double amount1 = after - before;
        double amount2 = after + 2*3.14159 - before;
        double amount  = (qAbs(amount2) < qAbs(amount1)) ? amount2 : amount1;
        if (amount > .1) amount = .1;
        Rotate(amount * 70);
        donesomething = true;
    }
    else if (rotmode)
    {
        float yangle = (static_cast<float>(event->x() - LastMouseXpos)) / SENSITIVITY;
        float xangle = (static_cast<float>(event->y() - LastMouseYpos)) / SENSITIVITY;
        for (int i = 0; i < SVObjects.count(); i++)
        {
            bool f = false;
            if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
            if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
            if (f)
            {
                QMatrix4x4 rotmatrix; rotmatrix.setToIdentity();
                rotmatrix.rotate(xangle, 1.0f, 0.0f, 0.0f);
                rotmatrix.rotate(yangle, 0.0f, 1.0f, 0.0f);
                if (i == 0) { rotationX += xangle; rotationY += yangle; }
                QMatrix4x4 svmatrix(SVObjects[i]->matrix);
                if (!sp2Lock) { rotmatrix *= svmatrix; rotmatrix.copyDataTo(SVObjects[i]->matrix); donesomething = true; isFileDirty = true; }
            }
        }
    }

    if (!rotmode && event->buttons() & Qt::LeftButton && !donesomething)
    {
        float ObjXpos =   static_cast<float>(event->x() - LastMouseXpos) / (SENSITIVITY * 100);
        float ObjYpos = -(static_cast<float>(event->y() - LastMouseYpos) / (SENSITIVITY * 100));
        for (int i = 0; i < SVObjects.count(); i++)
        {
            bool f = false;
            if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
            if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
            if (f)
            {
                QMatrix4x4 rotmatrix; rotmatrix.setToIdentity();
                rotmatrix.translate(ObjXpos, ObjYpos, 0.0f);
                if (i == 0) { transformX += ObjXpos; transformY += ObjYpos; }
                QMatrix4x4 svmatrix(SVObjects[i]->matrix);
                if (!sp2Lock) { rotmatrix *= svmatrix; rotmatrix.copyDataTo(SVObjects[i]->matrix); donesomething = true; isFileDirty = true; }
            }
        }
    }
    LastMouseXpos = event->x();
    LastMouseYpos = event->y();
    if (donesomething) update();
}

void GlWidget::moveCameraZ(double value) { cameraZ += value; update(); }

void GlWidget::ZRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 r; r.setToIdentity(); r.rotate(angle, 0,0,1);
            if (i==0) rotationZ += angle;
            QMatrix4x4 s(SVObjects[i]->matrix);
            if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); isFileDirty = true; }
        }
    }
}

void GlWidget::YRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 r; r.setToIdentity(); r.rotate(angle, 0,1,0);
            if (i==0) rotationY += angle;
            QMatrix4x4 s(SVObjects[i]->matrix);
            if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); isFileDirty = true; }
        }
    }
}

void GlWidget::XRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 r; r.setToIdentity(); r.rotate(angle, 1,0,0);
            if (i==0) rotationX += angle;
            QMatrix4x4 s(SVObjects[i]->matrix);
            if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); isFileDirty = true; }
        }
    }
}

void GlWidget::Translate(float x, float y, float z)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 r; r.setToIdentity(); r.translate(x/10.0f, y/10.0f, z/10.0f);
            if (i==0) { transformX+=x; transformY+=y; transformZ+=z; }
            QMatrix4x4 s(SVObjects[i]->matrix);
            if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); isFileDirty = true; }
        }
    }
}

void GlWidget::Resize(float value)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 r; r.setToIdentity(); r.scale(value, value, value);
            SVObjects[i]->scale *= static_cast<double>(value);
            QMatrix4x4 s(SVObjects[i]->matrix);
            if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); isFileDirty = true; }
        }
    }
}

void GlWidget::ResetSize()
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            float inv = 1.0f / static_cast<float>(SVObjects[i]->scale);
            QMatrix4x4 r; r.setToIdentity(); r.scale(inv, inv, inv);
            QMatrix4x4 s(SVObjects[i]->matrix);
            if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); }
            SVObjects[i]->scale = 1.0;
            isFileDirty = true;
        }
    }
}

void GlWidget::ResetToDefault()
{
    for (int j = 0; j < SVObjects.count(); j++)
        if (SVObjects[j]->gotdefaultmatrix)
            for (int i = 0; i < 16; i++) SVObjects[j]->matrix[i] = SVObjects[j]->defaultmatrix[i];
    ClipAngle = defaultClipAngle;
    rotationX = rotationY = rotationZ = transformX = transformY = transformZ = 0;
    mainWindow->ui->ClipAngle->setValue(static_cast<int>(ClipAngle) * 10);
}

void GlWidget::NewDefault()
{
    if (SVObjects.count() > 0)
    {
        for (int j = 0; j < SVObjects.count(); j++)
            for (int i = 0; i < 16; i++) { SVObjects[j]->defaultmatrix[i] = SVObjects[j]->matrix[i]; SVObjects[j]->gotdefaultmatrix = true; }
        defaultClipAngle = ClipAngle;
        rotationX = rotationY = rotationZ = transformX = transformY = transformZ = 0;
    }
}

bool GlWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent *>(event));
    return QWidget::event(event);
}

void GlWidget::grabGestures(const QList<Qt::GestureType> &gestures)
{
    foreach (Qt::GestureType gesture, gestures) grabGesture(gesture);
}

bool GlWidget::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}

void GlWidget::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::RotationAngleChanged)
    {
        Rotate(gesture->lastRotationAngle() - gesture->rotationAngle());
        update();
    }
    if (changeFlags & QPinchGesture::ScaleFactorChanged)
    {
        double f = gesture->totalScaleFactor();
        int cur  = mainWindow->ui->ClipAngle->value();
        int mx   = mainWindow->ui->ClipAngle->maximum();
        int mn   = mainWindow->ui->ClipAngle->minimum();
        int nw   = cur;
        if (f > 0) { nw = static_cast<int>(cur * (1 + (f-1)/20)); if (nw>mx) nw=mx; }
        else if (f < 0) { nw = static_cast<int>(cur * (f/20)); if (nw<mn) nw=mn; }
        mainWindow->ui->ClipAngle->setValue(nw);
        SetClip(mainWindow->ui->ClipStart->value(), mainWindow->ui->ClipDepth->value(), mainWindow->ui->ClipAngle->value());
        update();
    }
    if (gesture->state() == Qt::GestureFinished) update();
}

float GlWidget::GetLightPowerMultiplier(int power)
{
    return float(pow(1.5f, float(power - 1)));
}

QVector3D GlWidget::GetLightColour(int lightPower, QColor lightColour)
{
    float p = GetLightPowerMultiplier(lightPower);
    return QVector3D(lightColour.redF()*p, lightColour.greenF()*p, lightColour.blueF()*p);
}

QVector3D GlWidget::GetLightDirection(int xyAngle, int zPos)
{
    float rad = qDegreesToRadians(static_cast<double>(xyAngle));
    float x = sin(rad);
    float y = cos(rad);
    float z = (static_cast<double>(zPos - 25)) / 50.0;
    return QVector3D(-x, -y, -z).normalized();
}

double GlWidget::getFOV() { return currentFOV; }

void GlWidget::updateFOV()
{
    float scale   = 1.0f / static_cast<float>(mmPerUnit);
    float divider = (this->height() * globalRescale) / 30.0f;
    currentFOV    = static_cast<double>(ClipAngle / (divider * scale));
}

void GlWidget::glDebug(QString string)
{
    GLenum err;
    QString errMess;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        switch (err)
        {
        case GL_INVALID_ENUM:                  errMess = "Invalid Enum"; break;
        case GL_INVALID_VALUE:                 errMess = "Invalid Value"; break;
        case GL_INVALID_OPERATION:             errMess = "Invalid Operation"; break;
        case GL_STACK_OVERFLOW:                errMess = "Stack Overflow"; break;
        case GL_STACK_UNDERFLOW:               errMess = "Stack Underflow"; break;
        case GL_OUT_OF_MEMORY:                 errMess = "Out of Memory"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errMess = "Invalid Framebuffer Operation"; break;
        case GL_TABLE_TOO_LARGE:               errMess = "Table too large"; break;
        }
        qDebug() << QString("[GL Error] %1 - %2").arg(errMess).arg(string);
    }
}
