/**
 * @file
 * Header: About
 *
 * All SPIERSalign code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSalign code is Copyright 2008-2019 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

#include "ui_about.h"

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

private:
    Ui::About *ui;
    QString returnLicense();
};

#endif // ABOUT_H
