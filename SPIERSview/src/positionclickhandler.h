/**
 * @file
 * Header: Positionclickhandler
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
#ifndef POSITIONCLICKHANDLER_H
#define POSITIONCLICKHANDLER_H
#include <QObject>
#include <QVector3D>

#include "qtmetamacros.h"

class ScaleGridOverlay;

class PositionClickHandler: public QObject
{
    Q_OBJECT
public:
    PositionClickHandler(ScaleGridOverlay *overlay, QObject *parent);
private slots:
    void ClickHandler(QVector3D pos, bool hit, int objectIndex);
private:
    float worldDistanceMm(const QVector3D &a, const QVector3D &b);
    ScaleGridOverlay *overlay;
    QVector3D clickPoints[2];
    int       clickCount = 0;
};

#endif // POSITIONCLICKHANDLER_H
