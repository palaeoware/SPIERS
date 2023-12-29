/**
 * @file
 * Source: SlicePacingDialogImpl
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

#include "slicespacingdialogimpl.h"
#include "globals.h"

bool stoploopflag;

slicespacingdialogImpl::slicespacingdialogImpl( QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    Absolute->setValue(1.0 / SlicePerMM);
    stoploopflag = false;
}

void slicespacingdialogImpl::on_Relative_valueChanged(double )
{
    if (stoploopflag == false)
    {
        stoploopflag = true;
        Absolute->setValue((Relative->value() / 100) / SlicePerMM);
        stoploopflag = false;
    }
}

void slicespacingdialogImpl::on_Absolute_valueChanged(double )
{
    if (stoploopflag == false)
    {
        stoploopflag = true;
        Relative->setValue(100 * (Absolute->value()*SlicePerMM));
        stoploopflag = false;
    }
}

void slicespacingdialogImpl::on_buttonBox_accepted()
{
    relative = Relative->value() / 100;
    close();
}

void slicespacingdialogImpl::on_buttonBox_rejected()
{
    relative = -1;
    close();
}

