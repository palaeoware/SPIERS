/**
 * @file
 * Header: Main
 *
 * All SPIERSview code is released under the GNU General Public License.
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

#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QString>
#include <QEvent>
#include <QKeyEvent>

class main : public QApplication
{
    Q_OBJECT
public:
    main(int &argc, char *argv[]);

    QString fn;
    bool namereceived;
    bool donthandlefileevent;


private:
    // bool event(QEvent *);
};

#endif // MAIN_H
