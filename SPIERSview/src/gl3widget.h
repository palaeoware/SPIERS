/**
 * @file
 * Header: GL Widget
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
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
#include "drawglscalegrid.h"
#include "drawglscaleball.h"

// Resolution of each shadow depth map — 2048x2048 per light
#define SHADOW_MAP_SIZE 2048

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GlWidget(QWidget *parent);
    ~GlWidget();
    void grabGestures(const QList<Qt::GestureType> &gestures);

    void glDebug(QString string);

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

    QOpenGLVertexArrayObject vao;
    QOpenGLFunctions *glfunctions;
    QOpenGLExtraFunctions *glextrafunctions;

    // Main lighting shader — compiled in three variants
    QOpenGLShaderProgram lightingShaderProgram;        // no shadows
    QOpenGLShaderProgram lightingShaderProgramShadow;  // hard shadows
    QOpenGLShaderProgram lightingShaderProgramPCF;     // soft shadows (PCF)

    // Depth-only shader for shadow pass
    QOpenGLShaderProgram shadowDepthShaderProgram;

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

    // Shadow map infrastructure
    void initShadowFBOs();
    void renderShadowPass(int lightIndex, QVector3D lightDir, QMatrix4x4 &lightSpaceMatrixOut);
    QMatrix4x4 computeLightSpaceMatrix(QVector3D lightDir);
    QOpenGLShaderProgram *selectShader();

    // One FBO + depth texture per light slot (always allocated, used when shadow enabled)
    GLuint shadowFBO[3];
    GLuint shadowDepthTexture[3];

    // Light space matrices — recomputed each frame for active shadow lights
    QMatrix4x4 lightSpaceMatrix[3];

    // Dummy 1x1 white texture bound to shadow slots that aren't casting shadows
    // Prevents undefined behaviour from unbound sampler2DShadow uniforms
    GLuint dummyShadowTexture;

    DrawGLScaleGrid *scaleGrid;
    DrawGLScaleBall *scaleBall;
};

#endif // GL3WIDGET_H
