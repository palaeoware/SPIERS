/**
 * @file
 * Header: Undo
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

#ifndef __UNDO_H__
#define __UNDO_H__

#include <QImage>
#include <QByteArray>
#include <QList>

#include "mainwindowimpl.h"
#include "globals.h"
#include "curves.h"

//max amount of memory (roughly) for buffer
extern int TotalUndoSize;

/**
 * @brief The UndoDataObject class
 */
class UndoDataObject
{
public:
    UndoDataObject(int type, const int curve_index);
    int Type; //-3=masks, -2=locks, -1=curves, 0+ = segments (seg number)
    QByteArray StoredData;
    QImage StoredImage;
    PointList CurvePoints;
    int CurveNumber;
    int FileNumber;
    int undosize;

};

/**
 * @brief The UndoEvent class
 */
class UndoEvent
{
public:
    UndoEvent(int Dfrom, int Dto);
    ~UndoEvent();
    void Undo(MainWindowImpl *m);
    void Redo(MainWindowImpl *m);
    QString Type;
    int FileNumber; //if this ISN'T -1 then it's index of file moved to - a move event in fact
    int FileTo; //move To this (for redo)
    QList <class UndoDataObject *> DataObjects;
};

extern QList <UndoEvent *> UndoEvents;
extern QList <UndoEvent *> RedoEvents;

#endif // __UNDO_H__
