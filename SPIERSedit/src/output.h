/**
 * @file
 * Header: Output
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <QByteArray>
#include <QLabel>

extern void PopulateOutputArray (int *FullOutArrayCount, QByteArray *FullOutArray, int *Count, int FirstOutputFile, int LastOutputFile, int Object, int awidth, int aheight,  QLabel *pb);
extern bool freached(int f, int to, int from);
extern uchar ResampleForLoRes(int X, int Y, QByteArray *a);

#endif // __OUTPUT_H__
