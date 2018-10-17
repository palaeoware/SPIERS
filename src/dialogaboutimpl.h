/**
 * @file
 * Header: Dialog About Impl.
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef DIALOGABOUTIMPL_H
#define DIALOGABOUTIMPL_H

#include <QDialog>
#include "../ui/ui_about.h"

/**
 * @brief The DialogAboutImpl class
 */
class DialogAboutImpl : public QDialog, public Ui::About
{
    Q_OBJECT

public:
    DialogAboutImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );

private:
    QString returnLicense();
};

#endif // ABOUT_H
