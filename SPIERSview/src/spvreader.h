/**
 * @file
 * Header: SPV Reader
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef SPVREADER_H
#define SPVREADER_H

#include <QString>
#include <QProgressBar>
#include <QLabel>

class SPV;

/**
 * @brief The SPVReader class
 */
class SPVReader
{
public:
    SPVReader();
    void processFile(QString filename);
    int processFileReplacement(QString filename, int spvIndex);

private:
    int replaceIndex;
    QString currentFilename;

    void fixKeyCodeData();
    void internalProcessFile(QString filename);
    void fileReadFailed(QString filename, bool write, int n);
    int processSPV(QString filename, float *passedMatrix);
    void version5Below(QString filename, float *passedMatrix);
    void version6Plus(QString filename);
};

#endif // SPVREADER_H
