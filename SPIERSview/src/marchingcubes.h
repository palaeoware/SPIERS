#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include <QVarLengthArray>

#include "isosurface.h"
#include "scalarfieldlayer.h"
#include "svobject.h"

class MarchingCubes
{
public:
    MarchingCubes(SVObject *);
    void surfaceObject();

private:
    int iDim;
    int jDim;
    int kDim;
    int pointcount;
    SVObject *object;
    static int edgeTable[];
    static int triTable[][16];
    unsigned char *slicebuffers[6]; //hold uncompressed data for slices
    unsigned char *slicebuffers_copy[6]; //backup copies of the pointers

    Isosurface *marchChunked(ScalarFieldLayer *layer, int k, int vertbase, unsigned char *grid, int gridxscale, int gridyscale);
    void marchNonChunked(unsigned char *dataset, ScalarFieldLayer *layer, int k, float threshold, Isosurface *iso);
    int makeVertex(int whichEdge, int i, int j, int k, float threshold, unsigned char *dataset, Isosurface *layerIso);
    int makeVertexFast(int whichEdge, int i, int j, int k, Isosurface *layerIso, int vertbase);
    void surfaceNonChunked();
    void surfaceChunked();
};

#endif // MARCHINGCUBES_H
