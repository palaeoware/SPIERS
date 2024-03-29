/**
 * @file
 * NetModule
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2023 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "netmodule.h"
#include "semanticversion.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QString>
#include <QDesktopServices>
#include <QUrl>

bool macClickedNoForUpdateDownload;

/**
 * @brief NetModule::NetModule
 */
NetModule::NetModule(): QObject()
{
    checkFinished = false;
    downloadURL = "";
    doingCheck = false;
    downloadDone = false;
    downloadError = false;
    errorText = "";
    progressBar = nullptr;
    progressDialog = nullptr;
}

/**
 * @brief NetModule::checkHash
 * @param stlhash
 * @param commlist
 */
void NetModule::checkHash(QByteArray stlhash, QStringList *commlist)
{
    doingHash = true;
    i_comm = commlist;

    QString h1(stlhash.toHex());

    //looks for a file on server named for the hash value of the STLs
    QNetworkRequest request;
    manager = new QNetworkAccessManager(this);

    QString url;
    QTextStream t(&url);
    t << "https://www.spiers-software.org/" << stlhash.toHex() << ".txt";
    request.setUrl(QUrl(url));

    request.setRawHeader("User-Agent", "SPIERS");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

/**
 * @brief NetModule::checkForNew
 */
void NetModule::checkForNew()
{
    //return; //deactivate
    QNetworkRequest request;
    doingCheck = true;
    manager = new QNetworkAccessManager(this);

    request.setUrl(QUrl("https://www.spiers-software.org/SPIERSUpdateStatus.txt"));

    request.setRawHeader("User-Agent", "SPIERS");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

/**
 * @brief NetModule::getUpdate
 * @param url
 * @param SaveFile
 */
void NetModule::getUpdate(QString url, QString saveFile)
{
    output.setFileName(saveFile);
    doingCheck = false;

    progressDialog = new QProgressDialog("Update downloading.", "Cancel", 0, 100);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0);

    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancel()));

    downloadDone = false;
    downloadError = false;
    errorText = "";
    output.setFileName(saveFile);
    doingCheck = false;

    if (!output.open(QIODevice::WriteOnly))
    {
        downloadError = true;
        errorText = "Could not open file on local system";
        return;
    }

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(downloadFinished(QNetworkReply *)));
    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "SPIERS");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(downloadProgress (qint64, qint64 )), this, SLOT(progress(qint64, qint64 )));
}

/**
 * @brief NetModule::doDownload
 * @param url
 * @param FileName
 * @param pb
 */
void NetModule::doDownload(QString url, QString fileName, QProgressBar *pb)
{
    // qDebug()<<"Donwloading "<<url<<"to"<<FileName;
    downloadDone = false;
    downloadError = false;
    errorText = "";
    progressBar = pb;
    output.setFileName(fileName);
    doingCheck = false;

    if (!output.open(QIODevice::WriteOnly))
    {
        downloadError = true;
        errorText = "Could not open file on local system";
        return;
    }

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(downloadFinished(QNetworkReply *)));
    QNetworkRequest request;

    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "SPIERS");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(downloadProgress (qint64, qint64 )), this, SLOT(progress(qint64, qint64 )));
}

/**
 * @brief NetModule::progress
 * @param bytesReceived
 * @param bytesTotal
 */
void NetModule::progress(qint64 bytesReceived, qint64 bytesTotal)
{
    int total = static_cast<int>((100 * bytesReceived) / (bytesTotal));
    if (progressBar) progressBar->setValue(total);
    if (progressDialog)
    {
        progressDialog->setValue(total);
    }
}

/**
 * @brief NetModule::slotReadyRead
 */
void NetModule::slotReadyRead()
{
    // if (progressDialog) qDebug()<<"readyread..."<<progressDialog->value();
    if (doingCheck)
    {
        checkFinished = true;

        //QByteArray b=reply->readAll();
        SemanticVersion versionOnline = SemanticVersion::fromString(reply->readLine());
        SemanticVersion versionCurrent = SemanticVersion::fromString(SOFTWARE_VERSION);
        qDebug() << "Version Online: " << versionOnline.str() << "Version Current: " << versionCurrent.str();

        if (versionOnline > versionCurrent)
        {
            downloadURL = reply->readLine();

            QMessageBox *popupMessage = new QMessageBox;
            popupMessage->setParent(nullptr);
            popupMessage->setWindowTitle("SPIERS Update Available");
            popupMessage->setTextFormat(Qt::RichText);
            popupMessage->setText("<p>A new version of SPIERS (v" + versionOnline.str() +
                                  ") is available for download.</p><p>Please visit <a href=\"https://github.com/palaeoware/SPIERS/releases\"><span style=\"color: white\">https://github.com/palaeoware/SPIERS/releases</span></a>.</p>");
            popupMessage->setStandardButtons(QMessageBox::Close);
            popupMessage->setDefaultButton(QMessageBox::Close);
            popupMessage->setWindowFlags(Qt::WindowStaysOnTopHint);
            popupMessage->raise();
            popupMessage->exec();
        }
    }
    else if (doingHash)
    {
        checkFinished = true;

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

/**
 * @brief NetModule::cancel
 */
void NetModule::cancel()
{
    // qDebug()<<"In cancel"<<ProgressDialog->wasCanceled();
    // qDebug()<<"And..."<<ProgressDialog->value();
    downloadError = true;
    downloadDone = true;
    output.close();
    reply->abort();
    reply->deleteLater();
}

/**
 * @brief NetModule::slotError
 * @param Error
 */
void NetModule::slotError(QNetworkReply::NetworkError Error)
{
    if (downloadError) return;
    qDebug() << "Error reported by netmodule";
    if (doingCheck || doingHash) checkFinished = true;

    else
    {
        QString SError;
        QTextStream e(&SError);
        e << "Error code: " << Error;
        errorText = SError;
        downloadError = true;
        downloadDone = true;
        output.remove();
        reply->abort();
        reply->deleteLater();
    }
}

/**
 * @brief NetModule::downloadFinished
 * @param reply
 */
void NetModule::downloadFinished(QNetworkReply *reply)
{
    qDebug() << "Download finished";
    if (downloadError) return;
    // if (ProgressDialog) qDebug()<<"DoneFinished..."<<ProgressDialog->value();
    output.close();
    downloadDone = true;
    reply->deleteLater();
    if (progressDialog) progressDialog->hide();
    downloadError = false;
}

