/**
 * @file
 * UpdateDialog
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "updatedialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QFile>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

UpdateDialog::UpdateDialog(const QString          &latestVersion,
                           const QList<GitHubAsset> &assets,
                           bool                    showSkipButton,
                           QWidget                *parent)
    : QDialog(parent)
    , m_latestVersion(latestVersion)
    , m_assets(assets)
    , m_table(nullptr)
    , m_nam(new QNetworkAccessManager(this))
{
    setWindowTitle("SPIERS Update Available");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setMinimumWidth(560);

    // --- header label -------------------------------------------------------
    QLabel *header = new QLabel(
        QString("<b>A new version of SPIERS (v%1) is available.</b><br>"
                "Select a file below to download it.")
            .arg(latestVersion));
    header->setTextFormat(Qt::RichText);
    header->setWordWrap(true);

    // --- assets table -------------------------------------------------------
    m_table = new QTableWidget(assets.size(), 3, this);
    m_table->setHorizontalHeaderLabels({"File", "Size", ""});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);

    for (int i = 0; i < assets.size(); ++i)
    {
        const GitHubAsset &a = assets.at(i);

        auto *nameItem = new QTableWidgetItem(a.name);
        nameItem->setToolTip(a.downloadUrl);
        m_table->setItem(i, 0, nameItem);

        auto *sizeItem = new QTableWidgetItem(formatSize(a.size));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_table->setItem(i, 1, sizeItem);

        auto *btn = new QPushButton("Download");
        btn->setFixedWidth(90);
        // capture index by value so each button knows its own row
        connect(btn, &QPushButton::clicked, this, [this, i]() { onDownload(i); });
        m_table->setCellWidget(i, 2, btn);
    }

    // --- releases page link -------------------------------------------------
    QLabel *ghLink = new QLabel(
        "<a href=\"https://github.com/palaeoware/SPIERS/releases\">"
        "View full release notes on GitHub</a>");
    ghLink->setTextFormat(Qt::RichText);
    ghLink->setOpenExternalLinks(true);

    // --- bottom buttons -----------------------------------------------------
    auto *closeBtn = new QPushButton("Close");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setContentsMargins(0, 0, 0, 0);

    if (showSkipButton)
    {
        auto *skipBtn = new QPushButton("Skip This Version");
        skipBtn->setToolTip(
            QString("Don't remind me about v%1 at startup again.").arg(latestVersion));
        connect(skipBtn, &QPushButton::clicked, this, &UpdateDialog::onSkipVersion);
        btnRow->addWidget(skipBtn);
    }

    btnRow->addStretch();
    btnRow->addWidget(closeBtn);

    // --- main layout --------------------------------------------------------
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(header);
    layout->addSpacing(8);
    layout->addWidget(m_table);
    layout->addWidget(ghLink);
    layout->addSpacing(4);
    layout->addLayout(btnRow);
}

// ---------------------------------------------------------------------------
// Private slots
// ---------------------------------------------------------------------------

/**
 * @brief Saves the current version to the shared SPIERS settings so it is
 *        silently skipped on the next automatic startup check.
 */
void UpdateDialog::onSkipVersion()
{
    QSettings settings("Palaeoware", "SPIERS");
    settings.setValue("update/skippedVersion", m_latestVersion);
    settings.sync();
    accept();
}

/**
 * @brief Prompts the user for a save path then downloads the selected asset.
 */
void UpdateDialog::onDownload(int assetIndex)
{
    if (assetIndex < 0 || assetIndex >= m_assets.size()) return;
    const GitHubAsset &asset = m_assets.at(assetIndex);
    startDownload(asset.downloadUrl, asset.name);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

/**
 * @brief Asks the user where to save the file then downloads it, showing a
 *        progress dialog throughout.
 */
void UpdateDialog::startDownload(const QString &url, const QString &suggestedName)
{
    const QString downloadsDir =
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    const QString savePath = QFileDialog::getSaveFileName(
        this,
        "Save download as...",
        downloadsDir + "/" + suggestedName);

    if (savePath.isEmpty())
        return; // user cancelled

    QFile *file = new QFile(savePath, this);
    if (!file->open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Download Error",
                             "Cannot write to: " + savePath + "\n" + file->errorString());
        file->deleteLater();
        return;
    }

    QProgressDialog *progress = new QProgressDialog(
        "Downloading " + suggestedName + "...", "Cancel", 0, 100, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setMinimumDuration(0);
    progress->setValue(0);

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "SPIERS");
    // Follow redirects — GitHub asset URLs redirect to S3
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply = m_nam->get(req);

    connect(reply, &QNetworkReply::downloadProgress,
            this, [progress](qint64 received, qint64 total)
    {
        if (total > 0)
            progress->setValue(static_cast<int>((100 * received) / total));
    });

    connect(progress, &QProgressDialog::canceled, reply, &QNetworkReply::abort);

    connect(reply, &QNetworkReply::readyRead, this, [reply, file]()
    {
        file->write(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        file->flush();
        file->close();
        progress->close();

        if (reply->error() == QNetworkReply::NoError)
        {
            QMessageBox::information(this, "Download Complete",
                                     "Saved to:\n" + savePath);
        }
        else if (reply->error() != QNetworkReply::OperationCanceledError)
        {
            file->remove();
            QMessageBox::warning(this, "Download Error",
                                 "Download failed:\n" + reply->errorString());
        }
        else
        {
            // User cancelled — remove partial file
            file->remove();
        }

        reply->deleteLater();
        progress->deleteLater();
        file->deleteLater();
    });
}

/**
 * @brief Formats a byte count as a human-readable string (B / KB / MB / GB).
 */
QString UpdateDialog::formatSize(qint64 bytes)
{
    if (bytes <= 0) return QString("—");
    if (bytes < 1024)
        return QString("%1 B").arg(bytes);
    if (bytes < 1024 * 1024)
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1024 * 1024 * 1024)
        return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
    return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}
