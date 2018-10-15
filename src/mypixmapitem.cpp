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

