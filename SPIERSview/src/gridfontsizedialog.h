/**
 * @file
 * Header: Scale Grid Font Size Dialog
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef GRIDFONTSIZEDIALOG_H
#define GRIDFONTSIZEDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class GridFontSizeDialog;
}

class GridFontSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridFontSizeDialog(QWidget *parent = nullptr);
    ~GridFontSizeDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::GridFontSizeDialog *ui;
};

#endif // GRIDFONTSIZEDIALOG_H
