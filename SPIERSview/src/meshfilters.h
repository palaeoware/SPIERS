/**
 * @file
 * Header: Mesh Filters
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

#ifndef MESHFILTERS_H
#define MESHFILTERS_H

#include <QVector>
#include <functional>

struct MeshData;

/**
 * @brief The MeshAdjacency struct
 * Adjacency data for a mesh, built on demand before filtering.
 *
 * vertexTriangles[v] = list of triangle indices that contain vertex v
 * vertexNeighbours[v] = list of vertex indices sharing an edge with v
 * isBoundary[v]       = true if vertex lies on a boundary edge
 */
struct MeshAdjacency
{
    QVector<QVector<int>> vertexTriangles;
    QVector<QVector<int>> vertexNeighbours;
    QVector<bool>         isBoundary;

    void build(const MeshData &mesh);
    bool isValid() const { return !vertexTriangles.isEmpty(); }
};

/**
 * @brief The MeshFilters namespace
 * Pure functions — input is never modified, a new MeshData is always returned.
 */
namespace MeshFilters
{
/**
     * @brief removeIslands
     * Removes disconnected mesh components below a size threshold.
     *
     * islandRemovalCode:
     *   0        = off
     *   1        = remove tiny   (< 20 triangles)
     *   2        = remove small  (< 100 triangles)
     *   3        = remove medium (< 600 triangles)
     *   4        = remove large  (< 4000 triangles)
     *   5        = keep largest island only
     *   negative = custom: remove islands with fewer than abs(code) triangles
     */
MeshData removeIslands(const MeshData &input, const MeshAdjacency &adj,
                       int islandRemovalCode);

/**
     * @brief smooth
     * Taubin smoothing approximating vtkWindowedSincPolyDataFilter (PassBand=0.05).
     * Boundary vertices are held fixed.
     *
     * smoothingCode:
     *   0 = off
     *   1 = very weak  (5 VTK iters * scale)
     *   2 = weak       (10 * scale)
     *   3 = medium     (20 * scale)
     *   4 = strongish  (40 * scale)
     *   5 = strong     (60 * scale)
     *   6 = strongest  (100 * scale)
     *   negative = custom: abs(code) Taubin iterations
     *
     * progressCallback(currentPass, totalPasses) — called every 10 passes.
     */
MeshData smooth(const MeshData &input, const MeshAdjacency &adj,
                int smoothingCode,
                std::function<void(int,int)> progressCallback = nullptr);

/**
     * @brief decimate
     * Mesh decimation (fidelity reduction) via edge collapse.
     *
     * resamplePercent: percentage of triangles to KEEP (1-100).
     *   100 = no decimation (fast path)
     *   50  = keep 50% of triangles
     *   1   = keep 1% (minimum, clamped from 0)
     *
     * resampleType:
     *   0 = fast decimation — edge collapse with plane-distance cost
     *       (approximates vtkDecimatePro, PreserveTopologyOn)
     *   1 = quality decimation — quadric error metrics (QEM)
     *       (approximates vtkQuadricDecimation)
     *
     * progressCallback(current, total) — called periodically during collapse.
     */
MeshData decimate(const MeshData &input, const MeshAdjacency &adj,
                  int resamplePercent, int resampleType,
                  std::function<void(int,int)> progressCallback = nullptr);

/**
     * @brief compactMesh
     * Shared utility: removes unkept triangles and unreferenced vertices,
     * remapping indices to be contiguous.
     */
MeshData compactMesh(const MeshData &input, const QVector<int> &keepTri);
}

#endif // MESHFILTERS_H
