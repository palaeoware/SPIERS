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
