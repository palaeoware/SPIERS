/**
 * @file
 * Source: Positionclickhandler
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
#include "positionclickhandler.h"
#include "globals.h"
#include "mainwindow.h"
#include "scalegridoverlay.h"

PositionClickHandler::PositionClickHandler(ScaleGridOverlay *overlay, QObject *parent)
    : QObject(parent)
{
    this->overlay = overlay;
    clickCount = 0;
    connect(mainWindow->gl3widget, &GlWidget::worldPositionClicked,
            this, &PositionClickHandler::ClickHandler);
}


void PositionClickHandler::ClickHandler(QVector3D pos, bool hit, int objectIndex)
{

    if (!hit) return;
    overlay->setMarker(clickCount % 2, pos, objectIndex);
    clickCount++;
    if (clickCount > 2) clickCount = 1; // second click onwards: replace oldest
}

float PositionClickHandler::worldDistanceMm(const QVector3D &a, const QVector3D &b)
{
    // Distance in post-globalMatrix scene units
    float sceneUnits = (b - a).length();

    // Convert to mm: scene units are scaled by globalRescale relative to
    // raw model units, and raw model units convert to mm via mmPerUnit
    return sceneUnits * mmPerUnit / globalRescale;
}