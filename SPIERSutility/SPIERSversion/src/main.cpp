/**
 * @file
 * Source: Main
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Alan R.T. Spencer, Russell J. Garwood,
 * and Mark D. Sutton.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include <QApplication>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QStyle>
#include <QSplashScreen>
#include <QWidget>
#include <QTimer>

#include "../../../SPIERScommon/src/darkstyletheme.h"
#include "mainwindow.h"
#include "globals.h"

#include "main.h"

/**
 * @brief qMain
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    // This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // This has QPixmap images use the @2x images when available
    // See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
#if (QT_VERSION >= 0x050600)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app( argc, argv );

    // Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    QPixmap splashPixmap(":/logo/palaeoware_square.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " </b></font>", Qt::AlignHCenter, Qt::white);
    app.processEvents();
    QTimer::singleShot(3000, splash, SLOT(close()));

    app.setQuitOnLastWindowClosed(true);

    // MainWindow show
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
