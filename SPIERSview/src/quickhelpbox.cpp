/**
 * @file
 * Source: Quickhelpbox
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
#include "quickhelpbox.h"
#include "ui_quickhelpbox.h"

/**
 * @brief QuickHelpBox::QuickHelpBox
 * @param parent
 */
QuickHelpBox::QuickHelpBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickHelpBox)
{
    ui->setupUi(this);
}

/**
 * @brief QuickHelpBox::~QuickHelpBox
 */
QuickHelpBox::~QuickHelpBox()
{
    delete ui;
}
