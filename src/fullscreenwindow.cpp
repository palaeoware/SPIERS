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

    // Included gere for testing in a windowed mode
    /*
    int nWidth = 1200;
    int nHeight = 800;
    if (parent != nullptr)
        setGeometry(parent->x() + parent->width() / 2 - nWidth / 2,
                    parent->y() + parent->height() / 2 - nHeight / 2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);
    */

    QHBoxLayout *fullScreenLayout = new QHBoxLayout(this);
    fullScreenLayout->setContentsMargins(0, 0, 0, 0);
    fullScreenLayout->addWidget(glwidget);
    setLayout(fullScreenLayout);
    glwidget->update();
}
