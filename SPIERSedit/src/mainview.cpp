#include "mainview.h"
#include "mainwindowimpl.h"
#include <QDebug>
#include <QEvent>
#include <QWheelEvent>

/**
 * @brief mainview::mainview
 * @param parent
 */
mainview::mainview(QObject *parent = nullptr) : QGraphicsView()
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
    AppMainWindow->MouseZoom(event->delta());
}

/**
 * @brief mainview::eventFilter
 * @param obj
 * @param event
 * @return
 */
bool mainview::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
    {
        AppMainWindow->MouseZoom((static_cast<QWheelEvent *>(event))->delta());
        return true; //stop further wheel processing
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
