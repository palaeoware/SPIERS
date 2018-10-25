/**
 * @file
 * Header: Resize Dialog Impl.
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef RESIZEDIALOGIMPL_H
#define RESIZEDIALOGIMPL_H

#include <QDialog>
#include <QIcon>

#include "ui_curveresize.h"

/**
 * @brief The ResizeDialogImpl class
 */
class ResizeDialogImpl : public QDialog, public Ui::ResizeDialog
{
    Q_OBJECT
public:
    ResizeDialogImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    bool Cancelled;
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};
#endif
