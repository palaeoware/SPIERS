
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

#include "globals.h"

/**
 * @brief GlWidget::GlWidget
 * @param parent
 */
GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // Set the widget format to the same as the global surface format
    // @see globals.h and main.cpp
    setFormat(surfaceFormat);

    // default mouse position X/Y
    LastMouseXpos = -1;
    LastMouseYpos = -1;
    setMouseTracking(true);//do nothing except pass parent on

    // Set default eye/camera position X/Y/Z
    cameraX = 0;
    cameraY = 0;
    cameraZ = 3;

    // Set default eye/camera center to look at
    centerX = 0;
    centerY = 0;
    centerZ = 0;

    // Default stereo seperation
    StereoSeparation = static_cast<float>(.04);

    // Defautl clip angle
    defaultClipAngle = mainWindow->ui->ClipAngle->value() / 10;

    setFocusPolicy(Qt::NoFocus);

    // Capture the following touch screen gestures
    QList<Qt::GestureType> gestures;
    gestures << Qt::PinchGesture;
    grabGestures(gestures);

    // Create new GL scale grid
    scaleGrid = new DrawGLScaleGrid(this);
    scaleBall = new DrawGLScaleBall(this);
}

/**
 * @brief GlWidget::~GlWidget
 */
GlWidget::~GlWidget()
{
    lightingShaderProgram.release();
    lightingShaderProgramForColour.release();
    lightingShaderProgram.deleteLater();
    lightingShaderProgramForColour.deleteLater();
}

/**
 * @brief GlWidget::initializeGL
 */
void GlWidget::initializeGL()
{
    //qDebug() << "[Where I'm I?] In initializeGL";

    // Make the current context current
    makeCurrent();

    initializeOpenGLFunctions();

    // To allow access to the context function from outside this class
    glfunctions = this->context()->functions();

#ifdef __APPLE__
    // OpenGL version 3.x+ requires the VertexArrayBuffer to be created and bound at initialization.
    // Current only needed for macOS
    m_vao.create();
    m_vao.bind();
#endif

#ifdef __APPLE__
    // We have slightly different shaders for macOS as OpenGL is version 3.3 Core
    lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingVertexShader_mac.vsh");
    lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingFragmentShader_mac.fsh");
    lightingShaderProgramForColour.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingVertexShaderTextured_mac.vsh");
    lightingShaderProgramForColour.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingFragmentShaderTextured_mac.fsh");
#endif

#ifndef __APPLE__
    lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingVertexShader.vsh");
    lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingFragmentShader.fsh");
    lightingShaderProgramForColour.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingVertexShaderTextured.vsh");
    lightingShaderProgramForColour.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingFragmentShaderTextured.fsh");
#endif

    lightingShaderProgram.link();
    lightingShaderProgramForColour.link();

    // Initalize the GL Scale grid
    scaleGrid->initializeGL();

    // Initalize the GL Scale ball
    scaleBall->initializeGL();
}

/**
 * @brief GlWidget::resizeGL
 * @param width
 * @param height
 */
void GlWidget::resizeGL(int width, int height)
{
    //qDebug() << "in rgl";
    if (height == 0)
    {
        height = 1;
    }

    // Scale all x and y by the application screen ratio
    xdim = static_cast<int>(static_cast<double>(width) * applicationScaleX);
    ydim = static_cast<int>(static_cast<double>(height) * applicationScaleY);

    DoPMatrix(xdim, ydim);

    glViewport(0, 0, xdim, ydim);

    update();
}

/**
 * @brief GlWidget::DoPMatrix
 * @param width
 * @param height
 */
void GlWidget::DoPMatrix(int width, int height)
{
    float asp = static_cast<float>(width) / static_cast<float>(height);
    float fudge = static_cast<float>(1300) / static_cast<float>(width);

    pMatrix.setToIdentity();
    if (mainWindow->ui->actionOrthographic_View->isChecked())
        pMatrix.ortho((0 - ClipAngle / (10 * fudge)), ClipAngle / (10 * fudge), (0 - ClipAngle / (10 * fudge)) / asp, ClipAngle / (10 * fudge) / asp, ClipStart, ClipDepth);
    else
        pMatrix.perspective(ClipAngle, static_cast<float>(width) / static_cast<float>(height), ClipStart, ClipDepth);
}

/**
 * @brief GlWidget::DrawObjects
 * @param rightview
 * @param halfsize
 */
void GlWidget::DrawObjects(bool rightview, bool halfsize)
{
    //qDebug() << "[Where I'm I?] In DrawObjects";

    Q_UNUSED(halfsize)

    QMatrix4x4 vMatrix; //view matrix
    vMatrix.setToIdentity();

    QVector3D camera = QVector3D(cameraX, cameraY, cameraZ);
    QVector3D rightcameraPosition = QVector3D(static_cast<float>(StereoSeparation) * static_cast<float>(STEREO_SEPARATION_MODIFIER) / static_cast<float>(cameraZ), 0, cameraZ);
    QVector3D cameraUpDirection = QVector3D(0, 1, 0);
    QVector3D center = QVector3D(centerX, centerY, centerZ);

    if (rightview)
        vMatrix.lookAt(rightcameraPosition, center, cameraUpDirection);
    else
        vMatrix.lookAt(camera, center, cameraUpDirection);

    QOpenGLShaderProgram *useshader;

    glDepthMask(true);


    for (int trans = 0; trans < 2; trans++) //two runs - opaques first
    {
        if (trans == 1)
        {
            glfunctions->glDepthMask(false);    //set up transparency run
            glfunctions->glEnable(GL_BLEND);
        }
        else
        {
            glfunctions->glDepthMask(true);
            glfunctions->glDisable(GL_BLEND);
        }
        for (int i = 0; i < SVObjects.count(); i++)
        {

            // We must have a file load by this point so set the global to true.
            isFileLoaded = true;

            if (!(SVObjects[i]->IsGroup))
                if (CanISee(i))
                {
                    QMatrix4x4 mMatrix(SVObjects[i]->matrix); //model view matrix - transform the objects

                    QMatrix4x4 mvMatrix;
                    mvMatrix = vMatrix;

                    mvMatrix.translate(0, 0, -1); //for backward compatibility
                    mvMatrix *= mMatrix;
                    mvMatrix *= globalMatrix;

                    QMatrix3x3 normalMatrix;
                    normalMatrix = mvMatrix.normalMatrix();

                    if ((SVObjects[i]->Transparency == 0 && trans == 0) || (SVObjects[i]->Transparency != 0 && trans == 1)) //do trans on second run
                    {
                        if (SVObjects[i]->colour)
                            useshader = &lightingShaderProgramForColour;
                        else
                            useshader = &lightingShaderProgram;

                        useshader->bind();
                        useshader->setUniformValue("mvpMatrix", pMatrix * mvMatrix);
                        useshader->setUniformValue("mvMatrix", mvMatrix);
                        useshader->setUniformValue("normalMatrix", normalMatrix);
                        useshader->setUniformValue("lightPosition", vMatrix * camera);

                        float mcolor[3];
                        if (mainWindow->ui->actionMute_Colours->isChecked())
                        {
                            mcolor[0] = (static_cast<GLfloat>(SVObjects[i]->Colour[0] / 3 + 170));
                            mcolor[1] = (static_cast<GLfloat>(SVObjects[i]->Colour[1] / 3 + 170));
                            mcolor[2] = (static_cast<GLfloat>(SVObjects[i]->Colour[2] / 3 + 170));
                        }
                        else
                        {
                            mcolor[0] = (static_cast<GLfloat>(SVObjects[i]->Colour[0]));
                            mcolor[1] = (static_cast<GLfloat>(SVObjects[i]->Colour[1]));
                            mcolor[2] = (static_cast<GLfloat>(SVObjects[i]->Colour[2]));
                        }

                        if (mainWindow->ui->actionBounding_Box->isChecked())
                        {
                            if (SVObjects[i]->boundingBoxBuffer.isCreated())
                            {
                                //qDebug()<<"Exists, drawing";
                                useshader->setUniformValue("ambientColor", QColor(static_cast<int>(mcolor[0]), static_cast<int>(mcolor[1]), static_cast<int>(mcolor[2])));
                                useshader->setUniformValue("diffuseColor", QColor(0.0, 0.0, 0.0));
                                useshader->setUniformValue("specularColor", QColor(0.0, 0.0, 0.0));
                                useshader->setUniformValue("alpha", static_cast<GLfloat>(1.0));
                                useshader->setUniformValue("ambientReflection", static_cast<GLfloat>(1.0));
                                useshader->setUniformValue("diffuseReflection", static_cast<GLfloat>(1.0));
                                useshader->setUniformValue("specularReflection", static_cast<GLfloat>(0.0));
                                useshader->setUniformValue("shininess", static_cast<GLfloat>(100.0));
                                SVObjects[i]->boundingBoxBuffer.bind();
                                useshader->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
                                useshader->enableAttributeArray("vertex");
                                useshader->setAttributeBuffer("normal", GL_FLOAT, 12 * 6 * sizeof(GLfloat), 3, 0);
                                useshader->enableAttributeArray("normal");

                                SVObjects[i]->boundingBoxBuffer.release();
                                glfunctions->glDrawArrays(GL_LINES, 0, 24);
                            }
                            //else //qDebug()<<"Not created";
                        }
                        else
                        {

                            GLfloat shininess = 0.0;
                            if (SVObjects[i]->Shininess == 3) shininess = static_cast<float>(1.0);
                            if (SVObjects[i]->Shininess == 2) shininess = static_cast<float>(.7);
                            if (SVObjects[i]->Shininess == 1) shininess = static_cast<float>(.3);
                            if (SVObjects[i]->Shininess == 0) shininess = static_cast<float>(0.0);
                            if (SVObjects[i]->Shininess < 0) shininess = (static_cast<GLfloat>(0 - SVObjects[i]->Shininess)) / static_cast<float>(100.0);

                            useshader->setUniformValue("ambientReflection", static_cast<GLfloat>(1.0));
                            useshader->setUniformValue("diffuseReflection", static_cast<GLfloat>(1.0));
                            useshader->setUniformValue("specularReflection", shininess);
                            useshader->setUniformValue("shininess", static_cast<GLfloat>(200.0 - pow(static_cast<double>(180.0), shininess)));

                            //calculate alpha
                            float t = 1.0;
                            if (SVObjects[i]->Transparency == 4) t = static_cast<float>(.25);
                            if (SVObjects[i]->Transparency == 3) t = static_cast<float>(.45);
                            if (SVObjects[i]->Transparency == 2) t = static_cast<float>(.6);
                            if (SVObjects[i]->Transparency == 1) t = static_cast<float>(.8);
                            if (SVObjects[i]->Transparency < 0) t = (static_cast<float>(100 + SVObjects[i]->Transparency)) / static_cast<float>(100.0);
                            useshader->setUniformValue("alpha", t);

                            if (!(SVObjects[i]->colour))
                            {
                                useshader->setUniformValue("ambientColor", QColor(
                                                               static_cast<int>(mcolor[0]) / 5,
                                                               static_cast<int>(mcolor[1]) / 5,
                                                               static_cast<int>(mcolor[2]) / 5
                                                           ));
                                useshader->setUniformValue("diffuseColor", QColor(
                                                               static_cast<int>(mcolor[0] / static_cast<float>(1.5)),
                                                               static_cast<int>(mcolor[1] / static_cast<float>(1.5)),
                                                               static_cast<int>(mcolor[2] / static_cast<float>(1.5))
                                                           ));
                                useshader->setUniformValue("specularColor", QColor(
                                                               static_cast<int>(mcolor[0]),
                                                               static_cast<int>(mcolor[1]),
                                                               static_cast<int>(mcolor[2])
                                                           ));
                            }

                            for (int j = 0; j < SVObjects[i]->VertexBuffers.count(); j++)
                            {

                                SVObjects[i]->VertexBuffers[j]->bind();
                                useshader->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
                                useshader->enableAttributeArray("vertex");
                                useshader->setAttributeBuffer("normal", GL_FLOAT, 3 * static_cast<int>(SVObjects[i]->VBOVertexCounts[j])*static_cast<int>(sizeof(GLfloat)), 3, 0);
                                useshader->enableAttributeArray("normal");
                                SVObjects[i]->VertexBuffers[j]->release();

                                if (SVObjects[i]->colour)
                                {
                                    SVObjects[i]->ColourBuffers[j]->bind();
                                    useshader->setAttributeBuffer("colour", GL_FLOAT, 0, 3, 0);
                                    useshader->enableAttributeArray("colour");
                                    SVObjects[i]->ColourBuffers[j]->release();
                                }
                                glfunctions->glDrawArrays(GL_TRIANGLES, 0, SVObjects[i]->VBOVertexCounts[j]);

                            }
                        }
                        useshader->release();
                    }
                }
        }
    }


    glDepthMask(true);

    // Update the stored FOV
    updateFOV();

    //glDebug("Line 358");

    // Show/Hide GL Scale Ball
    scaleBall->draw(vMatrix, vMatrix * camera);

    // Show/Hide GL Scale Grid
    if (mainWindow->ui->actionShow_Scale_Grid->isChecked())
        scaleGrid->draw(vMatrix, vMatrix * camera);

    //glDebug("Line 367");
}

/**
 * @brief GlWidget::paintGL
 */
void GlWidget::paintGL()
{
    //qDebug() << "[Where I'm I?] In paintGL";

    glClearColor(
        static_cast<float>(colorBackgroundRed) / static_cast<float>(255),
        static_cast<float>(colorBackgroundGreen) / static_cast<float>(255),
        static_cast<float>(colorBackgroundBlue) / static_cast<float>(255),
        0.5f
    );

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
        //glfunctions->glDrawBuffer(GL_BACK_LEFT); //TODO - WTF DO I USE HERE?
        DrawObjects(false, false);
        //glfunctions->glDrawBuffer(GL_BACK_LEFT); //TODO - WTF DO I USE HERE?
        glfunctions->glClear(GL_DEPTH_BUFFER_BIT);
        DrawObjects(true, false);
        return;
    }

    //default - normal draw
    DoPMatrix(xdim, ydim);
    DrawObjects(false, false);


}

//Functions from old widget
/**
 * @brief GlWidget::CanISee
 * @param index
 * @return
 */
bool GlWidget::CanISee(int index)
{
    //if not in a group - just return my visibility
    if (SVObjects[index]->Visible == false) return false;
    if (SVObjects[index]->InGroup == -1) return true; //not in a group,visble, fine

    //in a group and visible - return visibility of parents
    return CanISee(SVObjects[index]->Parent()); //run again on my parents
}

/**
 * @brief GlWidget::SetStereoSeparation
 * @param s
 */
void GlWidget::SetStereoSeparation(float s)
{
    StereoSeparation = s;
}

/**
 * @brief GlWidget::SetClip
 * @param Start
 * @param Depth
 * @param Angle
 */
void GlWidget::SetClip(int Start, int Depth, int Angle)
{
    //qDebug() << "In SetClip ";
    ClipStart = static_cast<float>(Start + 1) / 50;
    ClipDepth = static_cast<float>(ClipStart) + static_cast<float>(0.005) + (static_cast<float>(Depth)) / static_cast<float>(100);
    ClipAngle = static_cast<float>(Angle) / 10;
    update();
}

/**
 * @brief GlWidget::Rotate
 * @param angle
 */
void GlWidget::Rotate(double angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 rotmatrix;
            rotmatrix.setToIdentity();
            rotmatrix.rotate(static_cast<float>(angle), 0.0f, 0.0f, 1.0f);
            if (i == 0) rotationZ += angle;

            QMatrix4x4 svmatrix(SVObjects[i]->matrix);
            if (!sp2Lock)
            {
                rotmatrix *= svmatrix;
                rotmatrix.copyDataTo(SVObjects[i]->matrix);
                isFileDirty = true;
            }
        }
    }
}

/**
 * @brief GlWidget::mouseMoveEvent
 * @param event
 */
void GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "In Mouse Move Event";

    // Make sure the mouse events only come from a real mouse and not something like a touchscreen
    /*if ((event != nullptr) && (event->source() == Qt::MouseEventSource::MouseEventSynthesizedBySystem))
    {
        //qDebug() << "Mouse Event from = " << event->source();
        event->ignore();
        return;
    }*/

    // If we have got this far we know this is a real mouse event...
    bool donesomething = false;
    bool rotmode = false;

    if (
        event->buttons() & Qt::RightButton
        || ((event->buttons() & Qt::LeftButton) && event->modifiers() == Qt::CTRL)
        || ((event->buttons() & Qt::LeftButton) && mainWindow->ui->actionRotate_Lock->isChecked())
    )
    {
        rotmode = true;
    }
    //If shift held - rotate round center
    if ((rotmode && event->modifiers() == Qt::SHIFT)  || (event->modifiers() == Qt::ALT && event->buttons() & Qt::LeftButton) )
    {
        //rotate amount is angular distance between prev and now
        double before = atan2(static_cast<double>(LastMouseXpos - xdim / 2), static_cast<double>(LastMouseYpos - ydim / 2));
        double after = atan2(static_cast<double>(event->x() - xdim / 2), static_cast<double>(event->y() - ydim / 2));
        double amount;
        //handle wrapping
        double amount1 = after - before;
        double amount2 = after + 2 * 3.14159 - before;
        if (abs(amount2) < abs(amount1)) amount = amount2;
        else amount = amount1;
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
            if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
            if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
            if (f)
            {

                QMatrix4x4 rotmatrix;
                rotmatrix.setToIdentity();
                rotmatrix.rotate(xangle, 1.0f, 0.0f, 0.0f);
                rotmatrix.rotate(yangle, 0.0f, 1.0f, 0.0f);

                if (i == 0)
                {
                    rotationX += xangle;    //WTF does this do?
                    rotationY += yangle;
                }
                QMatrix4x4 svmatrix(SVObjects[i]->matrix);
                if (!sp2Lock)
                {
                    rotmatrix *= svmatrix;
                    rotmatrix.copyDataTo(SVObjects[i]->matrix);
                    donesomething = true;
                    isFileDirty = true;
                }

            }
        }
    }

    if (!rotmode && event->buttons() & Qt::LeftButton && donesomething == false)
    {
        float ObjXpos = (static_cast<float>(event->x() - LastMouseXpos)) / (SENSITIVITY * 100);
        float ObjYpos = 0 - (static_cast<float>(event->y() - LastMouseYpos)) / (SENSITIVITY * 100);

        for (int i = 0; i < SVObjects.count(); i++)
        {
            bool f = false;
            if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
            if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
            if (f)
            {
                {
                    QMatrix4x4 rotmatrix;
                    rotmatrix.setToIdentity();
                    rotmatrix.translate(ObjXpos, ObjYpos, 0.0f);
                    if (i == 0)
                    {
                        transformX += ObjXpos;    //WTF does this do?
                        transformY += ObjYpos;
                    }
                    QMatrix4x4 svmatrix(SVObjects[i]->matrix);
                    if (!sp2Lock)
                    {
                        rotmatrix *= svmatrix;
                        rotmatrix.copyDataTo(SVObjects[i]->matrix);
                        donesomething = true;
                        isFileDirty = true;
                    }

                }
            }
        }
    }
    LastMouseXpos = event->x();
    LastMouseYpos = event->y();

    if (donesomething) update();
}

/**
 * @brief GlWidget::moveCameraZ
 * This function add z to the current cameraZ value.
 * @param z
 */
void GlWidget::moveCameraZ(double value)
{
    cameraZ += value;
    update();
}

/**
 * @brief GlWidget::ZRotate
 * @param angle
 */
void GlWidget::ZRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 rotmatrix;
            rotmatrix.setToIdentity();
            rotmatrix.rotate(angle, 0.0f, 0.0f, 1.0f);
            if (i == 0) rotationZ += angle;

            QMatrix4x4 svmatrix(SVObjects[i]->matrix);
            if (!sp2Lock)
            {
                rotmatrix *= svmatrix;
                rotmatrix.copyDataTo(SVObjects[i]->matrix);
                isFileDirty = true;
            }
        }
    }
}

/**
 * @brief GlWidget::YRotate
 * @param angle
 */
void GlWidget::YRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 rotmatrix;
            rotmatrix.setToIdentity();
            rotmatrix.rotate(angle, 0.0f, 1.0f, 0.0f);
            if (i == 0) rotationY += angle;

            QMatrix4x4 svmatrix(SVObjects[i]->matrix);
            if (!sp2Lock)
            {
                rotmatrix *= svmatrix;
                rotmatrix.copyDataTo(SVObjects[i]->matrix);
                isFileDirty = true;
            }
        }
    }
}

/**
 * @brief GlWidget::XRotate
 * @param angle
 */
void GlWidget::XRotate(float angle)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 rotmatrix;
            rotmatrix.setToIdentity();
            rotmatrix.rotate(angle, 1.0f, 0.0f, 0.0f);
            if (i == 0) rotationX += angle;

            QMatrix4x4 svmatrix(SVObjects[i]->matrix);
            if (!sp2Lock)
            {
                rotmatrix *= svmatrix;
                rotmatrix.copyDataTo(SVObjects[i]->matrix);
                isFileDirty = true;
            }
        }
    }
}

/**
 * @brief GlWidget::Translate
 * @param x
 * @param y
 * @param z
 */
void GlWidget::Translate(float x, float y, float z)
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 rotmatrix;
            rotmatrix.setToIdentity();
            rotmatrix.translate(static_cast<float>(x) / static_cast<float>(10.0), static_cast<float>(y) / static_cast<float>(10.0), static_cast<float>(z) / static_cast<float>(10.0));
            if (i == 0)
            {
                transformX += x;
                transformY += y;
                transformZ += z;
            }

            QMatrix4x4 svmatrix(SVObjects[i]->matrix);
            if (!sp2Lock)
            {
                rotmatrix *= svmatrix;
                rotmatrix.copyDataTo(SVObjects[i]->matrix);
                isFileDirty = true;
            }
        }
    }
}

/**
 * @brief GlWidget::Resize
 * @param value
 */
void GlWidget::Resize(float value)
{
    //qDebug() << "In resize ";
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            QMatrix4x4 rotmatrix;
            rotmatrix.setToIdentity();
            rotmatrix.scale(value, value, value);
            SVObjects[i]->scale *= static_cast<double>(value);

            QMatrix4x4 svmatrix(SVObjects[i]->matrix);
            if (!sp2Lock)
            {
                rotmatrix *= svmatrix;
                rotmatrix.copyDataTo(SVObjects[i]->matrix);
                isFileDirty = true;
            }
        }
    }
}

/**
 * @brief GlWidget::ResetSize
 */
void GlWidget::ResetSize()
{
    for (int i = 0; i < SVObjects.count(); i++)
    {
        bool f = false;
        if (mainWindow->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f = true;
        if (!mainWindow->ui->actionReposition_Selected->isChecked()) f = true;
        if (f)
        {
            {
                QMatrix4x4 rotmatrix;
                rotmatrix.setToIdentity();
                rotmatrix.scale(static_cast<float>(1.0) / static_cast<float>(SVObjects[i]->scale), static_cast<float>(1.0) / static_cast<float>(SVObjects[i]->scale),
                                static_cast<float>(1.0) / static_cast<float>(SVObjects[i]->scale));

                QMatrix4x4 svmatrix(SVObjects[i]->matrix);
                if (!sp2Lock)
                {
                    rotmatrix *= svmatrix;
                    rotmatrix.copyDataTo(SVObjects[i]->matrix);
                }
                SVObjects[i]->scale = 1.0;
                isFileDirty = true;
            }
        }
    }
}

/**
 * @brief GlWidget::ResetToDefault
 */
void GlWidget::ResetToDefault()
{
    //qDebug() << "In Reset to Default";
    for (int j = 0; j < SVObjects.count(); j++)
    {
        if (SVObjects[j]->gotdefaultmatrix)
            for (int i = 0; i < 16; i++) SVObjects[j]->matrix[i] = SVObjects[j]->defaultmatrix[i];
    }

    ClipAngle = defaultClipAngle;
    rotationX = 0;
    rotationY = 0;
    rotationZ = 0;
    transformX = 0;
    transformY = 0;
    transformZ = 0;
    mainWindow->ui->ClipAngle->setValue(static_cast<int>(ClipAngle) * 10); //should trigger a refresh? not if no change!
}

/**
 * @brief GlWidget::NewDefault
 */
void GlWidget::NewDefault()
{
    //qDebug() << "In New Default";
    if (SVObjects.count() > 0) //if need to record default and not some spurious early call with no data
    {
        for (int j = 0; j < SVObjects.count(); j++)
        {
            for (int i = 0; i < 16; i++)
            {
                SVObjects[j]->defaultmatrix[i] = SVObjects[j]->matrix[i];
                SVObjects[j]->gotdefaultmatrix = true;
            }
        }
        defaultClipAngle = ClipAngle; //yes, doing for each one, for convenience
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
        transformX = 0;
        transformY = 0;
        transformZ = 0;
    }
}

/**
 * @brief GlWidget::event
 * @param event
 * @return
 */
bool GlWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent *>(event));
    return QWidget::event(event);
}

/**
 * @brief GlWidget::grabGestures
 * @param gestures
 */
void GlWidget::grabGestures(const QList<Qt::GestureType> &gestures)
{
    foreach (Qt::GestureType gesture, gestures)
        grabGesture(gesture);
}

/**
 * @brief GlWidget::gestureEvent
 * @param event
 * @return
 */
bool GlWidget::gestureEvent(QGestureEvent *event)
{
    //qDebug() << "gestureEvent():" << event;

    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    {
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }
    return true;
}

/**
 * @brief GlWidget::pinchTriggered
 * @param gesture
 */
void GlWidget::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    double currentStepScaleFactor = 0;

    if (changeFlags & QPinchGesture::RotationAngleChanged)
    {
        double rotationDelta = gesture->lastRotationAngle() - gesture->rotationAngle();
        Rotate(rotationDelta);
        update();

        //qDebug() << "pinchTriggered(): rotate by" << rotationDelta;
    }

    if (changeFlags & QPinchGesture::ScaleFactorChanged)
    {
        currentStepScaleFactor = gesture->totalScaleFactor();
        int currentClipAngle = mainWindow->ui->ClipAngle->value();
        int maximumClipAngleAllowed = mainWindow->ui->ClipAngle->maximum();
        int minimumClipAngleAllowed = mainWindow->ui->ClipAngle->minimum();
        int newClipAngle = currentClipAngle;

        if (currentStepScaleFactor > 0.0)
        {
            // Zoom in
            newClipAngle = static_cast<int>(static_cast<double>(currentClipAngle) * (1 + ((currentStepScaleFactor - 1) / 20)));
            if (newClipAngle > maximumClipAngleAllowed) newClipAngle = maximumClipAngleAllowed;
            mainWindow->ui->ClipAngle->setValue(newClipAngle);
        }
        else if (currentStepScaleFactor < 0.0)
        {
            // Zoom Out
            newClipAngle = static_cast<int>(static_cast<double>(currentClipAngle) * (currentStepScaleFactor / 20));
            if (newClipAngle < minimumClipAngleAllowed) newClipAngle = minimumClipAngleAllowed;
            mainWindow->ui->ClipAngle->setValue(newClipAngle);
        }
        SetClip(mainWindow->ui->ClipStart->value(), mainWindow->ui->ClipDepth->value(), mainWindow->ui->ClipAngle->value());
        update();

        //qDebug() << "pinchTriggered(): zoom by" << gesture->scaleFactor() << "->" << currentStepScaleFactor << " Current Clip = " << currentClipAngle << " New Clip = " << newClipAngle;
    }

    if (gesture->state() == Qt::GestureFinished)
    {
        update();
    }

}

/**
 * @brief GlWidget::getFOV
 * Return field of view in mm
 * @return double
 */
double GlWidget::getFOV()
{
    return currentFOV;
}

/**
 * @brief GlWidget::updateFOV
 * Work out x field of view in mm
 */
void GlWidget::updateFOV()
{
    float scale = static_cast<float>(1.0) / static_cast<float>(mmPerUnit);
    float divider = (this->height() * globalRescale) / static_cast<float>(30.0);

    currentFOV = static_cast<double>(ClipAngle / (divider * scale));
}

void GlWidget::glDebug(QString string = QString())
{
    GLenum err;
    QString errMess;
    while((err = glGetError()) != GL_NO_ERROR) {
        // Process errors
        switch(err)
        {
        case GL_INVALID_ENUM:
            errMess = "Invalid Enum";
            break;
        case GL_INVALID_VALUE:
            errMess = "Invalid Value";
            break;
        case GL_INVALID_OPERATION:
            errMess = "Invalid Operation";
            break;
        case GL_STACK_OVERFLOW:
            errMess = "Stack Overflow";
            break;
        case GL_STACK_UNDERFLOW:
            errMess = "Stack Underflow";
            break;
        case GL_OUT_OF_MEMORY:
            errMess = "Out of Menory";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errMess = "Invalid Frame Buffer Opperation";
            break;
        case GL_TABLE_TOO_LARGE:
            errMess = "Table too large";
            break;
        }

        qDebug() << QString("[GL Error] %1 - %2").arg(errMess).arg(string);
    }
}
