/**
 * @file
 * Header: SPV
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

#ifndef SPV_H
#define SPV_H

#include <QList>
#include <QStringList>

class SVObject;

/**
 * @brief The SPV class
 */
class SPV
{
public:
    SPV(int index, int FileVersion, int iDim, int jDim, int kDim);
    ~SPV();

    bool MirrorFlag;

    float bright;

    double *stretches;
    double PixPerMM;
    double SlicePerMM;
    double SkewDown;
    double SkewLeft;

    int FileVersion;
    int GridSize;
    int iDim;
    int jDim;
    int kDim;
    int index;
    int size;

    QList <SVObject *> ComponentObjects;
    QString filename;
    QString filenamenopath;

    unsigned char *fullarray;
    unsigned char *AllData;
};

extern QList <SPV *> SPVs;

#endif // SPV_H
