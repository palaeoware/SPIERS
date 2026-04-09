/**
 * @file
 * Header: FBX Exporter
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

#ifndef FBXEXPORTER_H
#define FBXEXPORTER_H

#include <QString>
#include <QList>

class SVObject;

/**
 *
 * Exports SPIERSview geometry to FBX format.
 *
 * FBX is an industry-standard 3D interchange format that preserves:
 * - Vertex positions and normals
 * - Triangle/face data
 * - Per-object material assignments
 *
 * Transform matrices are baked into vertex geometry (pre-applied before export).
 * The FBX file includes material definitions with RGB color, transparency, and roughness.
 *
 * This exporter creates FBX 7300 format binary files suitable for import into
 * Blender, Maya, Cinema4D, and other professional 3D applications.
 *
 **/
class FBXExporter
{
public:

    struct FBXNodeHeader7300
    {
        quint32 endOffset;
        quint32 numProperties;
        quint32 propertyListLen;
        quint8  nameLen;
    };

    struct FBXColor
    {
        double r;
        double g;
        double b;
    };

    /**
     *
     * Export SPIERSview objects to FBX file.
     *
     * Bakes transformation matrices into vertex coordinates.
     * Creates a single .fbx file containing all geometry and materials.
     *
     * @param fbxFilepath    Path where .fbx file will be written
     * @param visibleObjects List of SVObject pointers to export
     * @return true if export succeeds, false on file I/O error
     *
     **/
    static bool exportToFBX(
        const QString &fbxFilepath,
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
     * Write FBX file header (magic string and version).
     *
     * @param outFile File open for binary writing
     * @return true if write succeeds
     *
     **/
    static bool writeFBXHeader(
        class QDataStream &outFile);

    /**
     *
     * Write FBX document structure with geometry nodes.
     *
     * @param outFile File open for binary writing
     * @param visibleObjects Objects to export
     * @return true if write succeeds
     *
     **/
    static bool writeFBXDocument(
        class QDataStream &outFile,
        const QList<SVObject*> &visibleObjects);

    /**
     * @brief writeNodeHeader
     * @param out
     * @param name
     * @param endOffset
     * @param numProps
     * @param propListLen
     * @return true if write succeeds
     */
    static bool writeNodeHeader(QDataStream &out, const QByteArray &name,
                         quint32 endOffset, quint32 numProps, quint32 propListLen);

    /**
     *
     * Write FBX file footer (null terminator and footer data).
     *
     * @param outFile File open for binary writing
     * @return true if write succeeds
     *
     **/
    static bool writeFBXFooter(
        class QDataStream &outFile);

    /**
     *
     * Convert SPIERSview transparency code to alpha value.
     *
     * Transparency codes (0-4) map linearly to alpha (1.0-0.0).
     *
     * @param transparency Transparency code 0-4
     * @return Alpha value 0.0-1.0
     *
     **/
    static float transparencyToAlpha(int transparency);

    /**
     *
     * Convert SPIERSview shininess code to roughness value.
     *
     * Shininess codes (0-3) map to roughness values for PBR shaders.
     * Higher shininess = lower roughness (more reflective).
     *
     * @param shininess Shininess code 0-3
     * @return Roughness value 0.0-0.8
     *
     **/
    static float shininessToRoughness(int shininess);
};

#endif // FBXEXPORTER_H
