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
#include <QDesktopWidget>
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
        QString path = QString("%1/SPIERSView_debug.log").arg(QDir::homePath());
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
/**
 * @brief qMain::qMain
 * @param argc
 * @param argv
 */
main::main(int &argc, char *argv[]) : QApplication(argc, argv)
{
}

/**
 * @brief qMain::event
 * @param event
 * @return
 */
bool main::event(QEvent *event)
{
    return QApplication::event(event);
}

/**
 * @brief qmain
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    // This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Set OpenGL surface format as global
    // @see global.h and global.cpp

    surfaceFormat.setMajorVersion(GL_MAJOR);
    surfaceFormat.setMinorVersion(GL_MINOR);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    // Set to allow the OpenGL context (ie. the same threads) to be shared between normal and full screen mode
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    // This has QPixmap images use the @2x images when available
    // See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
#if (QT_VERSION >= 0x050600)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app( argc, argv );

    // Install the message handler to log to file
    qInstallMessageHandler(logMessageOutput);

    // Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    QPixmap splashPixmap(":/logo/palaeoware_square.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " </b></font>", Qt::AlignHCenter, Qt::white);
    app.processEvents();
    QTimer::singleShot(2000, splash, SLOT(close()));

    app.setQuitOnLastWindowClosed(true);

    // Set the fname global from argument
    QStringList args = app.arguments();

    // Sets fname (= filename) global to open from args
    fname = "";
    if (args.count() > 1)
    {
        fname = args[1];
    }

    // Check for any version updates
    NetModule netModule;
    netModule.checkForNew();

    // MainWindow with Event filter - deals with glitchy keyboard shortcuts
    MainWindow mainWindow;
    app.installEventFilter(&mainWindow);
    mainWindow.show();

    return app.exec();
}
#endif


#ifdef __APPLE__

/**
 * @brief qMain::qMain
 * @param argc
 * @param argv
 */

main::main(int &argc, char *argv[]) : QApplication(argc, argv)
{
    //do nothing
    donthandlefileevent = false;
}

/**
 * @brief qMain::event
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

        if (fname != "")
        {
            QString program = qApp->applicationFilePath();
            QStringList arguments;
            arguments << fn;

            QProcess::startDetached(program, arguments, qApp->applicationDirPath());
        }

        namereceived = true;

        return true;

        break;
    }
}

/**
 * @brief qMain
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    macClickedNoForUpdateDownload = false;

    // This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Set OpenGL surface format as global
    // @see global.h and global.cpp
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setMajorVersion(GL_MAJOR_MAC);
    surfaceFormat.setMinorVersion(GL_MINOR_MAC);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    // Set to allow the OpenGL context (ie. the same threads) to be shared between normal and full screen mode
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    // Install the message handler to log to file
    qInstallMessageHandler(logMessageOutput);

    if (argc == 2) {
        // Check that the passed file name has at least 2 characters
        if (QString(argv[1]).length() < 2)
            argc = 1; //this to cure weird mac crash
    }

    // WFT is this doing here? This is forcing the program to NEVER look for a filename passed by int argc!
    //argc = 1;

    // Create main class
    class main app(argc, argv);

    //Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    // Check for any version updates
    NetModule netModule;
    netModule.checkForNew();

    app.fn = "";
    app.namereceived = false;

    app.setQuitOnLastWindowClosed(true);

    // Set the fname global from argument if the argc value is === 2, also check for - and x to quit hte application, anything else we ignore and set the global fname to null.
    if (argc != 2)
    {
        fname = "";
    }
    else
    {
        if ((*(argv[1]) == '-') && (*(argv[1] + 1) == 'x'))
        {
            QCoreApplication::quit();
        }
        else
        {
            fname = argv[1];

            // Make sure we don't handle file event at all
            app.donthandlefileevent = true;

        }
    }

    // Do nothing until event is received
    app.processEvents();

    // At this point if we had a filename set QFileOpenEvent then set the global fname variable.
    if (app.namereceived)
        fname = app.fn;

    // MainWindow with Event filter - deals with glitchy keyboard shortcuts
    MainWindow mainWindow;
    app.installEventFilter(&mainWindow);
    mainWindow.show();

    return app.exec();
}
#endif
