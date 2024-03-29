/**
 * @file
 * Header: Constrast Impl
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

#ifndef CONTRASTIMPL_H
#define CONTRASTIMPL_H

#include <QDialog>
#include "ui_contrast.h"

/**
 * @brief The ContrastImpl class
 */
class ContrastImpl : public QDialog, public Ui::ContrastDialog
{
    Q_OBJECT
public:
    ContrastImpl( int MaskNo, QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    bool Cancelled;
    int MaskNumber;
private slots:
    void on_ContrastSlider_valueChanged(int value);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void RedrawColour();
};

#endif
