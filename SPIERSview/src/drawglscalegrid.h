#ifndef DRAWGLSCALEGRID_H
#define DRAWGLSCALEGRID_H

#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

class GlWidget;

class DrawGLScaleGrid
{
public:
    DrawGLScaleGrid(GlWidget *parent);

    void initializeGL();
    void draw(QMatrix4x4 vMatrix, QVector3D lPosition);

    GlWidget *glWidget;

private:
    int numOccVertices;

    GLfloat CharacterWidths[15]; //widths of these characters
    GLfloat CharacterLineCounts[15]; //widths of these characters
    QOpenGLBuffer VBOcharacters[15]; //0-9 are digits 0-9, 10 is -, 11 is ., 12 is m, 13 is u
    QOpenGLBuffer VBOline;
    QOpenGLBuffer occBuffer;
    QMatrix4x4 scaleMatrix; //Manipulations to scale ball


    void drawLine(QMatrix4x4 vMatrix, QVector3D lPosition, float pos, bool major, bool horizontal);
    void renderCharacter(GLfloat x, GLfloat y, GLfloat z, int charactercode, QMatrix4x4 vMatrix, QColor Colour);
    void renderNumber(GLfloat x, GLfloat y, GLfloat z, float number, bool major, QMatrix4x4 vMatrix);
};

#endif // DRAWGLSCALEGRID_H
