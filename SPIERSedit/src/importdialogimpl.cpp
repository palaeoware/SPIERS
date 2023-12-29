/**
 * @file
 * Source: ImportDialog
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

#include "importdialogimpl.h"

ImportDialogImpl::ImportDialogImpl( QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(OK_Click()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(Cancel_Click()));
    LabelDownsample->setVisible(false);
    spinBox->setVisible(false);
    spinBoxZ->setVisible(false);
    CheckMirrored->setVisible(false);
    CheckMirrored->setChecked(true);
    Cancelled = true;
}

void ImportDialogImpl::OK_Click()
{
    fname = lineEdit->text();
    notes = textEdit->toPlainText();
    Cancelled = false;
    close();
}

void ImportDialogImpl::HideCopy()
{
    //checkBox->setVisible(false);
    LabelDownsample->setVisible(true);
    spinBox->setVisible(true);
    spinBoxZ->setVisible(true);
    CheckMirrored->setVisible(true);
}

void ImportDialogImpl::Cancel_Click()
{
    Cancelled = true;
    close();
}


