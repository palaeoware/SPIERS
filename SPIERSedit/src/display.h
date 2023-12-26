/**
 * @file
 * Header: Display
 *
 * All SPIERSedit code is released under the GNU General Public License.
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

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QString>
#include "globals.h"
#include "beamhardening.h"

extern void ShowImage(QGraphicsView *gv);
extern void InitImage(QGraphicsView *gv);
extern void DeleteDisplayObjects();
extern void ClearImages();
extern void MakeLinearGreyScale(int seg, int fnum, bool flag);
extern void ApplyLCE(int seg, int fnum, bool flag);
extern void ApplyGradient(int seg, int fnum);
extern void ApplyRadial(int seg, int fnum, BeamHardening *bh, bool flag);
extern void MakeBlankGreyScale(int seg, int fnum, bool flag);
extern uchar GreyScalePixel(int w, int h, int r, int g, int b, int glob);
extern uchar LCEPixel(int w, int h, uchar *original_data, QByteArray *new_locks);
extern uchar RadialPixel(int w, int h, uchar *original_data, QByteArray *new_locks, BeamHardening *bh);
extern void MakePolyGreyScale(int seg, int fnum, bool flag);
extern void MakeRangeGreyScale(int seg, int fnum, bool flag);
extern uchar PolyPixel(int w, int h, int seg);
extern uchar RangePixel(int w, int h, int bot, int top, double cen, double gra, int seg);
extern uchar GenPixel(int x, int y, int s, QVector<uchar> *sample, QByteArray *locks);
extern double CalcPoly(unsigned char r, unsigned char g, unsigned char b, Segment *seg);
extern void SaveMainImage(QString fname);
extern QByteArray DoMaskLocking();

#endif // __DISPLAY_H__
