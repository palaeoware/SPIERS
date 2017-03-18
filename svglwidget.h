#ifndef SVGLWIDGET_H
#define SVGLWIDGET_H
//#ifdef __APPLE__
//#include <gl.h>
//#include <glu.h>
//#endif

//#ifndef __APPLE__
//#include <GL/gl.h>
//#include <GL/glu.h>
//#endif

//#include <QtOpenGL/QtOpenGL>

//#include <QtOpenGL/QGLWidget>
#include <QMouseEvent>
#include <QtOpenGL/QGLShaderProgram>
//#include <GL/glu.h>
#include <QGLFunctions>
#include <QMatrix4x4>

class SVGlWidget : public QGLWidget, protected QGLFunctions
{
     Q_OBJECT        // must include this if you use Qt signals/slots

 public:
     SVGlWidget(QWidget *parent);
     void SetZoom(int);
     void SetClip(int,int, int);
     void Rotate(double angle);
     void MoveAway(double dist);
     void Resize(float angle);
     void ResetSize();
     void YRotate(float angle);
      void ZRotate(float angle);
      void XRotate(float angle);
      void Translate(float x, float y, float z);
  void SetStereoSeparation(float s);
  bool CanISee(int index);
  void setsamples(int i);
  void recenter_ball();
  void ResizeScaleBall(float d);
  void ResetToDefault();
  void NewDefault();
  QGLShaderProgram simple_shader;
  QMatrix4x4 projection;
  QVector<QVector3D> vertices;

protected:

  void initializeGL();
  void  resizeGL(int w, int h);
  void paintGL();
  void mouseMoveEvent(QMouseEvent *event);
  void SetView(bool half);

private:
  //GLUquadricObj *quadratic;
  GLfloat StereoSeparation;
  void DrawObjects(bool);
  static GLfloat LightAmbient[];

  GLfloat LightDiffuse[4];
  GLfloat LightGreen[4];
  GLfloat LightRed[4];

  static GLfloat LightPosition[];
  static GLfloat LightPosition1[];
  int LastMouseXpos;
  int LastMouseYpos;
  int zoom;
  float ClipStart;
  float ClipDepth;
  float ClipAngle;
  int xdim,ydim;
  bool LastDrawQuad;
};

#endif // SVGLWIDGET_H

