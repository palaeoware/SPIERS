/**
 * @file
 * Header: About
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class aboutdialog;
}

class aboutdialog : public QDialog
{
    Q_OBJECT
public:
    aboutdialog(QWidget *parent = nullptr);
    ~aboutdialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::aboutdialog *m_ui;

private slots:
    void on_closebutton_clicked();
    QString returnLicense();
};

#endif // ABOUTDIALOG_H
