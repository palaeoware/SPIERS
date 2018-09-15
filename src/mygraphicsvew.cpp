/*********************************************

SPIERSedit 2: mygraphicsview.cpp

Subclassed QGraphicsView object -
just has a resize handler. Huge amount
off faff for this if you ask me!


**********************************************/

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
