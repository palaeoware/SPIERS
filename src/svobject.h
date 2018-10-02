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

#include "mc.h" //for isosurface
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include <QObject>
#include <QFile>
#include "vtkPolyDataNormals.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "myconnectivityfilter.h"
#include "vtkIdTypeArray.h"
#include "vtkPointData.h"
#include "vtkTriangle.h"
#include "vtkDataArray.h"
#include "vtkQuadricDecimation.h"
#include "vtkDecimatePro.h"
#include "vtkCallbackCommand.h"
#include "vtkCellArray.h"
#include "vtkIdTypeArray.h"

class SPV;
class CompressedSlice;
class QTreeWidgetItem;

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
    void ForceUpdates(int,int);
    int Parent();
    int Index;
    QString Name;
    QChar Key;
    uchar Colour[3]; //r,g,b
    QList <isosurface *> Isosurfaces;
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
    int Voxels;
    int Shininess;  //codes 0-3
    bool colour; //does it have colour data?
    QTreeWidgetItem *widgetitem;
    bool Expanded;
    QList <int> displaylists;

    //Newer VBO stuff
    //QList <QOpenGLVertexArrayObject *> VBOs;
    QList <QOpenGLBuffer *> VertexBuffers;
    QList <QOpenGLBuffer *> ColourBuffers;
    //QList <QOpenGLBuffer *> NormalBuffers;
    QList <int> VBOVertexCounts;

    double objectxmin, objectymin, objectzmin, objectxmax, objectymax, objectzmax;
    bool donebox;
    QOpenGLBuffer BoundingBoxBuffer;

    bool UsesVBOs;

    SPV *spv;
    float matrix[16];
    float defaultmatrix[16];
    bool gotdefaultmatrix;
    void ResetMatrix();
    QByteArray CompressedPolyData;
    bool PolyDataCompressed;
    bool SurfaceMe;
    bool killme;
    double scale;
    bool BuggedData;
    vtkPolyData *pd;
    void MakeDlists();
    int AppendCompressedFaces(QString mainfile, QString internalfile, QDataStream *out);

    void MakeVBOs();
private:
    void DeleteVTKObjects();
    void GetFinalPolyData();
    void MakePolyVerts(int slice, int VertexBase);
    QString DoMatrixDXFoutput(int v,float x,float y, float z);
    int NextVertex;
    vtkPolyData *PolyData; //Currently one object for whole thing
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

    MyConnectivityFilter *islandfinder;

    QVector <float> normalx;
    QVector <float> normaly;
    QVector <float> normalz;
 };

extern QList <SVObject *> SVObjects;

#endif // SVOBJECT_H
