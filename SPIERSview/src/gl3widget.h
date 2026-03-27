/**
 * @file
 * Header: GL Widget
 */

#ifndef GL3WIDGET_H
#define GL3WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QGestureEvent>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLDebugLogger>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

#include "globals.h"
#include "drawglscaleball.h"

#define SHADOW_MAP_SIZE 2048

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

signals:
    void resized();  //for comms with the overlying painter

public:
    GlWidget(QWidget *parent);
    ~GlWidget();
    void grabGestures(const QList<Qt::GestureType> &gestures);
    void glDebug(QString string);

    float cameraX, cameraY, cameraZ;
    float centerX, centerY, centerZ;
    float ClipStart, ClipDepth, ClipAngle, defaultClipAngle;
    float StereoSeparation;
    int LastMouseXpos, LastMouseYpos;
    int xdim, ydim;

    QOpenGLVertexArrayObject vao;
    QOpenGLFunctions      *glfunctions;
    QOpenGLExtraFunctions *glextrafunctions;

    // Lighting shaders — three shadow variants compiled from one source
    QOpenGLShaderProgram lightingShaderProgram;        // no shadows
    QOpenGLShaderProgram lightingShaderProgramShadow;  // hard shadows
    QOpenGLShaderProgram lightingShaderProgramPCF;     // soft shadows (PCF)

    // Shadow depth pass shader
    QOpenGLShaderProgram shadowDepthShaderProgram;

    // OIT accumulation shaders — three shadow variants, same as lighting
    QOpenGLShaderProgram oitAccumShaderProgram;        // no shadows
    QOpenGLShaderProgram oitAccumShaderProgramShadow;  // hard shadows
    QOpenGLShaderProgram oitAccumShaderProgramPCF;     // soft shadows (PCF)

    // OIT composite shader — single variant, no shadows
    QOpenGLShaderProgram oitCompositeShaderProgram;

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
    double getFOV();
    void updateFOV();

    double realOrthoWidthMm, realOrthoHeightMm;
protected:
    bool event(QEvent *event) override;
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    bool gestureEvent(QGestureEvent *event);
    void pinchTriggered(QPinchGesture *gesture);
    QVector3D GetLightColour(int lightPower, QColor lightColour);
    QVector3D GetLightDirection(int xyAngle, int zPos);
    float GetLightPowerMultiplier(int power);

    // Shadow infrastructure
    void initShadowFBOs();
    void renderShadowPass(int lightIndex, QVector3D lightDir, QMatrix4x4 &lightSpaceMatrixOut);
    QMatrix4x4 computeLightSpaceMatrix(QVector3D lightDir);
    QOpenGLShaderProgram *selectShader();
    QOpenGLShaderProgram *selectOITShader();

    GLuint shadowFBO[3];
    GLuint shadowDepthTexture[3];
    QMatrix4x4 lightSpaceMatrix[3];
    GLuint dummyShadowTexture;

    // OIT infrastructure
    void initOIT();
    void resizeOIT(int w, int h);
    void renderOITAccumPass(bool rightview, QMatrix4x4 &vMatrix);
    void renderOITCompositePass();
    void setLightAndShadowUniforms(QOpenGLShaderProgram *shader,
                                   bool shadowsActive,
                                   const QMatrix4x4 &objMatrix,
                                   int lightCount,
                                   const QVector3D *directions,
                                   const QVector3D *colors);

    GLuint oitFBO;
    GLuint oitAccumTexture;   // RGBA16F — weighted colour accumulation
    GLuint oitRevealTexture;  // R16F    — reveal (transmittance) factor
    GLuint oitDepthRBO;       // shared depth renderbuffer (matches opaque pass)
    GLuint fullscreenQuadVBO; // [-1,1] quad for composite pass

    DrawGLScaleBall *scaleBall;
};

#endif // GL3WIDGET_H
