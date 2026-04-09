/**
 * @file
 * Header: OBJ Exporter
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

#ifndef OBJEXPORTER_H
#define OBJEXPORTER_H

#include <QString>
#include <QList>
#include <QFile>

class SVObject;

/**
 *
 * Exports SPIERSview geometry to OBJ format with accompanying MTL material files.
 *
 * OBJ is a widely-supported 3D model format that preserves:
 * - Vertex positions and normals
 * - Triangle/face data
 * - Per-object material assignments
 *
 * MTL files store material definitions including:
 * - RGB color (Ka - ambient, Kd - diffuse)
 * - Transparency (d - dissolve/alpha)
 * - Shininess (Ns - specular exponent)
 *
 * Transform matrices are baked into vertex geometry (pre-applied before export).
 * The OBJ file references a single MTL file containing all material definitions.
 *
 **/
class OBJExporter
{
public:
    /**
     *
     * Export SPIERSview objects to OBJ + MTL files.
     *
     * Bakes transformation matrices into vertex coordinates.
     * Creates two files: .obj (geometry) and .mtl (materials).
     *
     * @param objFilepath    Path where .obj file will be written
     * @param visibleObjects List of SVObject pointers to export
     * @return true if export succeeds, false on file I/O error
     *
     **/
    static bool exportToOBJ(
        const QString &objFilepath,
        const QList<SVObject*> &visibleObjects);

private:
    /**
     *
     * Apply 4x4 transformation matrix to mesh vertex positions.
     *
     * @param vertices  Array of vertex positions [x0, y0, z0, x1, y1, z1, ...]
     * @param matrix    4x4 transformation matrix in row-major format
     * @return New vertex array with transforms applied
     *
     **/
    static QList<float> applyMatrixToVertices(
        const QList<float> &vertices,
        const float matrix[16]);

    /**
     *
     * Compute vertex normals from triangle data using Gouraud shading.
     *
     * Averages face normals at each vertex to produce smooth shading.
     *
     * @param vertices Array of vertex positions
     * @param triangles Array of triangle indices
     * @return Normal vector for each vertex [nx0, ny0, nz0, nx1, ny1, nz1, ...]
     *
     **/
    static QList<float> computeVertexNormals(
        const QList<float> &vertices,
        const QList<int> &triangles);

    /**
     *
     * Write OBJ file header and geometry data.
     *
     * @param objFile OBJ file open for writing
     * @param mtlName Name of MTL file (written as reference in OBJ header)
     * @param visibleObjects Objects to export
     * @param allVertices All transformed vertices [x0, y0, z0, ...]
     * @param allNormals All computed normals [nx0, ny0, nz0, ...]
     * @param allTriangles All triangle indices
     * @return true if write succeeds
     *
     **/
    static bool writeOBJGeometry(
        QFile &objFile,
        const QString &mtlName,
        const QList<SVObject*> &visibleObjects,
        const QVector<float> &allVertices,
        const QVector<float> &allNormals,
        const QVector<int> &allTriangles);

    /**
     *
     * Write MTL file with material definitions for all objects.
     *
     * @param mtlFilepath Path where MTL file will be written
     * @param visibleObjects Objects with material/color data
     * @return true if write succeeds
     *
     **/
    static bool writeMTLFile(
        const QString &mtlFilepath,
        const QList<SVObject*> &visibleObjects);
};

#endif // OBJEXPORTER_H
