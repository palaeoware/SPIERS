#include <QHBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QShortcut>

#include "fullscreenwindow.h"
#include "globals.h"

FullScreenWindow::FullScreenWindow(QWidget *parent, GlWidget *gl3widget)
    : QDialog(parent)
{
    glwidget = gl3widget;

    int nWidth = 1200;
    int nHeight = 800;
    if (parent != nullptr)
        setGeometry(parent->x() + parent->width() / 2 - nWidth / 2,
                    parent->y() + parent->height() / 2 - nHeight / 2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);

    QHBoxLayout *fullScreenLayout = new QHBoxLayout(this);
    fullScreenLayout->setContentsMargins(0, 0, 0, 0);
    fullScreenLayout->addWidget(glwidget);
    setLayout(fullScreenLayout);
    setWindowTitle("test");
    glwidget->update();

    QShortcut *myShortcut = new QShortcut(this);
    myShortcut->setKey(Qt::Key_Control + Qt::Key_F); //Or any other non modifier key
    myShortcut->setContext(Qt::ApplicationShortcut);

    connect(myShortcut, SIGNAL(activated()), parent, SLOT(handleShortCutSlot()));
}

/**
 * @brief FullScreenWindow::keyPressEvent
 * @param e
 */
void FullScreenWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        {
            qDebug() << "Key Press" << event->key();
            QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, event->key(), event->modifiers ());
            qApp->postEvent(this->parent(), newEvent, 0);
        }
    }
}
