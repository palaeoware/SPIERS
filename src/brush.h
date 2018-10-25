/**
 * @file
 * Header: Brush Class
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

#ifndef __BRUSH_H__
#define __BRUSH_H__

#include <QList>
#include <QGraphicsLineItem>

/**
 * @brief The Brush_class class
 */
class Brush_class
{
public:
    Brush_class();
    ~Brush_class();
    void resize(int size, int shape, double o);
    bool draw(int x, int y);
    void brighten(int x, int y, int segment, int effect);
    void lock(int x, int y, int effect); //effect is true or false in this case
    void mask(int x, int y, int mask);
    void Brush_Flag_Restart();
    void segment(int x, int y, int effect);
    void recalc(int x, int y, int segment);
    //more to follow!
    int PixelCount; //applies to all three lists above
private:
    QList <int> Xpos;
    QList <int> Ypos;
    QList <double> Dist;
    QList <int> Soft;
    int Radius;
    QList <int> LeftLinesY;
    QList <int> RightLinesY;
    QList <int> TopLinesY;
    QList <int> BottomLinesY;
    QList <int> LeftLinesX;
    QList <int> RightLinesX;
    QList <int> TopLinesX;
    QList <int> BottomLinesX;
    int LeftLinesCount, RightLinesCount, TopLinesCount, BottomLinesCount;
    bool RestartFlag;
    QList <QGraphicsLineItem *> LineList;
    void ThreeDBrushPixels(QList <int> *X, QList <int> *Y, int *PointCount, double offset, int mode);
};

extern Brush_class Brush;

#endif // __BRUSH_H__
