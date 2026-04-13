/**
 * @file
 * Header: Crash Dialog
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

#ifndef CRASHDIALOG_H
#define CRASHDIALOG_H

#include <QDialog>

class QTextEdit;
class QPushButton;
class QLabel;

/**
 * @brief The CrashDialog class — displays crash report with user actions
 */
class CrashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrashDialog(const QString &appName, const QString &crashReport, QWidget *parent = nullptr);
    ~CrashDialog();

private slots:
    /// User clicked "Copy to Clipboard" button
    void on_copyButton_clicked();

    /// User clicked "Open GitHub Issues" button
    void on_githubButton_clicked();

    /// User clicked "Close" button
    void on_closeButton_clicked();

private:
    /// Create UI layout and widgets
    void setupUI(const QString &appName, const QString &crashReport);

    /// UI widgets
    QLabel *titleLabel;
    QTextEdit *reportTextEdit;
    QPushButton *copyButton;
    QPushButton *githubButton;
    QPushButton *closeButton;

    /// Store report for copying to clipboard
    QString m_crashReport;
};

#endif // CRASHDIALOG_H
