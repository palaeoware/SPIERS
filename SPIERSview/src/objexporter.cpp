/**
 * @file
 * Source: OBJ Exporter
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

#include "objexporter.h"
#include "svobject.h"

#include <QFile>
#include <QTextStream>
#include <QVector>
#include <cmath>

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
bool OBJExporter::exportToOBJ(
    const QString &objFilepath,
    const QList<SVObject*> &visibleObjects)
{
    // Construct MTL filename from OBJ path
    QString mtlFilepath = objFilepath;
    int dotPos = mtlFilepath.lastIndexOf('.');
    if (dotPos > 0)
    {
        mtlFilepath.truncate(dotPos);
    }
    mtlFilepath.append(".mtl");

    QString mtlName = mtlFilepath;
    int slashPos = mtlName.lastIndexOf('/');
    if (slashPos < 0)
    {
        slashPos = mtlName.lastIndexOf('\\');
    }
    if (slashPos >= 0)
    {
        mtlName = mtlName.mid(slashPos + 1);
    }

    // Write MTL file first
    if (!writeMTLFile(mtlFilepath, visibleObjects))
    {
        return false;
    }

    // Open OBJ file for writing
    QFile objFile(objFilepath);
    if (!objFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&objFile);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(6);

    // Write header
    out << "# Exported from SPIERSview\n";
    out << "mtllib " << mtlName << "\n\n";

    // Collect all vertex and triangle data
    QVector<float> allVertices;
    QVector<float> allNormals;
    QVector<int> allTriangles;
    int vertexOffset = 0;

    // Process each object
    for (SVObject *obj : visibleObjects)
    {
        // Use localMesh which contains the actual geometry after GetFinalPolyData()
        int vertexCount = obj->localMesh.vertexCount();
        int triangleCount = obj->localMesh.triangleCount();

        if (vertexCount == 0 || triangleCount == 0)
        {
            continue;
        }

        // Apply transformation matrix to vertices
        QVector<float> meshVertices(obj->localMesh.vertices.constData(),
                                     obj->localMesh.vertices.constData() + vertexCount * 3);
        QVector<float> transformedVertices = applyMatrixToVertices(meshVertices, obj->matrix);

        // Compute vertex normals from triangle data
        QVector<int> meshTriangles(obj->localMesh.triangles.constData(),
                                    obj->localMesh.triangles.constData() + triangleCount * 3);
        QVector<float> normals = computeVertexNormals(transformedVertices, meshTriangles);

        // Append vertices to global list
        allVertices.append(transformedVertices);
        allNormals.append(normals);

        // Append triangles with offset
        for (int i = 0; i < triangleCount * 3; i++)
        {
            allTriangles.append(obj->localMesh.triangles[i] + vertexOffset);
        }

        vertexOffset += vertexCount;
    }

    // Write all geometry to OBJ file
    if (!writeOBJGeometry(objFile, mtlName, visibleObjects, allVertices, allNormals, allTriangles))
    {
        objFile.close();
        return false;
    }

    objFile.close();
    return true;
}

/**
 *
 * Apply 4x4 transformation matrix to mesh vertex positions.
 *
 * @param vertices  Array of vertex positions [x0, y0, z0, x1, y1, z1, ...]
 * @param matrix    4x4 transformation matrix in row-major format
 * @return New vertex array with transforms applied
 *
 **/
QVector<float> OBJExporter::applyMatrixToVertices(
    const QVector<float> &vertices,
    const float matrix[16])
{
    QVector<float> result;
    result.reserve(vertices.size());

    // Process vertices in groups of 3 (x, y, z)
    for (int i = 0; i < vertices.size(); i += 3)
    {
        float x = vertices[i];
        float y = vertices[i + 1];
        float z = vertices[i + 2];

        // Apply 4x4 matrix: [x' y' z' w'] = [x y z 1] * matrix
        float x_new = matrix[0] * x + matrix[4] * y + matrix[8] * z + matrix[12];
        float y_new = matrix[1] * x + matrix[5] * y + matrix[9] * z + matrix[13];
        float z_new = matrix[2] * x + matrix[6] * y + matrix[10] * z + matrix[14];

        result.append(x_new);
        result.append(y_new);
        result.append(z_new);
    }

    return result;
}

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
QVector<float> OBJExporter::computeVertexNormals(
    const QVector<float> &vertices,
    const QVector<int> &triangles)
{
    int vertexCount = vertices.size() / 3;
    QVector<float> normals(vertexCount * 3, 0.0f);

    // Accumulate face normals for each vertex
    for (int i = 0; i < triangles.size(); i += 3)
    {
        int idx0 = triangles[i];
        int idx1 = triangles[i + 1];
        int idx2 = triangles[i + 2];

        // Get vertex positions
        float x0 = vertices[idx0 * 3];
        float y0 = vertices[idx0 * 3 + 1];
        float z0 = vertices[idx0 * 3 + 2];

        float x1 = vertices[idx1 * 3];
        float y1 = vertices[idx1 * 3 + 1];
        float z1 = vertices[idx1 * 3 + 2];

        float x2 = vertices[idx2 * 3];
        float y2 = vertices[idx2 * 3 + 1];
        float z2 = vertices[idx2 * 3 + 2];

        // Compute edge vectors
        float edge1x = x1 - x0;
        float edge1y = y1 - y0;
        float edge1z = z1 - z0;

        float edge2x = x2 - x0;
        float edge2y = y2 - y0;
        float edge2z = z2 - z0;

        // Compute cross product (face normal)
        float nx = edge1y * edge2z - edge1z * edge2y;
        float ny = edge1z * edge2x - edge1x * edge2z;
        float nz = edge1x * edge2y - edge1y * edge2x;

        // Accumulate normal at each vertex
        normals[idx0 * 3] += nx;
        normals[idx0 * 3 + 1] += ny;
        normals[idx0 * 3 + 2] += nz;

        normals[idx1 * 3] += nx;
        normals[idx1 * 3 + 1] += ny;
        normals[idx1 * 3 + 2] += nz;

        normals[idx2 * 3] += nx;
        normals[idx2 * 3 + 1] += ny;
        normals[idx2 * 3 + 2] += nz;
    }

    // Normalize all normal vectors
    for (int i = 0; i < normals.size(); i += 3)
    {
        float nx = normals[i];
        float ny = normals[i + 1];
        float nz = normals[i + 2];

        float length = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (length > 1e-6f)
        {
            normals[i] = nx / length;
            normals[i + 1] = ny / length;
            normals[i + 2] = nz / length;
        }
        else
        {
            normals[i] = 0.0f;
            normals[i + 1] = 0.0f;
            normals[i + 2] = 1.0f;
        }
    }

    return normals;
}

/**
 *
 * Write OBJ file header and geometry data.
 *
 * @param objFile           OBJ file open for writing
 * @param mtlName           Name of MTL file (written as reference in OBJ header)
 * @param visibleObjects    Objects to export
 * @param allVertices       All transformed vertices [x0, y0, z0, ...]
 * @param allNormals        All computed normals [nx0, ny0, nz0, ...]
 * @param allTriangles      All triangle indices
 * @return true if write succeeds
 *
 **/
bool OBJExporter::writeOBJGeometry(
    QFile &objFile,
    const QString &mtlName,
    const QList<SVObject*> &visibleObjects,
    const QVector<float> &allVertices,
    const QVector<float> &allNormals,
    const QVector<int> &allTriangles)
{
    QTextStream out(&objFile);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(6);

    // Write all vertices
    for (int i = 0; i < allVertices.size(); i += 3)
    {
        out << "v " << allVertices[i] << " "
            << allVertices[i + 1] << " "
            << allVertices[i + 2] << "\n";
    }

    out << "\n";

    // Write all normals
    for (int i = 0; i < allNormals.size(); i += 3)
    {
        out << "vn " << allNormals[i] << " "
            << allNormals[i + 1] << " "
            << allNormals[i + 2] << "\n";
    }

    out << "\n";

    // Write faces grouped by object
    int vertexOffset = 0;
    int triangleOffset = 0;

    for (SVObject *obj : visibleObjects)
    {
        if (obj->Isosurfaces.empty())
        {
            continue;
        }

        const Isosurface *iso = obj->Isosurfaces[0];
        int triangleCount = iso->nTriangles;

        // Write material assignment
        out << "usemtl " << obj->Name << "_Material\n";
        out << "g " << obj->Name << "\n";

        // Write faces for this object
        for (int i = 0; i < triangleCount * 3; i += 3)
        {
            int idx0 = allTriangles[triangleOffset + i] + 1;     // OBJ indices are 1-based
            int idx1 = allTriangles[triangleOffset + i + 1] + 1;
            int idx2 = allTriangles[triangleOffset + i + 2] + 1;

            out << "f " << idx0 << "//" << idx0 << " "
                << idx1 << "//" << idx1 << " "
                << idx2 << "//" << idx2 << "\n";
        }

        out << "\n";

        vertexOffset += iso->nVertices;
        triangleOffset += triangleCount * 3;
    }

    return true;
}

/**
 *
 * Write MTL file with material definitions for all objects.
 *
 * @param mtlFilepath   Path where MTL file will be written
 * @param visibleObjects Objects with material/color data
 * @return true if write succeeds
 *
 **/
bool OBJExporter::writeMTLFile(
    const QString &mtlFilepath,
    const QList<SVObject*> &visibleObjects)
{
    QFile mtlFile(mtlFilepath);
    if (!mtlFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&mtlFile);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(4);

    // Write header
    out << "# Material library exported from SPIERSview\n\n";

    // Write material for each object
    for (SVObject *obj : visibleObjects)
    {
        if (obj->Isosurfaces.empty())
        {
            continue;
        }

        out << "newmtl " << obj->Name << "_Material\n";

        // Normalize RGB to 0.0-1.0 range
        float r = obj->Colour[0] / 255.0f;
        float g = obj->Colour[1] / 255.0f;
        float b = obj->Colour[2] / 255.0f;

        // Ambient color (Ka) - usually same as diffuse
        out << "Ka " << r << " " << g << " " << b << "\n";

        // Diffuse color (Kd)
        out << "Kd " << r << " " << g << " " << b << "\n";

        // Specular color (Ks) - white for most objects
        out << "Ks 1.0 1.0 1.0\n";

        // Specular exponent (Ns) - map from shininess codes
        // Shininess: 0=0.8, 1=0.5, 2=0.2, 3=0.0 (roughness)
        // Specular exponent: 0.0 (dull) to 128.0 (shiny)
        // Map: roughness 0.8 -> Ns 4, roughness 0.0 -> Ns 128
        float roughness = 0.8f - (obj->Shininess * 0.2667f);  // 0-3 -> 0.8-0.0 (approx)
        float specularExponent = (1.0f - roughness) * 128.0f;
        out << "Ns " << specularExponent << "\n";

        // Alpha (dissolve factor d)
        // Transparency: 0-4 -> alpha: 1.0-0.0
        float alpha = 1.0f - (obj->Transparency * 0.25f);
        out << "d " << alpha << "\n";

        out << "\n";
    }

    mtlFile.close();
    return true;
}
