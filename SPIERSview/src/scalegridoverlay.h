/**
 * @file
 * Header: Scalegridoverlay
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
#ifndef SCALEGRIDOVERLAY_H
#define SCALEGRIDOVERLAY_H

#include <QWidget>
#include <QVector3D>
#include <QPointF>

class GlWidget;

class ScaleGridOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit ScaleGridOverlay(GlWidget *glWidget, QWidget *parent = nullptr);

    // Set a marker — worldPos is the clicked position in post-globalMatrix space,
    // objectIndex is the SVObjects index (-1 if no object hit)
    void setMarker(int index, QVector3D worldPos, int objectIndex);

    // Clear both markers
    void clearMarkers();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GlWidget *glWidget;

    struct Marker {
        QVector3D worldPos;         // position at click time (used for distance)
        QVector3D localPos;         // position in object local space (for tracking)
        int       objectIndex = -1; // -1 = no object, marker fixed in world space
        bool      valid = false;
    };
    Marker markers[2];

    // Project raw world-space position to overlay pixel coordinates
    QPointF worldToScreen(const QVector3D &worldPos) const;

    // Project a marker to screen — uses localPos + current object matrix
    // so the marker tracks correctly when the object is rotated/translated
    QPointF markerToScreen(const Marker &marker) const;

    // Draw a crosshair at a screen position
    void drawMarker(QPainter &painter, QPointF screenPos,
                    const QColor &colour, const QColor &bgColour) const;
};

#endif // SCALEGRIDOVERLAY_H
