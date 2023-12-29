/**
 * @file
 * Source: MyGraphicsView
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include <QGraphicsView>
#include <QDebug>
#include <QEvent>
#include <QWheelEvent>
#include "mygraphicsview.h"

mygraphicsview::mygraphicsview() : QGraphicsView()
{
    return;
}

void mygraphicsview::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    fitInView(0, 0, 255, 255);
}

void mygraphicsview::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}
