/**
 * @file
 * NetModule
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

#include "netmodule.h"
#include "semanticversion.h"

#include <QNetworkInformation>

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QString>
#include <QDesktopServices>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include "updatedialog.h"

bool macClickedNoForUpdateDownload;

/**
 * @brief NetModule::NetModule
 */
NetModule::NetModule(QObject *parent): QObject(parent)
{
    checkFinished = false;
    downloadURL = "";
    doingCheck = false;
    doingGitHubCheck = false;
    manualCheck = false;
    testVersion = "";
    downloadDone = false;
    downloadError = false;
    errorText = "";
    progressBar = nullptr;
    progressDialog = nullptr;
}

/**
 * @brief NetModule::isOnline
 * Returns true when the system has (or may have) internet connectivity.
 * Treats Unknown reachability as online so the actual network request acts
 * as the real gatekeeper.  Returns true unconditionally when no
 * QNetworkInformation backend is loaded.
 */
bool NetModule::isOnline()
{
    const QNetworkInformation *ni = QNetworkInformation::instance();
    if (!ni)
        return true; // no backend — assume online
    return ni->reachability() != QNetworkInformation::Reachability::Disconnected;
}

/**
 * @brief NetModule::startConnectivityWatch
 * Loads the QNetworkInformation backend (if available) and connects its
 * reachabilityChanged signal so that this object emits connectivityChanged()
 * whenever the system goes online or offline.
 */
void NetModule::startConnectivityWatch()
{
    if (!QNetworkInformation::loadBackendByFeatures(QNetworkInformation::Feature::Reachability))
    {
        qDebug() << "NetModule: QNetworkInformation backend unavailable; connectivity watch disabled.";
        return;
    }

    QNetworkInformation *ni = QNetworkInformation::instance();
    if (!ni) return;

    connect(ni, &QNetworkInformation::reachabilityChanged, this,
            [this](QNetworkInformation::Reachability r)
            {
                const bool online = (r != QNetworkInformation::Reachability::Disconnected);
                qDebug() << "NetModule: connectivity changed — online:" << online;
                emit connectivityChanged(online);
            });

    qDebug() << "NetModule: connectivity watch active — currently online:" << isOnline();
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
    t << "https://spiers-software.org/" << stlhash.toHex() << ".txt";
    request.setUrl(QUrl(url));

    request.setRawHeader("User-Agent", "SPIERS");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

/**
 * @brief NetModule::checkForNewManual
 * Same as checkForNew() but also shows a dialog when no update is found,
 * suitable for a user-triggered "Check for Updates" menu action.
 * Shows an informational message and returns immediately if offline.
 */
void NetModule::checkForNewManual()
{
    if (!isOnline())
    {
        QMessageBox *msg = new QMessageBox;
        msg->setParent(nullptr);
        msg->setWindowTitle("No Internet Connection");
        msg->setIcon(QMessageBox::Information);
        msg->setText("No internet connection is available.\n\nPlease check your connection and try again.");
        msg->setStandardButtons(QMessageBox::Ok);
        msg->setWindowFlags(msg->windowFlags() | Qt::WindowStaysOnTopHint);
        msg->raise();
        msg->exec();
        return;
    }
    manualCheck = true;
    checkForNew();
}

/**
 * @brief NetModule::checkForNew
 * Tries the GitHub releases API first; falls back to the legacy .txt file if
 * the API request fails or returns an unparseable response.
 * Emits updateCheckFinished() immediately and returns if offline.
 */
void NetModule::checkForNew()
{
    //return; //force deactivate

    if (!isOnline())
    {
        qDebug() << "NetModule: update check skipped — no internet connection.";
        emit updateCheckFinished();
        return;
    }

    doingCheck = true;
    doingGitHubCheck = true;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotGitHubCheckFinished(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl("https://api.github.com/repos/palaeoware/SPIERS/releases/latest"));
    request.setRawHeader("User-Agent", "SPIERS");
    request.setRawHeader("Accept", "application/vnd.github+json");
    manager->get(request);
}

/**
 * @brief NetModule::showUpdateAvailableDialog
 * Shows the UpdateDialog with asset download links.
 * @param versionString  Latest version, e.g. "4.1.0"
 * @param assets         Downloadable files from the GitHub release (may be empty
 *                       when falling back to the legacy .txt path)
 */
void NetModule::showUpdateAvailableDialog(const QString &versionString,
                                          const QList<GitHubAsset> &assets)
{
    UpdateDialog dlg(versionString, assets, !manualCheck);
    dlg.exec();
}

/**
 * @brief NetModule::showNoUpdateDialog
 * Shows a dialog confirming the software is up to date (manual check only).
 */
void NetModule::showNoUpdateDialog(const QString &currentVersion, const QString &latestVersion)
{
    QMessageBox *msg = new QMessageBox;
    msg->setParent(nullptr);
    msg->setWindowTitle("No Update Available");
    msg->setIcon(QMessageBox::Information);
    msg->setText(QString("No update found.\n\nYou are on v%1.\nLatest GitHub version is v%2.")
                     .arg(currentVersion, latestVersion));
    msg->setStandardButtons(QMessageBox::Ok);
    msg->setWindowFlags(msg->windowFlags() | Qt::WindowStaysOnTopHint);
    msg->raise();
    msg->exec();
}

/**
 * @brief NetModule::fallbackToTxtCheck
 * Makes a request to the legacy plain-text version file as a fallback.
 */
void NetModule::fallbackToTxtCheck()
{
    qDebug() << "Falling back to .txt version check";
    doingGitHubCheck = false;

    manager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl("https://spiers-software.org/SPIERSUpdateStatus.txt"));
    request.setRawHeader("User-Agent", "SPIERS");
    reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

/**
 * @brief NetModule::slotGitHubCheckFinished
 * Handles the response from the GitHub releases API.
 * Parses the JSON tag_name field; falls back to the .txt file on any failure.
 */
void NetModule::slotGitHubCheckFinished(QNetworkReply *ghReply)
{
    bool parsedOk = false;

    if (ghReply->error() == QNetworkReply::NoError)
    {
        const QByteArray data = ghReply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(data);

        if (!doc.isNull() && doc.isObject())
        {
            const QJsonObject root = doc.object();
            QString tagName = root.value("tag_name").toString().trimmed();

            // Strip a leading 'v' (e.g. "v3.1.0" → "3.1.0")
            if (tagName.startsWith('v') || tagName.startsWith('V'))
                tagName = tagName.mid(1);

            // Parse the assets array into GitHubAsset structs
            QList<GitHubAsset> assets;
            const QJsonArray jsonAssets = root.value("assets").toArray();
            for (const QJsonValue &v : jsonAssets)
            {
                if (!v.isObject()) continue;
                const QJsonObject obj = v.toObject();
                GitHubAsset a;
                a.name        = obj.value("name").toString();
                a.downloadUrl = obj.value("browser_download_url").toString();
                a.size        = static_cast<qint64>(obj.value("size").toDouble());
                if (!a.name.isEmpty() && !a.downloadUrl.isEmpty())
                    assets.append(a);
            }

            // Append the source-code archives (not in assets[], provided separately)
            const QString zipUrl    = root.value("zipball_url").toString();
            const QString tarUrl    = root.value("tarball_url").toString();
            const QString tagForName = root.value("tag_name").toString().trimmed();
            if (!zipUrl.isEmpty())
            {
                GitHubAsset zip;
                zip.name        = QString("source-code-%1.zip").arg(tagForName);
                zip.downloadUrl = zipUrl;
                zip.size        = 0; // size not provided by API for source archives
                assets.append(zip);
            }
            if (!tarUrl.isEmpty())
            {
                GitHubAsset tar;
                tar.name        = QString("source-code-%1.tar.gz").arg(tagForName);
                tar.downloadUrl = tarUrl;
                tar.size        = 0;
                assets.append(tar);
            }

            try
            {
                SemanticVersion versionOnline  = SemanticVersion::fromString(tagName);
                SemanticVersion versionCurrent = SemanticVersion::fromString(
                    testVersion.isEmpty() ? QString(SOFTWARE_VERSION) : testVersion);
                qDebug() << "GitHub version:" << versionOnline.str() << " Current:" << versionCurrent.str();

                parsedOk = true;
                checkFinished = true;

                if (versionOnline > versionCurrent)
                {
                    // On automatic check, honour the user's "skip this version" preference
                    if (!manualCheck)
                    {
                        QSettings settings("Palaeoware", "SPIERS");
                        const QString skipped = settings.value("update/skippedVersion").toString();
                        if (skipped == versionOnline.str())
                        {
                            qDebug() << "Version" << versionOnline.str() << "is skipped by user preference.";
                            ghReply->deleteLater();
                            emit updateCheckFinished();
                            return;
                        }
                    }
                    showUpdateAvailableDialog(versionOnline.str(), assets);
                }
                else if (manualCheck)
                {
                    showNoUpdateDialog(versionCurrent.str(), versionOnline.str());
                }
            }
            catch (const std::exception &e)
            {
                qDebug() << "GitHub version parse failed:" << e.what();
            }
        }
        else
        {
            qDebug() << "GitHub API returned non-JSON or empty response";
        }
    }
    else
    {
        qDebug() << "GitHub version check network error:" << ghReply->errorString();
    }

    ghReply->deleteLater();

    if (!parsedOk)
        fallbackToTxtCheck();
    else
        emit updateCheckFinished();
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
    connect(reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
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
    connect(reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
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

        try
        {
            //QByteArray b=reply->readAll();
            SemanticVersion versionOnline = SemanticVersion::fromString(QString(reply->readLine()).trimmed());
            SemanticVersion versionCurrent = SemanticVersion::fromString(
                    testVersion.isEmpty() ? QString(SOFTWARE_VERSION) : testVersion);
            qDebug() << "Version Online: " << versionOnline.str() << "Version Current: " << versionCurrent.str();

            if (versionOnline > versionCurrent)
                showUpdateAvailableDialog(versionOnline.str());
            else if (manualCheck)
                showNoUpdateDialog(versionCurrent.str(), versionOnline.str());

            emit updateCheckFinished();
        }
        catch (const std::exception &e)
        {
            qDebug() << "Version check failed (malformed response):" << e.what();
            emit updateCheckFinished();
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
    if (doingCheck || doingHash)
    {
        checkFinished = true;
        emit updateCheckFinished();
    }
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

