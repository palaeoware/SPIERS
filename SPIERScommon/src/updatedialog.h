/**
 * @file
 * Header: UpdateDialog
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2024 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>

class QLabel;
class QTableWidget;
class QPushButton;
class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;
class QFile;

/**
 * @brief Holds metadata for a single downloadable asset from a GitHub release.
 */
struct GitHubAsset
{
    QString name;
    QString downloadUrl;
    qint64  size = 0; ///< size in bytes; 0 if unknown
};

/**
 * @brief Dialog shown when a SPIERS update is available.
 *
 * Displays the new version, a table of downloadable release assets (each with
 * a Download button), and optionally a "Skip This Version" button for the
 * automatic startup check.
 */
class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @param latestVersion   Version string from GitHub, e.g. "4.1.0"
     * @param assets          List of downloadable assets in the release
     * @param showSkipButton  Pass true for the automatic check; false for manual
     * @param parent          Optional parent widget
     */
    explicit UpdateDialog(const QString          &latestVersion,
                          const QList<GitHubAsset> &assets,
                          bool                    showSkipButton,
                          QWidget                *parent = nullptr);

private slots:
    void onSkipVersion();
    void onDownload(int assetIndex);

private:
    QString            m_latestVersion;
    QList<GitHubAsset> m_assets;
    QTableWidget      *m_table;
    QNetworkAccessManager *m_nam;

    void startDownload(const QString &url, const QString &suggestedName);
    static QString formatSize(qint64 bytes);
};

#endif // UPDATEDIALOG_H
