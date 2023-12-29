/**
 * @file
 * Header: MainWindow
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Alan R.T. Spencer, Russell J. Garwood,
 * and Mark D. Sutton.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Ui::MainWindow *ui;

    void getFileVersions();

private:
    QDir directory;
    QStringList files;
    QTimer *startTimer;

    void invertEndian(unsigned char *data, int count);

private slots:
    void on_actionBugIssueFeatureRequest_triggered();
    void on_actionCode_on_GitHub_triggered();
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void startTimerFired();
    void on_selectDirectoryButton_clicked();
};

extern MainWindow *mainWindow;

#endif // MAINWINDOW_H
