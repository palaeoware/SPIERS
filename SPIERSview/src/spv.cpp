/**
 * @file
 * Source: Spv
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */
#include <QDebug>

#include "spv.h"
#include "globals.h"

/**
 * @brief SPVs
 */
QList <SPV *> SPVs;

/**
 * @brief SPV::SPV
 * @param Index
 * @param version
 * @param x
 * @param y
 * @param z
 */
SPV::SPV(int Index, int version, int x, int y, int z)
{
    index = Index;
    FileVersion = version;
    iDim = x;
    jDim = y;
    kDim = z;
    size = iDim * jDim;
    GridSize = (iDim / GRIDSIZE + 1) * (jDim / GRIDSIZE + 1);
    //qDebug()<<"Grid size is "<<GridSize;
    fullarray = nullptr;
    stretches = nullptr;
}

/**
 * @brief SPV::~SPV
 */
SPV::~SPV()
{
    if (stretches) free(stretches);
}
