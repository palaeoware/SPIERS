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

#define UPDATEVERSION "3.0.0-alpha"

extern bool MacBodgeClickedNoForUpdateDownload;

class NetModule : public QObject
{
    Q_OBJECT

public:
    bool CheckFinished;
    bool DownloadNeeded;
    bool DoingCheck;
    bool DoingHash;
    bool DownloadDone;
    bool DownloadError;
    QFile output;
    QProgressBar *ProgressBar;
    QProgressDialog *ProgressDialog;
    QStringList *i_comm;
    QString ErrorText;
    QString DownloadURL;

    NetModule();
    void CheckForNew();
    void CheckHash(QByteArray vaxmlhash, QStringList *commlist);
    void doDownload(QString url, QString FileName, QProgressBar *pb);
    void getUpdate(QString url, QString SaveFile);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

private slots:
    void cancel();
    void downloadFinished(QNetworkReply *reply);
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError Error);
};

#endif // NETMODULE_H
