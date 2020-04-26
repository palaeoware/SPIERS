/**
 * @file
 * Source: MyRangeScene
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

#include "myrangescene.h"
#include "globals.h"
#include "mainwindowimpl.h"
#include "display.h"

#include <QList>
#include <QPen>
#include <QBrush>


myrangescene *rangescene = nullptr;

myrangescene::myrangescene() : QGraphicsScene()
{
    button = 0;
    setBackgroundBrush(Qt::gray);
    selectedtop = -1;
    selectedbottom = -1;
}

int myrangescene::closesttop(int x, int *dist)
{
    int d = 500;
    int c = -1;
    for (int i = 0; i < SegmentCount; i++)
        if (Segments[i]->Activated)
        {
            if (qAbs(Segments[i]->RangeTop - x) < d)
            {
                c = i;
                d =    qAbs(Segments[i]->RangeTop - x);
            }
        }
    *dist = d;
    return c;
}

int myrangescene::closestbase(int x, int *dist)
{
    int d = 500;
    int c = -1;
    for (int i = 0; i < SegmentCount; i++)
        if (Segments[i]->Activated)
        {
            if (qAbs(Segments[i]->RangeBase - x) < d)
            {
                c = i;
                d =   qAbs(Segments[i]->RangeBase - x);
            }
        }
    *dist = d;
    return c;
}


int myrangescene::LimitTop(int x)
{
    //returns x if in limits - is a top
    if (x > 255) x = 255;
    if (x < 0) x = 0;

    //stop it going below it's base
    if (x <= Segments[selectedtop]->RangeBase) x = Segments[selectedtop]->RangeBase + 1;

    //all else is allowed
    return x;
}

int myrangescene::LimitBase(int x)
{
    //returns x if in limits - is a top
    if (x > 255) x = 255;
    if (x < 0) x = 0;

    //stop it going below it's base
    if (x >= Segments[selectedbottom]->RangeTop) x = Segments[selectedbottom]->RangeTop - 1;

    //all else is allowed
    return x;
}

void myrangescene::DoMouse(int x, int y, int PressedButton)
{
    int dist, dist2;
    Q_UNUSED(y);

    if (PressedButton >= 1)
    {
        //do selection
        int o = CurrentSegment;
        //can  choose a segment by clicking in it   - may get overridden later
        if (selectedbottom == -1 && selectedtop == -1 && button == 0)
            for (int i = 0; i < SegmentCount; i++)
                if (Segments[i]->Activated)
                {
                    if (x >= Segments[i]->RangeBase && x <= Segments[i]->RangeTop)
                        CurrentSegment = i;
                }

        button = PressedButton;
        if (selectedtop == -1 && selectedbottom == -1) //nothing selected - see if we can pick up
        {
            int i = closesttop(x, &dist);  //find closest top line, and distance to
            if (i != -1 && dist < 10) //pick up top line
            {
                selectedtop = i;
                int j = closestbase(Segments[i]->RangeTop, &dist2);
                if (dist2 == 0 && button == 1) //found a base at same position
                    selectedbottom = j; //only do this for left mouse button
                else  //is base closer?, but not on same - drop top
                {
                    int j = closestbase(x, &dist2);
                    if (dist2 < dist)
                    {
                        selectedbottom = j;
                        selectedtop = -1;
                    }
                }
            }
            else //maybe pick up bottom only
            {
                int j = closestbase(x, &dist);
                if (j != -1 && dist < 10)
                    selectedbottom = j;
            }
        }

        bool flag = false;
        Q_UNUSED(flag);

        //do snapping
        if (button == 1) //do snapping
        {
            if (selectedtop >= 0 && selectedbottom == -1) //can snap to a baseline
            {
                //find closest base that's not of this segment and is higher
                int min = 1000;
                int t = -1;
                for (int i = 0; i < SegmentCount; i++)
                    if (Segments[i]->Activated)
                    {
                        if ((Segments[i]->RangeBase - x) > 0 && ((Segments[i]->RangeBase - x) < min))
                        {
                            t = i;
                            min = Segments[i]->RangeBase - x;
                        }
                    }
                //min is now distance up to closest
                if (min < 5 && min > 0 && t != -1) x = Segments[t]->RangeBase;
            }

            if (selectedbottom >= 0 && selectedtop == -1) //can snap to a topline
            {
                //find closest base that's not of this segment and is higher
                int min = 1000;
                int t = -1;
                for (int i = 0; i < SegmentCount; i++)
                    if (Segments[i]->Activated)
                    {
                        if ((x - Segments[i]->RangeTop) > 0 && ((x - Segments[i]->RangeTop) < min))
                        {
                            min = x - Segments[i]->RangeTop;
                            t = i;
                        }
                    }
                //min is now distance up to closest
                if (min < 5 && min > 0 && t != -1) x = Segments[t]->RangeTop;
            }
        }

        if (selectedtop >= 0)
        {
            x = LimitTop(x);
            if (selectedbottom >= 0) x = LimitBase(x);
            Segments[selectedtop]->RangeTop = x;
            flag = true;
        }
        if (selectedbottom >= 0)
        {
            x = LimitBase(x);
            Segments[selectedbottom]->RangeBase = x;
            flag = true;
        }

        //do selection
        if (selectedtop >= 0 && selectedbottom == -1) CurrentSegment = selectedtop;
        if (selectedbottom >= 0 && selectedtop == -1) CurrentSegment = selectedbottom;


        if (o != CurrentSegment) //changed segment
        {
            mainwin->RefreshOneSegmentItem(Segments[CurrentSegment]->widgetitem, CurrentSegment);
            mainwin->RefreshOneSegmentItem(Segments[o]->widgetitem, o);
            mainwin->RefreshSegmentsBoxes();
            mainwin->SetUpGenerationToolbox(CurrentSegment);
            ShowImage(mainwin->graphicsView);
        }


        Refresh();
        if (mainwin->GenerateAuto->checkState()) mainwin->GenButton();
    }
}

void myrangescene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//this gets string of calls if mouse button is held
{
    QPointF position = event->scenePos();
    int x, y;

    x = static_cast<int>(position.x());
    y = static_cast<int>(position.y());
    DoMouse(x, y, button); //pass existing button value
}


void myrangescene::mousePressEvent(QGraphicsSceneMouseEvent *event )
{
    QPointF position = event->scenePos();
    int x, y;

    x = static_cast<int>(position.x());
    y = static_cast<int>(position.y());

    int but = 0;
    if (event->button() == Qt::LeftButton) but = 1;
    if (event->button() == Qt::RightButton) but = 2;

    DoMouse(x, y, but);
    return;
}

void myrangescene::MouseUp()
//Handle mouse release - release any grabs
{
    selectedtop = -1;
    selectedbottom = -1;
    button = 0;
}

void myrangescene::mouseReleaseEvent ( QGraphicsSceneMouseEvent *event )
{
    QPointF position = event->scenePos();
    int x, y;
    Q_UNUSED(x);
    Q_UNUSED(y);

    x = static_cast<int>(position.x());
    y = static_cast<int>(position.y());
    MouseUp();

}

void myrangescene::Refresh()
{
    //does refresh of the scene
    if (!Active) return;
    QPen mypen(Qt::SolidLine);
    mypen.setCosmetic(true);

    for (int i = 0; i < SegmentCount; i++)
        if (Segments[i]->Activated)
        {
            //draw it
            Segments[i]->rectitem->setRect(Segments[i]->RangeBase, 32, Segments[i]->RangeTop - Segments[i]->RangeBase, 192);
            mypen.setColor(QColor(255 - Segments[i]->Colour[0], 255, 0));
            Segments[i]->rectitem->setPen(QPen(Qt::NoPen));
            if (i == CurrentSegment)
            {
                Segments[i]->rectitem->setBrush(QBrush(QColor(Segments[i]->Colour[0], Segments[i]->Colour[1], Segments[i]->Colour[2], 170)));
                Segments[i]->rectitem->setZValue(2);
                Segments[i]->rectitem->setPen(mypen);
            }
            else
                Segments[i]->rectitem->setBrush(QBrush(QColor(Segments[i]->Colour[0], Segments[i]->Colour[1], Segments[i]->Colour[2], 70)));
            Segments[i]->rectitem->setZValue(1);
            Segments[i]->rectitem->setVisible(true);
            /*
                    Segments[i]->textitem->setPlainText(Segments[i]->Name.left(15));
                    Segments[i]->textitem->setFont(QFont("Arial",12));
                    Segments[i]->textitem->setDefaultTextColor(QColor(Segments[i]->Colour[0], Segments[i]->Colour[1], Segments[i]->Colour[2]));
                    Segments[i]->textitem->setPos(Segments[i]->RangeBase,20);
                    Segments[i]->textitem->setTextWidth(Segments[i]->RangeTop-Segments[i]->RangeBase);
                    Segments[i]->textitem->setZValue(1);
                    Segments[i]->textitem->setVisible(true);
            */
        }
        else Segments[i]->rectitem->setVisible(false);

    if (CurrentSegment >= 0)
    {
        mainwin->SpinBoxRangeBase->setValue(Segments[CurrentSegment]->RangeBase);
        mainwin->SpinBoxRangeTop->setValue(Segments[CurrentSegment]->RangeTop);
        mainwin->SpinBoxRangeTop->setVisible(true);
        mainwin->SpinBoxRangeBase->setVisible(true);
    }
    else
    {
        //SpinBoxRangeBase=Segments[CurrentSegment]->RangeBase;
        //SpinBoxRangeTop=Segments[CurrentSegment]->RangeTop;
        mainwin->SpinBoxRangeTop->setVisible(false);
        mainwin->SpinBoxRangeBase->setVisible(false);
    }
}
