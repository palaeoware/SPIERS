/**
 * @file
 * Source: Isosurface
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */
#include "isosurface.h"

/**
 * @brief Isosurface::Isosurface
 */
Isosurface::Isosurface()
{
    used = false;
    nVertices = 0;
    nTriangles = 0;
    trianglearraysize = 1000;
    vertexarraysize = 1000;
    triangles.resize(3 * trianglearraysize);
    vertices.resize(3 * vertexarraysize);
}
