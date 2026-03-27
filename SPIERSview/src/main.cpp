#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QProcess>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QStyle>
#include <QSplashScreen>
#include <QTimer>
#include <QScreen>
#include <QFileOpenEvent>

#include "main.h"
#include "mainwindow.h"
#include "globals.h"
#include "../SPIERScommon/src/netmodule.h"
#include "../SPIERScommon/src/darkstyletheme.h"

#ifdef _WIN64
#include <windows.h>
#endif

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
        QString path = QString("%1/SPIERSView_debug.log").arg(QDir::homePath());
        QFile outFile(path);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream log(&outFile);
        log << txt << Qt::endl;

        QTextStream console(stdout);
        console << txt << Qt::endl;
    }
    else
    {
        QTextStream console(stdout);
        console << txt << Qt::endl;
    }
}

#ifndef __APPLE__
/**
 * @brief qMain::qMain
 */
main::main(int &argc, char *argv[]) : QApplication(argc, argv)
{
}

/**
 * @brief qMain::event
 */
bool main::event(QEvent *event)
{
    return QApplication::event(event);
}

/**
 * @brief qmain
 */
int main(int argc, char *argv[])
{
    // Set OpenGL surface format as global
    surfaceFormat.setMajorVersion(GL_MAJOR);
    surfaceFormat.setMinorVersion(GL_MINOR);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    // Allow OpenGL context sharing between normal and full screen mode
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);

    qInstallMessageHandler(logMessageOutput);

    QApplication::setStyle(new DarkStyleTheme);

    QPixmap splashPixmap(":/logo/palaeoware_square.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    //splash->show();
    //splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " </b></font>", Qt::AlignHCenter, Qt::white);
    app.processEvents();
    QTimer::singleShot(2000, splash, &QSplashScreen::close);

    app.setQuitOnLastWindowClosed(true);

    QStringList args = app.arguments();
    fname = "";
    if (args.count() > 1)
    {
        fname = args[1];
    }

    NetModule netModule;
    netModule.checkForNew();

    MainWindow mainWindow;
    app.installEventFilter(&mainWindow);
    mainWindow.show();

    return app.exec();
}
#endif


#ifdef __APPLE__

/**
 * @brief qMain::qMain
 */
main::main(int &argc, char *argv[]) : QApplication(argc, argv)
{
    ignoreFileOpenEvent = false;
    receivedFileName = "";
    hasReceivedFileName = false;
}

/**
 * @brief qMain::event
 */
bool main::event(QEvent *event)
{
    if (ignoreFileOpenEvent == true)
    {
        qDebug() << "Don't handle file open event";
        return QApplication::event(event);
    }

    switch (event->type())
    {
    default:
        return QApplication::event(event);
        break;

    case QEvent::FileOpen:

        qDebug() << "File Open Event Caught";

        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        receivedFileName = openEvent->file();

        qDebug() << "File name passed is: " << receivedFileName;

        if (fname != "")
        {
            QString pathToApplication = qApp->applicationFilePath();
            QString pathToWorkingDirectory = qApp->applicationDirPath();

            QStringList argumentsToPass;
            argumentsToPass << receivedFileName;

            qint64 *pid = new qint64;
            QProcess process;
            process.setProgram(pathToApplication);
            process.setArguments(argumentsToPass);
            process.setWorkingDirectory(pathToWorkingDirectory);
            bool startSuccess = process.startDetached(pid);

            qDebug() << "pathToApplication == " << pathToApplication << " pathToWorkingDirectory = " << pathToWorkingDirectory << " Receieved File Name: " << receivedFileName;
            if (startSuccess)
                qDebug() << "Process start == OK : " << pid;
            else
                qDebug() << "Process start == Failed";
        }

        hasReceivedFileName = true;
        return QApplication::event(event);
        break;
    }
}

/**
 * @brief qmain
 */
int main(int argc, char *argv[])
{
    qInstallMessageHandler(logMessageOutput);

    macClickedNoForUpdateDownload = false;

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

    if (argc == 2)
    {
        qDebug() << "argc == 2";
        if (QString(argv[1]).length() < 2)
            argc = 1;
        qDebug() << "argc == " << argc << " argv = " << QString(argv[1]) << "argv[2]" << QString(argv[1]);
    }

    class main app(argc, argv);

    QApplication::setStyle(new DarkStyleTheme);

    app.setQuitOnLastWindowClosed(true);

    if (argc != 2)
    {
        qDebug() << "fname = {blank}";
        fname = "";
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
            fname = argv[1];
        }
    }

    app.processEvents();
    app.processEvents();

    if (app.hasReceivedFileName)
    {
        qDebug() << "Setting fname from recieved file name by QEvent::FileOpen";
        fname = app.receivedFileName;
    }

    qDebug() << "Moving to start application mainwindow...";

    NetModule netModule;
    netModule.checkForNew();

    MainWindow mainWindow;
    app.installEventFilter(&mainWindow);
    mainWindow.show();

    return app.exec();
}
#endif