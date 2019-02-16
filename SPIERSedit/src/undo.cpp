/**
 * @file
 * Source: Undo
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "globals.h"
#include "undo.h"

//max amount of memory (roughly) for buffer
QList <UndoEvent *> UndoEvents;
QList <UndoEvent *> RedoEvents;

int TotalUndoSize;

//constructor for data holding object - pass it type and it grabs data
UndoDataObject::UndoDataObject(int type, int curve_index = -1)
{
    Type = type;
    //todo - get my data from the object in question
    if (type == -3)
    {
        StoredData = Masks;
        undosize = Masks.size();
        TotalUndoSize += undosize;
    }
    if (type == -2)
    {
        StoredData = Locks;
        undosize = Locks.size();
        TotalUndoSize += undosize;
    }
    if (type == -1)
    {
        CurvePoints.X = Curves[curve_index]->SplinePoints[CurrentFile * zsparsity]->X;
        CurvePoints.Y = Curves[curve_index]->SplinePoints[CurrentFile * zsparsity]->Y;
        CurvePoints.Count = Curves[curve_index]->SplinePoints[CurrentFile * zsparsity]->Count;
        CurveNumber = curve_index;
        undosize = CurvePoints.Count * (sizeof(double)) * 2;
        TotalUndoSize += undosize;
    }
    if (type >= 0)
    {
        StoredImage = *GA[type];
        undosize = GA[type]->byteCount();
        TotalUndoSize += undosize;
    }
}

//constructor for event object - it finds all dirty arrays and stashes them OR stores a move event
//If it's not a move event (e.g. from timer) it's passed -1, -1
//If we need to stash all curves for all slices, passed -1, -2
UndoEvent::UndoEvent(int Dfrom, int Dto)
{
    //qDebug()<<"Creating Undo "<<Dfrom<<","<<Dto<<" pointer"<<this;
    bool OK = false;
    FileNumber = Dfrom;
    FileTo = Dto;
    DataObjects.clear();
    Type = "";
    if (MasksUndoDirty || LocksUndoDirty || CurvesUndoDirty) OK = true;
    else for (int i = 0; i < SegmentCount; i++) if (Segments[i]->UndoDirty) OK = true;

    //qDebug()<<"Making an undo"<<Dfrom<<","<<Dto<<" pointer is "<<this;
    //for now - stash all at every point
    if (OK) DataObjects.append(new UndoDataObject(-3));
    if (OK) DataObjects.append(new UndoDataObject(-2));
    if (OK)
    {
        if (FileTo == -2) //store all curves
        {
            int oldCurrentFile = CurrentFile;
            //qDebug()<<"FileCount is "<<FileCount;
            for (int f = 0; f < FileCount; f++)
            {
                CurrentFile = f; //bodge - constructor uses this
                for (int i = 0; i < CurveCount; i++)
                {
                    UndoDataObject *d = new  UndoDataObject(-1, i);
                    d->FileNumber = f;
                    //qDebug()<<"Storing curve, filenumber is "<<d->FileNumber;
                    DataObjects.append(d);
                }
            }
            CurrentFile = oldCurrentFile;
        }
        else
            for (int i = 0; i < CurveCount; i++)
            {
                UndoDataObject *d = new  UndoDataObject(-1, i);
                d->FileNumber = CurrentFile;
                DataObjects.append(d);
            }
    }
    for (int i = 0; i < SegmentCount; i++)
    {
        if (OK) DataObjects.append(new UndoDataObject(i));
        Segments[i]->UndoDirty = false;
    }

    MasksUndoDirty = false;
    LocksUndoDirty = false;
    CurvesUndoDirty = false;
}

UndoEvent::~UndoEvent()
{
    //modify the count
    //qDebug()<<"Deleting undo pointer is "<<this;
    TotalUndoSize -= 100;
    if (DataObjects.count())
    {
        foreach (UndoDataObject * o, DataObjects) TotalUndoSize -= o->undosize;

        qDeleteAll(DataObjects.begin(), DataObjects.end());
    }
}

//Copy undo information back whence it came - assumes currentfile is correct
void UndoEvent::Undo(MainWindowImpl *m)
{
    //qDebug()<<"Doing an undo pointer is "<<this<<" object count is "<<DataObjects.count()<<" type is "<<Type;
    TotalUndoSize += 100; // guess for overhead on one of these classes - those empty lists/images must take up something though!
    if (FileNumber != -1) //move event
    {
        HorribleBodgeFlagDontStoreUndo = true; //Global to flag move system not to store a new undo event. Shudder.
        (m->SliderPos)->setValue(FileNumber + 1); //will move image - WILL also create a new undo[0]
        //m->Moveimage(FileNumber+1); //do the code first - bit of a bodge
        HorribleBodgeFlagDontStoreUndo = false; //Turn the nasty thing off again
    }
    else
    {
        foreach (UndoDataObject *o, DataObjects)
        {
            //according to type restore - and mark as dirty for file io purposes
            if (o->Type == -3)
            {
                Masks = o->StoredData;
                MasksDirty = true;
                MasksUndoDirty = false;
            }
            if (o->Type == -2)
            {
                Locks = o->StoredData;
                LocksDirty = true;
                LocksUndoDirty = false;
            }
            if (o->Type == -1) //curves
            {
                //qDebug()<<"Undoing a curve, filenumber "<<o->FileNumber;
                if (o->CurveNumber >= 0 && o->CurveNumber < CurveCount)
                {
                    Curves[o->CurveNumber]->SplinePoints[o->FileNumber * zsparsity]->X = o->CurvePoints.X;
                    Curves[o->CurveNumber]->SplinePoints[o->FileNumber * zsparsity]->Y = o->CurvePoints.Y;
                    Curves[o->CurveNumber]->SplinePoints[o->FileNumber * zsparsity]->Count = o->CurvePoints.Count;
                    CurvesDirty = true;
                    CurvesUndoDirty = false;
                }
            }
            if (o->Type >= 0)
            {
                *GA[o->Type] = o->StoredImage;
                Segments[o->Type]->Dirty = true;
                Segments[o->Type]->UndoDirty = false;
            }
        }
    }
}

//Similar to undo - but moves work in opposite direction
void UndoEvent::Redo(MainWindowImpl *m)
{
    if (FileNumber != -1) //move event
    {
        HorribleBodgeFlagDontStoreUndo = true; //Global to flag move system not to store a new undo event. Shudder.
        (m->SliderPos)->setValue(FileTo + 1); //will move image
        //m->Moveimage(FileTo+1); //do the code first - bit of a bodge
        HorribleBodgeFlagDontStoreUndo = false; //Turn the nasty thing off again
    }
    else
    {
        foreach (UndoDataObject *o, DataObjects)
        {
            //according to type restore - and mark as dirty for file io purposes
            if (o->Type == -3)
            {
                Masks = o->StoredData;
                MasksDirty = true;
                MasksUndoDirty = false;
            }
            if (o->Type == -2)
            {
                Locks = o->StoredData;
                LocksDirty = true;
                MasksUndoDirty = false;
            }
            if (o->Type == -1)
            {
                if (o->CurveNumber >= 0 && o->CurveNumber < CurveCount)
                {
                    Curves[o->CurveNumber]->SplinePoints[o->FileNumber * zsparsity]->X = o->CurvePoints.X;
                    Curves[o->CurveNumber]->SplinePoints[o->FileNumber * zsparsity]->Y = o->CurvePoints.Y;
                    Curves[o->CurveNumber]->SplinePoints[o->FileNumber * zsparsity]->Count = o->CurvePoints.Count;
                    CurvesDirty = true;
                    CurvesUndoDirty = false;
                }
            }
            if (o->Type >= 0)
            {
                *GA[o->Type] = o->StoredImage;
                Segments[o->Type]->Dirty = true;
                Segments[o->Type]->UndoDirty = false;
            }
        }
    }
}

