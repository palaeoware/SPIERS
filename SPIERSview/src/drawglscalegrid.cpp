#include <math.h>

#include "drawglscalegrid.h"
#include "rowmans.h"
#include "globals.h"
#include "gl3widget.h"

/**
 * @brief DrawGLScaleGrid::DrawGLScaleGrid
 * @param parent
 */
DrawGLScaleGrid::DrawGLScaleGrid(GlWidget *parent)
{
    glWidget = parent;
}

/**
 * @brief DrawGLScaleGrid::initializeGL
 */
void DrawGLScaleGrid::initializeGL()
{
    // Set up lines
    QVector<QVector3D> lineVertices;

    lineVertices << QVector3D(0, -10000000.0,  0.0) << QVector3D( 0, 10000000.0,  0.0);
    lineVertices << QVector3D(-10000000, 0.0,  0.0) << QVector3D(10000000, 0.0,  0.0);  //second two are horizontal line

    VBOline.create();
    VBOline.bind();
    VBOline.allocate(12 * sizeof(GLfloat));
    VBOline.write(0, lineVertices.constData(), 12 * sizeof(GLfloat));
    VBOline.release();

    // Set up fonts
    //0-9 are digits 0-9, 10 is -, 11 is ., 12 is m

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
 * @brief DrawGLScaleGrid::draw
 * @param vMatrix
 * @param lPosition
 */
void DrawGLScaleGrid::draw(QMatrix4x4 vMatrix, QVector3D lPosition)
{
    //qDebug() << "[Where I'm I?] In DrawScaleGrid";

    glWidget->lightingShaderProgram.bind();
    glWidget->lightingShaderProgram.setUniformValue("lightPosition", lPosition);
    glWidget->lightingShaderProgram.setUniformValue("ambientReflection", static_cast<GLfloat>(1.0));
    glWidget->lightingShaderProgram.setUniformValue("diffuseReflection", static_cast<GLfloat>(1.0));
    glWidget->lightingShaderProgram.setUniformValue("specularReflection", static_cast<GLfloat>(1.0));
    glWidget->lightingShaderProgram.setUniformValue("shininess", static_cast<GLfloat>(1000.0));

    //draw occlusion thing
    glWidget->lightingShaderProgram.setUniformValue("mvpMatrix", glWidget->pMatrix * vMatrix);
    glWidget->lightingShaderProgram.setUniformValue("mvMatrix", vMatrix);
    glWidget->lightingShaderProgram.setUniformValue("normalMatrix", vMatrix.normalMatrix());
    glWidget->lightingShaderProgram.setUniformValue("ambientColor", QColor(colorBackgroundRed, colorBackgroundGreen, colorBackgroundBlue));
    glWidget->lightingShaderProgram.setUniformValue("diffuseColor", QColor(0, 0.0, 0.0));
    glWidget->lightingShaderProgram.setUniformValue("specularColor", QColor(0, 0.0, 0.0));
    glWidget->lightingShaderProgram.setUniformValue("alpha", static_cast<GLfloat>(0.6));

    occBuffer.bind();
    glWidget->lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
    glWidget->lightingShaderProgram.enableAttributeArray("vertex");
    glWidget->lightingShaderProgram.setAttributeBuffer("normal", GL_FLOAT, 3 * numOccVertices * static_cast<int>(sizeof(GLfloat)), 3, 0);
    glWidget->lightingShaderProgram.enableAttributeArray("normal");
    occBuffer.release();

    glWidget->glfunctions->glDrawArrays(GL_TRIANGLES, 0, numOccVertices);

    glWidget->lightingShaderProgram.setUniformValue("alpha", static_cast<GLfloat>(1.0));
    glWidget->glfunctions->glClear(GL_DEPTH_BUFFER_BIT);

    float s = static_cast<float>(1.0) / static_cast<float>(mmPerUnit);

    //work out field of view (max of height and width)
    float divider = (glWidget->height() * globalRescale) / static_cast<float>(30.0);

    //work out x field of view in mm
    double fov;
    fov = static_cast<double>(glWidget->ClipAngle / (divider * s));
    //OK, work out correct scale in mm
    //Look at fov and find coarse level to use
    //fov 5mm - want coarse 1, fine 0.1
    //fov 50mm - want coarse 10, fine 1
    //fov .5mm - want coarse .1, fine .01 etc
    //work out coarse below

    float coarse = static_cast<float>(pow(static_cast<double>(10.0), (static_cast<double>(static_cast<int>(log10(fov) + .7))))); //double cast is to round it
    float fine = coarse / 10;

    //qDebug()<<"Coarse"<<coarse<<"Fine"<<fine;

    for (int i = -10; i < 10; i++) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * coarse * globalRescale, true, false);
    for (int i = -10; i < 10; i++) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * coarse * globalRescale, true, true);
    for (int i = -100; i < 100; i++) if (i % 10 != 0) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * fine * globalRescale, false, false);
    for (int i = -100; i < 100; i++) if (i % 10 != 0) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * fine * globalRescale, false, true);

    //and the values
    int dp = 0 - static_cast<int>(log10(fov) + .7); //decimal places
    if (dp < 0) dp = 0;
    bool mm = true;
    for (int i = -10; i < 10; i++)
    {
        renderNumber(
            static_cast<GLfloat>((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height()))),
            static_cast<GLfloat>(static_cast<float>(i))*s * coarse - ((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height()))),
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
            renderNumber(
                static_cast<GLfloat>(static_cast<float>(i) * s * coarse + ((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height())))),
                static_cast<GLfloat>(0 - (((10.0 / FONT_SCALE_FACTOR)*static_cast<double>(glWidget->ClipAngle) / static_cast<double>(glWidget->height())))),
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
 * @brief DrawGLScaleGrid::drawLine
 * @param vMatrix
 * @param lPosition
 * @param pos
 * @param major
 * @param horizontal
 */
void DrawGLScaleGrid::drawLine(QMatrix4x4 vMatrix, QVector3D lPosition, float pos, bool major, bool horizontal)
{
    //qDebug() << "[Where I'm I?] In DrawLine";

    Q_UNUSED(lPosition)

    if (!horizontal) vMatrix.translate(pos, 0, -1);
    else vMatrix.translate(0, pos, -1);
    vMatrix *= globalMatrix;

    glWidget->lightingShaderProgram.setUniformValue("mvpMatrix", glWidget->pMatrix * vMatrix);
    glWidget->lightingShaderProgram.setUniformValue("mvMatrix", vMatrix);
    glWidget->lightingShaderProgram.setUniformValue("normalMatrix", vMatrix.normalMatrix());

    if (major)
        glWidget->lightingShaderProgram.setUniformValue("ambientColor", QColor(colorGridRed, colorGridGreen, colorGridBlue));
    else
        glWidget->lightingShaderProgram.setUniformValue("ambientColor", QColor(colorGridMinorRed, colorGridMinorGreen, colorGridMinorBlue));

    glWidget->lightingShaderProgram.setUniformValue("diffuseColor", QColor(0.0, 0.0, 0.0));
    glWidget->lightingShaderProgram.setUniformValue("specularColor", QColor(0.0, 0.0, 0.0));

    VBOline.bind();
    if (!horizontal) glWidget->lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
    else glWidget->lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 6 * sizeof(GLfloat), 3, 0);

    glWidget->lightingShaderProgram.enableAttributeArray("vertex");
    VBOline.release();

    glWidget->glfunctions->glDrawArrays(GL_LINES, 0, 2);
}

/**
 * @brief DrawGLScaleGrid::renderCharacter
 * @param x
 * @param y
 * @param z
 * @param charactercode
 * @param vMatrix
 * @param Colour
 */
void DrawGLScaleGrid::renderCharacter(GLfloat x, GLfloat y, GLfloat z, int charactercode, QMatrix4x4 vMatrix, QColor Colour)
{
    //qDebug() << "[Where I'm I?] In RenderCharacter";

    vMatrix.translate(x, y, z);
    vMatrix.rotate(180.0, 1.0, 0.0, 0.0);
    vMatrix.scale((glWidget->ClipAngle / static_cast<float>(glWidget->height())));

    glWidget->lightingShaderProgram.setUniformValue("mvpMatrix", glWidget->pMatrix * vMatrix);
    glWidget->lightingShaderProgram.setUniformValue("mvMatrix", vMatrix);
    glWidget->lightingShaderProgram.setUniformValue("normalMatrix", vMatrix.normalMatrix());
    glWidget->lightingShaderProgram.setUniformValue("ambientColor", Colour);
    glWidget->lightingShaderProgram.setUniformValue("diffuseColor", QColor(0.0, 0.0, 0.0));
    glWidget->lightingShaderProgram.setUniformValue("specularColor", QColor(0.0, 0.0, 0.0));

    VBOcharacters[charactercode].bind();
    glWidget->lightingShaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
    glWidget->lightingShaderProgram.enableAttributeArray("vertex");
    VBOcharacters[charactercode].release();

    glWidget->glfunctions->glDrawArrays(GL_LINES, 0, static_cast<int>(CharacterLineCounts[charactercode]));
}

/**
 * @brief DrawGLScaleGrid::renderNumber
 * @param x
 * @param y
 * @param z
 * @param number
 * @param decimalplaces
 * @param mm
 * @param major
 * @param vMatrix
 */
void DrawGLScaleGrid::renderNumber(GLfloat x, GLfloat y, GLfloat z, float number, int decimalplaces, bool mm, bool major, QMatrix4x4 vMatrix)
{
    //qDebug() << "[Where I'm I?] In RenderNumber";

    QColor numcolour;
    if (major)
        numcolour = QColor(colorGridRed, colorGridGreen, colorGridBlue);
    else
        numcolour = QColor(colorGridMinorRed, colorGridMinorGreen, colorGridMinorBlue);

    // Construct number as list of characters
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
        renderCharacter(x + off, y, z, characters[i], vMatrix, numcolour);
        off += static_cast<GLfloat>(CharacterWidths[characters[i]] * ((static_cast<float>(0.9 / FONT_SCALE_FACTOR) * glWidget->ClipAngle / static_cast<float>(glWidget->height()))));
    }
}
