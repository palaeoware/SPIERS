/**
 * @file
 * Header: My Scene
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

#ifndef __MYSCENE_H__
#define __MYSCENE_H__

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QTime>

/**
 * @brief The myscene class
 */
class myscene : public QGraphicsScene
{
public:
    myscene();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    int button; //current button state - 0 = no, 1 = left down, 2 = right down
    void DoAction(int x, int y);
    void MouseUp();
    void DoMouse(int x, int y, int PressedButton);
    QTime mousetimer;
    int CurrentClosestNode;
    bool mouse_down;
    bool got_LCE_sample;
    QVector<uchar> LCE_sample;
    QByteArray gen_locks;
private slots:
    void ScreenUpdate();
};

extern myscene *scene;

#endif // __MYSCENE_H__
