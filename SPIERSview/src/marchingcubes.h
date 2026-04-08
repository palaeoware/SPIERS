/**
 * @file
 * Header: Marching Cubes
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

#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include <QVarLengthArray>

#include "isosurface.h"
#include "scalarfieldlayer.h"
#include "svobject.h"

/**
 *
 * Implements the Marching Cubes algorithm for extracting polygonal isosurfaces
 * from volumetric scalar field data. Supports both chunked (memory-limited) and
 * non-chunked (full-volume) processing modes.
 *
 **/
class MarchingCubes
{
public:
    /**
     *
     * Construct a MarchingCubes processor for the given SVObject.
     *
     * @param object Pointer to the SVObject whose scalar field will be processed
     *
     **/
    MarchingCubes(SVObject *object);

    /**
     *
     * Extract the complete isosurface from the associated SVObject's scalar field.
     * Automatically selects chunked or non-chunked processing based on memory constraints.
     *
     **/
    void surfaceObject();

private:
    /// Grid dimension along i axis (X direction)
    int iDim;

    /// Grid dimension along j axis (Y direction)
    int jDim;

    /// Grid dimension along k axis (Z direction)
    int kDim;

    /// Total number of scalar field points
    int pointcount;

    /// Pointer to the SVObject being processed
    SVObject *object;

    /// Static lookup table: which edges of a cube are intersected (256 entries, 1 per corner configuration)
    static int edgeTable[];

    /// Static lookup table: triangle vertex indices for each corner configuration (256 rows x 16 columns)
    static int triTable[][16];

    /// Array of slice buffers for chunked processing (up to 6 slices in memory at once)
    unsigned char *slicebuffers[6];

    /// Backup copies of slice buffer pointers for restoration after processing
    unsigned char *slicebuffers_copy[6];

    // --- Private helper methods ---

    /**
     *
     * Process a single layer using the chunked algorithm with precomputed grid data.
     * Used for datasets that exceed memory limits and must be processed slice-by-slice.
     *
     * @param layer         Pointer to the current scalar field layer
     * @param k             Layer index (z-coordinate)
     * @param vertbase      Base vertex index for this layer's output
     * @param grid          Grid acceleration structure for blank region skipping
     * @param gridxscale    X scaling factor for grid
     * @param gridyscale    Y scaling factor for grid
     * @return Pointer to the isosurface generated from this layer
     *
     **/
    Isosurface *marchChunked(
        ScalarFieldLayer *layer,
        int k,
        int vertbase,
        unsigned char *grid,
        int gridxscale,
        int gridyscale);

    /**
     *
     * Process a single layer using the non-chunked algorithm with direct dataset access.
     * Used when the entire volume fits in memory for maximum efficiency.
     *
     * @param dataset       Pointer to the complete scalar field volume
     * @param layer         Pointer to the current scalar field layer
     * @param k             Layer index (z-coordinate)
     * @param threshold     Isosurface value threshold
     * @param iso           Pointer to output isosurface object
     *
     **/
    void marchNonChunked(
        unsigned char *dataset,
        ScalarFieldLayer *layer,
        int k,
        float threshold,
        Isosurface *iso);

    /**
     *
     * Create a vertex on a cube edge where the scalar field crosses the isosurface threshold.
     * Used in non-chunked mode where full dataset is available for interpolation.
     *
     * @param whichEdge     Index of the cube edge (0-11)
     * @param i             Grid position (x coordinate)
     * @param j             Grid position (y coordinate)
     * @param k             Grid position (z coordinate)
     * @param threshold     Isosurface value threshold
     * @param dataset       Pointer to complete scalar field data
     * @param layerIso      Pointer to output isosurface
     * @return Index of the newly created vertex in the isosurface vertex array
     *
     **/
    int makeVertex(
        int whichEdge,
        int i,
        int j,
        int k,
        float threshold,
        unsigned char *dataset,
        Isosurface *layerIso);

    /**
     *
     * Create a vertex on a cube edge using fast coordinate assignment.
     * Optimized version for chunked mode that uses precomputed edge offset lookup table.
     *
     * @param whichEdge     Index of the cube edge (0-11)
     * @param i             Grid position (x coordinate)
     * @param j             Grid position (y coordinate)
     * @param k             Grid position (z coordinate)
     * @param layerIso      Pointer to output isosurface
     * @param vertbase      Base vertex index offset for this layer
     * @return Index of the newly created vertex in the isosurface vertex array
     *
     **/
    int makeVertexFast(
        int whichEdge,
        int i,
        int j,
        int k,
        Isosurface *layerIso,
        int vertbase);

    /**
     *
     * Process the complete scalar field volume in non-chunked mode.
     * Iterates through all voxels and applies the marching cubes algorithm.
     *
     **/
    void surfaceNonChunked();

    /**
     *
     * Process the scalar field volume in chunked mode, slice-by-slice.
     * Maintains a limited set of slices in memory to accommodate large datasets.
     *
     **/
    void surfaceChunked();
};

#endif // MARCHINGCUBES_H
