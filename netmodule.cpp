//Does the inital check (not actually the download)

#include "netmodule.h"
#include "semanticversion.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QString>
#include <QDesktopServices>
#include <QUrl>

//First section is my code for checking the update status
bool MacBodgeClickedNoForUpdateDownload;

NetModule::NetModule(): QObject()
{
    CheckFinished = false;
    //DownloadNeeded=false;
    DownloadURL = "";
    DoingCheck = false;
    DownloadDone = false;
    DownloadError = false;
    ErrorText = "";
    ProgressBar = nullptr;
    ProgressDialog = nullptr;
}

void NetModule::CheckHash(QByteArray stlhash, QStringList *commlist)
{
    DoingHash = true;
    i_comm = commlist;

    QString h1(stlhash.toHex());

    //looks for a file on server named for the hash value of the STLs
    QNetworkRequest request;
    manager = new QNetworkAccessManager(this);

    QString url;
    QTextStream t(&url);
    t << "http://www.spiers-software.org/" << stlhash.toHex() << ".txt";
    request.setUrl(QUrl(url));

    request.setRawHeader("User-Agent", "SPIERS 2");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void NetModule::CheckForNew()
{
    //return; //deactivate
    QNetworkRequest request;
    DoingCheck = true;
    manager = new QNetworkAccessManager(this);


#ifdef __APPLE__
    //QMessageBox::question(0,"SPIERS update","Mac Update",QMessageBox::No,QMessageBox::Yes);
    request.setUrl(QUrl("http://www.spiers-software.org/SPIERSstatus-mac.txt"));
#endif

#ifndef __APPLE__
    //QMessageBox::question(0,"SPIERS update","A new version of SPIERS ",QMessageBox::No,QMessageBox::Yes);
    request.setUrl(QUrl("http://www.spiers-software.org/SPIERSstatus-win64.txt"));
#endif

    request.setRawHeader("User-Agent", "SPIERS 2");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotError(QNetworkReply::NetworkError)));
}

void NetModule::getUpdate(QString url, QString SaveFile)
{
    output.setFileName(SaveFile);
    DoingCheck = false;

    ProgressDialog = new QProgressDialog("Update downloading.", "Cancel", 0, 100);
    ProgressDialog->setWindowModality(Qt::WindowModal);
    ProgressDialog->setMinimumDuration(0);

    connect(ProgressDialog, SIGNAL(canceled()), this, SLOT(cancel()));

    DownloadDone = false;
    DownloadError = false;
    ErrorText = "";
    output.setFileName(SaveFile);
    DoingCheck = false;

    if (!output.open(QIODevice::WriteOnly))
    {
        DownloadError = true;
        ErrorText = "Could not open file on local system";
        return;
    }

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(downloadFinished(QNetworkReply *)));
    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "SPIERS 2");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(downloadProgress (qint64, qint64 )), this, SLOT(progress(qint64, qint64 )));
}

void NetModule::doDownload(QString url, QString FileName, QProgressBar *pb)
{
//    qDebug()<<"Donwloading "<<url<<"to"<<FileName;
    DownloadDone = false;
    DownloadError = false;
    ErrorText = "";
    ProgressBar = pb;
    output.setFileName(FileName);
    DoingCheck = false;

    if (!output.open(QIODevice::WriteOnly))
    {
        DownloadError = true;
        ErrorText = "Could not open file on local system";
        return;
    }

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(downloadFinished(QNetworkReply *)));
    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "SPIERS 2");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(downloadProgress (qint64, qint64 )), this, SLOT(progress(qint64, qint64 )));
}

void NetModule::progress(qint64 bytesReceived, qint64 bytesTotal)
{
    int total = (int)((100 * bytesReceived) / (bytesTotal));
    if (ProgressBar) ProgressBar->setValue(total);
    if (ProgressDialog)
    {
        ProgressDialog->setValue(total);
    }
//    qDebug()<<"Set"<<total;
}

void NetModule::slotReadyRead()
{
//    if (ProgressDialog) qDebug()<<"readyread..."<<ProgressDialog->value();
    if (DoingCheck)
    {
        CheckFinished = true;

        //QByteArray b=reply->readAll();
        SemanticVersion version_online = SemanticVersion::fromString(reply->readLine());
        SemanticVersion version_current = SemanticVersion::fromString(UPDATEVERSION);
        qDebug() << "Version Online: " << version_online.str() << "Version Current: " << version_current.str();
        if (version_online > version_current)
        {
            //QMessageBox::information(0,"Update","Here3");
            MacBodgeClickedNoForUpdateDownload = true;
            //DownloadNeeded=true;
            DownloadURL = reply->readLine();
            if (QMessageBox::question(nullptr, "SPIERS update",
                                      "A new version of SPIERS is available.\nDo you want to download it?\n\nNote that after downloading this program will close before running the new installer.", QMessageBox::No,
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
#ifndef __APPLE__
                NetModule n2;

                QString SaveFile = QDir::tempPath() + "SPIERSupdate.exe";
//                    qDebug()<<SaveFile;
                n2.getUpdate(DownloadURL, SaveFile);
                do
                {
                    qApp->processEvents();
                    //ProgressDialog->setWindowFlags(Qt::WindowStaysOnTopHint);
                    //n2.ProgressDialog->raise();
                }
                while (n2.DownloadDone == false);

//                    qDebug()<<n2.DownloadDone;
//                    qDebug()<<n2.DownloadError;

                if (n2.DownloadError) return;

                QFileInfo f(SaveFile);
                if (f.size() < 1000000) //If < 1Mb definately something up
                {
                    QMessageBox::warning(0, "SPIERS update error",
                                         "File does not appear to have downloaded properly - this may be a virus-checker/firewall issue. Visit the SPIERS website (www.spiers-software.org) to manually download the latest version");
                    return;
                }
                //run the thing, and quit

                QMessageBox::information(0, "Update", "Updater downloaded - click OK to close this program and start the updater");
                QProcess::startDetached(SaveFile);
                //The openUrl method may work on PC, but this definately does, so stick with it


                QCoreApplication::quit();
#endif
#ifdef __APPLE__
                QMessageBox::information(0, "Update",
                                         "Updater wil be downloaded via your web-browser. Once downloaded run it manually if it does not start automatically. Close all SPIERS applications before installing!");
                QDesktopServices::openUrl(QUrl(DownloadURL, QUrl::TolerantMode));
#endif
            }
            //qDebug()<<"Here";
        }
    }
    else if (DoingHash)
    {
        CheckFinished = true;

        QString readcopy = reply->readLine();
        if (readcopy.length() < 4) return;
        if (readcopy.left(3) == "(c)" || readcopy.left(3) == "(C)")
        {
            //qDebug()<<"Read:"<<readcopy<<" Size"<<readcopy.length();

            //find first copyright item
            for (int i = 0; i < i_comm->length(); i++)
            {
                if (i_comm->at(i).left(3) == "(c)" || i_comm->at(i).left(3) == "(C)")
                {
                    i_comm->replace(i, readcopy);
                    return;
                }
                //no copyright item, so prepend this one

            }
            i_comm->prepend(readcopy);
        }
    }
    else
    {
        output.write(reply->readAll());
        //qDebug()<<"Read some stuff";
    }
    //qDebug()<<"Here2";
}

void NetModule::cancel()
{
//    qDebug()<<"In cancel"<<ProgressDialog->wasCanceled();
//    qDebug()<<"And..."<<ProgressDialog->value();
    DownloadError = true;
    DownloadDone = true;
    output.close();
    reply->abort();
    reply->deleteLater();
}

void NetModule::slotError(QNetworkReply::NetworkError Error)
{
    if (DownloadError) return;
    qDebug() << "Error reported by netmodule";
    if (DoingCheck || DoingHash) CheckFinished = true;

    else
    {
        QString SError;
        QTextStream e(&SError);
        e << "Error code: " << Error;
        ErrorText = SError;
        DownloadError = true;
        DownloadDone = true;
        output.remove();
        reply->abort();
        reply->deleteLater();

    }
}

void NetModule::downloadFinished(QNetworkReply *reply)
{
    qDebug() << "Download finished";
    if (DownloadError) return;
//    if (ProgressDialog) qDebug()<<"DoneFinished..."<<ProgressDialog->value();
    output.close();
    DownloadDone = true;
    reply->deleteLater();
    if (ProgressDialog) ProgressDialog->hide();
    DownloadError = false;
}

