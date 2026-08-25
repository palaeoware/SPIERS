/**
 * @file
 * Header: Mypixmapitem
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
#ifndef __MYPIXMAPITEM_H__
#define __MYPIXMAPITEM_H__
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QString>


class mypixmapitem : public QGraphicsPixmapItem
{
public:
	mypixmapitem(QString fname);
	void testmethod();
protected:
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);
	
};

extern mypixmapitem *MainImage;

#endif // __MYPIXMAPITEM_H__
