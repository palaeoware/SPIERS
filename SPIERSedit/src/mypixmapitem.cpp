/**
 * @file
 * Source: Mypixmapitem
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
#include "mypixmapitem.h"
#include "globals.h"

mypixmapitem *MainImage;

mypixmapitem::mypixmapitem(QString fname) : QGraphicsPixmapItem(fname)
{
	;//qDebug()<<"Test-1";
}

void mypixmapitem::testmethod()
{
	//qDebug()<<"Test-2";	
}

void mypixmapitem::mouseMoveEvent(QMouseEvent * event)
{
	//qDebug()<<"Moved";	
}

void mypixmapitem::mousePressEvent ( QMouseEvent * event )
{
     if (event->button() == Qt::LeftButton) 
     {
     	//qDebug()<<"LeftClicked";
     }	
}

void mypixmapitem::mouseReleaseEvent ( QMouseEvent * event )
{
     if (event->button() == Qt::LeftButton) 
     {
     	//qDebug()<<"LeftUp";
     }	
}

