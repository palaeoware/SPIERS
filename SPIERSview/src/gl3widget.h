#ifndef GL3WIDGET_H
#define GL3WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QGestureEvent>

#include "globals.h"

class GlWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    GlWidget(QWidget *parent);
    ~GlWidget();
    void grabGestures(const QList<Qt::GestureType> &gestures);

    float cameraX;
    float cameraY;
    float cameraZ;
    float centerX;
    float centerY;
    float centerZ;
    float ClipStart;
    float ClipDepth;
    float ClipAngle;
    float defaultClipAngle;
    float StereoSeparation;

    int LastMouseXpos;
    int LastMouseYpos;
    int xdim;
    int ydim;

    QOpenGLFunctions *glfunctions;

    bool CanISee(int index);
    void SetClip(int Start, int Depth, int Angle);
    void DoPMatrix(int width, int height);
    void Rotate(double angle);
    void moveCameraZ(double value);
    void DrawObjects(bool rightview, bool halfsize);
    void SetStereoSeparation(float s);
    void YRotate(float angle);
    void ZRotate(float angle);
    void XRotate(float angle);
    void Translate(float x, float y, float z);
    void Resize(float value);
    void ResetSize();
    void ResetToDefault();
    void NewDefault();
    void DrawScaleGrid(QMatrix4x4 vMatrix, QVector3D lPosition);
    void DrawLine(QMatrix4x4 vMatrix, QVector3D lPosition, float pos, bool major, bool horizontal);
    void RenderCharacter(GLfloat x, GLfloat y, GLfloat z, int charactercode, QMatrix4x4 vMatrix, QColor Colour);
    void RenderNumber(GLfloat x, GLfloat y, GLfloat z, float number, int decimalplaces, bool mm, bool major, QMatrix4x4 vMatrix);

protected:
    // Overrides
    bool event(QEvent *event);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mouseMoveEvent(QMouseEvent *event);

    int numOccVertices;
    GLfloat CharacterWidths[13]; //widths of these characters
    GLfloat CharacterLineCounts[13]; //widths of these characters
    QOpenGLBuffer VBOcharacters[13]; //0-9 are digits 0-9, 10 is -, 11 is ., 12 is m
    QOpenGLBuffer VBOline;
    QOpenGLBuffer occBuffer;

private:
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture *gesture);

    QMatrix4x4 pMatrix;
    QMatrix4x4 scaleMatrix; //Manipulations to scale ball
    QOpenGLShaderProgram lightingShaderProgram;
    QOpenGLShaderProgram lightingShaderProgramForColour;
};

#endif // GL3WIDGET_H
