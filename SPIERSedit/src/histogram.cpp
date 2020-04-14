/**
 * @file
 * Source: Histogram
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

#include "histogram.h"
#include "globals.h"

#include <QDebug>
histgv *GVHist;
#include <QResizeEvent>

/**
 * @brief histgv::histgv
 */
histgv::histgv()
{
    histscene = new QGraphicsScene();

    for (int i = 0; i < 256; i++)
    {
        histscene->addItem(&(Hist_Grey[i]));
    }
    ThreshLine.setLine(128, 0, 128, 255);
    ThreshLine.setPen(QPen(Qt::magenta));
    histscene->addItem(&ThreshLine);
    setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff);
    setScene(histscene);
    Refresh();
}

/**
 * @brief histgv::resizeEvent
 * @param event
 */
void histgv::resizeEvent ( QResizeEvent *event )
{
    Q_UNUSED(event);

    fitInView(QRectF(0, 0, 256, 256), Qt::IgnoreAspectRatio);
}

/**
 * @brief histgv::Refresh
 */
void histgv::Refresh()
{

    if (!Active) return;
    int bins[256];

    for (int i = 0; i < 256; i++)
        bins[i] = 0;

    uchar *data;
    data = GA[CurrentSegment]->bits();
    int max = fwidth * fheight;

    if (MenuHistSelectedOnly)
        for (int i = 0; i < max; i++)
        {
            if (Locks[i * 2]) bins[data[i]]++;
        }
    else
        for (int ix = 0; ix < fwidth; ix++)
            for (int iy = 0; iy < fheight; iy++)
            {
                bins[data[fwidth4 * iy + ix]]++;
            }


    int bmax = 0;
    for (int i = 0; i < 256; i++)
        if (bins[i] > bmax)
            bmax = bins[i];

    bmax /= 240;
    if (bmax == 0) bmax = 1;
    for (int i = 0; i < 256; i++)
    {
        Hist_Grey[i].setLine(i, 256, i, 256 - (bins[i] / bmax));
    }
}
