#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QShortcut>
#include <QScreen>

#include "fullscreenwindow.h"
#include "globals.h"

FullScreenWindow::FullScreenWindow(QWidget *parent, GlWidget *gl3widget)
    : QDialog(parent)
{
    glwidget = gl3widget;

#ifdef __linux__
    currentScreen = availableScreens.at(QApplication::desktop()->screenNumber(this));
    if (parent != nullptr)
        setGeometry(currentScreen->geometry());
    else
        resize(currentScreen->geometry().width(), currentScreen->geometry().height());
#endif

    QHBoxLayout *fullScreenLayout = new QHBoxLayout(this);
    fullScreenLayout->setContentsMargins(0, 0, 0, 0);
    fullScreenLayout->addWidget(glwidget);
    setLayout(fullScreenLayout);
    glwidget->update();

#ifdef __linux__
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    show();
#else
    showFullScreen();
#endif
}
