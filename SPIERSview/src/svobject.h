/**
 * @file
 * Header: SV (SPEIRSview) Object
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef SVOBJECT_H
#define SVOBJECT_H

#include <QList>
#include <QString>
#include <QDataStream>
#include <QDir>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QObject>
#include <QFile>
#include <QTreeWidgetItem>

#include "isosurface.h"
#include "spv.h"
#include "compressedslice.h"
#include "meshfilters.h"

/**
 * @brief Plain C++ replacement for vtkPolyData.
 * Holds triangle mesh geometry as flat arrays of vertices and triangle indices,
 * plus per-vertex normals computed without VTK.
 */
struct MeshData
{
    QVector<float> vertices;   // x,y,z per vertex (3 floats each)
    QVector<float> normals;    // nx,ny,nz per vertex (3 floats each)
    QVector<int>   triangles;  // 3 vertex indices per triangle

    void clear() { vertices.clear(); normals.clear(); triangles.clear(); }
    int vertexCount()   const { return vertices.size()  / 3; }
    int triangleCount() const { return triangles.size() / 3; }
};

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
    void setMesh(const MeshData &mesh); // used by vaxml.cpp to inject imported geometry
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
    QTreeWidgetItem *widgetitem;
    bool Expanded;
    QList <int> displaylists;

    //Newer VBO stuff
    QList <QOpenGLBuffer *> VertexBuffers;
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
    bool isSurfacing;
private:
    void GetFinalPolyData();
    void MakePolyVerts(int slice, int VertexBase);
    QString DoMatrixDXFoutput(int v, float x, float y, float z);

    MeshData localMesh;   // input mesh (built from Isosurfaces)
    MeshData finalMesh;   // output mesh (after processing - currently a direct copy)

    int object_ktr;

    QVector <float> normalx;
    QVector <float> normaly;
    QVector <float> normalz;

};

extern QList <SVObject *> SVObjects;

#endif // SVOBJECT_H
