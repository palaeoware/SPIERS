/**
 * @file
 * Source: MyScene
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

#include "myscene.h"
#include "globals.h"
#include "brush.h"
#include "display.h"
#include "curves.h"
#include "copyingimpl.h"
#include "info.h"
#include <QPointF>
#include <QGraphicsView>

myscene *scene;
int Counter2;
int dx, dy;
QPointF mouseDownViewCenter;


myscene::myscene() : QGraphicsScene()
{
    button = 0;
    mousetimer.start();
    setBackgroundBrush(QColor(35, 35, 35));
    CurrentClosestNode = -1;
}

void myscene::DoMouse(int x, int y, int PressedButton)
{

    if (x != LastMouseX || y != LastMouseY) // an actual move
    {

        LastMouseX = x;
        LastMouseY = y;

        MoveFlag = true;
        if (button > 0)
        {
            Counter2++;    //else ChangeFlag=true;
            DoAction(LastMouseX, LastMouseY);
        }
    }
    else
    {
        //Not a move - maybe a click though?

        if (button == PressedButton) return; //nothing new

        if (PressedButton == 0 && button != 0)
        {
            MouseUp();    //no button
            return;
        }
        //catch new button presses
        if (PressedButton == 1 && button == 0)
        {
            Counter2 = 0;
            button = 1;
            DoAction(LastMouseX, LastMouseY);
            return;
        }

        if (PressedButton == 2 && button == 0)
        {
            Counter2 = 0;
            button = 2;
            DoAction(LastMouseX, LastMouseY);
            return;
        }

        if (PressedButton == 3 && button == 0)
        {
            Counter2 = 0;
            button = 3;
            dx = LastMouseX;
            dy = LastMouseY;

            //mouseDownViewCenter = mainwin->graphicsView->mapFromScene(mainwin->graphicsView->viewport()->rect().center());
            //qDebug() << mouseDownViewCenter << mainwin->graphicsView->viewport()->rect().center();

            DoAction(LastMouseX, LastMouseY);
            return;
        }

        //catch button swaps
        if (PressedButton == 1  && button == 2)
        {
            MouseUp();
            button = 1;
            DoAction(LastMouseX, LastMouseY);
            return;
        }
        if (PressedButton == 2  && button == 1)
        {
            MouseUp();
            button = 2;
            DoAction(LastMouseX, LastMouseY);
            return;
        }
    }
}

void myscene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//this gets string of calls if mouse button is held
{
    QPointF position = event->scenePos();
    int x, y;

    x = (int)position.x();
    y = (int)position.y();
    DoMouse(x, y, button); //pass existing button value

    //do info stuff
    if (MenuInfoChecked)
    {
        ShowInfo(x, y);
    }
}


void myscene::DoAction(int x, int y)
//Brushing action required - work it out, call appropriate brush
{
    if (ThreeDmode && CurrentMode != 2) //curve manipulation not applicable to 3D brush
    {
        CopyingImpl cop;
        cop.Apply3DBrush(button);
        ChangeFlag = true;
        MouseUp();
    }
    //RJG - If middle mouse drag and viewport has scroll bars - scroll view
    else if (button == 3)
    {

        int newX = mainwin->graphicsView->horizontalScrollBar()->value()-(x-dx);
        mainwin->graphicsView->horizontalScrollBar()->setValue(newX);

        int newY =  mainwin->graphicsView->verticalScrollBar()->value()-(y-dy);
        mainwin->graphicsView->verticalScrollBar()->setValue(newY);

        dx=x;
        dy=y;
    }
    else
    {
        //CurrentMode; //0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc
        switch (CurrentMode)
        {
        case 0: //brighten
            if (button == 1) Brush.brighten(LastMouseX, LastMouseY, CurrentSegment, BrightUp);
            else Brush.brighten(LastMouseX, LastMouseY, CurrentSegment, 0 - BrightDown);
            break;
        case 1: //masks
            if (button == 1) Brush.mask(LastMouseX, LastMouseY, SelectedMask);
            else Brush.mask(LastMouseX, LastMouseY, SelectedRMask);
            break;
        case 2:
            if (button == 1)
            {
                if (CurrentClosestNode == -1) CurrentClosestNode = FindClosestNode((double)x, (double)y);
                if (CurrentClosestNode >= 0) //selected curve must be 0 or more for this
                {
                    FilesDirty[CurrentFile] = true;
                    CurvesDirty = true;
                    CurvesUndoDirty = true;
                    if (CurveShapeLocked)
                    {
                        double offsetx = ((double)x / static_cast<double>(ColMonoScale)) - Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X[CurrentClosestNode];
                        double offsety = ((double)y / static_cast<double>(ColMonoScale)) - Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y[CurrentClosestNode];
                        for (int n = 0; n < Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Count; n++)
                        {
                            Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X[n] += offsetx;
                            Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y[n] += offsety;
                        }
                    }
                    else
                    {
                        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X[CurrentClosestNode] = ((double)x / static_cast<double>(ColMonoScale));
                        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y[CurrentClosestNode] = ((double)y / static_cast<double>(ColMonoScale));
                    }
                }
            }
            break;
        case 3: //lock
            if (button == 1) Brush.lock(LastMouseX, LastMouseY, 255);
            else Brush.lock(LastMouseX, LastMouseY,  0);
            break;
        case 4:
            if (button == 1) Brush.segment(LastMouseX, LastMouseY, 255);
            else Brush.segment(LastMouseX, LastMouseY,  0);
            break;
        case 5:
            if (tabwidget->currentIndex() < 2 || RangeSelectedOnly)
                Brush.recalc(LastMouseX, LastMouseY, CurrentSegment);
            else
                //all segs
                for (int i = 0; i < SegmentCount; i++) if (Segments[i]->Activated) Brush.recalc(LastMouseX, LastMouseY, i);
            break;
        }

        ChangeFlag = true;
    }
}


void myscene::mousePressEvent(QGraphicsSceneMouseEvent *event )
{
    QPointF position = event->scenePos();
    int x, y;

    x = (int)position.x();
    y = (int)position.y();

    int but = 0;
    if (event->button() == Qt::LeftButton) but = 1;
    if (event->button() == Qt::RightButton) but = 2;
    if (event->button() == Qt::MiddleButton) but = 3;

    DoMouse(x, y, but);
    return;
}

void myscene::MouseUp()
//Handle mouse release - mainly resetting of dirty array
{
    int n;
    CurrentClosestNode = -1;
    button = 0;
    for (n = 0; n < fwidth * fheight; n++) dirty[n] = 0;
    return;
}

void myscene::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
    QPointF position = event->scenePos();
    int x, y;
    Q_UNUSED(x);
    Q_UNUSED(y);

    x = (int)position.x();
    y = (int)position.y();
    MouseUp();

}
