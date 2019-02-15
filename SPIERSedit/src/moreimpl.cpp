/**
 * @file
 * Source: MoreImpl
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "moreimpl.h"
#include "globals.h"

moreimpl::moreimpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(OK_Click()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(Cancel_Click()));

    //now set up the list
    foreach (RecentFiles rf, RecentFileList)
        listWidget->addItem(rf.File);
}

void moreimpl::OK_Click()
{
    QListWidgetItem *retitem;
    retitem = listWidget->currentItem();
    fname = retitem->text();
    Cancelled = false;
    close();
}


void moreimpl::Cancel_Click()
{
    Cancelled = true;
    close();
}

