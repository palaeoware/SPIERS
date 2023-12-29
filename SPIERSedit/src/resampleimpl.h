/**
 * @file
 * Header: Resample
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

#ifndef RESAMPLEIMPL_H
#define RESAMPLEIMPL_H

#include <QDialog>

#include "ui_resample.h"

/**
 * @brief The resampleImpl class
 */
class resampleImpl : public QDialog, public Ui::resample
{
    Q_OBJECT
public:
    resampleImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
private:
    int ClosestTo(QString NewFileString);
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif





