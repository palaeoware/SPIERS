#ifndef MC_H
#define MC_H
#include <QVarLengthArray>

class SVObject;

typedef unsigned char datum;

struct Layer {
  datum *botData;
  datum *topData;
  int  *edges;  /* tag indexing into vertex list */
};

class isosurface {
  public:
  isosurface(); //constructor
  QVarLengthArray <int,3000> triangles;
  QVarLengthArray <int,3000> vertices;
  int      nVertices;
  int      nTriangles;
  int      trianglearraysize;
  int      vertexarraysize;
  bool used;      //Have I converted to VTK format?
};


class mc
{
public:
    mc(SVObject *);
    void SurfaceObject();

private:
    int iDim, jDim, kDim;
    SVObject *object;
    static int edgeTable[];
    static int triTable[][16];
    unsigned char* slicebuffers[6]; //hold uncompressed data for slices
    unsigned char* slicebuffers_copy[6]; //backup copies of the pointers

    //the surfacing functions
    void Surface_Non_Chunked();
    void Surface_Chunked();

    int pointcount;
    isosurface * march_chunked(Layer *layer, int k, int vertbase, unsigned char *grid, int gridxscale, int gridyscale);
    void march_non_chunked(datum *dataset, Layer *layer, int k, float threshold, isosurface *iso);
    int makeVertex(int whichEdge, int i, int j, int k,float threshold, datum *dataset, isosurface *layerIso);
    int makeVertex2(int whichEdge, int i, int j, int k, isosurface *layerIso, int vertbase);
};

#endif // MC_H
