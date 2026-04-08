/**
 * @file
 * Source: Staticfunctions
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
#include "staticfunctions.h"

#include <QMatrix4x4>

/**
 * @brief StaticFunctions::TransposeMatrix
 * @param m
 */
void StaticFunctions::transposeMatrix(float *matrix)
{
    QMatrix4x4 thematrix(matrix);
    thematrix = thematrix.transposed();
    thematrix.copyDataTo(matrix);
}

/**
 * @brief StaticFunctions::invertEndian
 * @param data
 * @param count
 */
void StaticFunctions::invertEndian(unsigned char *data, int count)
{
#ifdef _BIG_ENDIAN
    unsigned char newdata[8]; //max size is double
    int n;

    for (n = 0; n < count; n++) newdata[count - 1 - n] = data[n];
    for (n = 0; n < count; n++) data[n] = newdata[n];
#else
    Q_UNUSED(data)
    Q_UNUSED(count)
#endif
    return;
}
