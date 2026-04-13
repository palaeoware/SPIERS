/**
 * @file
 * Source: Crash Dialog
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

#include "crashdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

CrashDialog::CrashDialog(const QString &appName, const QString &crashReport, QWidget *parent)
    : QDialog(parent), m_crashReport(crashReport)
{
    setWindowTitle(appName + QStringLiteral(" - Crash Report"));
    setMinimumWidth(700);
    setMinimumHeight(600);
    setModal(true);

    setupUI(appName, crashReport);
}

CrashDialog::~CrashDialog()
{
}

void CrashDialog::setupUI(const QString &appName, const QString &crashReport)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    /// Title
    titleLabel = new QLabel(this);
    titleLabel->setText(appName + QStringLiteral(" has crashed"));
    titleLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 14px; margin-bottom: 10px;"));
    mainLayout->addWidget(titleLabel);

    /// Description
    QLabel *descLabel = new QLabel(this);
    descLabel->setText(QStringLiteral("An unexpected error occurred. The crash report below can be copied and submitted to help us fix the issue."));
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);

    /// Crash report text area
    reportTextEdit = new QTextEdit(this);
    reportTextEdit->setPlainText(crashReport);
    reportTextEdit->setReadOnly(true);
    reportTextEdit->setStyleSheet(QStringLiteral("background-color: #f5f5f5; font-family: monospace; font-size: 9pt;"));
    mainLayout->addWidget(reportTextEdit);

    /// Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    copyButton = new QPushButton(QStringLiteral("Copy to Clipboard"), this);
    connect(copyButton, &QPushButton::clicked, this, &CrashDialog::on_copyButton_clicked);
    buttonLayout->addWidget(copyButton);

    githubButton = new QPushButton(QStringLiteral("Open GitHub Issues"), this);
    githubButton->setStyleSheet(QStringLiteral("QPushButton { background-color: #28a745; color: white; padding: 5px; border-radius: 3px; }"));
    connect(githubButton, &QPushButton::clicked, this, &CrashDialog::on_githubButton_clicked);
    buttonLayout->addWidget(githubButton);

    buttonLayout->addStretch();

    closeButton = new QPushButton(QStringLiteral("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &CrashDialog::on_closeButton_clicked);
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void CrashDialog::on_copyButton_clicked()
{
    QApplication::clipboard()->setText(m_crashReport);
    copyButton->setText(QStringLiteral("Copied!"));
    copyButton->setEnabled(false);

    /// Re-enable button after 2 seconds
    QTimer::singleShot(2000, this, [this]() {
        copyButton->setText(QStringLiteral("Copy to Clipboard"));
        copyButton->setEnabled(true);
    });
}

void CrashDialog::on_githubButton_clicked()
{
    /// Copy report to clipboard first
    QApplication::clipboard()->setText(m_crashReport);

    /// Open GitHub issues page
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://github.com/palaeoware/SPIERS/issues/new")));
}

void CrashDialog::on_closeButton_clicked()
{
    accept();
}
