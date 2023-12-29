/**
 * @file
 * Header: Distribute Dialog Impl.
 *
 * All SPIERSedit code is released under the GNU General Public License.
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

#ifndef DISTRIBUTEDIALOGIMPL_H
#define DISTRIBUTEDIALOGIMPL_H

#include <QDialog>

#include "ui_distribute.h"

/**
 * @brief The DistributeDialogImpl class
 */
class DistributeDialogImpl : public QDialog, public Ui::DistributeDialog
{
    Q_OBJECT
public:
    DistributeDialogImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
private slots:
    void on_SpinBoxFrom_valueChanged(int );
    void on_SpinBoxTo_valueChanged(int );
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};
#endif





