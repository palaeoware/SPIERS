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

#include "darkstyletheme.h"
#include "mainwindow.h"
#include "globals.h"
#include "../SPIERScommon/netmodule.h"

#ifndef __APPLE__

#ifdef _WIN64
#include <windows.h>
#endif

#include "main.h"

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
 * @brief logMessageOutput
 * @param type
 * @param context
 * @param msg
 */
void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString txt;

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

#ifdef QT_DEBUG
    // Save to debug.log
    QFile outFile("debug.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream log(&outFile);
    log << txt << endl;

    // Now print to stout too
    QTextStream console(stdout);
    console << txt << endl;
#else
    // Print to stout only
    QTextStream console(stdout);
    console << txt << endl;
#endif
}

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

    // Install the message handler to log to file
    qInstallMessageHandler(logMessageOutput);

    // Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    QPixmap splashPixmap(":/resources/palaeoware_logo_square.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " v" + QString(UPDATEVERSION) + " </b></font>", Qt::AlignHCenter, Qt::white);
    app.processEvents();
    QTimer::singleShot(3000, splash, SLOT(close()));

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

#include "main.h"

/**
 * @brief qMain::qMain
 * @param argc
 * @param argv
 */
main::main(int argc, char *argv[]) : QApplication(argc, argv)
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

/**
 * @brief qMain
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    macClickedNoForUpdateDownload = false;


    if (argc == 2) if (QString(argv[1]).length() < 2) argc = 1; //this to cure weird mac crash
    argc = 1;

    main a(argc, argv);



    NetModule n;
    n.CheckForNew();

    a.fn = "";
    a.namereceived = false;

    a.setQuitOnLastWindowClosed(true);
    //set the fname global from argument



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
            //Make sure we don't handle file event at all
            a.donthandlefileevent = true;

        }
    }

    //Do nothing until event is received

    a.processEvents();

    if (a.namereceived) fname = a.fn;

    //a.spawn=true;
    MainWindow w;
    //deal with any incoming events - i.e. file open ones

    w.show();

    return a.exec();
}
#endif
