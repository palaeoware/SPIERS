/**
 * @file
 * Header: Histogram
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

#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include <QGraphicsLineItem>
#include <QGraphicsView>

/**
 * @brief The histgv class
 */
class histgv : public QGraphicsView
{
public:
    histgv();
    void Refresh();

protected:
    void resizeEvent ( QResizeEvent *event );

private:
    QGraphicsScene *histscene;

    //the line items for the scene
    QGraphicsLineItem Hist_Grey[256];
    QGraphicsLineItem Hist_Red[256];
    QGraphicsLineItem Hist_Green[256];
    QGraphicsLineItem Hist_Blue[256];
    QGraphicsLineItem ThreshLine;
};

extern histgv *GVHist;

#endif // __HISTOGRAM_H__
