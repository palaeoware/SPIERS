/**
 * @file
 * Header: BackThread
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

#ifndef __BACKTHREAD_H__
#define __BACKTHREAD_H__

#include <QThread>

/**
 * @brief The MyThread class
 */
class MyThread : public QThread
{
private:
    Q_OBJECT

public:
    void run();

private slots:
    void TimerFired();
    bool TryCaching(int fnumber);
};

extern MyThread *BackThread;

#endif // __BACKTHREAD_H__
