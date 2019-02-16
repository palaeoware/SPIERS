/**
 * @file
 * Header: Main VIew
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

#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>

/**
 * @brief The mainview class
 */
class mainview : public QGraphicsView
{
    Q_OBJECT
public:
    explicit mainview(QObject *parent);

protected:
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

signals:

public slots:

};

#endif // MAINVIEW_H
