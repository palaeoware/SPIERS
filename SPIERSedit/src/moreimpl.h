/**
 * @file
 * Header: More Impl.
 *
 * All SPIERSedit code is released under the GNU General Public License.
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

#ifndef MOREIMPL_H
#define MOREIMPL_H

#include "ui_more.h"

/**
 * @brief The moreimpl class
 */
class moreimpl : public QDialog, public Ui::Dialog
{
    Q_OBJECT
public:
    moreimpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    QString fname;
    bool Cancelled;
private slots:
    void OK_Click();
    void Cancel_Click();
};
#endif






