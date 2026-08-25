/**
 * @file
 * Source: Fullscreenwindow
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