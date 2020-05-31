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

/**
 * @brief logMessageOutput
 * @param type
 * @param context
 * @param msg
 */
void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString txt;
    bool logToFile = true;

    switch (type)
    {
    case QtDebugMsg:
        txt = QString("Debug: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtWarningMsg:
        txt = QString("Info: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    }
    if (logToFile) {
        // Save to debug.log
        QString path = QString("%1/SPIERSEdit_debug.log").arg(QDir::homePath());
        QFile outFile(path);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream log(&outFile);
        log << txt << endl;

        // Now print to stout too
        QTextStream console(stdout);
        console << txt << endl;
    } else {
        // Print to stout only
        QTextStream console(stdout);
        console << txt << endl;
    }
}

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

main::main(int &argc, char *argv[]) : QApplication(argc, argv)
{
    //do nothing
    donthandlefileevent = false;
}

/**
 * @brief main::event
 * @param event
 * @return
 */
bool main::event(QEvent *event)
{
    //we don't do anything if we were passed and argv1 - i.e. if we are a child process of first one
    if (donthandlefileevent == true) {
        qDebug() << "Don't handle file open event";
        return QApplication::event(event);
    }

    switch (event->type())
    {
    default:
        return QApplication::event(event);
        break;

    case QEvent::FileOpen:

        qDebug() << "File Open Event";

        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        fn = openEvent->file();

        qDebug() << "File name passed is: " << fn;

        openfile = fn;

        namereceived = true;

        return true;

        break;
    }
}

int main(int argc, char *argv[])
{
    // Install the message handler to log to file
    qInstallMessageHandler(logMessageOutput);

    if (argc == 2) {
        qDebug() << "argc == 2";
        // Check that the passed file name has at least 2 characters
        if (QString(argv[1]).length() < 2)
            argc = 1; //this to cure weird mac crash

        qDebug() << "argc == " << argc << " argv = " << QString(argv[1]) << "argv[2]" << QString(argv[1]);
    }


    class main app(argc, argv);

    //Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    NetModule n;
    n.checkForNew();

    app.fn = "";
    app.namereceived = false;

    app.setQuitOnLastWindowClosed(true);

    // Set the fname global from argument if the argc value is === 2, also check for - and x to quit hte application, anything else we ignore and set the global fname to null.
    if (argc != 2)
    {
        qDebug() << "openfile = null";
        openfile = "";
    }
    else
    {
        qDebug() << "argc is 2 so we do stuff...";
        if ((*(argv[1]) == '-') && (*(argv[1] + 1) == 'x'))
        {
            QCoreApplication::quit();
        }
        else
        {
            qDebug() << "Setting fname to argv[1]";

            openfile = argv[1];

            // Make sure we don't handle file event at all
            app.donthandlefileevent = true;

        }
    }

    // Do nothing until event is received
    app.processEvents();


    qDebug() << "Moving to start application mainwindow...";

    MainWindowImpl win;
    win.show();

    return app.exec();
}

#endif

