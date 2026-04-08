/**
 * @file
 * Header: Beamhardening
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
#ifndef BEAMHARDENING_H
#define BEAMHARDENING_H

#include <QImage>
#include <QList>
#include <QListWidget>
#include <QLabel>

class BeamHardening
{
public:
    BeamHardening();
    void Measure(QListWidget *SliceSelectorList, int cX, int cY, QLabel *label, int radius);
    int GetCorrectionAtWorkingImageCoordinates(int x, int y);
    bool HasSample();
    void SetParams(int x, int y, int radius, int adjust);
private:
    int *sampleBuffer, *sampleCountBuffer;
    int sampleBufferSize;
    long Sample(int file, int centerX, int centerY);
    bool IsLocked(int x, int y);
    int cx, cy, rad, adj;
};

#endif // BEAMHARDENING_H
