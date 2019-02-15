/**
 * @file
 * Header: Settings Impl.
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

#ifndef SETTINGSIMPL_H
#define SETTINGSIMPL_H

#include <QDialog>

#include "ui_settings.h"

/**
 * @brief The SettingsImpl class
 */
class SettingsImpl : public QDialog, public Ui::Settings
{
    Q_OBJECT
public:
    SettingsImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
private slots:
    void on_RecompressSourceFiles_pressed();
    void on_RecompressFiles_pressed();
    void on_SliderFileCompression_valueChanged(int value);
    void on_SliderCacheCompression_valueChanged(int value);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushButton_clicked();
};

#endif
