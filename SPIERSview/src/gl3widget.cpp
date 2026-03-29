
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

GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFormat(surfaceFormat);
    LastMouseXpos = -1; LastMouseYpos = -1;
    setMouseTracking(true);
    cameraX = 0; cameraY = 0; cameraZ = 3;
    centerX = 0; centerY = 0; centerZ = 0;
    StereoSeparation = static_cast<float>(.04);
    defaultClipAngle = mainWindow->ui->ClipAngle->value() / 10;
    setFocusPolicy(Qt::NoFocus);
    QList<Qt::GestureType> gestures;
    gestures << Qt::PinchGesture;
    grabGestures(gestures);

    for (int i = 0; i < 3; i++) { shadowFBO[i] = 0; shadowDepthTexture[i] = 0; }
    dummyShadowTexture = 0;
    oitFBO = 0; oitAccumTexture = 0; oitRevealTexture = 0;
    oitDepthRBO = 0; fullscreenQuadVBO = 0;
    pickFBO = 0; pickColourTexture = 0; pickDepthRBO = 0;

    realOrthoHeightMm = 0;
    realOrthoWidthMm = 0;
}

GlWidget::~GlWidget()
{
    makeCurrent();
    lightingShaderProgram.release();
    lightingShaderProgramShadow.release();
    lightingShaderProgramPCF.release();
    shadowDepthShaderProgram.release();
    oitAccumShaderProgram.release();
    oitAccumShaderProgramShadow.release();
    oitAccumShaderProgramPCF.release();
    oitCompositeShaderProgram.release();
    lightingShaderProgram.deleteLater();
    lightingShaderProgramShadow.deleteLater();
    lightingShaderProgramPCF.deleteLater();
    shadowDepthShaderProgram.deleteLater();
    oitAccumShaderProgram.deleteLater();
    oitAccumShaderProgramShadow.deleteLater();
    oitAccumShaderProgramPCF.deleteLater();
    oitCompositeShaderProgram.deleteLater();
    for (int i = 0; i < 3; i++)
    {
        if (shadowFBO[i])          glextrafunctions->glDeleteFramebuffers(1, &shadowFBO[i]);
        if (shadowDepthTexture[i]) glDeleteTextures(1, &shadowDepthTexture[i]);
    }
    if (dummyShadowTexture) glDeleteTextures(1, &dummyShadowTexture);
    if (oitFBO)             glextrafunctions->glDeleteFramebuffers(1, &oitFBO);
    if (oitAccumTexture)    glDeleteTextures(1, &oitAccumTexture);
    if (oitRevealTexture)   glDeleteTextures(1, &oitRevealTexture);
    if (oitDepthRBO)        glextrafunctions->glDeleteRenderbuffers(1, &oitDepthRBO);
    if (fullscreenQuadVBO)  glDeleteBuffers(1, &fullscreenQuadVBO);
    if (pickFBO)            glextrafunctions->glDeleteFramebuffers(1, &pickFBO);
    if (pickColourTexture)  glDeleteTextures(1, &pickColourTexture);
    if (pickDepthRBO)       glextrafunctions->glDeleteRenderbuffers(1, &pickDepthRBO);

    vao.release(); vao.deleteLater();

    doneCurrent();
}

// ---------------------------------------------------------------------------
// Shader loader helper — prepends defines after #version line
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// initializeGL
// ---------------------------------------------------------------------------
void GlWidget::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();
    glfunctions      = this->context()->functions();
    glextrafunctions = this->context()->extraFunctions();

    vao.create(); vao.bind();

    const QString noShadow   = "#define SHADOWS_ENABLED 0\n#define PCF_ENABLED 0\n";
    const QString hardShadow = "#define SHADOWS_ENABLED 1\n#define PCF_ENABLED 0\n";
    const QString softShadow = "#define SHADOWS_ENABLED 1\n#define PCF_ENABLED 1\n";

    // Lighting shader — three variants
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

    // Shadow depth shader
    shadowDepthShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/shadowDepthVertexShader.vsh");
    shadowDepthShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shadowDepthFragmentShader.fsh");
    shadowDepthShaderProgram.link();
    if (!shadowDepthShaderProgram.isLinked())
        qDebug() << "Depth shader link failed:" << shadowDepthShaderProgram.log();

    // OIT accumulation shader — three variants
    addShaderWithDefines(oitAccumShaderProgram,       QOpenGLShader::Vertex,   ":/oitAccumVertexShader.vsh", noShadow);
    addShaderWithDefines(oitAccumShaderProgramShadow, QOpenGLShader::Vertex,   ":/oitAccumVertexShader.vsh", hardShadow);
    addShaderWithDefines(oitAccumShaderProgramPCF,    QOpenGLShader::Vertex,   ":/oitAccumVertexShader.vsh", softShadow);
    addShaderWithDefines(oitAccumShaderProgram,       QOpenGLShader::Fragment, ":/oitAccumFragmentShader.fsh", noShadow);
    addShaderWithDefines(oitAccumShaderProgramShadow, QOpenGLShader::Fragment, ":/oitAccumFragmentShader.fsh", hardShadow);
    addShaderWithDefines(oitAccumShaderProgramPCF,    QOpenGLShader::Fragment, ":/oitAccumFragmentShader.fsh", softShadow);
    oitAccumShaderProgram.link();
    oitAccumShaderProgramShadow.link();
    oitAccumShaderProgramPCF.link();

    if (!oitAccumShaderProgram.isLinked())
        qDebug() << "OIT accum no-shadow link failed:" << oitAccumShaderProgram.log();
    if (!oitAccumShaderProgramShadow.isLinked())
        qDebug() << "OIT accum hard shadow link failed:" << oitAccumShaderProgramShadow.log();
    if (!oitAccumShaderProgramPCF.isLinked())
        qDebug() << "OIT accum PCF link failed:" << oitAccumShaderProgramPCF.log();

    // OIT composite shader — single variant
    oitCompositeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/oitCompositeVertexShader.vsh");
    oitCompositeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/oitCompositeFragmentShader.fsh");
    oitCompositeShaderProgram.link();
    if (!oitCompositeShaderProgram.isLinked())
        qDebug() << "OIT composite link failed:" << oitCompositeShaderProgram.log();

    qDebug() << "OpenGL version:"
             << context()->format().majorVersion()
             << "."
             << context()->format().minorVersion();

        initShadowFBOs();
    initOIT();
    initPickFBO();

    // Pick shader — flat colour, no lighting
    pickShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/pickVertexShader.vsh");
    pickShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/pickFragmentShader.fsh");
    pickShaderProgram.link();
    if (!pickShaderProgram.isLinked())
        qDebug() << "Pick shader link failed:" << pickShaderProgram.log();

}

// ---------------------------------------------------------------------------
// Shadow FBO setup
// ---------------------------------------------------------------------------
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
// OIT FBO setup — called once at init, resized when viewport changes
// ---------------------------------------------------------------------------
void GlWidget::initOIT()
{
    // Fullscreen quad — two triangles covering NDC [-1,1]
    // Stored as vec2 positions only, z=0 supplied by composite vertex shader
    static const float quadVerts[] = {
        -1.0f, -1.0f,   1.0f, -1.0f,   1.0f,  1.0f,
        -1.0f, -1.0f,   1.0f,  1.0f,  -1.0f,  1.0f
    };
    glGenBuffers(1, &fullscreenQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create FBO and textures at initial size — resizeOIT handles actual sizing
    glextrafunctions->glGenFramebuffers(1, &oitFBO);
    glGenTextures(1, &oitAccumTexture);
    glGenTextures(1, &oitRevealTexture);
    glextrafunctions->glGenRenderbuffers(1, &oitDepthRBO);

    // Size to current viewport (will be recreated on resize)
    resizeOIT(qMax(1, xdim), qMax(1, ydim));
}

// ---------------------------------------------------------------------------
// Resize OIT textures — called from resizeGL and initOIT
// ---------------------------------------------------------------------------
void GlWidget::resizeOIT(int w, int h)
{
    // Accum texture — RGBA16F, additive blended
    glBindTexture(GL_TEXTURE_2D, oitAccumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Reveal texture — R16F, multiplicative blended
    glBindTexture(GL_TEXTURE_2D, oitRevealTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, w, h, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Depth renderbuffer — same size, used to test transparent frags against opaque depth
    glextrafunctions->glBindRenderbuffer(GL_RENDERBUFFER, oitDepthRBO);
    glextrafunctions->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glextrafunctions->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach all to OIT FBO
    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, oitFBO);
    glextrafunctions->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_TEXTURE_2D, oitAccumTexture, 0);
    glextrafunctions->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                              GL_TEXTURE_2D, oitRevealTexture, 0);
    glextrafunctions->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                 GL_RENDERBUFFER, oitDepthRBO);
    GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glextrafunctions->glDrawBuffers(2, drawBufs);
    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ---------------------------------------------------------------------------
// Light + shadow uniform helper — shared between opaque and OIT accum passes
// ---------------------------------------------------------------------------
void GlWidget::setLightAndShadowUniforms(QOpenGLShaderProgram *shader,
                                          bool shadowsActive,
                                          const QMatrix4x4 &objMatrix,
                                          int lightCount,
                                          const QVector3D *directions,
                                          const QVector3D *colors)
{
    shader->setUniformValueArray("lightDirections", directions, lightCount);
    shader->setUniformValueArray("lightColors",     colors,     lightCount);
    shader->setUniformValue("lightCount", lightCount);

    if (!shadowsActive) return;

    int castsShadow[3] = {0, 0, 0};
    int packedIndex = 0;
    castsShadow[packedIndex++] = (mainLightShadows != ShadowMode::None) ? 1 : 0;
    if (headlightActive)
        castsShadow[packedIndex++] = (headlightShadows != ShadowMode::None) ? 1 : 0;
    if (secondaryLightActive)
        castsShadow[packedIndex++] = (secondaryLightShadows != ShadowMode::None) ? 1 : 0;

    shader->setUniformValueArray("lightCastsShadow", castsShadow, 3);

    int fboSlot[3] = {0, 0, 0};
    int p = 0;
    fboSlot[p++] = 0;
    if (headlightActive)      fboSlot[p++] = 1;
    if (secondaryLightActive) fboSlot[p++] = 2;

    QMatrix4x4 combinedLightMatrices[3];
    for (int s = 0; s < 3; s++)
        combinedLightMatrices[s] = lightSpaceMatrix[fboSlot[s]] * objMatrix;

    shader->setUniformValueArray("lightSpaceMatrices", combinedLightMatrices, 3);
    shader->setUniformValue("shadowBias",         0.005f);
    shader->setUniformValue("shadowMapTexelSize", 1.5f / SHADOW_MAP_SIZE);

    for (int s = 0; s < 3; s++)
    {
        glextrafunctions->glActiveTexture(GL_TEXTURE1 + s);
        bool inUse = (s < lightCount);
        bool casts = (castsShadow[s] == 1);
        glBindTexture(GL_TEXTURE_2D, (inUse && casts)
                      ? shadowDepthTexture[fboSlot[s]]
                      : dummyShadowTexture);
        QString uniformName = QString("shadowMaps[%1]").arg(s);
        shader->setUniformValue(uniformName.toStdString().c_str(), s + 1);
    }
    glextrafunctions->glActiveTexture(GL_TEXTURE0);
}

// ---------------------------------------------------------------------------
// selectShader / selectOITShader
// ---------------------------------------------------------------------------
QOpenGLShaderProgram *GlWidget::selectShader()
{
    bool anyHard = (mainLightShadows == ShadowMode::Hard)
                || (headlightActive      && headlightShadows      == ShadowMode::Hard)
                || (secondaryLightActive && secondaryLightShadows == ShadowMode::Hard);
    bool anySoft = (mainLightShadows == ShadowMode::Soft)
                || (headlightActive      && headlightShadows      == ShadowMode::Soft)
                || (secondaryLightActive && secondaryLightShadows == ShadowMode::Soft);
    if (anySoft) return &lightingShaderProgramPCF;
    if (anyHard) return &lightingShaderProgramShadow;
    return &lightingShaderProgram;
}

QOpenGLShaderProgram *GlWidget::selectOITShader()
{
    bool anyHard = (mainLightShadows == ShadowMode::Hard)
                || (headlightActive      && headlightShadows      == ShadowMode::Hard)
                || (secondaryLightActive && secondaryLightShadows == ShadowMode::Hard);
    bool anySoft = (mainLightShadows == ShadowMode::Soft)
                || (headlightActive      && headlightShadows      == ShadowMode::Soft)
                || (secondaryLightActive && secondaryLightShadows == ShadowMode::Soft);
    if (anySoft) return &oitAccumShaderProgramPCF;
    if (anyHard) return &oitAccumShaderProgramShadow;
    return &oitAccumShaderProgram;
}

// ---------------------------------------------------------------------------
// computeLightSpaceMatrix / renderShadowPass — unchanged
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
        QMatrix4x4 objMatrix;
        objMatrix.translate(0, 0, -1);
        objMatrix *= QMatrix4x4(SVObjects[i]->matrix);
        objMatrix *= globalMatrix;
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

// ---------------------------------------------------------------------------
// resizeGL
// ---------------------------------------------------------------------------
void GlWidget::resizeGL(int width, int height)
{
    if (height == 0) height = 1;
    xdim = static_cast<int>(static_cast<double>(width)  * applicationScaleX);
    ydim = static_cast<int>(static_cast<double>(height) * applicationScaleY);
    DoPMatrix(xdim, ydim);
    glViewport(0, 0, xdim, ydim);
    // OIT textures must match viewport size
    if (oitFBO)  resizeOIT(xdim, ydim);
    // Pick FBO must match viewport size
    if (pickFBO) resizePickFBO(xdim, ydim);
    update();
    emit resized(); //for the painter overlay
}

void GlWidget::DoPMatrix(int width, int height)
{
    float asp   = static_cast<float>(width) / static_cast<float>(height);
    float fudge = static_cast<float>(1300)  / static_cast<float>(width);
    pMatrix.setToIdentity();
    if (mainWindow->ui->actionOrthographic_View->isChecked())
        pMatrix.ortho(-(ClipAngle/(10*fudge)), ClipAngle/(10*fudge),
                      -(ClipAngle/(10*fudge))/asp, ClipAngle/(10*fudge)/asp,
                      ClipStart, ClipDepth);
    else
        pMatrix.perspective(ClipAngle, asp, ClipStart, ClipDepth);
}

// ---------------------------------------------------------------------------
// paintGL
// ---------------------------------------------------------------------------
void GlWidget::paintGL()
{
    // Shadow passes
    QVector3D mainLightDirection      = GetLightDirection(mainLightXYAngle, mainLightZPos);
    QVector3D secondaryLightDirection = GetLightDirection(secondaryLightXYAngle, secondaryLightZPos);
    if (mainLightShadows != ShadowMode::None)
        renderShadowPass(0, mainLightDirection, lightSpaceMatrix[0]);
    if (headlightActive && headlightShadows != ShadowMode::None)
        renderShadowPass(1, QVector3D(0.0f, 0.0f, 1.0f), lightSpaceMatrix[1]);
    if (secondaryLightActive && secondaryLightShadows != ShadowMode::None)
        renderShadowPass(2, secondaryLightDirection, lightSpaceMatrix[2]);

    // Main framebuffer clear
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
        glfunctions->glViewport(0, 0, xdim/2, ydim);
        DrawObjects(false, true);
        glfunctions->glClear(GL_DEPTH_BUFFER_BIT);
        glfunctions->glViewport(xdim/2, 0, xdim/2, ydim);
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

    DoPMatrix(xdim, ydim);
    DrawObjects(false, false);
    //qDebug()<<"Emmitting";
    emit glUpdate();
}

// ---------------------------------------------------------------------------
// OIT accumulation pass
// Renders all transparent objects into the OIT FBO accumulation buffers.
// Depth test is ON (transparent frags behind opaque are discarded) but
// depth write is OFF (transparent frags don't occlude each other).
// ---------------------------------------------------------------------------
void GlWidget::renderOITAccumPass(bool rightview, QMatrix4x4 &vMatrix)
{
    // Copy opaque depth buffer into OIT depth renderbuffer so transparent
    // fragments are correctly discarded behind opaque geometry
    glextrafunctions->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glextrafunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oitFBO);
    glextrafunctions->glBlitFramebuffer(
        0, 0, xdim, ydim,
        0, 0, xdim, ydim,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, oitFBO);
    glViewport(0, 0, xdim, ydim);

    // Clear accum to (0,0,0,0) and reveal to (1) — fully transparent initially
    static const float clearAccum[]  = {0.0f, 0.0f, 0.0f, 0.0f};
    static const float clearReveal[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glextrafunctions->glClearBufferfv(GL_COLOR, 0, clearAccum);
    glextrafunctions->glClearBufferfv(GL_COLOR, 1, clearReveal);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);  // transparent frags do not write depth
    glEnable(GL_BLEND);

    // Accum attachment: additive blend — accumulate weighted colours
    glextrafunctions->glBlendFunci(0, GL_ONE, GL_ONE);
    // Reveal attachment: multiplicative blend — multiply transmittances
    glextrafunctions->glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

    QOpenGLShaderProgram *accumShader = selectOITShader();
    bool shadowsActive = (accumShader != &oitAccumShaderProgram);

    QVector3D mainLightDirection      = GetLightDirection(mainLightXYAngle, mainLightZPos);
    QVector3D mainLightCol            = GetLightColour(mainLightPower - 1, mainLightColour);
    QVector3D secondaryLightDirection = GetLightDirection(secondaryLightXYAngle, secondaryLightZPos);
    QVector3D secondaryLightCol       = GetLightColour(secondaryLightPower - 5, secondaryLightColour);
    QVector3D headlightCol            = GetLightColour(headlightPower - 4, headlightColour);

    for (int i = 0; i < SVObjects.count(); i++)
    {
        if (SVObjects[i]->IsGroup || SVObjects[i]->isSurfacing) continue;
        if (!CanISee(i)) continue;
        if (SVObjects[i]->Transparency == 0) continue;  // opaque handled elsewhere

        QMatrix4x4 mMatrix(SVObjects[i]->matrix);
        QMatrix4x4 mvMatrix = vMatrix;
        mvMatrix.translate(0, 0, -1);
        mvMatrix *= mMatrix;
        mvMatrix *= globalMatrix;
        QMatrix3x3 normalMatrix = mvMatrix.normalMatrix();

        QMatrix4x4 objMatrix;
        objMatrix.translate(0, 0, -1);
        objMatrix *= mMatrix;
        objMatrix *= globalMatrix;

        // Build light list
        QVector3D directions[3], colors[3];
        int lightCount = 0;
        directions[lightCount] = mainLightDirection; colors[lightCount] = mainLightCol; lightCount++;
        if (headlightActive)      { directions[lightCount] = QVector3D(0,0,1); colors[lightCount] = headlightCol; lightCount++; }
        if (secondaryLightActive) { directions[lightCount] = secondaryLightDirection; colors[lightCount] = secondaryLightCol; lightCount++; }

        // Roughness
        GLfloat roughness = 0.6f;
        if      (SVObjects[i]->Shininess == 3) roughness = 0.15f;
        else if (SVObjects[i]->Shininess == 2) roughness = 0.35f;
        else if (SVObjects[i]->Shininess == 1) roughness = 0.55f;
        else if (SVObjects[i]->Shininess == 0) roughness = 0.75f;
        else if (SVObjects[i]->Shininess < 0)
            roughness = 1.0f - (static_cast<GLfloat>(0 - SVObjects[i]->Shininess) / 100.0f);

        // Alpha
        float t = 1.0f;
        if      (SVObjects[i]->Transparency == 4) t = 0.40f;
        else if (SVObjects[i]->Transparency == 3) t = 0.20f;
        else if (SVObjects[i]->Transparency == 2) t = 0.10f;
        else if (SVObjects[i]->Transparency == 1) t = 0.05f;
        else if (SVObjects[i]->Transparency < 0)
            t = static_cast<float>(abs(SVObjects[i]->Transparency)) / 100.0f;

        // Colour
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

        accumShader->bind();
        accumShader->setUniformValue("mvpMatrix",      pMatrix * mvMatrix);
        accumShader->setUniformValue("mvMatrix",       mvMatrix);
        accumShader->setUniformValue("normalMatrix",   normalMatrix);
        accumShader->setUniformValue("albedo",         QVector3D(mcolor[0]/255.0f, mcolor[1]/255.0f, mcolor[2]/255.0f));
        accumShader->setUniformValue("roughness",      roughness);
        accumShader->setUniformValue("metallic",       static_cast<GLfloat>(0.0f));
        accumShader->setUniformValue("alpha",          static_cast<GLfloat>(t));
        accumShader->setUniformValue("ambientStrength",static_cast<GLfloat>(0.02f));

        setLightAndShadowUniforms(accumShader, shadowsActive, objMatrix,
                                   lightCount, directions, colors);

        for (int j = 0; j < SVObjects[i]->VertexBuffers.count(); j++)
        {
            SVObjects[i]->VertexBuffers[j]->bind();
            accumShader->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
            accumShader->enableAttributeArray("vertex");
            accumShader->setAttributeBuffer("normal", GL_FLOAT,
                3 * static_cast<int>(SVObjects[i]->VBOVertexCounts[j]) * static_cast<int>(sizeof(GLfloat)), 3, 0);
            accumShader->enableAttributeArray("normal");
            SVObjects[i]->VertexBuffers[j]->release();
            glfunctions->glDrawArrays(GL_TRIANGLES, 0, SVObjects[i]->VBOVertexCounts[j]);
        }
        accumShader->release();
    }

    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthMask(true);
}

// ---------------------------------------------------------------------------
// OIT composite pass
// Blends the accumulated transparent layers over the opaque scene.
// ---------------------------------------------------------------------------
void GlWidget::renderOITCompositePass()
{
    // Back to default framebuffer, no depth test needed for fullscreen quad
    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // src alpha controls how much of the transparent colour replaces the opaque colour
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    oitCompositeShaderProgram.bind();

    // Bind accumulation textures
    glextrafunctions->glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, oitAccumTexture);
    oitCompositeShaderProgram.setUniformValue("accumTexture", 0);

    glextrafunctions->glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, oitRevealTexture);
    oitCompositeShaderProgram.setUniformValue("revealTexture", 1);

    glextrafunctions->glActiveTexture(GL_TEXTURE0);

    // Draw fullscreen quad
    glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadVBO);
    oitCompositeShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 2, 0);
    oitCompositeShaderProgram.enableAttributeArray("vertex");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glfunctions->glDrawArrays(GL_TRIANGLES, 0, 6);

    oitCompositeShaderProgram.release();

    // Restore depth test for subsequent frames
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ---------------------------------------------------------------------------
// DrawObjects — renders opaque objects, then triggers OIT if needed
// ---------------------------------------------------------------------------
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

    // ------------------------------------------------------------------
    // Opaque pass — depth write ON, no blending
    // ------------------------------------------------------------------
    glDepthMask(true);
    glfunctions->glDisable(GL_BLEND);

    for (int i = 0; i < SVObjects.count(); i++)
    {
        isFileLoaded = true;
        if (SVObjects[i]->IsGroup || SVObjects[i]->isSurfacing) continue;
        if (!CanISee(i)) continue;
        if (SVObjects[i]->Transparency != 0) continue;  // skip transparent

        QMatrix4x4 mMatrix(SVObjects[i]->matrix);
        QMatrix4x4 mvMatrix = vMatrix;
        mvMatrix.translate(0, 0, -1);
        mvMatrix *= mMatrix;
        mvMatrix *= globalMatrix;
        QMatrix3x3 normalMatrix = mvMatrix.normalMatrix();

        QMatrix4x4 objMatrix;
        objMatrix.translate(0, 0, -1);
        objMatrix *= mMatrix;
        objMatrix *= globalMatrix;

        // Build light list
        QVector3D directions[3], colors[3];
        int lightCount = 0;
        directions[lightCount] = mainLightDirection; colors[lightCount] = mainLightCol; lightCount++;
        if (headlightActive)      { directions[lightCount] = QVector3D(0,0,1); colors[lightCount] = headlightCol; lightCount++; }
        if (secondaryLightActive) { directions[lightCount] = secondaryLightDirection; colors[lightCount] = secondaryLightCol; lightCount++; }

        GLfloat roughness = 0.6f;
        if      (SVObjects[i]->Shininess == 3) roughness = 0.15f;
        else if (SVObjects[i]->Shininess == 2) roughness = 0.35f;
        else if (SVObjects[i]->Shininess == 1) roughness = 0.55f;
        else if (SVObjects[i]->Shininess == 0) roughness = 0.75f;
        else if (SVObjects[i]->Shininess < 0)
            roughness = 1.0f - (static_cast<GLfloat>(0 - SVObjects[i]->Shininess) / 100.0f);

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

        useshader->bind();

        useshader->setUniformValue("mvpMatrix",       pMatrix * mvMatrix);
        useshader->setUniformValue("mvMatrix",        mvMatrix);
        useshader->setUniformValue("normalMatrix",    normalMatrix);
        useshader->setUniformValue("albedo",          QVector3D(mcolor[0]/255.0f, mcolor[1]/255.0f, mcolor[2]/255.0f));
        useshader->setUniformValue("roughness",       roughness);
        useshader->setUniformValue("metallic",        static_cast<GLfloat>(0.0f));
        useshader->setUniformValue("alpha",           static_cast<GLfloat>(1.0f));
        useshader->setUniformValue("ambientStrength", static_cast<GLfloat>(0.02f));

        setLightAndShadowUniforms(useshader, shadowsActive, objMatrix,
                                   lightCount, directions, colors);

        if (mainWindow->ui->actionBounding_Box->isChecked())
        {
            if (SVObjects[i]->boundingBoxBuffer.isCreated())
            {
                useshader->setUniformValue("albedo",          QVector3D(mcolor[0]/255.0f, mcolor[1]/255.0f, mcolor[2]/255.0f));
                useshader->setUniformValue("roughness",       static_cast<GLfloat>(1.0f));
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
                glDebug(QString("after DrawArrays obj %1 vbo %2").arg(i).arg(j));
            }
        }
        useshader->release();
    }

    // ------------------------------------------------------------------
    // OIT transparent pass — only if any transparent objects exist
    // ------------------------------------------------------------------
    bool anyTransparent = false;
    for (int i = 0; i < SVObjects.count(); i++)
        if (!SVObjects[i]->IsGroup && !SVObjects[i]->isSurfacing
            && CanISee(i) && SVObjects[i]->Transparency != 0)
            { anyTransparent = true; break; }

    if (anyTransparent)
    {
        renderOITAccumPass(rightview, vMatrix);
        renderOITCompositePass();
    }

    glDepthMask(true);
    updateFOV();

}

// ---------------------------------------------------------------------------
// Everything below unchanged from original
// ---------------------------------------------------------------------------
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
    bool donesomething = false, rotmode = false;
    if (event->buttons() & Qt::RightButton
        || ((event->buttons() & Qt::LeftButton) && event->modifiers() == Qt::CTRL)
        || ((event->buttons() & Qt::LeftButton) && mainWindow->ui->actionRotate_Lock->isChecked()))
        rotmode = true;

    if ((rotmode && event->modifiers() == Qt::SHIFT) || (event->modifiers() == Qt::ALT && event->buttons() & Qt::LeftButton))
    {
        double before  = atan2(static_cast<double>(LastMouseXpos - xdim/2), static_cast<double>(LastMouseYpos - ydim/2));
        double after   = atan2(static_cast<double>(event->x() - xdim/2), static_cast<double>(event->y() - ydim/2));
        double amount1 = after - before, amount2 = after + 2*3.14159 - before;
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
                QMatrix4x4 r; r.setToIdentity();
                r.rotate(xangle, 1,0,0); r.rotate(yangle, 0,1,0);
                if (i==0) { rotationX += xangle; rotationY += yangle; }
                QMatrix4x4 s(SVObjects[i]->matrix);
                if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); donesomething = true; isFileDirty = true; }
            }
        }
    }

    if (!rotmode && event->buttons() & Qt::LeftButton && !donesomething)
    {
        float ObjXpos =   static_cast<float>(event->x() - LastMouseXpos) / (SENSITIVITY*100);
        float ObjYpos = -(static_cast<float>(event->y() - LastMouseYpos) / (SENSITIVITY*100));
        for (int i = 0; i < SVObjects.count(); i++)
        {
            bool f = false;
            if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
            if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
            if (f)
            {
                QMatrix4x4 r; r.setToIdentity(); r.translate(ObjXpos, ObjYpos, 0);
                if (i==0) { transformX += ObjXpos; transformY += ObjYpos; }
                QMatrix4x4 s(SVObjects[i]->matrix);
                if (!sp2Lock) { r *= s; r.copyDataTo(SVObjects[i]->matrix); donesomething = true; isFileDirty = true; }
            }
        }
    }
    LastMouseXpos = event->x(); LastMouseYpos = event->y();
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
        if (f) { QMatrix4x4 r; r.setToIdentity(); r.rotate(angle,0,0,1); if(i==0) rotationZ+=angle; QMatrix4x4 s(SVObjects[i]->matrix); if(!sp2Lock){r*=s;r.copyDataTo(SVObjects[i]->matrix);isFileDirty=true;} }
    }
}

void GlWidget::YRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f) { QMatrix4x4 r; r.setToIdentity(); r.rotate(angle,0,1,0); if(i==0) rotationY+=angle; QMatrix4x4 s(SVObjects[i]->matrix); if(!sp2Lock){r*=s;r.copyDataTo(SVObjects[i]->matrix);isFileDirty=true;} }
    }
}

void GlWidget::XRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f) { QMatrix4x4 r; r.setToIdentity(); r.rotate(angle,1,0,0); if(i==0) rotationX+=angle; QMatrix4x4 s(SVObjects[i]->matrix); if(!sp2Lock){r*=s;r.copyDataTo(SVObjects[i]->matrix);isFileDirty=true;} }
    }
}

void GlWidget::Translate(float x, float y, float z)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f) { QMatrix4x4 r; r.setToIdentity(); r.translate(x/10.0f,y/10.0f,z/10.0f); if(i==0){transformX+=x;transformY+=y;transformZ+=z;} QMatrix4x4 s(SVObjects[i]->matrix); if(!sp2Lock){r*=s;r.copyDataTo(SVObjects[i]->matrix);isFileDirty=true;} }
    }
}

void GlWidget::Resize(float value)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f) { QMatrix4x4 r; r.setToIdentity(); r.scale(value,value,value); SVObjects[i]->scale*=static_cast<double>(value); QMatrix4x4 s(SVObjects[i]->matrix); if(!sp2Lock){r*=s;r.copyDataTo(SVObjects[i]->matrix);isFileDirty=true;} }
    }
}

void GlWidget::ResetSize()
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && SVObjects[i]->widgetitem->isSelected()) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f) { float inv=1.0f/static_cast<float>(SVObjects[i]->scale); QMatrix4x4 r; r.setToIdentity(); r.scale(inv,inv,inv); QMatrix4x4 s(SVObjects[i]->matrix); if(!sp2Lock){r*=s;r.copyDataTo(SVObjects[i]->matrix);} SVObjects[i]->scale=1.0; isFileDirty=true; }
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
            for (int i = 0; i < 16; i++) { SVObjects[j]->defaultmatrix[i]=SVObjects[j]->matrix[i]; SVObjects[j]->gotdefaultmatrix=true; }
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
{ foreach (Qt::GestureType gesture, gestures) grabGesture(gesture); }

bool GlWidget::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}

void GlWidget::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::RotationAngleChanged) { Rotate(gesture->lastRotationAngle() - gesture->rotationAngle()); update(); }
    if (changeFlags & QPinchGesture::ScaleFactorChanged)
    {
        double f=gesture->totalScaleFactor(); int cur=mainWindow->ui->ClipAngle->value();
        int mx=mainWindow->ui->ClipAngle->maximum(), mn=mainWindow->ui->ClipAngle->minimum(), nw=cur;
        if (f>0){nw=static_cast<int>(cur*(1+(f-1)/20));if(nw>mx)nw=mx;}
        else if(f<0){nw=static_cast<int>(cur*(f/20));if(nw<mn)nw=mn;}
        mainWindow->ui->ClipAngle->setValue(nw);
        SetClip(mainWindow->ui->ClipStart->value(),mainWindow->ui->ClipDepth->value(),mainWindow->ui->ClipAngle->value());
        update();
    }
    if (gesture->state() == Qt::GestureFinished) update();
}

float    GlWidget::GetLightPowerMultiplier(int power) { return float(pow(1.5f, float(power-1))); }
QVector3D GlWidget::GetLightColour(int lightPower, QColor lightColour) { float p=GetLightPowerMultiplier(lightPower); return QVector3D(lightColour.redF()*p, lightColour.greenF()*p, lightColour.blueF()*p); }
QVector3D GlWidget::GetLightDirection(int xyAngle, int zPos) { float rad=qDegreesToRadians(static_cast<double>(xyAngle)); return QVector3D(-sin(rad),-cos(rad),-(static_cast<double>(zPos-25)/50.0)).normalized(); }
double   GlWidget::getFOV() { return currentFOV; }

void GlWidget::updateFOV()
{
    float scale=1.0f/static_cast<float>(mmPerUnit); float divider=(this->height()*globalRescale)/30.0f; currentFOV=static_cast<double>(ClipAngle/(divider*scale));
    realOrthoWidthMm = (ClipAngle * xdim * mmPerUnit) / (6500.0f * globalRescale);
    realOrthoHeightMm = (ClipAngle * ydim * mmPerUnit) / (6500.0f * globalRescale);
}

void GlWidget::glDebug(QString string)
{
    GLenum err; QString errMess;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        switch(err) {
        case GL_INVALID_ENUM:                  errMess="Invalid Enum"; break;
        case GL_INVALID_VALUE:                 errMess="Invalid Value"; break;
        case GL_INVALID_OPERATION:             errMess="Invalid Operation"; break;
        case GL_STACK_OVERFLOW:                errMess="Stack Overflow"; break;
        case GL_STACK_UNDERFLOW:               errMess="Stack Underflow"; break;
        case GL_OUT_OF_MEMORY:                 errMess="Out of Memory"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errMess="Invalid Framebuffer Operation"; break;
        case GL_TABLE_TOO_LARGE:               errMess="Table too large"; break;
        }
        qDebug() << QString("[GL Error] %1 - %2").arg(errMess).arg(string);
    }
}



void GlWidget::mousePressEvent(QMouseEvent *event)
{
    // Only handle ctrl+left click
    if (!(event->button() == Qt::MiddleButton))
        return;

    // Bail in stereo modes — depth buffer is split/ambiguous
    if (mainWindow->ui->actionSplit_Stereo->isChecked())   return;
    if (mainWindow->ui->actionAnaglyph_Stereo->isChecked()) return;

    makeCurrent();

    // Qt mouse coords are top-left origin; OpenGL depth buffer is bottom-left
    int mouseX = event->x();
    int mouseY = event->y();

    // Scale to actual framebuffer pixels (handles HiDPI)
    int fbX = static_cast<int>(mouseX * applicationScaleX);
    int fbY = static_cast<int>(mouseY * applicationScaleY);

    // Flip Y for OpenGL
    int glX = fbX;
    int glY = ydim - fbY - 1;

    // Read depth at click pixel
    GLfloat depth = 1.0f;
    glReadPixels(glX, glY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    // depth == 1.0 means no geometry was hit (empty space)
    if (depth >= 1.0f)
    {
        emit worldPositionClicked(QVector3D(), false, -1);
        return;
    }

    // Convert to NDC [-1, 1]
    float ndcX =  2.0f * static_cast<float>(glX) / static_cast<float>(xdim) - 1.0f;
    float ndcY =  2.0f * static_cast<float>(glY) / static_cast<float>(ydim) - 1.0f;
    float ndcZ =  2.0f * depth - 1.0f;

    // Build the same view matrix that was used to render this frame
    QMatrix4x4 vMatrix;
    vMatrix.setToIdentity();
    QVector3D camera(cameraX, cameraY, cameraZ);
    QVector3D center(centerX, centerY, centerZ);
    QVector3D up(0, 1, 0);
    vMatrix.lookAt(camera, center, up);

    // Unproject: invert the full projection * view transform
    QMatrix4x4 mvp = pMatrix * vMatrix;
    bool invertible = false;
    QMatrix4x4 mvpInv = mvp.inverted(&invertible);

    if (!invertible)
    {
        emit worldPositionClicked(QVector3D(), false, -1);
        return;
    }
    QVector4D clipPos(ndcX, ndcY, ndcZ, 1.0f);
    QVector4D worldPos4 = mvpInv * clipPos;

    // Perspective divide
    if (qAbs(worldPos4.w()) < 1e-6f)
    {
        emit worldPositionClicked(QVector3D(), false, -1);
        return;
    }
    worldPos4 /= worldPos4.w();

    QVector3D worldPos(worldPos4.x(), worldPos4.y(), worldPos4.z());

    // Pick which object was hit
    int objectIndex = pickObject(mouseX, mouseY);
    /*
    if (objectIndex >= 0 && objectIndex < SVObjects.count())
        qDebug() << "Hit object:" << objectIndex << SVObjects[objectIndex]->Name;
    else
        qDebug() << "No object hit (background)";
    */

    emit worldPositionClicked(worldPos, true, objectIndex);

    doneCurrent();
}



// ---------------------------------------------------------------------------
// initPickFBO — creates the colour pick FBO (called once at init)
// ---------------------------------------------------------------------------
void GlWidget::initPickFBO()
{
    glGenTextures(1, &pickColourTexture);
    glextrafunctions->glGenRenderbuffers(1, &pickDepthRBO);
    glextrafunctions->glGenFramebuffers(1, &pickFBO);
    resizePickFBO(qMax(1, xdim), qMax(1, ydim));
}

// ---------------------------------------------------------------------------
// resizePickFBO — recreates pick textures at new viewport size
// ---------------------------------------------------------------------------
void GlWidget::resizePickFBO(int w, int h)
{
    // RGBA8 colour texture — encodes object index as RGB
    glBindTexture(GL_TEXTURE_2D, pickColourTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Depth renderbuffer for correct front-surface selection
    glextrafunctions->glBindRenderbuffer(GL_RENDERBUFFER, pickDepthRBO);
    glextrafunctions->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glextrafunctions->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach to FBO
    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, pickFBO);
    glextrafunctions->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_TEXTURE_2D, pickColourTexture, 0);
    glextrafunctions->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                 GL_RENDERBUFFER, pickDepthRBO);
    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ---------------------------------------------------------------------------
// pickObject — renders all visible objects with unique flat colours,
// reads back the pixel at (mouseX, mouseY), decodes and returns object index.
// Returns -1 if no object was hit (background).
// Object indices are encoded as 1-based RGB: index 0 = background (black).
// ---------------------------------------------------------------------------
int GlWidget::pickObject(int mouseX, int mouseY)
{
    if (!pickFBO) return -1;

    // Build view matrix — same as main render
    QMatrix4x4 vMatrix;
    vMatrix.setToIdentity();
    vMatrix.lookAt(QVector3D(cameraX, cameraY, cameraZ),
                   QVector3D(centerX, centerY, centerZ),
                   QVector3D(0, 1, 0));

    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, pickFBO);
    glViewport(0, 0, xdim, ydim);

    // Clear to black (index 0 = background)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDisable(GL_BLEND);

    pickShaderProgram.bind();

    for (int i = 0; i < SVObjects.count(); i++)
    {
        if (SVObjects[i]->IsGroup || SVObjects[i]->isSurfacing) continue;
        if (!CanISee(i)) continue;

        // Encode (i+1) as RGB — 1-based so background stays black (0)
        int encoded = i + 1;
        float r = static_cast<float>( encoded        & 0xFF) / 255.0f;
        float g = static_cast<float>((encoded >>  8) & 0xFF) / 255.0f;
        float b = static_cast<float>((encoded >> 16) & 0xFF) / 255.0f;
        pickShaderProgram.setUniformValue("pickColour", QVector4D(r, g, b, 1.0f));

        QMatrix4x4 mMatrix(SVObjects[i]->matrix);
        QMatrix4x4 mvMatrix = vMatrix;
        mvMatrix.translate(0, 0, -1);
        mvMatrix *= mMatrix;
        mvMatrix *= globalMatrix;
        pickShaderProgram.setUniformValue("mvpMatrix", pMatrix * mvMatrix);

        for (int j = 0; j < SVObjects[i]->VertexBuffers.count(); j++)
        {
            SVObjects[i]->VertexBuffers[j]->bind();
            pickShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
            pickShaderProgram.enableAttributeArray("vertex");
            SVObjects[i]->VertexBuffers[j]->release();
            glfunctions->glDrawArrays(GL_TRIANGLES, 0, SVObjects[i]->VBOVertexCounts[j]);
        }
    }

    pickShaderProgram.release();

    // Read back the pixel at the click position
    // HiDPI scaling and Y flip — same as world position unproject
    int fbX = static_cast<int>(mouseX * applicationScaleX);
    int fbY = ydim - static_cast<int>(mouseY * applicationScaleY) - 1;

    // Clamp to framebuffer bounds
    fbX = qBound(0, fbX, xdim - 1);
    fbY = qBound(0, fbY, ydim - 1);

    GLubyte pixel[4] = {0, 0, 0, 0};
    glReadPixels(fbX, fbY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    glextrafunctions->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Decode RGB back to index
    int encoded = static_cast<int>(pixel[0])
                | (static_cast<int>(pixel[1]) << 8)
                | (static_cast<int>(pixel[2]) << 16);

    if (encoded == 0) return -1;  // background
    return encoded - 1;           // convert back to 0-based SVObjects index
}
