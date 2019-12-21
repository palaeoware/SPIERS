/**
 * @file
 * Source: Main
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
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
#include <QProcess>
#include <QByteArray>
#include <QStringList>
#include <QMessageBox>
#include <QSplashScreen>
#include <QStyle>
#include <QDesktopWidget>

#include "mainwindowimpl.h"
#include "display.h"
#include "globals.h"
#include "../../SPIERScommon/src/darkstyletheme.h"
#include "../../SPIERScommon/src/netmodule.h"

#ifndef __APPLE__
int main(int argc, char **argv)
{
    //This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //This has QPixmap images use the @2x images when available
    //See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
#if (QT_VERSION >= 0x050600)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app( argc, argv );

    //Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    QPixmap splashPixmap(":/logo/palaeoware_square.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " </b></font>", Qt::AlignHCenter, Qt::white);
    app.processEvents();
    QTimer::singleShot(3000, splash, SLOT(close()));

    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    NetModule n;
    n.checkForNew();

    QStringList args = app.arguments();

    if (args.count() > 1)
    {
        openfile = args[1];
    }
    else openfile = "";

    MainWindowImpl mainWindow;
    mainWindow.show();
    return app.exec();
}
#endif

#ifdef __APPLE__
#include "main.h"


QString fname;

myapp::myapp(int argc, char *argv[]) : QApplication(argc, argv)
{
    //do nothing
    donthandlefileevent = false;
}

bool myapp::event(QEvent *event)
{
    //we don't do anything if we were passed and argv1 - i.e. if we are a child process of first one
    if (donthandlefileevent == true) return QApplication::event(event);

    switch (event->type())
    {
    case QEvent::FileOpen:

        fn = static_cast<QFileOpenEvent *>(
                 event)->file();
        if (fname != "")
        {
            QString program = qApp->applicationFilePath();
            QStringList arguments;
            arguments << fn;

            QProcess::startDetached (program, arguments, qApp->applicationDirPath());
        }
        namereceived = true;
        return true;
    default:
        return QApplication::event(event);
    }
}

int main(int argc, char *argv[])
{
    myapp a(argc, argv);

    NetModule n;
    n.checkForNew();

    a.fn = "";
    a.namereceived = false;

    a.setQuitOnLastWindowClosed(true);
    //set the fname global from argument
    if (argc != 2) openfile = "";
    else openfile = argv[1];


    if (argc == 2) if (QString(argv[1]).length() == 0) argc = 1; //this to cure weird mac crash

    if (argc != 2)
    {
        fname = "";
    }
    else
    {
        if ((*(argv[1]) == '-') && (*(argv[1] + 1) == 'x'))
        {
            exit(0);
        }
        else
        {
            fname = argv[1];
            //Make sure we don't handle file event at all
            a.donthandlefileevent = true;
        }
    }

    //Do nothing until event is received
    a.processEvents();
    if (a.namereceived) openfile = a.fn;

    //a.spawn=true;
    MainWindowImpl win;

    //deal with any incoming events - i.e. file open ones

    win.show();

    return a.exec();
}

#endif


