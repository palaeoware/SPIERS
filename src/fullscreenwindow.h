#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QDialog>

#include "gl3widget.h"

class FullScreenWindow : public QDialog
{
public:
    FullScreenWindow(QWidget *parent, GlWidget *gl3widget);
    GlWidget *glwidget;

private:
    void keyPressEvent(QKeyEvent *event);
};

#endif // FULLSCREENWINDOW_H
