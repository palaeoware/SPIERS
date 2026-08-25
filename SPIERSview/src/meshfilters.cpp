/**
 * @file
 * Mesh Filters
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

#include "meshfilters.h"
#include "svobject.h"

#include <QHash>
#include <QSet>
#include <QtConcurrent/QtConcurrent>
#include <climits>
#include <cmath>
#include <queue>
#include <algorithm>

// ============================================================================
// Parallel chunk helpers
// ============================================================================

/**
 * @brief Range
 * Simple index range used to split work across threads.
 * begin/end follow STL half-open convention [begin, end).
 * chunkIndex is set explicitly at creation to avoid any pointer arithmetic.
 */
struct Range
{
    int begin;
    int end;
    int chunkIndex;
};

/**
 * @brief makeRanges
 * Splits [0, total) into evenly-sized chunks, one per logical CPU core.
 * minChunkSize prevents thread overhead dominating for small meshes.
 */
static QVector<Range> makeRanges(int total, int minChunkSize = 1000)
{
    int nThreads = QThread::idealThreadCount();
    if (nThreads < 1) nThreads = 1;

    int chunkSize = qMax(minChunkSize, (total + nThreads - 1) / nThreads);

    QVector<Range> ranges;
    int idx = 0;
    for (int start = 0; start < total; start += chunkSize, idx++)
        ranges.append({start, qMin(start + chunkSize, total), idx});

    return ranges;
}

// ============================================================================
// MeshAdjacency
// ============================================================================

/**
 * @brief MeshAdjacency::build
 * Builds vertex->triangle, vertex->neighbour adjacency lists, and boundary
 * vertex flags for a mesh.
 *
 * The fill pass (pass 2) is sequential because addIfAbsent neighbour
 * deduplication has per-vertex write contention that isn't worth locking.
 * The pre-reservation from pass 1 avoids repeated QVector resizing.
 */
void MeshAdjacency::build(const MeshData &mesh)
{
    int nVerts = mesh.vertexCount();
    int nTris  = mesh.triangleCount();

    vertexTriangles.clear();
    vertexTriangles.resize(nVerts);
    vertexNeighbours.clear();
    vertexNeighbours.resize(nVerts);
    isBoundary.fill(false, nVerts);

    // Pre-reservation pass (sequential) — avoids repeated QVector resizing
    // in the fill pass below. Simple triangle scan, very fast.
    {
        QVector<int> triRefCount(nVerts, 0);
        for (int t = 0; t < nTris; t++)
        {
            triRefCount[mesh.triangles[t * 3]]++;
            triRefCount[mesh.triangles[t * 3 + 1]]++;
            triRefCount[mesh.triangles[t * 3 + 2]]++;
        }
        for (int v = 0; v < nVerts; v++)
        {
            vertexTriangles[v].reserve(triRefCount[v]);
            vertexNeighbours[v].reserve(triRefCount[v]);
        }
    }

    // Pass 2 (sequential): fill adjacency and detect boundary edges
    QHash<QPair<int,int>, int> edgeCount;
    edgeCount.reserve(nTris * 3);

    auto addIfAbsent = [](QVector<int> &list, int val) {
        if (!list.contains(val)) list.append(val);
    };

    for (int t = 0; t < nTris; t++)
    {
        int v[3] = {
            mesh.triangles[t * 3],
            mesh.triangles[t * 3 + 1],
            mesh.triangles[t * 3 + 2]
        };

        for (int i = 0; i < 3; i++)
        {
            vertexTriangles[v[i]].append(t);

            int a = v[i], b = v[(i + 1) % 3];
            addIfAbsent(vertexNeighbours[a], b);
            addIfAbsent(vertexNeighbours[b], a);

            QPair<int,int> edge(qMin(a,b), qMax(a,b));
            edgeCount[edge]++;
        }
    }

    for (auto it = edgeCount.constBegin(); it != edgeCount.constEnd(); ++it)
    {
        if (it.value() == 1)
        {
            isBoundary[it.key().first]  = true;
            isBoundary[it.key().second] = true;
        }
    }
}

// ============================================================================
// Union-Find helpers
// ============================================================================

static int ufFind(QVector<int> &parent, int x)
{
    while (parent[x] != x)
    {
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}

static void ufUnion(QVector<int> &parent, QVector<int> &rank, int a, int b)
{
    int ra = ufFind(parent, a);
    int rb = ufFind(parent, b);
    if (ra == rb) return;
    if (rank[ra] < rank[rb]) qSwap(ra, rb);
    parent[rb] = ra;
    if (rank[ra] == rank[rb]) rank[ra]++;
}

// ============================================================================
// MeshFilters::compactMesh
// ============================================================================

MeshData MeshFilters::compactMesh(const MeshData &input, const QVector<int> &keepTri)
{
    int nVerts = input.vertexCount();
    int nTris  = input.triangleCount();

    // Step 1 (sequential): mark used vertices and build remapping
    // Sequential here is fine — O(T) with simple array access is very fast
    QVector<int> oldToNew(nVerts, -1);
    {
        QVector<int> vertexUsed(nVerts, 0);
        for (int t = 0; t < nTris; t++)
        {
            if (!keepTri[t]) continue;
            vertexUsed[input.triangles[t * 3]]     = 1;
            vertexUsed[input.triangles[t * 3 + 1]] = 1;
            vertexUsed[input.triangles[t * 3 + 2]] = 1;
        }
        int newVertCount = 0;
        for (int v = 0; v < nVerts; v++)
            if (vertexUsed[v]) oldToNew[v] = newVertCount++;
    }

    // Count new vertices and triangles
    int newVertCount = 0;
    for (int v = 0; v < nVerts; v++)
        if (oldToNew[v] >= 0) newVertCount++;

    int newTriCount = 0;
    QVector<int> triOutIdx(nTris, -1);
    for (int t = 0; t < nTris; t++)
        if (keepTri[t]) triOutIdx[t] = newTriCount++;

    MeshData output;
    output.vertices.resize(newVertCount * 3);
    output.triangles.resize(newTriCount * 3);

    // Step 2 (sequential): copy vertices
    for (int v = 0; v < nVerts; v++)
    {
        int nv = oldToNew[v];
        if (nv < 0) continue;
        output.vertices[nv * 3]     = input.vertices[v * 3];
        output.vertices[nv * 3 + 1] = input.vertices[v * 3 + 1];
        output.vertices[nv * 3 + 2] = input.vertices[v * 3 + 2];
    }

    // Step 3 (sequential): copy triangles with remapped indices
    for (int t = 0; t < nTris; t++)
    {
        int outTri = triOutIdx[t];
        if (outTri < 0) continue;
        output.triangles[outTri * 3]     = oldToNew[input.triangles[t * 3]];
        output.triangles[outTri * 3 + 1] = oldToNew[input.triangles[t * 3 + 1]];
        output.triangles[outTri * 3 + 2] = oldToNew[input.triangles[t * 3 + 2]];
    }

    return output;
}

// ============================================================================
// MeshFilters::removeIslands
// ============================================================================

MeshData MeshFilters::removeIslands(const MeshData &input, const MeshAdjacency & /*adj*/, int islandRemovalCode)
{
    int minTriangles;
    switch (islandRemovalCode)
    {
    case 0:  return input;
    case 1:  minTriangles = 20;      break;
    case 2:  minTriangles = 100;     break;
    case 3:  minTriangles = 600;     break;
    case 4:  minTriangles = 4000;    break;
    case 5:  minTriangles = INT_MAX; break;
    default: minTriangles = (islandRemovalCode < 0) ? -islandRemovalCode : 0; break;
    }

    int nVerts = input.vertexCount();
    int nTris  = input.triangleCount();
    if (nVerts == 0 || nTris == 0) return input;

    // Union-find (sequential — dependencies between operations)
    QVector<int> parent(nVerts);
    QVector<int> rank(nVerts, 0);
    for (int i = 0; i < nVerts; i++) parent[i] = i;

    for (int t = 0; t < nTris; t++)
    {
        ufUnion(parent, rank, input.triangles[t * 3], input.triangles[t * 3 + 1]);
        ufUnion(parent, rank, input.triangles[t * 3], input.triangles[t * 3 + 2]);
    }

    // Fully compress all paths so parallel reads need no further modification
    for (int i = 0; i < nVerts; i++) ufFind(parent, i);

    // Count triangles per component (parallel — parent[] is now read-only)
    QVector<Range> ranges = makeRanges(nTris);
    int nChunks = ranges.size();
    QVector<QHash<int,int>> chunkCounts(nChunks);

    QtConcurrent::blockingMap(ranges, [&](const Range &r) {
        QHash<int,int> &counts = chunkCounts[r.chunkIndex];
        for (int t = r.begin; t < r.end; t++)
            counts[parent[input.triangles[t * 3]]]++;  // parent[] fully compressed — no ufFind needed
    });

    // Reduce
    QHash<int,int> componentTriCount;
    componentTriCount.reserve(1024);
    for (int c = 0; c < nChunks; c++)
        for (auto it = chunkCounts[c].constBegin(); it != chunkCounts[c].constEnd(); ++it)
            componentTriCount[it.key()] += it.value();

    // Find largest if needed
    int largestRoot = -1;
    if (islandRemovalCode == 5)
    {
        int largestCount = 0;
        for (auto it = componentTriCount.constBegin(); it != componentTriCount.constEnd(); ++it)
        {
            if (it.value() > largestCount)
            {
                largestCount = it.value();
                largestRoot  = it.key();
            }
        }
    }

    // Build keepTri mask (parallel — parent[] and componentTriCount are read-only)
    QVector<int> keepTri(nTris, 0);
    QtConcurrent::blockingMap(ranges, [&](const Range &r) {
        for (int t = r.begin; t < r.end; t++)
        {
            int root = parent[input.triangles[t * 3]];
            if (islandRemovalCode == 5)
            {
                if (root == largestRoot) keepTri[t] = 1;
            }
            else
            {
                if (componentTriCount.value(root, 0) >= minTriangles) keepTri[t] = 1;
            }
        }
    });

    return compactMesh(input, keepTri);
}

// ============================================================================
// MeshFilters::smooth  (Taubin smoothing)
// ============================================================================

static const int    TAUBIN_ITER_SCALE = 3;
static const double TAUBIN_LAMBDA     =  0.6307;
static const double TAUBIN_MU         = -0.6732;

/**
 * @brief taubinPass
 * Parallel Taubin half-step using a delta buffer (Jacobi update).
 *
 * delta is allocated by the caller and passed in — this guarantees its
 * lifetime exceeds both blockingMap calls, avoiding any risk of the
 * lambda capturing a reference to a stack variable that could be reused
 * by the thread pool between calls.
 *
 * Pass 1: all threads read vertices[] (read-only) and write to their own
 *         non-overlapping range of delta[] — no contention.
 * Pass 2: all threads read delta[] and write to their own non-overlapping
 *         range of vertices[] — no contention.
 * The blockingMap barrier between passes ensures pass 1 is fully complete
 * before any thread begins pass 2.
 */
static void taubinPass(QVector<float> &vertices, QVector<float> &delta,
                       const MeshAdjacency &adj, double stepSize, int nVerts,
                       const QVector<Range> &ranges)
{
    Q_UNUSED(nVerts)
    // Pass 1: compute deltas (parallel, read-only access to vertices[])
    // No need to zero delta first — boundary vertices are skipped in pass 2,
    // so stale values for those entries are never read.
    QtConcurrent::blockingMap(ranges, [&](const Range &r) {
        for (int v = r.begin; v < r.end; v++)
        {
            if (adj.isBoundary[v]) continue;

            const QVector<int> &neighbours = adj.vertexNeighbours[v];
            int degree = neighbours.size();
            if (degree == 0) continue;

            float sumX = 0.0f, sumY = 0.0f, sumZ = 0.0f;
            for (int n : neighbours)
            {
                sumX += vertices[n * 3];
                sumY += vertices[n * 3 + 1];
                sumZ += vertices[n * 3 + 2];
            }

            float invDeg = static_cast<float>(stepSize) / static_cast<float>(degree);
            delta[v * 3]     = invDeg * (sumX - static_cast<float>(degree) * vertices[v * 3]);
            delta[v * 3 + 1] = invDeg * (sumY - static_cast<float>(degree) * vertices[v * 3 + 1]);
            delta[v * 3 + 2] = invDeg * (sumZ - static_cast<float>(degree) * vertices[v * 3 + 2]);
        }
    });

    // Pass 2: apply deltas (parallel, each thread writes its own range only)
    QtConcurrent::blockingMap(ranges, [&](const Range &r) {
        for (int v = r.begin; v < r.end; v++)
        {
            if (adj.isBoundary[v]) continue;
            vertices[v * 3]     += delta[v * 3];
            vertices[v * 3 + 1] += delta[v * 3 + 1];
            vertices[v * 3 + 2] += delta[v * 3 + 2];
        }
    });
}

MeshData MeshFilters::smooth(const MeshData &input, const MeshAdjacency &adj,
                             int smoothingCode,
                             std::function<void(int,int)> progressCallback)
{
    int iterations;
    switch (smoothingCode)
    {
    case 0:  return input;
    case 1:  iterations = 5   * TAUBIN_ITER_SCALE; break;
    case 2:  iterations = 10  * TAUBIN_ITER_SCALE; break;
    case 3:  iterations = 20  * TAUBIN_ITER_SCALE; break;
    case 4:  iterations = 40  * TAUBIN_ITER_SCALE; break;
    case 5:  iterations = 60  * TAUBIN_ITER_SCALE; break;
    case 6:  iterations = 100 * TAUBIN_ITER_SCALE; break;
    default: iterations = (smoothingCode < 0) ? -smoothingCode : 0; break;
    }

    if (iterations == 0) return input;

    int nVerts = input.vertexCount();
    if (nVerts == 0) return input;

    MeshData output = input;

    // Force QVector copy-on-write detach BEFORE any parallel work.
    // QVector uses implicit sharing — "output = input" is a shallow copy
    // that shares the same data buffer. The first write in a parallel
    // taubinPass would trigger a COW detach mid-parallel-execution,
    // which is not thread-safe. Forcing detach here ensures output.vertices
    // owns its own buffer before any thread touches it.
    output.vertices.detach();

    int totalPasses = iterations * 2;
    int passNum = 0;

    // Allocate delta buffer and ranges once — reused across all iterations.
    QVector<float> delta(nVerts * 3, 0.0f);
    QVector<Range> ranges = makeRanges(nVerts, 2000);

    for (int iter = 0; iter < iterations; iter++)
    {
        taubinPass(output.vertices, delta, adj, TAUBIN_LAMBDA, nVerts, ranges);
        ++passNum;
        if (progressCallback && (passNum % 10 == 0 || passNum == totalPasses))
            progressCallback(passNum, totalPasses);

        taubinPass(output.vertices, delta, adj, TAUBIN_MU, nVerts, ranges);
        ++passNum;
        if (progressCallback && (passNum % 10 == 0 || passNum == totalPasses))
            progressCallback(passNum, totalPasses);
    }

    return output;
}

// ============================================================================
// MeshFilters::decimate
// ============================================================================

/*
 * Edge collapse decimation framework.
 *
 * Both ResampleType 0 (fast, plane-distance cost) and ResampleType 1 (QEM)
 * use the same edge-collapse engine. Only the cost function differs.
 *
 * Algorithm overview:
 *   1. Build per-vertex quadric matrices (type 1) or plane sets (type 0)
 *   2. For every edge, compute collapse cost and optimal collapse point
 *   3. Insert all edges into a min-heap ordered by cost
 *   4. Repeatedly extract the cheapest edge, collapse it, update affected
 *      edges in the heap, until the target triangle count is reached
 *   5. Compact the mesh (remove deleted vertices/triangles)
 *
 * Topology preservation (matching vtkDecimatePro PreserveTopologyOn):
 *   - Never collapse a boundary edge
 *   - Never collapse an edge if it would cause a triangle flip
 *     (checked via normal consistency test)
 *   - Never collapse an edge that would create a non-manifold configuration
 *     (checked via link condition: the 1-ring intersection of the two
 *      endpoints must contain exactly 2 vertices for interior edges)
 *
 * Parallelisation note: the collapse loop is inherently sequential due to
 * data dependencies (each collapse changes the neighbourhood). However,
 * step 1 (quadric initialisation) is embarrassingly parallel and step 2
 * (initial cost computation) can be parallelised over edges.
 */


// ----------------------------------------------------------------------------
// 4x4 symmetric matrix for quadric error metrics
// Stored as upper triangle: q[0..9] = {a,b,c,d, e,f,g, h,i, j}
// representing the matrix:
//   [a b c d]
//   [b e f g]
//   [c f h i]
//   [d g i j]
// ----------------------------------------------------------------------------
struct Quadric
{
    double q[10];

    Quadric() { memset(q, 0, sizeof(q)); }

    // Add fundamental error quadric for plane ax+by+cz+d=0
    // Q = [a b c d]^T [a b c d] — outer product of plane normal+offset
    void addPlane(double a, double b, double c, double d)
    {
        q[0] += a*a; q[1] += a*b; q[2] += a*c; q[3] += a*d;
        q[4] += b*b; q[5] += b*c; q[6] += b*d;
        q[7] += c*c; q[8] += c*d;
        q[9] += d*d;
    }

    // Evaluate quadric error at point (x,y,z)
    // error = v^T Q v  where v = (x,y,z,1)
    double eval(double x, double y, double z) const
    {
        return q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x
               +   q[4]*y*y + 2*q[5]*y*z + 2*q[6]*y
               +   q[7]*z*z + 2*q[8]*z
               +   q[9];
    }

    // Add another quadric
    Quadric &operator+=(const Quadric &o)
    {
        for (int i = 0; i < 10; i++) q[i] += o.q[i];
        return *this;
    }
};

// ----------------------------------------------------------------------------
// Edge collapse candidate
// ----------------------------------------------------------------------------
struct CollapseCandidate
{
    double cost;
    int    v0, v1;       // edge vertices (v0 < v1 always)
    float  px, py, pz;  // optimal collapse point

    bool operator>(const CollapseCandidate &o) const { return cost > o.cost; }
};

// ----------------------------------------------------------------------------
// Compute the normal of triangle (v0,v1,v2) — returns false if degenerate
// ----------------------------------------------------------------------------
static bool triNormal(const QVector<float> &verts, int v0, int v1, int v2,
                      double &nx, double &ny, double &nz)
{
    double ax = verts[v1*3]   - verts[v0*3];
    double ay = verts[v1*3+1] - verts[v0*3+1];
    double az = verts[v1*3+2] - verts[v0*3+2];
    double bx = verts[v2*3]   - verts[v0*3];
    double by = verts[v2*3+1] - verts[v0*3+1];
    double bz = verts[v2*3+2] - verts[v0*3+2];
    nx = ay*bz - az*by;
    ny = az*bx - ax*bz;
    nz = ax*by - ay*bx;
    double len = sqrt(nx*nx + ny*ny + nz*nz);
    if (len < 1e-12) return false;
    nx /= len; ny /= len; nz /= len;
    return true;
}

// ----------------------------------------------------------------------------
// Link condition check for topology preservation.
// The collapse of edge (v0,v1) is safe iff the intersection of their
// 1-ring neighbourhoods contains exactly 2 vertices (the shared neighbours).
// Collapsing when there are more shared neighbours would create non-manifold
// geometry (T-junctions, handles merging, etc.)
// ----------------------------------------------------------------------------
static bool linkConditionOK(int v0, int v1,
                            const QVector<QVector<int>> &neighbours,
                            const QVector<bool> &deleted)
{
    // Count vertices in both neighbourhoods (excluding v0 and v1 themselves)
    int sharedCount = 0;
    for (int n : neighbours[v0])
    {
        if (n == v1 || deleted[n]) continue;
        for (int m : neighbours[v1])
        {
            if (m == v0 || deleted[m]) continue;
            if (n == m) { sharedCount++; break; }
        }
    }
    // For a manifold interior edge exactly 2 shared neighbours expected
    // For boundary edge: 1 shared neighbour expected
    // We allow the collapse if sharedCount <= 2 to be conservative
    return sharedCount <= 2;
}

// ----------------------------------------------------------------------------
// Check that collapsing (v0,v1) to point p won't flip any triangle normals
// ----------------------------------------------------------------------------
static bool noFlips(int v0, int v1,
                    float px, float py, float pz,
                    const QVector<float> &verts,
                    const QVector<QVector<int>> &vertTris,
                    const QVector<int> &triV0,
                    const QVector<int> &triV1,
                    const QVector<int> &triV2,
                    const QVector<bool> &triDeleted)
{
    // Check all triangles containing v0 or v1 (that don't contain both)
    auto checkVertex = [&](int v, int other) -> bool {
        for (int t : vertTris[v])
        {
            if (triDeleted[t]) continue;
            int a = triV0[t], b = triV1[t], c = triV2[t];
            // Skip triangles containing the other endpoint — they'll be removed
            if (a == other || b == other || c == other) continue;

            // Compute normal before collapse
            double nx0, ny0, nz0;
            if (!triNormal(verts, a, b, c, nx0, ny0, nz0)) continue;

            // Temporarily substitute v->p and recompute normal
            // Make a tiny local copy of the 3 positions
            float va[3] = {verts[a*3], verts[a*3+1], verts[a*3+2]};
            float vb[3] = {verts[b*3], verts[b*3+1], verts[b*3+2]};
            float vc[3] = {verts[c*3], verts[c*3+1], verts[c*3+2]};
            if (a == v) { va[0]=px; va[1]=py; va[2]=pz; }
            if (b == v) { vb[0]=px; vb[1]=py; vb[2]=pz; }
            if (c == v) { vc[0]=px; vc[1]=py; vc[2]=pz; }

            double ax = vb[0]-va[0], ay = vb[1]-va[1], az = vb[2]-va[2];
            double bx = vc[0]-va[0], by = vc[1]-va[1], bz = vc[2]-va[2];
            double nx1 = ay*bz-az*by, ny1 = az*bx-ax*bz, nz1 = ax*by-ay*bx;
            double len = sqrt(nx1*nx1+ny1*ny1+nz1*nz1);
            if (len < 1e-12) return false; // degenerate after collapse

            nx1/=len; ny1/=len; nz1/=len;
            // If dot product of old and new normal is below threshold, it
            // flipped or came dangerously close to flipping — reject.
            if (nx0*nx1 + ny0*ny1 + nz0*nz1 < 0.1) return false;
        }
        return true;
    };

    return checkVertex(v0, v1) && checkVertex(v1, v0);
}

// ----------------------------------------------------------------------------
// Compute collapse cost and optimal point for one edge
// Type 0: cost = squared distance from midpoint to adjacent triangle planes
// Type 1: cost = QEM error at optimal point (or midpoint if matrix singular)
// ----------------------------------------------------------------------------
static CollapseCandidate computeCollapse(int v0, int v1,
                                         const QVector<float> &verts,
                                         const QVector<Quadric> &quadrics,
                                         int resampleType)
{
    CollapseCandidate c;
    c.v0 = v0; c.v1 = v1;

    float mx = (verts[v0*3]   + verts[v1*3])   * 0.5f;
    float my = (verts[v0*3+1] + verts[v1*3+1]) * 0.5f;
    float mz = (verts[v0*3+2] + verts[v1*3+2]) * 0.5f;

    Quadric Q = quadrics[v0];
    Q += quadrics[v1];

    // Compute edge length squared — used to validate optimal point position
    double ex = verts[v1*3]   - verts[v0*3];
    double ey = verts[v1*3+1] - verts[v0*3+1];
    double ez = verts[v1*3+2] - verts[v0*3+2];
    double edgeLenSq = ex*ex + ey*ey + ez*ez;

    if (resampleType == 1)
    {
        // QEM: try to find optimal point by solving dQ/dx=dQ/dy=dQ/dz=0
        double a00=Q.q[0], a01=Q.q[1], a02=Q.q[2], b0=-Q.q[3];
        double              a11=Q.q[4], a12=Q.q[5], b1=-Q.q[6];
        double                           a22=Q.q[7], b2=-Q.q[8];

        double det = a00*(a11*a22 - a12*a12)
                     - a01*(a01*a22 - a12*a02)
                     + a02*(a01*a12 - a11*a02);

        bool useOptimal = false;
        if (fabs(det) > 1e-10)
        {
            double ix = (b0*(a11*a22-a12*a12) - a01*(b1*a22-a12*b2) + a02*(b1*a12-a11*b2)) / det;
            double iy = (a00*(b1*a22-a12*b2) - b0*(a01*a22-a12*a02) + a02*(a01*b2-b1*a02)) / det;
            double iz = (a00*(a11*b2-b1*a12) - a01*(a01*b2-b1*a02) + b0*(a01*a12-a11*a02)) / det;

            // Validate: optimal point must lie within 2x edge length of midpoint.
            // If it's further away the quadric is poorly conditioned — use midpoint.
            double dpx = ix - mx, dpy = iy - my, dpz = iz - mz;
            double distSq = dpx*dpx + dpy*dpy + dpz*dpz;
            if (distSq <= 4.0 * edgeLenSq)
            {
                c.px = static_cast<float>(ix);
                c.py = static_cast<float>(iy);
                c.pz = static_cast<float>(iz);
                c.cost = Q.eval(ix, iy, iz);
                useOptimal = true;
            }
        }

        if (!useOptimal)
        {
            c.px = mx; c.py = my; c.pz = mz;
            c.cost = Q.eval(mx, my, mz);
        }
    }
    else
    {
        // Type 0: midpoint with quadric cost
        c.px = mx; c.py = my; c.pz = mz;
        c.cost = Q.eval(mx, my, mz);
    }

    if (c.cost < 0.0) c.cost = 0.0;
    return c;
}

MeshData MeshFilters::decimate(const MeshData &input, const MeshAdjacency & /*adj*/,
                               int resamplePercent, int resampleType,
                               std::function<void(int,int)> progressCallback)
{
    // Clamp and fast-path
    resamplePercent = qMax(1, qMin(100, resamplePercent));
    if (resamplePercent == 100) return input;

    int nVerts = input.vertexCount();
    int nTris  = input.triangleCount();
    if (nVerts == 0 || nTris == 0) return input;

    int targetTris = qMax(1, (nTris * resamplePercent) / 100);
    if (targetTris >= nTris) return input;

    // -----------------------------------------------------------------------
    // Working copies — we modify these in place during collapse
    // -----------------------------------------------------------------------
    QVector<float> verts = input.vertices;
    verts.detach();

    // Per-triangle vertex indices (flat arrays for cache efficiency)
    QVector<int> triV0(nTris), triV1(nTris), triV2(nTris);
    for (int t = 0; t < nTris; t++)
    {
        triV0[t] = input.triangles[t*3];
        triV1[t] = input.triangles[t*3+1];
        triV2[t] = input.triangles[t*3+2];
    }

    QVector<bool> triDeleted(nTris, false);
    QVector<bool> vertDeleted(nVerts, false);

    // Per-vertex triangle lists (mutable — updated during collapse)
    QVector<QVector<int>> vertTris(nVerts);
    for (int t = 0; t < nTris; t++)
    {
        vertTris[triV0[t]].append(t);
        vertTris[triV1[t]].append(t);
        vertTris[triV2[t]].append(t);
    }

    // Per-vertex neighbour lists (mutable)
    QVector<QVector<int>> neighbours(nVerts);
    {
        auto addIfAbsent = [](QVector<int> &list, int val) {
            if (!list.contains(val)) list.append(val);
        };
        for (int t = 0; t < nTris; t++)
        {
            int a=triV0[t], b=triV1[t], c=triV2[t];
            addIfAbsent(neighbours[a], b); addIfAbsent(neighbours[a], c);
            addIfAbsent(neighbours[b], a); addIfAbsent(neighbours[b], c);
            addIfAbsent(neighbours[c], a); addIfAbsent(neighbours[c], b);
        }
    }

    // Boundary detection
    QVector<bool> isBoundary(nVerts, false);
    {
        QHash<QPair<int,int>,int> edgeCount;
        edgeCount.reserve(nTris * 3);
        for (int t = 0; t < nTris; t++)
        {
            int v[3] = {triV0[t], triV1[t], triV2[t]};
            for (int i = 0; i < 3; i++)
            {
                QPair<int,int> e(qMin(v[i],v[(i+1)%3]), qMax(v[i],v[(i+1)%3]));
                edgeCount[e]++;
            }
        }
        for (auto it = edgeCount.constBegin(); it != edgeCount.constEnd(); ++it)
            if (it.value() == 1)
            {
                isBoundary[it.key().first]  = true;
                isBoundary[it.key().second] = true;
            }
    }

    // -----------------------------------------------------------------------
    // Step 1: Build per-vertex quadrics
    // Each triangle contributes its plane's fundamental error quadric to all
    // three of its vertices. Using quadrics for both type 0 and type 1 —
    // type 0 just uses midpoint instead of optimal point.
    // -----------------------------------------------------------------------
    QVector<Quadric> quadrics(nVerts);
    for (int t = 0; t < nTris; t++)
    {
        double nx, ny, nz;
        if (!triNormal(verts, triV0[t], triV1[t], triV2[t], nx, ny, nz)) continue;
        // Plane equation: nx*x + ny*y + nz*z + d = 0
        // where d = -dot(normal, vertex)
        double d = -(nx*verts[triV0[t]*3] + ny*verts[triV0[t]*3+1] + nz*verts[triV0[t]*3+2]);
        quadrics[triV0[t]].addPlane(nx, ny, nz, d);
        quadrics[triV1[t]].addPlane(nx, ny, nz, d);
        quadrics[triV2[t]].addPlane(nx, ny, nz, d);
    }

    // -----------------------------------------------------------------------
    // Step 2: Build initial priority queue of all edges
    // Using a std::priority_queue (min-heap via greater<>)
    // Edges are stored canonically as (min(v0,v1), max(v0,v1))
    // -----------------------------------------------------------------------
    // We use a "lazy deletion" heap — when an edge is updated, we push a new
    // entry. Old entries are ignored when popped if their cost no longer
    // matches or the vertices are deleted.
    // This avoids the expensive decrease-key operation.
    using MinHeap = std::priority_queue<CollapseCandidate,
                                        std::vector<CollapseCandidate>,
                                        std::greater<CollapseCandidate>>;
    MinHeap heap;

    // Track which edges exist (canonical form)
    QSet<QPair<int,int>> edgeSet;
    for (int v = 0; v < nVerts; v++)
    {
        for (int n : neighbours[v])
        {
            if (n <= v) continue; // canonical: only push v0<v1
            QPair<int,int> e(v, n);
            if (edgeSet.contains(e)) continue;
            edgeSet.insert(e);
            heap.push(computeCollapse(v, n, verts, quadrics, resampleType));
        }
    }

    // -----------------------------------------------------------------------
    // Step 3: Iterative edge collapse
    // -----------------------------------------------------------------------
    int currentTris = nTris;
    int collapsesTotal = nTris - targetTris; // approximate
    int collapsesDone  = 0;
    int progressStep   = qMax(1, collapsesTotal / 100);

    // Cost threshold: stop collapsing when the cheapest remaining collapse
    // exceeds this value. This prevents severe quality degradation at low
    // fidelity — the mesh will stop decimating rather than produce
    // degenerate geometry. The threshold is expressed as a multiple of the
    // mean squared edge length of the mesh, computed once before decimation.
    // A multiplier of 1.0 is quite conservative; increase to allow more
    // aggressive decimation at the cost of quality.
    double maxCostThreshold;
    {
        double meanEdgeLenSq = 0.0;
        int edgeCount2 = 0;
        for (int t = 0; t < nTris && edgeCount2 < 10000; t++) // sample first 10k tris
        {
            auto edgeLen = [&](int a, int b) {
                double dx = verts[a*3]-verts[b*3];
                double dy = verts[a*3+1]-verts[b*3+1];
                double dz = verts[a*3+2]-verts[b*3+2];
                return dx*dx+dy*dy+dz*dz;
            };
            meanEdgeLenSq += edgeLen(triV0[t], triV1[t]);
            meanEdgeLenSq += edgeLen(triV1[t], triV2[t]);
            meanEdgeLenSq += edgeLen(triV2[t], triV0[t]);
            edgeCount2 += 3;
        }
        meanEdgeLenSq = (edgeCount2 > 0) ? meanEdgeLenSq / edgeCount2 : 1.0;
        // Allow collapses up to 1000x mean edge length squared.
        // This is generous enough to reach low fidelities on most meshes
        // while blocking truly degenerate collapses.
        maxCostThreshold = meanEdgeLenSq * 1000.0;
    }

    while (currentTris > targetTris && !heap.empty())
    {
        CollapseCandidate c = heap.top();
        heap.pop();

        int v0 = c.v0, v1 = c.v1;

        // Skip if either vertex deleted (lazy deletion)
        if (vertDeleted[v0] || vertDeleted[v1]) continue;

        // Stop if cost exceeds threshold — remaining collapses would
        // produce unacceptable geometry
        if (c.cost > maxCostThreshold) break;

        // Skip boundary edges (topology preservation)
        if (isBoundary[v0] && isBoundary[v1]) continue;

        // Skip if link condition violated (would create non-manifold)
        if (!linkConditionOK(v0, v1, neighbours, vertDeleted)) continue;

        // Skip if any triangle normal would flip
        if (!noFlips(v0, v1, c.px, c.py, c.pz, verts,
                     vertTris, triV0, triV1, triV2, triDeleted)) continue;

        // ----------------------------------------------------------------
        // Perform the collapse: merge v1 into v0
        // v0 moves to collapse point; v1 is deleted
        // All triangles referencing v1 are updated to reference v0,
        // except those containing both v0 and v1 (they are deleted)
        // ----------------------------------------------------------------
        verts[v0*3]   = c.px;
        verts[v0*3+1] = c.py;
        verts[v0*3+2] = c.pz;

        // Update quadric of v0 to be the combined quadric
        quadrics[v0] += quadrics[v1];

        // Process triangles containing v1
        for (int t : vertTris[v1])
        {
            if (triDeleted[t]) continue;

            int &a = triV0[t], &b = triV1[t], &c2 = triV2[t];
            bool hasV0 = (a==v0 || b==v0 || c2==v0);

            if (hasV0)
            {
                // Triangle contains both v0 and v1 — delete it
                triDeleted[t] = true;
                currentTris--;
                // Remove from v0's triangle list (will be rebuilt below)
            }
            else
            {
                // Remap v1->v0
                if (a == v1) a = v0;
                if (b == v1) b = v0;
                if (c2 == v1) c2 = v0;
                vertTris[v0].append(t);
            }
        }

        // Update neighbour lists
        // v0 gains all of v1's neighbours (except v1 itself)
        for (int n : neighbours[v1])
        {
            if (n == v0 || vertDeleted[n]) continue;
            // Remove v1 from n's neighbour list, add v0
            neighbours[n].removeOne(v1);
            if (!neighbours[n].contains(v0)) neighbours[n].append(v0);
            if (!neighbours[v0].contains(n)) neighbours[v0].append(n);
        }
        neighbours[v0].removeOne(v1);
        vertDeleted[v1] = true;

        // Update boundary flag for v0
        // v0 becomes boundary if either endpoint was boundary
        if (isBoundary[v1]) isBoundary[v0] = true;

        // Recompute collapse costs for all edges incident to v0
        for (int n : neighbours[v0])
        {
            if (vertDeleted[n]) continue;
            int a = qMin(v0, n), b = qMax(v0, n);
            heap.push(computeCollapse(a, b, verts, quadrics, resampleType));
        }

        collapsesDone++;
        if (progressCallback && (collapsesDone % progressStep == 0))
            progressCallback(qMin(collapsesDone, collapsesTotal), collapsesTotal);
    }

    if (progressCallback) progressCallback(collapsesTotal, collapsesTotal);

    // -----------------------------------------------------------------------
    // Step 4: Compact into output MeshData
    // -----------------------------------------------------------------------
    // Build keepTri mask
    QVector<int> keepTri(nTris);
    for (int t = 0; t < nTris; t++)
        keepTri[t] = triDeleted[t] ? 0 : 1;

    // Rebuild input-style flat triangle array for compactMesh
    MeshData working;
    working.vertices = verts;
    working.triangles.resize(nTris * 3);
    for (int t = 0; t < nTris; t++)
    {
        working.triangles[t*3]   = triV0[t];
        working.triangles[t*3+1] = triV1[t];
        working.triangles[t*3+2] = triV2[t];
    }

    return compactMesh(working, keepTri);
}
