/**
 * @file
 * Header: NetModule
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2019 by Russell J. Garwood, Mark D. Sutton,
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
#include <QNetworkInformation>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QProgressDialog>
#include <QByteArray>
#include <QStringList>
#include <QTextStream>
#include "updatedialog.h"

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

    explicit NetModule(QObject *parent = nullptr);
    void checkForNew();
    void checkForNewManual();

    /**
     * @brief Returns true when the system has (or may have) internet access.
     *
     * Uses QNetworkInformation when a backend is loaded.  Falls back to true
     * (assume online) if no backend is available, so the actual network request
     * is the gatekeeper.
     */
    static bool isOnline();

    /**
     * @brief Loads the QNetworkInformation backend and starts watching for
     *        connectivity changes.  Call once after constructing this object.
     *        Emits connectivityChanged(bool) whenever reachability changes.
     */
    void startConnectivityWatch();

    /**
     * @brief Override the current version used for update comparisons.
     *
     * Set this to a lower version string (e.g. "1.0.0") before calling
     * checkForNew() or checkForNewManual() to exercise the update dialog
     * without changing SOFTWARE_VERSION.  Leave empty (the default) for
     * normal behaviour.
     *
     * Example:
     * @code
     *   NetModule n;
     *   n.setTestVersion("1.0.0");
     *   n.checkForNewManual();
     * @endcode
     */
    void setTestVersion(const QString &version) { testVersion = version; }
    void checkHash(QByteArray vaxmlhash, QStringList *commlist);
    void doDownload(QString url, QString fileName, QProgressBar *pb);
    void getUpdate(QString url, QString saveFile);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    bool    doingGitHubCheck;
    bool    manualCheck;
    QString testVersion; ///< When non-empty, used instead of SOFTWARE_VERSION for comparisons

    void showUpdateAvailableDialog(const QString &versionString,
                                   const QList<GitHubAsset> &assets = {});
    void showNoUpdateDialog(const QString &currentVersion, const QString &latestVersion);
    void fallbackToTxtCheck();

signals:
    /**
     * @brief Emitted once the automatic update check has fully completed —
     *        either an update dialog was shown and closed, no update was found,
     *        the version was skipped, or a network error occurred.
     *        Not emitted when we are about to fall back to the .txt check;
     *        the .txt path emits it instead when it finishes.
     */
    void updateCheckFinished();

    /**
     * @brief Emitted when the system's internet reachability changes.
     * @param online  true if the system can reach the internet, false if disconnected.
     *
     * Only fired after startConnectivityWatch() has been called and a
     * QNetworkInformation backend is available.
     */
    void connectivityChanged(bool online);

private slots:
    void cancel();
    void downloadFinished(QNetworkReply *reply);
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void slotReadyRead();
    void slotError(QNetworkReply::NetworkError error);
    void slotGitHubCheckFinished(QNetworkReply *ghReply);
};

#endif // NETMODULE_H
