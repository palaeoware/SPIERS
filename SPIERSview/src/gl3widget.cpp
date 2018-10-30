
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

#include "rowmans.h"
#include "globals.h"

/**
 * @brief GlWidget::GlWidget
 * @param parent
 */
GlWidget::GlWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
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

    // Set GL surface format
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setMajorVersion(GL_MAJOR);
    surfaceFormat.setMinorVersion(GL_MINOR);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    setFormat(surfaceFormat);
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

    glfunctions = this->context()->functions();
    glfunctions->glEnable(GL_DEPTH_TEST);
    glfunctions->glEnable(GL_BLEND);
    glfunctions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingVertexShader.vsh");
    lightingShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingFragmentShader.fsh");
    lightingShaderProgram.link();

    lightingShaderProgramForColour.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingVertexShaderTextured.vsh");
    lightingShaderProgramForColour.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingFragmentShaderTextured.fsh");
    lightingShaderProgramForColour.link();

    //set up line
    QVector<QVector3D> lineVertices;

    lineVertices << QVector3D(0, -10000000.0,  0.0) << QVector3D( 0, 10000000.0,  0.0);
    lineVertices << QVector3D(-10000000, 0.0,  0.0) << QVector3D(10000000, 0.0,  0.0);  //second two are horizontal line

    VBOline.create();
    VBOline.bind();
    VBOline.allocate(12 * sizeof(GLfloat));
    VBOline.write(0, lineVertices.constData(), 12 * sizeof(GLfloat));
    VBOline.release();

    //set up fonts
    //0-9 are digits 0-9, 10 is -, 11 is ., 12 is m

    //digits
    for (int charnumber = 16; charnumber < 26; charnumber++)
    {
        QVector<QVector3D> fontVertices;
        for (int i = 0; i < rowmans_size[charnumber] / 4; i++)
        {
            fontVertices << QVector3D(
                             (static_cast<float>(rowmans[charnumber][i * 4])) / static_cast<float>(FONT_SCALE_FACTOR),
                             (static_cast<float>(rowmans[charnumber][i * 4 + 1])) / static_cast<float>(FONT_SCALE_FACTOR),
                             0.0)
                         << QVector3D(
                             (static_cast<float>(rowmans[charnumber][i * 4 + 2])) / static_cast<float>(FONT_SCALE_FACTOR),
                             (static_cast<float>(rowmans[charnumber][i * 4 + 3])) / static_cast<float>(FONT_SCALE_FACTOR),
                             0.0);
        }

        VBOcharacters[charnumber - 16].create();
        VBOcharacters[charnumber - 16].bind();
        VBOcharacters[charnumber - 16].allocate(6 * static_cast<int>(sizeof(GLfloat)) * (rowmans_size[charnumber] / 4));
        VBOcharacters[charnumber - 16].write(0, fontVertices.constData(), 6 * static_cast<int>(sizeof(GLfloat)) * (rowmans_size[charnumber] / 4));
        VBOcharacters[charnumber - 16].release();
        CharacterWidths[charnumber - 16] = rowmans_width[charnumber];
        CharacterLineCounts[charnumber - 16] = rowmans_size[charnumber] / 2;
    }

    for (int ii = 10; ii < 13; ii++)
    {
        int charnumber = 0;
        if (ii == 10) charnumber = 14;
        if (ii == 11) charnumber = 13;
        if (ii == 12) charnumber = 109 - 32; //ascii code - 32

        QVector<QVector3D> fontVertices;
        for (int i = 0; i < rowmans_size[charnumber] / 4; i++)
        {
            fontVertices << QVector3D((static_cast<float>(rowmans[charnumber][i * 4])) / static_cast<float>(FONT_SCALE_FACTOR),
                                      (static_cast<float>(rowmans[charnumber][i * 4 + 1])) / static_cast<float>(FONT_SCALE_FACTOR), 0.0)
                         << QVector3D((static_cast<float>(rowmans[charnumber][i * 4 + 2])) / static_cast<float>(FONT_SCALE_FACTOR),
                                      (static_cast<float>(rowmans[charnumber][i * 4 + 3])) / static_cast<float>(FONT_SCALE_FACTOR), 0.0);
        }

        VBOcharacters[ii].create();
        VBOcharacters[ii].bind();
        VBOcharacters[ii].allocate(6 * static_cast<int>(sizeof(GLfloat)) * (rowmans_size[charnumber] / 4));
        VBOcharacters[ii].write(0, fontVertices.constData(), 6 * static_cast<int>(sizeof(GLfloat)) * (rowmans_size[charnumber] / 4));
        VBOcharacters[ii].release();
        CharacterWidths[ii] = rowmans_width[charnumber];
        CharacterLineCounts[ii] = rowmans_size[charnumber] / 2;
    }

    //now the occlusion object for the scale grid
    QVector<QVector3D> occVertices;
    QVector<QVector3D> occNormals;

    occVertices << QVector3D(-500000, -50000,  static_cast<float>(-1.001))
                << QVector3D( 500000, -500000,  static_cast<float>(-1.001))
                << QVector3D( 500000,  500000,  static_cast<float>(-1.001)) // Front
                << QVector3D( 500000,  50000,  static_cast<float>(-1.001))
                << QVector3D(-500000,  500000,  static_cast<float>(-1.001))
                << QVector3D(-500000, -500000,  static_cast<float>(-1.001));

    occNormals << QVector3D( 0,  0,  1)
               << QVector3D( 0,  0,  1)
               << QVector3D( 0,  0,  1) // Front
               << QVector3D( 0,  0,  1)
               << QVector3D( 0,  0,  1)
               << QVector3D( 0,  0,  1);

    numOccVertices = 6;

    occBuffer.create();
    occBuffer.bind();
    occBuffer.allocate(numOccVertices * (3 + 3 + 2) * static_cast<int>(sizeof(GLfloat)));

    int offset = 0;
    occBuffer.write(offset, occVertices.constData(), numOccVertices * 3 * static_cast<int>(sizeof(GLfloat)));
    offset += numOccVertices * 3 * static_cast<int>(sizeof(GLfloat));
    occBuffer.write(offset, occNormals.constData(), numOccVertices * 3 * static_cast<int>(sizeof(GLfloat)));

    occBuffer.release();
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

    glfunctions->glViewport(0, 0, xdim, ydim);

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
 * @brief GlWidget::DrawLine
 * @param vMatrix
 * @param lPosition
 * @param pos
 * @param major
 * @param horizontal
 */
void GlWidget::DrawLine(QMatrix4x4 vMatrix, QVector3D lPosition, float pos, bool major, bool horizontal)
{
    //qDebug() << "[Where I'm I?] In DrawLine";

    Q_UNUSED(lPosition)

    //vMatrix.translate(pos,0,-1);
    if (!horizontal) vMatrix.translate(pos, 0, -1);
    else vMatrix.translate(0, pos, -1);
    vMatrix *= globalMatrix;

    lightingShaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix);
    lightingShaderProgram.setUniformValue("mvMatrix", vMatrix);
    lightingShaderProgram.setUniformValue("normalMatrix", vMatrix.normalMatrix());

    if (major)
        lightingShaderProgram.setUniformValue("ambientColor", QColor(colorGridRed, colorGridGreen, colorGridBlue));
    else
        lightingShaderProgram.setUniformValue("ambientColor", QColor(colorGridMinorRed, colorGridMinorGreen, colorGridMinorBlue));
    lightingShaderProgram.setUniformValue("diffuseColor", QColor(0.0, 0.0, 0.0));
    lightingShaderProgram.setUniformValue("specularColor", QColor(0.0, 0.0, 0.0));

    VBOline.bind();
    if (!horizontal) lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
    else lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 6 * sizeof(GLfloat), 3, 0);

    lightingShaderProgram.enableAttributeArray("vertex");
    VBOline.release();

    glfunctions->glDrawArrays(GL_LINES, 0, 2);
}

/**
 * @brief GlWidget::RenderCharacter
 * @param x
 * @param y
 * @param z
 * @param charactercode
 * @param vMatrix
 * @param Colour
 */
void GlWidget::RenderCharacter(GLfloat x, GLfloat y, GLfloat z, int charactercode, QMatrix4x4 vMatrix, QColor Colour)
{
    //qDebug() << "[Where I'm I?] In RenderCharacter";

    vMatrix.translate(x, y, z);
    vMatrix.rotate(180.0, 1.0, 0.0, 0.0);
    vMatrix.scale((ClipAngle / static_cast<float>(this->height())));

    //qDebug() << ClipAngle;

    lightingShaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix);
    lightingShaderProgram.setUniformValue("mvMatrix", vMatrix);
    lightingShaderProgram.setUniformValue("normalMatrix", vMatrix.normalMatrix());
    lightingShaderProgram.setUniformValue("ambientColor", Colour);
    lightingShaderProgram.setUniformValue("diffuseColor", QColor(0.0, 0.0, 0.0));
    lightingShaderProgram.setUniformValue("specularColor", QColor(0.0, 0.0, 0.0));

    VBOcharacters[charactercode].bind();
    lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
    lightingShaderProgram.enableAttributeArray("vertex");
    VBOcharacters[charactercode].release();

    glfunctions->glDrawArrays(GL_LINES, 0, static_cast<int>(CharacterLineCounts[charactercode]));
}

/**
 * @brief GlWidget::RenderNumber
 * mm true if size in mm - else render m
 * @param x
 * @param y
 * @param z
 * @param number
 * @param decimalplaces
 * @param mm
 * @param major
 * @param vMatrix
 */
void GlWidget::RenderNumber(GLfloat x, GLfloat y, GLfloat z, float number, int decimalplaces, bool mm, bool major, QMatrix4x4 vMatrix )
{
    //qDebug() << "[Where I'm I?] In RenderNumber";

    QColor numcolour;
    if (major)
        numcolour = QColor(colorGridRed, colorGridGreen, colorGridBlue);
    else
        numcolour = QColor(colorGridMinorRed, colorGridMinorGreen, colorGridMinorBlue);

    //construct number as list of characters
    QList<int>characters;

    if (number < 0)
    {
        characters.append(11); //add code for minus sign
        number = qAbs(number);
    }

    //add int part
    QString inumstring;
    inumstring.sprintf("%d", static_cast<int>(number));
    for (int i = 0; i < inumstring.length(); i++)
        characters.append(inumstring.toLatin1().at(i) - '0');

    if (decimalplaces > 0)
    {
        characters.append(10); // a dot

        number -= static_cast<float>((static_cast<int>(number))); //get decimal part
        number *= pow(10.0, static_cast<float>((decimalplaces)));

        inumstring.sprintf("%d", static_cast<int>(static_cast<double>(number) + .5));
        for (int i = 0; i < inumstring.length(); i++)
            characters.append(inumstring.toLatin1().at(i) - '0');
    }

    //finally the units
    characters.append(12); //for m
    if (mm) characters.append(12); //for mm

    GLfloat off = 0; //offset
    //now display them
    for (int i = 0; i < characters.length(); i++)
    {
        RenderCharacter(x + off, y, z, characters[i], vMatrix, numcolour);
        off += static_cast<GLfloat>(CharacterWidths[characters[i]] * ((static_cast<float>(0.9 / FONT_SCALE_FACTOR) * ClipAngle / static_cast<float>(this->height()))));
    }

}

/**
 * @brief GlWidget::DrawScaleGrid
 * @param vMatrix
 * @param lPosition
 */
void GlWidget::DrawScaleGrid(QMatrix4x4 vMatrix, QVector3D lPosition)
{
    //qDebug() << "[Where I'm I?] In DrawScaleGrid";

    lightingShaderProgram.bind();
    lightingShaderProgram.setUniformValue("lightPosition", lPosition);
    lightingShaderProgram.setUniformValue("ambientReflection", static_cast<GLfloat>(1.0));
    lightingShaderProgram.setUniformValue("diffuseReflection", static_cast<GLfloat>(1.0));
    lightingShaderProgram.setUniformValue("specularReflection", static_cast<GLfloat>(1.0));
    lightingShaderProgram.setUniformValue("shininess", static_cast<GLfloat>(1000.0));

    //draw occlusion thing
    lightingShaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix);
    lightingShaderProgram.setUniformValue("mvMatrix", vMatrix);
    lightingShaderProgram.setUniformValue("normalMatrix", vMatrix.normalMatrix());
    lightingShaderProgram.setUniformValue("ambientColor", QColor(colorBackgroundRed, colorBackgroundGreen, colorBackgroundBlue));
    lightingShaderProgram.setUniformValue("diffuseColor", QColor(0, 0.0, 0.0));
    lightingShaderProgram.setUniformValue("specularColor", QColor(0, 0.0, 0.0));
    lightingShaderProgram.setUniformValue("alpha", static_cast<GLfloat>(0.6));

    occBuffer.bind();
    lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
    lightingShaderProgram.enableAttributeArray("vertex");
    lightingShaderProgram.setAttributeBuffer("normal", GL_FLOAT, 3 * numOccVertices * static_cast<int>(sizeof(GLfloat)), 3, 0);
    lightingShaderProgram.enableAttributeArray("normal");
    occBuffer.release();

    glfunctions->glDrawArrays(GL_TRIANGLES, 0, numOccVertices);

    lightingShaderProgram.setUniformValue("alpha", static_cast<GLfloat>(1.0));
    glfunctions->glClear(GL_DEPTH_BUFFER_BIT);

    float s = static_cast<float>(1.0) / static_cast<float>(mmPerUnit);

    //work out field of view (max of height and width)
    float divider = (this->height() * globalRescale) / static_cast<float>(30.0);

    //work out x field of view in mm
    double fov;
    fov = static_cast<double>(ClipAngle / (divider * s));
    //OK, work out correct scale in mm
    //Look at fov and find coarse level to use
    //fov 5mm - want coarse 1, fine 0.1
    //fov 50mm - want coarse 10, fine 1
    //fov .5mm - want coarse .1, fine .01 etc
    //work out coarse below

    float coarse = static_cast<float>(pow(static_cast<double>(10.0), (static_cast<double>(static_cast<int>(log10(fov) + .7))))); //double cast is to round it
    float fine = coarse / 10;

    //qDebug()<<"Coarse"<<coarse<<"Fine"<<fine;

    for (int i = -10; i < 10; i++) DrawLine(vMatrix, lPosition, (static_cast<float>(i))*s * coarse * globalRescale, true, false);
    for (int i = -10; i < 10; i++) DrawLine(vMatrix, lPosition, (static_cast<float>(i))*s * coarse * globalRescale, true, true);
    for (int i = -100; i < 100; i++) if (i % 10 != 0) DrawLine(vMatrix, lPosition, (static_cast<float>(i))*s * fine * globalRescale, false, false);
    for (int i = -100; i < 100; i++) if (i % 10 != 0) DrawLine(vMatrix, lPosition, (static_cast<float>(i))*s * fine * globalRescale, false, true);

    //and the values
    int dp = 0 - static_cast<int>(log10(fov) + .7); //decimal places
    if (dp < 0) dp = 0;
    bool mm = true;
    for (int i = -10; i < 10; i++)
    {
        RenderNumber(
            static_cast<GLfloat>((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(ClipAngle) / static_cast<float>(this->height()))),
            static_cast<GLfloat>(static_cast<float>(i))*s * coarse - ((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(ClipAngle) / static_cast<float>(this->height()))),
            static_cast<GLfloat>(-.99),
            (static_cast<float>(i) * coarse),
            dp,
            mm,
            true,
            vMatrix
        );
    }
    for (int i = -10; i < 10; i++)
    {
        if (i != 0)
        {
            RenderNumber(
                static_cast<GLfloat>(static_cast<float>(i) * s * coarse + ((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(ClipAngle) / static_cast<float>(this->height())))),
                static_cast<GLfloat>(0 - (((10.0 / FONT_SCALE_FACTOR)*static_cast<double>(ClipAngle) / static_cast<double>(this->height())))),
                static_cast<GLfloat>(-.99),
                (static_cast<float>(i) * coarse),
                dp,
                mm,
                true,
                vMatrix
            );
        }
    }
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

    glfunctions->glDepthMask(true);

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
    glfunctions->glDepthMask(true);

    if (mainWindow->ui->actionShow_Scale_Grid->isChecked())
        DrawScaleGrid(vMatrix, vMatrix * camera);
}

/**
 * @brief GlWidget::paintGL
 */
void GlWidget::paintGL()
{
    //qDebug() << "[Where I'm I?] In paintGL";

    glfunctions->glClearColor(
        static_cast<float>(colorBackgroundRed) / static_cast<float>(255),
        static_cast<float>(colorBackgroundGreen) / static_cast<float>(255),
        static_cast<float>(colorBackgroundBlue) / static_cast<float>(255),
        0.5f
    );
    glfunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glfunctions->glViewport(0, 0, xdim, ydim);

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
