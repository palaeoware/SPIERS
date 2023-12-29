/**
 * @file
 * Header: Compressed Slice
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef COMPRESSEDSLICE_H
#define COMPRESSEDSLICE_H

#include <QString>

class SVObject;

/**
 * @brief The CompressedSlice class
 */
class CompressedSlice
{
public:
    CompressedSlice(SVObject *parent, bool isEmpty);
    ~CompressedSlice();

    void merge(CompressedSlice *slice, QString filename);
    void dump(QString filename);
    void getFullData(unsigned char *);

    unsigned char *data;
    unsigned char *grid;
    bool empty;
    int datasize;
    SVObject *svObject;
};

#endif // COMPRESSEDSLICE_H
