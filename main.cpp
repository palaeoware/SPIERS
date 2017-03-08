
#include <QtWidgets/QApplication>
#include "mainwindow.h"
#include "SPIERSviewglobals.h"
#include "../SPIERScommon/netmodule.h"

#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QString>
#include <QProcess>

#include <QTextStream>
#include <QFile>

#ifndef __APPLE__

//RJG remove for linux
//#include <windows.h>

#include "main.h"

myapp::myapp(int argc, char *argv[]) : QApplication(argc, argv)
{

}



bool myapp::event(QEvent *event)
{
    return QApplication::event(event);
}


int main(int argc, char *argv[])
{

    myapp a(argc, argv);


    a.setQuitOnLastWindowClosed(true);
    //set the fname global from argument
    if (argc!=2) fname="";
     else
    {
         if ((*(argv[1])=='-') && (*(argv[1] + 1)=='x')) exit(0);
         else fname=argv[1];
     }
    NetModule n;
    n.CheckForNew();
    MainWindow w;
    a.installEventFilter(&w); //event filter - deal with glitchy keyboard shortcuts
    w.show();
    //QString s=qApp->applicationDirPath() + "/SPIERSupdater.exe";
    //ShellExecuteW(0, 0, (WCHAR *)(s.utf16()), 0, 0, SW_SHOWNORMAL);
     // Normal version is:
    //QProcess::startDetached(s);  //but this falls foul of vista UAC

    return a.exec();
}
#endif


#ifdef __APPLE__

#include "main.h"

/*
void log(QString m)
{
    QFile f1("log.txt");
    f1.open(QIODevice::Append);
    QTextStream g2(&f1);
    g2<<m<<"\n";
}*/

myapp::myapp(int argc, char *argv[]) : QApplication(argc, argv)
{
    //do nothing
    donthandlefileevent=false;
}


bool myapp::event(QEvent *event)
{
    //we don't do anything if we were passed and argv1 - i.e. if we are a child process of first one
    if (donthandlefileevent==true) return QApplication::event(event);

    switch (event->type()) {
    case QEvent::FileOpen:

        fn = static_cast<QFileOpenEvent *>(
                 event)->file();
        if (fname!="")
        {
            QString program = qApp->applicationFilePath();
             QStringList arguments;
             arguments << fn;

             QProcess::startDetached (program, arguments, qApp->applicationDirPath());
        }
        namereceived=true;
        return true;
    default:
        return QApplication::event(event);
    }
}

int main(int argc, char *argv[])
{
    MacBodgeClickedNoForUpdateDownload=false;


    if (argc==2) if (QString(argv[1]).length()<2) argc=1; //this to cure weird mac crash
    argc=1;

    myapp a(argc, argv);



    NetModule n;
    n.CheckForNew();

    a.fn="";
    a.namereceived=false;

    a.setQuitOnLastWindowClosed(true);
    //set the fname global from argument



    if (argc!=2) {
        fname="";
     }
    else
    {
         if ((*(argv[1])=='-') && (*(argv[1] + 1)=='x')) {
             exit(0);}
         else {
             fname=argv[1];
             //Make sure we don't handle file event at all
             a.donthandlefileevent=true;

         }
     }

    //Do nothing until event is received

    a.processEvents();

    if (a.namereceived) fname=a.fn;

    //a.spawn=true;
    MainWindow w;
    //deal with any incoming events - i.e. file open ones

    w.show();

    return a.exec();
}


#endif


