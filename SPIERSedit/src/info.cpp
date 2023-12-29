/**
 * @file
 * Source: Info
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "info.h"
#include "globals.h"

extern void ShowInfo(int x, int y)
{

    //x and y are in HiRes
    int lx, ly;
    lx = x / ColMonoScale;
    ly = y / ColMonoScale;
    //first find pointer to the text label!
    QLabel *label = mainwin->InfoLabel;
    QString out;
    QTextStream o(&out);

    if (lx < 0 || lx >= fwidth || ly < 0 || ly >= fheight)
    {
        o << "(--,--) [--,--]\n------ (------)\n";

    }
    else
    {
        o << "(" << x << "," << y << ") ";
        o << "[" << lx << "," << ly << "]\n";
        QString mn = MasksSettings[static_cast<quint8>(Masks[(fheight - ly - 1) * fwidth + lx])]->Name;
        if (mn.length() >= 15) mn = mn.left(12) + "...";
        o << mn << " (";

        int seg = SegmentMap[ly * fwidth + lx];


        if (seg == -1) o << "None";
        else
        {
            QString nm = Segments[seg]->Name;
            if (nm.length() >= 15) nm = nm.left(12) + "...";
            o << nm;
        }
        o << ")";
        if (Locks[((fheight - ly - 1)*fwidth + lx) * 2]) o << " [Yes]\n";
        else o << "\n";
    }
    //now do general image details
    //Source - size, format
    if (GreyImage) o << "Greyscale, ";
    else o << "RGB Colour, ";
    o << cwidth << "x" << cheight << "\n";
    o << "1:" << ColMonoScale << ", " << fwidth << "x" << fheight;
    label->setText(out);

}
// place your code here
