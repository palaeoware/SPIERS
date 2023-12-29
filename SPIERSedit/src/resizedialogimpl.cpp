/**
 * @file
 * Source: ResizeDialog
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "resizedialogimpl.h"

ResizeDialogImpl::ResizeDialogImpl( QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

}

void ResizeDialogImpl::on_buttonBox_accepted()
{
    Cancelled = false;
    close();
}

void ResizeDialogImpl::on_buttonBox_rejected()
{
    Cancelled = true;
    close();
}

