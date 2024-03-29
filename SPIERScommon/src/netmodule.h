/**
 * @file
 * Header: NetModule
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

#ifndef NETMODULE_H
#define NETMODULE_H

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QProgressDialog>
#include <QByteArray>
#include <QStringList>

#include <stdio.h>

extern bool macClickedNoForUpdateDownload;

class NetModule : public QObject
{
    Q_OBJECT

public:
    bool checkFinished;
    bool doingCheck;
    bool doingHash;
    bool downloadDone;
    bool downloadError;
    QFile output;
    QProgressBar *progressBar;
    QProgressDialog *progressDialog;
    QStringList *i_comm;
    QString errorText;
    QString downloadURL;

    NetModule();
    void checkForNew();
    void checkHash(QByteArray vaxmlhash, QStringList *commlist);
    void doDownload(QString url, QString fileName, QProgressBar *pb);
    void getUpdate(QString url, QString saveFile);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

private slots:
    void cancel();
    void downloadFinished(QNetworkReply *reply);
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError error);
};

#endif // NETMODULE_H
