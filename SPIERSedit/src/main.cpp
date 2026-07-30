/**
 * @file
 * Source: Main
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
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
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>
#include <QSurfaceFormat>
#include <QImageReader>

#include "mainwindow.h"
#include "globals.h"
#include "mlinterface.h"
#include "../../SPIERScommon/src/customstyletheme.h"
#include "../../SPIERScommon/src/netmodule.h"
#include "../../SPIERScommon/src/crashdetector.h"

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
    bool logToFile = false;

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
    if (logToFile)
    {
        // Save to debug.log
        QString path = QString("%1/SPIERSEdit_debug.log").arg(QDir::homePath());
        QFile outFile(path);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append)) return;
        QTextStream log(&outFile);
        log << txt << Qt::endl;

        // Now print to stout too
        QTextStream console(stdout);
        console << txt << Qt::endl;
    }
    else
    {
        // Print to stout only
        QTextStream console(stdout);
        console << txt << Qt::endl;
    }
}

#ifndef __APPLE__
int main(int argc, char **argv)
{
    QImageReader::setAllocationLimit(2048);

    /// Install crash handlers early
    CrashDetector::installCrashHandlers(QStringLiteral("SPIERSedit"));

    // Set OpenGL surface format as global
    surfaceFormat.setMajorVersion(GL_MAJOR);
    surfaceFormat.setMinorVersion(GL_MINOR);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    // Allow OpenGL context sharing between normal and full screen mode
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app( argc, argv );

    //Style program with our dark style
    QApplication::setStyle(new CustomStyleTheme(CustomStyleTheme::readThemeSetting()));

    QPixmap splashPixmap(":/logo/palaeoware_square.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " </b></font>", Qt::AlignHCenter, Qt::white);
    app.processEvents();
    QTimer::singleShot(3000, splash, &QSplashScreen::close);

    MLInterface::TestML();

    qDebug() << "OpenCV enabled? " << MLInterface::enabled;
    if (MLInterface::enabled)
    {
        mlInterface = new MLInterface();
    }

    QStringList args = app.arguments();

    if (args.count() > 1)
    {
        openfile = args[1];
    }
    else openfile = "";

    MainWindow mainWindow;
    mainWindow.show();

    // Kick off the update check.
    NetModule *netModule = new NetModule(&app);
    //netModule->setTestVersion("1.0.0");
    // Test for and watch for an internet connection
    netModule->startConnectivityWatch();
    // Set initial enabled state for network-dependent menu items.
    mainWindow.onConnectivityChanged(NetModule::isOnline());
    QObject::connect(netModule, &NetModule::connectivityChanged,
                     &mainWindow, &MainWindow::onConnectivityChanged);
    netModule->checkForNew();

    return app.exec();
}
#endif


#ifdef __APPLE__
#include "main.h"

main::main(int &argc, char *argv[]) : QApplication(argc, argv)
{
    // Set so we do NOT ignore calls to QEvent::FileOpen
    ignoreFileOpenEvent = false;

    // Set the default received file name to be a empty QString
    receivedFileName = "";

    // Set the recieved file name flag to false
    hasReceivedFileName = false;
}

/**
 * @brief main::event
 * @param event
 * @return
 */
bool main::event(QEvent *event)
{
    // We don't do anything if we were passed and argv1 - i.e. if we are a child process of first one
    if (ignoreFileOpenEvent == true)
    {
        qDebug() << "Don't handle file open event";
        return QApplication::event(event);
    }

    switch (event->type())
    {

    // Pass all events other than QEvent:FileOpen to the main application
    default:
        return QApplication::event(event);
        break;

    // Catch the QEvent::FileOpen event
    case QEvent::FileOpen:

        qDebug() << "File Open Event Caught";

        // Getting the recevied file name from the passed event
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        receivedFileName = openEvent->file();

        qDebug() << "File name passed is: " << receivedFileName << " Open File: " << openfile;

        // Here we spin up a new SPIERSview application and pass the received file name as an argument
        // But we only do this if our current application already has a file open... otherwise we expect our
        // current applciation to open the file instead.
        if(currentOpenFileName != "") {

            // This should be the path to the apllication we will need to call
            // plus the application working directory
            QString pathToApplication = qApp->applicationFilePath();
            QString pathToWorkingDirectory = qApp->applicationDirPath();

            // Set up arguments
            QStringList argumentsToPass;
            argumentsToPass << receivedFileName;

            qint64 *pid = new qint64;
            QProcess process;
            process.setProgram(pathToApplication);
            process.setArguments(argumentsToPass);
            process.setWorkingDirectory(pathToWorkingDirectory);
            bool startSuccess = process.startDetached(pid);

            qDebug() << "pathToApplication == " << pathToApplication << " pathToWorkingDirectory = " << pathToWorkingDirectory << " Receieved File Name: " << receivedFileName;
            if(startSuccess) {
                qDebug() << "Process start == OK : " << pid;
            } else {
                qDebug() << "Process start == Failed";
            }

        }

        hasReceivedFileName = true;

        return QApplication::event(event);

        break;
    }
}

int main(int argc, char *argv[])
{
    // Install the message handler to log to file
    qInstallMessageHandler(logMessageOutput);

    // This is a sanity check for any passed arguments...
    if (argc == 2)
    {
        qDebug() << "argc == 2";
        // Check that the passed file name has at least 2 characters
        if (QString(argv[1]).length() < 2)
            argc = 1; //this to cure weird mac crash

        qDebug() << "argc == " << argc << " argv = " << QString(argv[1]) << "argv[2]" << QString(argv[1]);
    }


    // Set OpenGL surface format as global
    {
        surfaceFormat.setDepthBufferSize(24);
        surfaceFormat.setMajorVersion(GL_MAJOR_MAC);
        surfaceFormat.setMinorVersion(GL_MINOR_MAC);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(surfaceFormat);
    }
    // Allow OpenGL context sharing between normal and full screen mode
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    class main app(argc, argv);

    //Style program with our dark style
    QApplication::setStyle(new CustomStyleTheme(CustomStyleTheme::readThemeSetting()));

    app.setQuitOnLastWindowClosed(true);

    // Set the fname global from argument if the argc value is === 2, also check for -x to quit the application, anything else we ignore and set the global fname to "".
    if (argc != 2)
    {
        qDebug() << "fname = {blank}";
        openfile = "";
    }
    else
    {
        qDebug() << "argc is 2 so we do stuff...";

        // This is to allow cmd line quiting of the program with: > {path to appllication} -x
        if ((*(argv[1]) == '-') && (*(argv[1] + 1) == 'x'))
        {
            QCoreApplication::quit();
        }


        // Else we assume that it is a valid file name passed as an argument. We then do the following...
        else
        {
            qDebug() << "Setting fname to argv[1]";


            // Set the global fname to the recived file name argument
            openfile = argv[1];

            // Make sure we don't handle file event at all
            //app.ignoreFileOpenEvent = true;

        }
    }

    // Do nothing until all events are received for the appllication start
    app.processEvents();

    // We then check again, as if seems that the QEvent::FileOpen event doesn't get picked up in the first check,
    // but does seem to be picked up on a second check! This is a real bodge, there must be a better way to do this...
    app.processEvents();

    // If we have got this far then we are trying to load a file by name that has been caught in the QEvent::FileOpen
    // but we either have not had a file name already passed by arguments or got a file already open
    if(app.hasReceivedFileName) {
        qDebug() << "Setting openfile from recieved file name by QEvent::FileOpen (we have no arguments passed nor a file already open)";
        openfile = app.receivedFileName;
    }


    qDebug() << "Moving to start application mainwindow...";

    MLInterface::TestML();

    qDebug() << "OpenCV enabled? " << MLInterface::enabled;
    if (MLInterface::enabled)
    {
        mlInterface = new MLInterface();
    }

    MainWindow win;
    win.show();

    NetModule *netModule = new NetModule(&app);
    netModule->startConnectivityWatch();
    win.onConnectivityChanged(NetModule::isOnline());
    QObject::connect(netModule, &NetModule::connectivityChanged,
                     &win, &MainWindow::onConnectivityChanged);
    netModule->checkForNew();

    return app.exec();
}

#endif

