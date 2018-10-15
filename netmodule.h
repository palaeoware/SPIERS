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

//#define UPDATEVERSION "3.0.0-alpha"
#define UPDATEVERSION "1.0.0"


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
