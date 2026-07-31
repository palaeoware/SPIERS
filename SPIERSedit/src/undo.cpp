/**
 * @file
 * Source: Undo
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
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
UndoDataObject::UndoDataObject(
    int type,
    int curve_index,
    int storedCurveSlice)
{
    Type = type;
    FileNumber = CurrentFile;
    CurveFileNumberIsStoredIndex = false;
    CurveAutomaticallyInterpolated = false;
    CurveAutomaticStartSlice = -1;
    CurveAutomaticEndSlice = -1;
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
        const int sourceSlice =
            storedCurveSlice >= 0
                ? storedCurveSlice
                : CurrentFile * zsparsity;
        CurvePoints.X =
            Curves[curve_index]->SplinePoints[sourceSlice]->X;
        CurvePoints.Y =
            Curves[curve_index]->SplinePoints[sourceSlice]->Y;
        CurvePoints.Fixed =
            Curves[curve_index]->SplinePoints[sourceSlice]
                ->Fixed;
        CurvePoints.Count =
            Curves[curve_index]->SplinePoints[sourceSlice]
                ->Count;
        CurveNumber = curve_index;
        FileNumber =
            storedCurveSlice >= 0
                ? storedCurveSlice
                : CurrentFile;
        CurveFileNumberIsStoredIndex =
            storedCurveSlice >= 0;
        CurveAutomaticallyInterpolated =
            Curves[curve_index]->AutomaticallyInterpolated;
        CurveAutomaticStartSlice =
            Curves[curve_index]->AutomaticStartSlice;
        CurveAutomaticEndSlice =
            Curves[curve_index]->AutomaticEndSlice;
        undosize = static_cast<int>(
            CurvePoints.Count * sizeof(double) * 2
            + CurvePoints.Fixed.size());
        TotalUndoSize += undosize;
    }
    if (type >= 0)
    {
        StoredImage = *GA[type];
        undosize = (int) (GA[type]->sizeInBytes());
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
            for (int i = 0; i < CurveCount; i++)
            {
                for (int storedSlice = 0;
                     storedSlice
                     < Curves[i]->SplinePoints.size();
                     storedSlice++)
                {
                    DataObjects.append(
                        new UndoDataObject(
                            -1,
                            i,
                            storedSlice));
                }
            }
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
void UndoEvent::Undo(MainWindow *m)
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
                    const int storedSlice =
                        o->CurveFileNumberIsStoredIndex
                            ? o->FileNumber
                            : o->FileNumber * zsparsity;
                    if (storedSlice < 0
                        || storedSlice
                               >= Curves[o->CurveNumber]
                                      ->SplinePoints.size())
                    {
                        continue;
                    }
                    PointList *points =
                        Curves[o->CurveNumber]
                            ->SplinePoints[storedSlice];
                    points->X = o->CurvePoints.X;
                    points->Y = o->CurvePoints.Y;
                    points->Fixed = o->CurvePoints.Fixed;
                    points->Count = o->CurvePoints.Count;
                    Curves[o->CurveNumber]
                        ->AutomaticallyInterpolated =
                        o->CurveAutomaticallyInterpolated;
                    Curves[o->CurveNumber]
                        ->AutomaticStartSlice =
                        o->CurveAutomaticStartSlice;
                    Curves[o->CurveNumber]
                        ->AutomaticEndSlice =
                        o->CurveAutomaticEndSlice;
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
void UndoEvent::Redo(MainWindow *m)
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
                    const int storedSlice =
                        o->CurveFileNumberIsStoredIndex
                            ? o->FileNumber
                            : o->FileNumber * zsparsity;
                    if (storedSlice < 0
                        || storedSlice
                               >= Curves[o->CurveNumber]
                                      ->SplinePoints.size())
                    {
                        continue;
                    }
                    PointList *points =
                        Curves[o->CurveNumber]
                            ->SplinePoints[storedSlice];
                    points->X = o->CurvePoints.X;
                    points->Y = o->CurvePoints.Y;
                    points->Fixed = o->CurvePoints.Fixed;
                    points->Count = o->CurvePoints.Count;
                    Curves[o->CurveNumber]
                        ->AutomaticallyInterpolated =
                        o->CurveAutomaticallyInterpolated;
                    Curves[o->CurveNumber]
                        ->AutomaticStartSlice =
                        o->CurveAutomaticStartSlice;
                    Curves[o->CurveNumber]
                        ->AutomaticEndSlice =
                        o->CurveAutomaticEndSlice;
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

