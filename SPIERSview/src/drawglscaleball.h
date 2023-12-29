/**
 * @file
 * Header: Draw GL Scale Ball
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

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
