#ifndef DRAWGLSCALEBALL_H
#define DRAWGLSCALEBALL_H

#include <QtOpenGL>

class GlWidget;

class DrawGLScaleBall
{
public:
    DrawGLScaleBall(GlWidget *parent);

    void initializeGL();
    void draw(QMatrix4x4 vMatrix, QVector3D lPosition);

    GlWidget *glWidget;

private:

};

#endif // DRAWGLSCALEBALL_H
