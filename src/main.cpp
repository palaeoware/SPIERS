#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
#include <QString>
#include <QStyle>

#include "darkstyletheme.h"
#include "mainwindowimpl.h"
#include "version.h"
#include "../SPIERScommon/netmodule.h"


int main(int argc, char **argv)
{
    //This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //This has QPixmap images use the @2x images when available
    //See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
#if (QT_VERSION >= 0x050600)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);

    //Style program with our dark style
    QApplication::setStyle(new DarkStyleTheme);

    QPixmap splashPixmap(":/palaeoware_logo_square.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " v" + QString(UPDATEVERSION) + " </b></font>", Qt::AlignHCenter, Qt::white);
    app.processEvents();
    QTimer::singleShot(3000, splash, SLOT(close()));

    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    NetModule netModule;
    netModule.checkForNew();

    MainWindowImpl mainWindow;
    mainWindow.show();
    return app.exec();
}
