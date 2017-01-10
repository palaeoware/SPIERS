#include <QApplication>
#include "mainwindowimpl.h"
#include "SPIERScommon/netmodule.h"
#include <QDebug>

int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	MainWindowImpl win;

        NetModule n;

        n.CheckForNew();

        win.show();
        app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
        return app.exec();
}
