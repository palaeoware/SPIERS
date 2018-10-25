/**
 * @file
 * Header: My Range Scene
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

#ifndef __MYRANGESCENE_H__
#define __MYRANGESCENE_H__

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QTime>

/**
 * @brief The myrangescene class
 */
class myrangescene : public QGraphicsScene
{
public:
    myrangescene();
    void Refresh();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    int button; //current button state - 0 = no, 1 = left down, 2 = right down
    int selectedtop;
    int selectedbottom;
    void MouseUp();
    void DoMouse(int x, int y, int PressedButton);
    int closestbase(int x, int *dist);
    int closesttop(int x, int *dist);
    int LimitTop(int x);
    int LimitBase(int x);
};

extern myrangescene *rangescene;

#endif // __MYRANGESCENE_H__
