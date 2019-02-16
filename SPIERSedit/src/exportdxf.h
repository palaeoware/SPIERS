/**
 * @file
 * Header: Export DXF
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

#ifndef EXPORTDXF_H
#define EXPORTDXF_H

#include <QDialog>

#include "ui_Copying.h"
#include "mainwindowimpl.h"

/**
 * @brief The ExportDXF class
 */
class ExportDXF : public QDialog, public Ui::Copying
{
    Q_OBJECT
public:
    ExportDXF( MainWindowImpl *mwp, QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    MainWindowImpl *mw;
};

#endif





