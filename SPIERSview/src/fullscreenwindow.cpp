#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QShortcut>

#include "fullscreenwindow.h"
#include "globals.h"

FullScreenWindow::FullScreenWindow(QWidget *parent, GlWidget *gl3widget)
    : QDialog(parent)
{
    glwidget = gl3widget;

#ifdef __linux__
    QScreen *currentScreen = (parent != nullptr) ? parent->screen() : QGuiApplication::primaryScreen();
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
}