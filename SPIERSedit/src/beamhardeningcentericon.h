/**
 * @file
 * Header: Beamhardeningcentericon
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
#ifndef BEAMHARDENINGCENTERICON_H
#define BEAMHARDENINGCENTERICON_H

#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>

class BeamHardeningCenterIcon
{
public:
    BeamHardeningCenterIcon();

    void ShowCenter(bool show);
    void PlaceCenter(int x, int y, int radius);
private:
    QGraphicsLineItem *center1, *center2;
    QGraphicsEllipseItem *radiusItem;
    bool centerVisible;
    int lastX, lastY, lastRadius;
};

#endif // BEAMHARDENINGCENTERICON_H
