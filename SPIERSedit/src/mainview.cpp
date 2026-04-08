/**
 * @file
 * Source: MainView
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "mainview.h"
#include "mainwindow.h"
#include <QDebug>
#include <QEvent>
#include <QWheelEvent>

/**
 * @brief mainview::mainview
 * @param parent
 */
mainview::mainview(QObject *parent) : QGraphicsView()
{
    Q_UNUSED(parent);
    return;
}

/**
 * @brief mainview::wheelEvent
 * @param event
 */
void mainview::wheelEvent(QWheelEvent *event)
{
    //event->ignore();
    AppMainWindow->MouseZoom(event->angleDelta().y());

}

/**
 * @brief mainview::eventFilter
 * @param obj
 * @param event
 * @return
 */
bool mainview::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel && underMouse())
    {
        AppMainWindow->MouseZoom((static_cast<QWheelEvent *>(event))->angleDelta().y());
        return true; //stop further wheel processing
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}
