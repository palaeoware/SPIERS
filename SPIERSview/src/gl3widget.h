#ifndef GL3WIDGET_H
#define GL3WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QGestureEvent>

#include "globals.h"
#include "drawglscalegrid.h"
#include "drawglscaleball.h"

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
    QOpenGLShaderProgram lightingShaderProgram;
    QOpenGLShaderProgram lightingShaderProgramForColour;
    QMatrix4x4 pMatrix;

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

protected:
    // Overrides
    bool event(QEvent *event);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mouseMoveEvent(QMouseEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture *gesture);

    DrawGLScaleGrid *scaleGrid;
    DrawGLScaleBall *scaleBall;
};

#endif // GL3WIDGET_H
