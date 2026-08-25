/**
 * @file
 * Source: Marching Cubes
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

#include <cstdio>

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QTextStream>

#include "marchingcubes.h"
#include "compressedslice.h"
#include "globals.h"
#include "mainwindow.h"
#include "spv.h"
#include "svobject.h"
#include "ui_mainwindow.h"

// ============================================================================
// Macros
// ============================================================================

/// Array-style indexing into the scalar field: translates (i,j,k) to linear offset
#define OFFSET(I, J, K, IDIM, JDIM) ((I) + ((IDIM) * (J)) + ((IDIM * JDIM) * (K)))

/// Array-style indexing into the layer edge table: translates edge, position to offset
#define EDGE_OFFSET(E, I, J, IDIM) ((E) + (12 * (I)) + ((12 * (IDIM-1)) * (J-1)))

/// Interpolation epsilon tolerance for floating-point edge calculations
#define EPSILON 0.000001

/// Sentinel value marking an edge that has not yet been computed
#define EMPTY_EDGE -1

/// Grid size for acceleration structure (skip blank regions in sparse volumes)
#define LOCAL_GRIDSIZE 10

// edge lookup table
int MarchingCubes::edgeTable[256] =
{
    0x0, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33, 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa, 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66, 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff, 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55, 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66, 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa, 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33, 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99, 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

// triangle lookup table
int MarchingCubes::triTable[256][16] =
{
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

/**
 *
 * Construct a MarchingCubes algorithm processor for the given SVObject.
 * Initializes grid dimensions from the associated SPV (SPIERS Volume) data.
 *
 * The marching cubes algorithm was first published in the 1987 SIGGRAPH proceedings
 * by Lorensen and Cline. It is an algorithm for extracting a polygonal mesh of an
 * isosurface from a three-dimensional discrete scalar field.
 *
 * @param object Pointer to the SVObject whose scalar field will be processed
 *
 * @see Lorensen, W. E. and Cline, H. E., "Marching Cubes: A High Resolution 3D
 *      Surface Construction Algorithm," Computer Graphics, vol. 21, no. 3,
 *      pp. 163-169, July 1987.
 * @see Lorensen, W. E., "Marching Through the Visible Man," IEEE Visualization,
 *      Proceedings of the 6th conference on Visualization '95, pp. 368-373, 1994.
 *
 **/
MarchingCubes::MarchingCubes(SVObject *object)
{
    this->object = object;
    iDim = object->spv->iDim;
    jDim = object->spv->jDim;
    kDim = object->spv->kDim;
    pointcount = 0;
}

/**
 *
 * Extract the complete isosurface from the associated SVObject's scalar field.
 * Automatically selects between chunked and non-chunked processing modes based on
 * the volume data format. Updates the SVObject's voxel count upon completion.
 *
 * Non-chunked mode is used when the entire volume fits in memory (fullarray present).
 * Chunked mode is used for large volumes that are stored as compressed slice data.
 *
 **/
void MarchingCubes::surfaceObject()
{
    // Check whether data is in old (full array) or new (chunked) format
    if (object->spv->fullarray)
    {
        // Non-chunked: entire volume is decompressed in memory
        surfaceNonChunked();
    }
    else
    {
        // Chunked: volume processed slice-by-slice with memory constraints
        surfaceChunked();
    }

    // Store the voxel count in the SVObject
    object->voxels = pointcount;
}

/**
 *
 * Process a large scalar field volume in chunked mode, maintaining only a limited
 * set of slices (layers) in memory at once to accommodate memory constraints.
 *
 * This method manages six slice buffers in a sliding window, decompressing new slices
 * as needed and processing each layer through the marching cubes algorithm. Individual
 * isosurfaces are generated per layer and accumulated in the SVObject's isosurface list.
 *
 * Uses a grid acceleration structure to skip blank regions and improve performance on
 * sparse volumes. Includes progress updates for UI responsiveness.
 *
 **/
void MarchingCubes::surfaceChunked()
{
    QElapsedTimer t;
    t.start();

    ScalarFieldLayer *layer; // scalar field data and edges
    int i;
    int j;
    int k;
    int e;
    bool empty[6];
    int gridxscale = ((object->spv->iDim) / LOCAL_GRIDSIZE) + 1;
    int gridyscale = ((object->spv->jDim) / LOCAL_GRIDSIZE) + 1;

    int size = object->spv->size;

    //unsigned char *temppointer;

    //set up an empty array for use with slices flagged as empty
    unsigned char *blankpointer = static_cast<unsigned char *>(malloc(static_cast<size_t>(size)));
    for (int ii = 0; ii < size; ii++) blankpointer[ii] = static_cast<unsigned char>(0);

    // Set up the 6 slice buffers
    for (int ii = 0; ii < 6; ii++)
    {
        slicebuffers[ii] = static_cast<unsigned char *>(malloc(static_cast<size_t>(size)));
        // 2nd copy of pointer
        slicebuffers_copy[ii] = slicebuffers[ii];
    }

    // Decompress the first four layers
    // Blank the first two (-1 and -2)
    slicebuffers[0] = blankpointer;
    slicebuffers[1] = blankpointer;
    empty[0] = true;
    empty[1] = true;

    // ...and read in the next four, blanking if not present
    for (int ii = 0; ii < 4; ii++)
    {
        if (kDim > ii && object->compressedslices[ii]->empty == false) //no need to copy pointers from backup - not yet touched
            object->compressedslices[ii]->getFullData(slicebuffers[ii + 2]);
        else //blank them
        {
            slicebuffers[ii + 2] = blankpointer;
        }

        empty[ii + 2] = true;
        if (ii < object->compressedslices.count())
            if (object->compressedslices[ii]->empty == false)
                empty[ii + 2] = false;
    }

    layer = static_cast<ScalarFieldLayer *>(malloc(sizeof(ScalarFieldLayer)));
    if (layer == nullptr)
    {
        QCoreApplication::quit();
    }

    // initially, botData points to the start of dataset, and topData points to the next layer
    layer->botData = slicebuffers[2];
    layer->topData = slicebuffers[3];

    // allocate storage to hold the indexing tags for edges in the layer
    layer->edges = static_cast<int *>(malloc(static_cast<unsigned long long>(iDim - 1) * static_cast<unsigned long long>(jDim - 1) * 12 * sizeof(int)));

    if (layer->edges==NULL)
    {
        qDebug()<<"OOPS"<<static_cast<unsigned long long>(iDim - 1) * static_cast<unsigned long long>(jDim - 1) * 12 * sizeof(int);
    }
    // initialize the edge table to EMPTY
    for (i = 0; i < (iDim - 1) * (jDim - 1) * 12; i++)
    {
        *(layer->edges + i) = EMPTY_EDGE;
    }

    // Is this empty?? Surface it anyway I think - grid will control the work
    object->Isosurfaces.append(marchChunked(layer, 0, 0, object->compressedslices[0]->grid, gridxscale, gridyscale));
    totalTriangles += object->Isosurfaces[0]->nTriangles;
    int vertbase = object->Isosurfaces[0]->nVertices;

    // ...now do the remaining layers
    for (k = 1; k < kDim - 1; k++)
    {
        // Update the progress bar
        QString text;
        QTextStream string(&text);
        string << "Surfacing: slice " << k << "  - Ktr " << totalTriangles / 1000;
        mainWindow->ui->OutputLabelSpecific->setText(text);
        mainWindow->setSpecificLabel(text);
        mainWindow->setSpecificProgress(((k + 1) * 100) / kDim);
        if (k % 10 == 0) qApp->processEvents();

        // Move everything down in arrays by one
        unsigned char *temppointer = slicebuffers[0];
        slicebuffers[0] = slicebuffers[1];
        slicebuffers[1] = slicebuffers[2];
        slicebuffers[2] = slicebuffers[3];
        slicebuffers[3] = slicebuffers[4];
        slicebuffers[4] = slicebuffers[5];
        slicebuffers[5] = temppointer;

        temppointer = slicebuffers_copy[0];
        slicebuffers_copy[0] = slicebuffers_copy[1];
        slicebuffers_copy[1] = slicebuffers_copy[2];
        slicebuffers_copy[2] = slicebuffers_copy[3];
        slicebuffers_copy[3] = slicebuffers_copy[4];
        slicebuffers_copy[4] = slicebuffers_copy[5];
        slicebuffers_copy[5] = temppointer;

        bool t = empty[0];
        empty[0] = empty[1];
        empty[1] = empty[2];
        empty[2] = empty[3];
        empty[3] = empty[4];
        empty[4] = empty[5];
        empty[5] = t;

        // ...now decompress next slice into 5 - or zero it
        empty[5] = false;
        if (k + 3 >= object->compressedslices.count())
            empty[5] = true;
        else if (object->compressedslices[k + 3]->empty == true)
            empty[5] = true;

        if (empty[5] == false)
        {
            slicebuffers[5] = slicebuffers_copy[5]; //put in data pointer
            object->compressedslices[k + 3]->getFullData((slicebuffers[5]));
        }
        else slicebuffers[5] = blankpointer;

        //qDebug()<<"Slice"<<k+3<<"is"<<empty[5];

        // update the layer for this iteration
        layer->botData = slicebuffers[2];
        layer->topData = slicebuffers[3];


        if ((!(empty[1])) || (!(empty[2])) || (!(empty[3]))) //shift edges down if last slice generated any
        {
            // Percolate the last layer's top edges to this layer's bottom edges
            for (i = 0; i < iDim - 1; i++)
            {
                for (j = 1; j < jDim; j++)
                {
                    for (e = 0; e < 4; e++)
                        *(layer->edges + EDGE_OFFSET(e, i, j, iDim)) = *(layer->edges + EDGE_OFFSET(e + 4, i, j, iDim));


                    // Reinitialize all of the remaining edges
                    for (e = 4; e < 12; e++)
                    {
                        *(layer->edges + EDGE_OFFSET(e, i, j, iDim)) = EMPTY_EDGE;
                    }
                }
            }
        }

        // Do a march if this slice is not empty
        if ((!(empty[2])) || (!(empty[3])) || (!(empty[4])))
        {
            //qDebug()<<"Marching slice "<<k;
            object->Isosurfaces.append(marchChunked(layer, k, vertbase, object->compressedslices[k]->grid, gridxscale, gridyscale));
            //qDebug()<<"DONE Marching slice "<<k;
            vertbase += object->Isosurfaces[k]->nVertices;
            totalTriangles += object->Isosurfaces[k]->nTriangles;
        }
        else
        {
            //add a blank Isosurface
            //qDebug()<<"XXXX Skipping slice "<<k;
            Isosurface *blankiso = new Isosurface;
            object->Isosurfaces.append(blankiso);

        }
    }

    // Do clean up
    free(layer->edges);
    free(layer);
    for (int ii = 0; ii < 6; ii++) free(slicebuffers_copy[ii]);
    free (blankpointer);

    return;
}

/**
 *
 * Process the entire scalar field volume in non-chunked mode when the complete
 * volume is present in memory as a single contiguous fullarray.
 *
 * This method iterates through all layers (k-slices) of the volume, processing each
 * with the marching cubes algorithm. All vertices and triangles are accumulated into
 * a single isosurface object.
 *
 * This mode provides maximum efficiency for small-to-medium volumes that fit completely
 * in available memory. Includes progress updates for UI responsiveness.
 *
 **/
void MarchingCubes::surfaceNonChunked()
{
    //Old version
    ScalarFieldLayer *layer; // scalar field data and edges
    int i, j, k, e;

    unsigned char *dataset = object->spv->fullarray;
    // sprintf(dummy, "Old non-chunking version in MC\n"); DebugPrint(dummy);
    layer = static_cast<ScalarFieldLayer *>(malloc(sizeof(ScalarFieldLayer)));
    if (layer == nullptr)
        QCoreApplication::quit();

    /* initially, botData points to the start of dataset, and */
    /* topData points to the next layer */
    layer->botData = dataset;
    layer->topData = dataset + OFFSET(0, 0, 1, iDim, jDim);

    /* allocate storage to hold the indexing tags for edges in the layer */
    layer->edges = static_cast<int *>(malloc(static_cast<unsigned long long>(iDim - 1) * static_cast<unsigned long long>(jDim - 1) * 12 * sizeof(int)));

    /* initialize the edge table to EMPTY */
    for (i = 0; i < (iDim - 1) * (jDim - 1) * 12; i++)
    {
        *(layer->edges + i) = EMPTY_EDGE;
    }

    /* allocate the storage for the Isosurface and initialize */
    Isosurface *iso = new Isosurface();
    iso->vertices.resize(0);
    iso->triangles.resize(0);
    iso->trianglearraysize = 0;
    iso->vertexarraysize = 0;
    marchNonChunked(dataset, layer, 0, 128, iso);
    /* now do the remaining layers */
    for (k = 1; k < kDim - 1; k++)
    {
        QString text;
        QTextStream s(&text);
        s << "Surfacing: slice " << k << "  - Ktr " << (totalTriangles + iso->nTriangles) / 1000;
        mainWindow->setSpecificLabel(text);
        mainWindow->setSpecificProgress(((k + 1) * 100) / kDim);
        if (k % 10 == 0) qApp->processEvents();

        /* update the layer for this iteration */
        layer->botData = layer->topData;
        /* now topData points to next layer of scalar data*/
        layer->topData = dataset + OFFSET(0, 0, k + 1, iDim, jDim);

        /* percolate the last layer's top edges to this layer's bottom edges */
        for (i = 0; i < iDim - 1; i++)
        {
            for (j = 1; j < jDim; j++)
            {
                for (e = 0; e < 4; e++)
                    *(layer->edges + EDGE_OFFSET(e, i, j, iDim)) = *(layer->edges + EDGE_OFFSET(e + 4, i, j, iDim));


                /* reinitialize all of the remaining edges */
                for (e = 4; e < 12; e++)
                {
                    *(layer->edges + EDGE_OFFSET(e, i, j, iDim)) = EMPTY_EDGE;
                }
            }
        }
        marchNonChunked(dataset, layer, k, 128, iso);
    }

    totalTriangles += iso->nTriangles;

    /* clean up */
    free(layer->edges);
    free(layer);

    object->Isosurfaces.append(iso); //put the single iso in
}

/**
 *
 * Process a single layer of the volume using the non-chunked marching cubes algorithm.
 * Computes vertex and triangle data for all voxels in this layer and accumulates
 * results into the provided isosurface.
 *
 * Vertex caching is used to avoid redundant interpolation: edge vertices are cached
 * and propagated to adjacent cells within the same layer and to adjacent layers.
 *
 * @param dataset       Pointer to the complete volumetric scalar field data
 * @param layer         Pointer to the current layer's data pointers and edge cache
 * @param k             Layer index (z-coordinate) being processed
 * @param threshold     Isosurface threshold value for level-set extraction
 * @param iso           Output isosurface object to accumulate vertices and triangles
 *
 **/
void MarchingCubes::marchNonChunked(unsigned char *dataset, ScalarFieldLayer *layer, int k, float threshold, Isosurface *iso)
{

    Isosurface *layerIso; // portion of Isosurface in layer
    unsigned char cell[8]; // grid values at 8 cell vertices
    int cellVerts[12]; // verts at the 12 edges in a cell
    int i;
    int j;
    int ii;
    int jj;
    int e;
    unsigned char cellIndex; // index for edgeTable lookup

    /* initialize the cellVerts table */
    for (i = 0; i < 12; i++)
    {
        cellVerts[i] = EMPTY_EDGE;
    }

    /* allocate and initialize storage for this layer's portion of Isosurface */
    layerIso = new Isosurface;

    /// Pre-allocate based on layer dimensions to reduce reallocation steps during processing.
    /// A surface covering ~10% of cells is a practical initial estimate; floor at 1000.
    {
        int estimatedCells = qMax(1000, (iDim - 1) * (jDim - 1) / 10);
        layerIso->trianglearraysize = estimatedCells * 2;
        layerIso->vertexarraysize   = estimatedCells * 3;
        layerIso->triangles.resize(layerIso->trianglearraysize * 3);
        layerIso->vertices.resize(layerIso->vertexarraysize * 3);
    }

    //Counting
    for (i = 0; i < iDim; i++)
        for (j = 0; j < jDim; j++)
            if (*(layer->botData +   j   * iDim +   i)) pointcount++;

    /* for each cell in the grid... */
    for (i = 0; i < iDim - 1; i++)
    {
        for (j = 1; j < jDim; j++)
        {

            /* do the marching cubes thing */
            /* fill the cell */
            cell[0] = *(layer->botData +   j   * iDim +   i);
            cell[1] = *(layer->botData +   j   * iDim + i + 1);
            cell[2] = *(layer->botData + (j - 1) * iDim + i + 1);
            cell[3] = *(layer->botData + (j - 1) * iDim +   i);
            cell[4] = *(layer->topData +   j   * iDim +   i);
            cell[5] = *(layer->topData +   j   * iDim + i + 1);
            cell[6] = *(layer->topData + (j - 1) * iDim + i + 1);
            cell[7] = *(layer->topData + (j - 1) * iDim +   i);

            /* compute the index for the edge intersections */
            cellIndex = 0;
            if (cell[0] < threshold) cellIndex |=   1;
            if (cell[1] < threshold) cellIndex |=   2;
            if (cell[2] < threshold) cellIndex |=   4;
            if (cell[3] < threshold) cellIndex |=   8;
            if (cell[4] < threshold) cellIndex |=  16;
            if (cell[5] < threshold) cellIndex |=  32;
            if (cell[6] < threshold) cellIndex |=  64;
            if (cell[7] < threshold) cellIndex |= 128;

            /* get the coordinates for the vertices, compute the triangulation */
            for (int edgeIdx = 0; edgeIdx < 12; edgeIdx++)
            {
                if (edgeTable[cellIndex] & (1 << edgeIdx))
                {
                    int *edgeSlot = layer->edges + EDGE_OFFSET(edgeIdx, i, j, iDim);
                    if (*edgeSlot == EMPTY_EDGE)
                    {
                        cellVerts[edgeIdx] = iso->nVertices +
                                             makeVertex(edgeIdx, i, j, k, threshold, dataset, layerIso);
                    }
                    else
                    {
                        cellVerts[edgeIdx] = *edgeSlot;
                    }
                }
            }

            /* put the cellVerts tags into this cell's layer->edges table */
            for (e = 0; e < 12; e++)
            {
                if (cellVerts[e] != EMPTY_EDGE)
                {
                    *(layer->edges + EDGE_OFFSET(e, i, j, iDim)) = cellVerts[e];
                }
            }

            /* now propagate the vertex/normal tags to the adjacent cells to */
            /* the right and behind in this layer. */
            if (i < iDim - 2)   /* we should propagate to the right */
            {
                *(layer->edges + EDGE_OFFSET( 3, i + 1, j, iDim)) = cellVerts[1];
                *(layer->edges + EDGE_OFFSET( 7, i + 1, j, iDim)) = cellVerts[5];
                *(layer->edges + EDGE_OFFSET( 8, i + 1, j, iDim)) = cellVerts[9];
                *(layer->edges + EDGE_OFFSET(11, i + 1, j, iDim)) = cellVerts[10];
            }
            if (j < jDim - 1)   /* we should propagate to the rear */
            {
                *(layer->edges + EDGE_OFFSET( 2, i, j + 1, iDim)) = cellVerts[0];
                *(layer->edges + EDGE_OFFSET( 6, i, j + 1, iDim)) = cellVerts[4];
                *(layer->edges + EDGE_OFFSET(11, i, j + 1, iDim)) = cellVerts[8];
                *(layer->edges + EDGE_OFFSET(10, i, j + 1, iDim)) = cellVerts[9];
            }

            /* compute the triangulation */
            ii = 0;
            while (triTable[cellIndex][ii] != -1)
            {
                for (jj = 0; jj < 3; jj++)
                {
                    layerIso->triangles[jj + layerIso->nTriangles * 3] = cellVerts[triTable[cellIndex][ii++]];
                }

                if (++(layerIso->nTriangles) >= layerIso->trianglearraysize)
                {
                    layerIso->trianglearraysize += 1000;
                    layerIso->triangles.resize(layerIso->trianglearraysize * 3);
                }
            }

        }
    }

    /* now that we have the new verts, normals and triangles */
    /* calculated, we need to add them to the master lists   */


    /* if new verts, expand the storage for the master lists */
    if (layerIso->nVertices > 0)
    {
        iso->triangles.append(layerIso->triangles.constData(), layerIso->nTriangles * 3);
        iso->vertices.append(layerIso->vertices.constData(), layerIso->nVertices * 3);
        iso->nTriangles += layerIso->nTriangles;
        iso->nVertices += layerIso->nVertices;
    }

    delete layerIso;
}

/**
 *
 * Process a single layer of the volume using the chunked marching cubes algorithm.
 * This variant is optimized for large volumes where only a limited number of slices
 * can be held in memory. Returns a newly allocated isosurface containing vertices
 * and triangles for this layer only.
 *
 * Uses a grid acceleration structure (LOCAL_GRIDSIZE blocks) to skip regions of the volume
 * that are completely empty, improving performance on sparse data. Vertex caching
 * operates identically to the non-chunked version but results are isolated per-layer.
 *
 * @param layer         Pointer to the current layer's data pointers and edge cache
 * @param k             Layer index (z-coordinate) being processed
 * @param vertbase      Base index offset for vertices in the global vertex array
 * @param grid          Grid acceleration structure for blank region detection
 * @param gridxscale    X scale factor for grid indexing
 * @param gridyscale    Y scale factor for grid indexing
 * @return Newly allocated Isosurface object containing this layer's mesh data
 *
 **/
Isosurface *MarchingCubes::marchChunked(ScalarFieldLayer *layer, int k, int vertbase, unsigned char *grid, int gridxscale, int gridyscale)
{
    Q_UNUSED(grid)
    Q_UNUSED(gridxscale)

    Isosurface *layerIso; // portion of Isosurface in layer
    unsigned char cell[8]; // grid values at 8 cell vertices
    int cellVerts[12]; // verts at the 12 edges in a cell
    int i;
    int j;
    int ii;
    int jj;
    int e;
    //int vertSize; // allocation for verts and normals
    //int triSize; // allocation for triangles
    unsigned char cellIndex; // xindex for edgeTable lookup

    // initialize the cellVerts table
    for (i = 0; i < 12; i++)
    {
        cellVerts[i] = EMPTY_EDGE;
    }

    // allocate and initialize storage for this layer's portion of Isosurface
    layerIso = new Isosurface;

    /// Pre-allocate based on layer dimensions to reduce reallocation steps during processing.
    /// A surface covering ~10% of cells is a practical initial estimate; floor at 1000.
    {
        int estimatedCells = qMax(1000, (iDim - 1) * (jDim - 1) / 10);
        layerIso->trianglearraysize = estimatedCells * 2;
        layerIso->vertexarraysize   = estimatedCells * 3;
        layerIso->triangles.resize(layerIso->trianglearraysize * 3);
        layerIso->vertices.resize(layerIso->vertexarraysize * 3);
    }

    //do counting
    for (i = 0; i < iDim; i++)
        for (j = 0; j < jDim; j++)
            if (*(layer->botData +   j   * iDim +   i)) pointcount++;

    for (i = 0; i < iDim - 1; i++)
    {
        bool flag = false;
        for (int gj = 0; gj < gridyscale; gj++)
        {
            //if (grid[i/LOCAL_GRIDSIZE + gj*gridxscale])
            {
                flag = true;
                int ymax = gj * LOCAL_GRIDSIZE + LOCAL_GRIDSIZE;
                if (ymax > jDim) ymax = jDim;
                for (j = gj * LOCAL_GRIDSIZE; j < ymax; j++)
                {
                    if (j == 0) j++; //should really start at 1!
                    //March

                    // fill the cell
                    cell[0] = *(layer->botData +   j   * iDim +   i);
                    cell[1] = *(layer->botData +   j   * iDim + i + 1);
                    cell[2] = *(layer->botData + (j - 1) * iDim + i + 1);
                    cell[3] = *(layer->botData + (j - 1) * iDim +   i);
                    cell[4] = *(layer->topData +   j   * iDim +   i);
                    cell[5] = *(layer->topData +   j   * iDim + i + 1);
                    cell[6] = *(layer->topData + (j - 1) * iDim + i + 1);
                    cell[7] = *(layer->topData + (j - 1) * iDim +   i);


                    // compute the index for the edge intersections
                    cellIndex = 0;
                    if (cell[0]) cellIndex |=   1;
                    if (cell[1]) cellIndex |=   2;
                    if (cell[2]) cellIndex |=   4;
                    if (cell[3]) cellIndex |=   8;
                    if (cell[4]) cellIndex |=  16;
                    if (cell[5]) cellIndex |=  32;
                    if (cell[6]) cellIndex |=  64;
                    if (cell[7]) cellIndex |= 128;

                    /* get the coordinates for the vertices, compute the triangulation */
                    for (int edgeIdx = 0; edgeIdx < 12; edgeIdx++)
                    {
                        if (edgeTable[cellIndex] & (1 << edgeIdx))
                        {
                            int *edgeSlot = layer->edges + EDGE_OFFSET(edgeIdx, i, j, iDim);
                            if (*edgeSlot == EMPTY_EDGE)
                            {
                                cellVerts[edgeIdx] = makeVertexFast(edgeIdx, i, j, k, layerIso, vertbase);
                            }
                            else
                            {
                                cellVerts[edgeIdx] = *edgeSlot;
                            }
                        }
                    }

                    /* put the cellVerts tags into this cell's layer->edges table */
                    for (e = 0; e < 12; e++)
                    {
                        if (cellVerts[e] != EMPTY_EDGE)
                        {
                            *(layer->edges + EDGE_OFFSET(e, i, j, iDim)) = cellVerts[e];
                        }
                    }

                    /* now propagate the vertex/normal tags to the adjacent cells to */
                    /* the right and behind in this layer. */
                    if (i < iDim - 2)   /* we should propagate to the right */
                    {
                        *(layer->edges + EDGE_OFFSET( 3, i + 1, j, iDim)) = cellVerts[1];
                        *(layer->edges + EDGE_OFFSET( 7, i + 1, j, iDim)) = cellVerts[5];
                        *(layer->edges + EDGE_OFFSET( 8, i + 1, j, iDim)) = cellVerts[9];
                        *(layer->edges + EDGE_OFFSET(11, i + 1, j, iDim)) = cellVerts[10];
                    }
                    if (j < jDim - 1)   /* we should propagate to the rear */
                    {
                        *(layer->edges + EDGE_OFFSET( 2, i, j + 1, iDim)) = cellVerts[0];
                        *(layer->edges + EDGE_OFFSET( 6, i, j + 1, iDim)) = cellVerts[4];
                        *(layer->edges + EDGE_OFFSET(11, i, j + 1, iDim)) = cellVerts[8];
                        *(layer->edges + EDGE_OFFSET(10, i, j + 1, iDim)) = cellVerts[9];
                    }

                    ii = 0;
                    while (triTable[cellIndex][ii] != -1)
                    {
                        for (jj = 0; jj < 3; jj++)
                        {
                            layerIso->triangles[jj + layerIso->nTriangles * 3] = cellVerts[triTable[cellIndex][ii++]];
                        }

                        if (++(layerIso->nTriangles) >= layerIso->trianglearraysize)
                        {
                            layerIso->trianglearraysize += 500;
                            layerIso->triangles.resize(layerIso->trianglearraysize * 3);
                        }
                    }

                }
            }
        }
        if (flag == false) i += (LOCAL_GRIDSIZE - 1); //if all cells blank - can skip to next grid column!
    }

    //do memory squeeze

    layerIso->triangles.resize(layerIso->nTriangles * 3);
    layerIso->vertices.resize(layerIso->nVertices * 3);

    return layerIso;
}

/**
 *
 * Create a vertex on a cube edge where the scalar field crosses the isosurface threshold.
 * Uses linear interpolation between the edge's two endpoint values to determine the
 * exact position of the isosurface crossing.
 *
 * The vertex coordinates are stored at twice the grid resolution (2x scaling) to preserve
 * precision when working with integer coordinates. Results are accumulated in the provided
 * isosurface's vertex array with automatic allocation growth.
 *
 * @param whichEdge     Index of the cube edge (0-11) where vertex lies
 * @param i             Grid position X coordinate
 * @param j             Grid position Y coordinate
 * @param k             Grid position Z coordinate
 * @param threshold     Isosurface threshold value for level-set computation
 * @param dataset       Pointer to complete volumetric scalar field
 * @param layerIso      Output isosurface to accumulate this vertex
 * @return Index of the newly created vertex in the isosurface vertex array
 *
 **/
int MarchingCubes::makeVertex(int whichEdge, int i, int j, int k, float threshold, unsigned char *dataset, Isosurface *layerIso)
{

    int from[3]; // first grid vertex
    int to[3]; // second grid vertex
    float v[3]; // the interpolated vertex
    float d; // relative distance along edge for interpolation
    int ii;

    switch (whichEdge)
    {
    case 0:
        from[0] =  i;
        from[1] =  j;
        from[2] =  k;
        to[0] = i + 1;
        to[1] =  j;
        to[2] =  k;
        break;
    case 1:
        from[0] = i + 1;
        from[1] =  j;
        from[2] =  k;
        to[0] = i + 1;
        to[1] = j - 1;
        to[2] =  k;
        break;
    case 2:
        from[0] = i + 1;
        from[1] = j - 1;
        from[2] =  k;
        to[0] =  i;
        to[1] = j - 1;
        to[2] =  k;
        break;
    case 3:
        from[0] =  i;
        from[1] = j - 1;
        from[2] =  k;
        to[0] =  i;
        to[1] =  j;
        to[2] =  k;
        break;
    case 4:
        from[0] =  i;
        from[1] =  j;
        from[2] = k + 1;
        to[0] = i + 1;
        to[1] =  j;
        to[2] = k + 1;
        break;
    case 5:
        from[0] = i + 1;
        from[1] =  j;
        from[2] = k + 1;
        to[0] = i + 1;
        to[1] = j - 1;
        to[2] = k + 1;
        break;
    case 6:
        from[0] = i + 1;
        from[1] = j - 1;
        from[2] = k + 1;
        to[0] =  i;
        to[1] = j - 1;
        to[2] = k + 1;
        break;
    case 7:
        from[0] =  i;
        from[1] = j - 1;
        from[2] = k + 1;
        to[0] =  i;
        to[1] =  j;
        to[2] = k + 1;
        break;
    case 8:
        from[0] =  i;
        from[1] =  j;
        from[2] =  k;
        to[0] =  i;
        to[1] =  j;
        to[2] = k + 1;
        break;
    case 9:
        from[0] = i + 1;
        from[1] =  j;
        from[2] =  k;
        to[0] = i + 1;
        to[1] =  j;
        to[2] = k + 1;
        break;
    case 10:
        from[0] = i + 1;
        from[1] = j - 1;
        from[2] =  k;
        to[0] = i + 1;
        to[1] = j - 1;
        to[2] = k + 1;
        break;
    case 11:
        from[0] =  i;
        from[1] = j - 1;
        from[2] =  k;
        to[0] =  i;
        to[1] = j - 1;
        to[2] = k + 1;
        break;
    default:
        fprintf(stderr, "MarchingCubes: makeVertex: bad edge index\n");
        QCoreApplication::exit();
    }

    /* determine the relative distance along edge from->to */
    /* that the Isosurface vertex lies */
    d = ( *(dataset + OFFSET(from[0], from[1], from[2], iDim, jDim)) - threshold) /
        ( *(dataset + OFFSET(from[0], from[1], from[2], iDim, jDim)) -
          * (dataset + OFFSET(to[0], to[1], to[2], iDim, jDim)) );
    if (d < static_cast<float>(EPSILON))
    {
        d = 0.0;
    }
    else if (d > static_cast<float>((1 - EPSILON)))
    {
        d = 1.0;
    }

    v[0] = static_cast<int>(2 * (from[0] + d * (to[0] - from[0])));
    v[1] = static_cast<int>(2 * (from[1] + d * (to[1] - from[1])));
    v[2] = static_cast<int>(2 * (from[2] + d * (to[2] - from[2])));

    /* insert the vertex into the Isosurface structure */
    /* compute the tag for this vertex, and return the tag        */

    for (ii = 0; ii < 3; ii++)
    {
        layerIso->vertices[ii + layerIso->nVertices * 3] = static_cast<int>(v[ii]);
    }
    if (++(layerIso->nVertices) >= layerIso->vertexarraysize)
    {
        layerIso->vertexarraysize += 500;
        layerIso->vertices.resize(layerIso->vertexarraysize * 3);
    }
    return layerIso->nVertices - 1;
}

/**
 *
 * Create a vertex on a cube edge using a fast lookup-table approach optimized for
 * chunked processing where only grid endpoints (not interpolated positions) are needed.
 *
 * This is a performance-critical optimization that replaces a 12-case switch statement
 * with a static lookup table of edge coordinate offsets. Each edge's position relative
 * to its cube is precomputed and stored in a compact table that remains in L1 cache.
 *
 * The vertex coordinates are stored at twice the grid resolution (2x scaling) to preserve
 * precision. Results are accumulated in the provided isosurface's vertex array with
 * automatic allocation growth.
 *
 * @param whichEdge     Index of the cube edge (0-11) where vertex lies
 * @param i             Grid position X coordinate
 * @param j             Grid position Y coordinate
 * @param k             Grid position Z coordinate
 * @param layerIso      Output isosurface to accumulate this vertex
 * @param vertbase      Base index offset for vertices in the global vertex array
 * @return Index of the newly created vertex in the isosurface vertex array
 *
 **/
int MarchingCubes::makeVertexFast(int whichEdge, int i, int j, int k, Isosurface *layerIso, int vertbase)
{
    /// Precomputed (dx, dy, dz) offsets from (2i, 2j, 2k) for each of the 12 cube edges.
    /// Replaces the 12-case switch: table fits in 3 cache lines and stays hot in L1.
    /// Values verified directly against the original switch cases.
    static const int edgeOffsets[12][3] =
    {
        { 1,  0,  0},  /// edge  0: midpoint of bottom-front edge
        { 2, -1,  0},  /// edge  1: midpoint of bottom-right edge
        { 1, -2,  0},  /// edge  2: midpoint of bottom-back edge
        { 0, -1,  0},  /// edge  3: midpoint of bottom-left edge
        { 1,  0,  2},  /// edge  4: midpoint of top-front edge
        { 2, -1,  2},  /// edge  5: midpoint of top-right edge
        { 1, -2,  2},  /// edge  6: midpoint of top-back edge
        { 0, -1,  2},  /// edge  7: midpoint of top-left edge
        { 0,  0,  1},  /// edge  8: midpoint of front-left vertical edge
        { 2,  0,  1},  /// edge  9: midpoint of front-right vertical edge
        { 2, -2,  1},  /// edge 10: midpoint of back-right vertical edge
        { 0, -2,  1},  /// edge 11: midpoint of back-left vertical edge
    };

    const int *off = edgeOffsets[whichEdge];
    const int base = layerIso->nVertices * 3;
    layerIso->vertices[base]     = 2 * i + off[0];
    layerIso->vertices[base + 1] = 2 * j + off[1];
    layerIso->vertices[base + 2] = 2 * k + off[2];

    if (++(layerIso->nVertices) >= layerIso->vertexarraysize)
    {
        layerIso->vertexarraysize += 500;
        layerIso->vertices.resize(layerIso->vertexarraysize * 3);
    }

    return layerIso->nVertices - 1 + vertbase;
}
