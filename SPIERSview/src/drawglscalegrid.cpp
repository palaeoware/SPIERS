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
    //0-9 are digits 0-9, 10 is -, 11 is ., 12 is m, 13 is u

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

    for (int ii = 10; ii < 15; ii++)
    {
        int charnumber = 0;
        if (ii == 10) charnumber = 14; // -
        if (ii == 11) charnumber = 13; // .
        if (ii == 12) charnumber = 77; // m
        if (ii == 13) charnumber = 85; // u
        if (ii == 14) charnumber = 67; // c

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

    // OK, work out correct scale in mm
    // Look at fov and find coarse level to use
    // fov 5mm - want coarse 1, fine 0.1
    // fov 50mm - want coarse 10, fine 1
    // fov .5mm - want coarse .1, fine .01 etc
    // work out coarse below
    float coarse = 1;

    // first find out if field of view need rectifying due to very small values
    if (currentFOV < static_cast<double>(1) && currentFOV > static_cast<double>(0.1))
    {
        coarse = static_cast<float>(1);
    }
    else if (currentFOV < static_cast<double>(0.1) && currentFOV > static_cast<double>(0.01))
    {
        coarse = static_cast<float>(0.1);
    }
    else if (currentFOV < static_cast<double>(0.01) && currentFOV > static_cast<double>(0.001))
    {
        coarse = static_cast<float>(0.01);
    }
    else if (currentFOV < static_cast<double>(0.001) && currentFOV > static_cast<double>(0.0001))
    {
        coarse = static_cast<float>(0.001);
    }
    else if (currentFOV < static_cast<double>(0.0001) && currentFOV > static_cast<double>(0.00001))
    {
        coarse = static_cast<float>(0.0001);
    }
    else if (currentFOV < static_cast<double>(0.00001) && currentFOV > static_cast<double>(0.000001))
    {
        coarse = static_cast<float>(0.00001);
    }
    else if (currentFOV < static_cast<double>(0.000001) && currentFOV > static_cast<double>(0.0000001))
    {
        coarse = static_cast<float>(0.000001);
    }
    else if (currentFOV < static_cast<double>(0.0000001) && currentFOV > static_cast<double>(0.00000001))
    {
        coarse = static_cast<float>(0.0000001);
    }
    else if (currentFOV < static_cast<double>(0.00000001) && currentFOV > static_cast<double>(0.000000001))
    {
        coarse = static_cast<float>(0.00000001);
    }
    else if (currentFOV > static_cast<double>(1) && currentFOV < static_cast<double>(10))
    {
        coarse = static_cast<float>(10);
    }
    else if (currentFOV >= static_cast<double>(10) && currentFOV < static_cast<double>(1000))
    {
        coarse = static_cast<float>(100);
    }
    else if (currentFOV >= static_cast<double>(1000))
    {
        coarse = static_cast<float>(1000);
    }

    float fine = coarse / 10;

    //qDebug() << "Coarse" << coarse << "Fine" << fine;

    float s = static_cast<float>(1.0) / static_cast<float>(mmPerUnit);

    for (int i = -10; i < 10; i++) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * coarse * globalRescale, true, false);
    for (int i = -10; i < 10; i++) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * coarse * globalRescale, true, true);
    for (int i = -100; i < 100; i++) if (i % 10 != 0) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * fine * globalRescale, false, false);
    for (int i = -100; i < 100; i++) if (i % 10 != 0) drawLine(vMatrix, lPosition, (static_cast<float>(i))*s * fine * globalRescale, false, true);

    //and the values
    if (showMinorGridValues)
    {
        for (int i = -100; i < 100; i++)
        {
            if ((i != 0) && (i % 10 != 0))
            {
                renderNumber(
                    static_cast<GLfloat>((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height()))),
                    static_cast<GLfloat>(static_cast<float>(i))*s * fine - ((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height()))),
                    static_cast<GLfloat>(-.99),
                    (static_cast<float>(i) * fine),
                    false,
                    vMatrix
                );
            }
        }
        for (int i = -100; i < 100; i++)
        {
            if ((i != 0) && (i % 10 != 0))
            {
                renderNumber(
                    static_cast<GLfloat>(static_cast<float>(i) * s * fine + ((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height())))),
                    static_cast<GLfloat>(0 - (((10.0 / FONT_SCALE_FACTOR)*static_cast<double>(glWidget->ClipAngle) / static_cast<double>(glWidget->height())))),
                    static_cast<GLfloat>(-.99),
                    (static_cast<float>(i) * fine),
                    false,
                    vMatrix
                );
            }
        }
    }

    for (int i = -10; i < 10; i++)
    {
        renderNumber(
            static_cast<GLfloat>((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height()))),
            static_cast<GLfloat>(static_cast<float>(i))*s * coarse - ((static_cast<float>(10.0 / FONT_SCALE_FACTOR)*static_cast<float>(glWidget->ClipAngle) / static_cast<float>(glWidget->height()))),
            static_cast<GLfloat>(-.99),
            (static_cast<float>(i) * coarse),
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
void DrawGLScaleGrid::renderNumber(GLfloat x, GLfloat y, GLfloat z, float number, bool major, QMatrix4x4 vMatrix)
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

    qDebug() << number;

    QString numberAsCharacter = QString::number(number);

    // If in m then /1000
    if (number >= static_cast<float>(1000))
    {
        numberAsCharacter = QString::number(number / 1000);
    }
    // If in cm then /100
    if (number < static_cast<float>(1000) && number >= static_cast<float>(10))
    {
        numberAsCharacter = QString::number(number / 100);
    }
    // If in um then *1000
    if (number < static_cast<float>(0.09))
    {
        numberAsCharacter = QString::number(number * 1000);
    }

    for (int i = 0; i < numberAsCharacter.length(); i++)
    {
        if (numberAsCharacter.at(i) == QChar(QString(".").toLatin1().at(0)))
        {
            characters.append(10); // a dot
        }
        else
        {
            characters.append(QString(numberAsCharacter.at(i)).toInt() - 0);
        }
    }

    // Finally the units
    // 12 = m
    // 13 = u
    // 14 = c

    if (number >= static_cast<float>(1000))
    {
        // Is in meter scale
        characters.append(12); //for m
    }
    else if (number >= static_cast<float>(10))
    {
        // Is in cm scale
        characters.append(14); //for c
        characters.append(12); //for m
    }
    else if (number >= static_cast<float>(0.09))
    {
        // Is in mm scale
        characters.append(12); //for m
        characters.append(12); //for m
    }
    else if (number < static_cast<float>(0.09))
    {
        // Is in um scale
        characters.append(13); //for u
        characters.append(12); //for m
    }

    // Now display them
    GLfloat off = 0; //offset
    for (int i = 0; i < characters.length(); i++)
    {
        renderCharacter(x + off, y, z, characters[i], vMatrix, numcolour);
        off += static_cast<GLfloat>(CharacterWidths[characters[i]] * ((static_cast<float>(0.9 / FONT_SCALE_FACTOR) * glWidget->ClipAngle / static_cast<float>(glWidget->height()))));
    }
}
