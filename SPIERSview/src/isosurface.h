#ifndef ISOSURFACE_H
#define ISOSURFACE_H

#include <QVarLengthArray>

class Isosurface
{
public:

    int nVertices;
    int nTriangles;
    int trianglearraysize;
    int vertexarraysize;
    bool used; //Flag: have I converted to VTK format?
    QVarLengthArray <int, 3000> triangles;
    QVarLengthArray <int, 3000> vertices;

    Isosurface();
};

#endif // ISOSURFACE_H
