/**
 * @file
 * Header: Curves
 *
 * All SPIERSedit code is released under the GNU General Public License.
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

#ifndef __CURVES_H__
#define __CURVES_H__

#include <QImage>
#include <QGraphicsScene>

#include "mainwindowimpl.h"

//maybe oneday make this modifyable?
#define TRIANGLE_DENSITY 100

extern void AddNode();
extern void KillNode(MainWindowImpl *th);
extern QList <QGraphicsItem *> MarkerList;
extern int FindClosestNode(double X, double Y);
extern void DrawCurve(int c, int mycol, int file, QImage *Thresh);
extern void DrawCurveOutput(int c, int file, uchar *fullarray, QList <bool> *UseMasks, bool remove);
extern void DrawCurveMarkers(QGraphicsScene *scene);
extern void PopulateTriangleList(int OutObject, int firstfile, int lastfile, QList<double> *stretches, int resamps, QVector<double> *TrigArray, int *TrigCount);

#endif // __CURVES_H__
