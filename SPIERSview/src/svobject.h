#ifndef SVOBJECT_H
#define SVOBJECT_H

#include <QList>
#include <QString>
#include <QDataStream>
#include <QDir>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QGLShaderProgram>
#include <QObject>
#include <QFile>
#include <QTreeWidgetItem>

#include "isosurface.h"
#include "dataconnectivityfilter.h"
#include "spv.h"
#include "compressedslice.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPolyDataNormals.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkIdTypeArray.h"
#include "vtkPointData.h"
#include "vtkTriangle.h"
#include "vtkDataArray.h"
#include "vtkQuadricDecimation.h"
#include "vtkDecimatePro.h"
#include "vtkCallbackCommand.h"
#include "vtkCellArray.h"
#include "vtkIdTypeArray.h"

/**
 * @brief The SVObject class
 */
class SVObject
{

public:
    SVObject(int index);
    ~SVObject();
    void MakePolyData();
    void WritePD(QFile *outfile);
    void ReadPD(QFile *infile);
    int WriteDXFfaces(QFile *outfile);
    int WriteSTLfaces(QDir stldir, QString fname);
    void UnCompressPolyData();
    void CompressPolyData(bool flag);
    void DoUpdates();
    void ForceUpdates(int, int);
    int Parent();
    void MakeDlists();
    int AppendCompressedFaces(QString mainfile, QString internalfile, QDataStream *out);
    void MakeVBOs();
    void ResetMatrix();

    int Index;
    QString Name;
    QChar Key;
    uchar Colour[3]; //r,g,b
    QList <Isosurface *> Isosurfaces;
    QList <CompressedSlice *> compressedslices;
    unsigned char *AllSlicesCompressed;
    int AllSlicesSize;
    int Transparency;
    bool IsGroup;
    int InGroup;
    int Position;
    bool Visible;
    int Resample;
    int ResampleType;
    bool Dirty;
    int IslandRemoval;
    int Smoothing;
    int Triangles;
    int voxels;
    int Shininess;  //codes 0-3
    bool colour; //does it have colour data?
    QTreeWidgetItem *widgetitem;
    bool Expanded;
    QList <int> displaylists;

    //Newer VBO stuff
    QList <QOpenGLBuffer *> VertexBuffers;
    QList <QOpenGLBuffer *> ColourBuffers;
    QList <int> VBOVertexCounts;

    double objectxmin, objectymin, objectzmin, objectxmax, objectymax, objectzmax;
    bool donebox;
    QOpenGLBuffer boundingBoxBuffer;

    bool usesVBOs;

    SPV *spv;
    float matrix[16];
    float defaultmatrix[16];
    bool gotdefaultmatrix;

    QByteArray compressedPolyData;
    bool polyDataCompressed;
    bool SurfaceMe;
    bool killme;
    double scale;
    bool buggedData;
    vtkPolyData *polydata;

private:
    void DeleteVTKObjects();
    void GetFinalPolyData();
    void MakePolyVerts(int slice, int VertexBase);
    QString DoMatrixDXFoutput(int v, float x, float y, float z);
    int NextVertex;
    vtkPolyData *localPolyData; //Currently one object for whole thing
    vtkPoints *verts;
    vtkCellArray *cellarray;
    vtkIdTypeArray *actualarray;

    int object_ktr;

    vtkPolyDataNormals *normals;
    vtkWindowedSincPolyDataFilter *smoother;
    vtkDecimatePro *decimator;
    vtkQuadricDecimation *qdecimator;
    vtkCallbackCommand *cb;
    vtkCallbackCommand *cberror;
    vtkPolyData *pdislands;

    DataConnectivityFilter *islandfinder;

    QVector <float> normalx;
    QVector <float> normaly;
    QVector <float> normalz;
};

extern QList <SVObject *> SVObjects;

#endif // SVOBJECT_H
