#include "mainview.h"
#include "mainwindowimpl.h"
#include <QDebug>
#include <QEvent>
#include <QWheelEvent>

mainview::mainview(QObject *parent = nullptr) : QGraphicsView()
{
    Q_UNUSED(parent);
    return;
}


void mainview::wheelEvent(QWheelEvent *event)
{
    //event->ignore();
    AppMainWindow->MouseZoom(event->delta());
}

bool mainview::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
    {
        AppMainWindow->MouseZoom(((QWheelEvent *)event)->delta());
        return true; //stop further wheel processing
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
