/**
 * @file
 * Source: Curves
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

#include "curves.h"
#include "globals.h"
#include <math.h>

#include <QGraphicsEllipseItem>
#include <QImage>
#include <QMessageBox>


#define M11 0
#define M12 1
#define M13 0
#define M14 0
#define M21 -0.5
#define M22 0
#define M23 0.5
#define M24 0
#define M31 1
#define M32 -2.5
#define M33 2
#define M34 -0.5
#define M41 -0.5
#define M42 1.5
#define M43 -1.5
#define M44 0.5

int WrapSpline(int a, int c) //vb version passed curve number as c, this wants the number of spline points
{
    if (a < 0) return a + c;
    if (a < c) return a;
    return a - c;
}


double DCatmullRomSpline(double X, double v1, double v2, double v3, double v4)
{
    double c1, c2, c3, c4;

    c1 = M12 * v2;
    c2 = M21 * v1 + M23 * v3;
    c3 = M31 * v1 + M32 * v2 + M33 * v3 + M34 * v4;
    c4 = M41 * v1 + M42 * v2 + M43 * v3 + M44 * v4;

    return (((c4 * X + c3) * X + c2) * X + c1);
}


int CatmullRomSpline(double X, double v1, double v2, double v3, double v4)
{
    double c1, c2, c3, c4;

    c1 = M12 * v2;
    c2 = M21 * v1 + M23 * v3;
    c3 = M31 * v1 + M32 * v2 + M33 * v3 + M34 * v4;
    c4 = M41 * v1 + M42 * v2 + M43 * v3 + M44 * v4;

    return static_cast<int>(qRound(((c4 * X + c3) * X + c2) * X + c1));
}

int limitx(int x)
{
    if (x >= fwidth) return fwidth;
    if (x < 0) return 0;
    return x;
}

double limitx(double x)
{
    if (x >= static_cast<double>(fwidth)) return static_cast<double>(fwidth);
    if (x < 0) return 0;
    return x;
}

int limity(int y)
{
    if (y >= fwidth) return fwidth;
    if (y < 0) return 0;
    return y;
}

double limity(double y)
{
    if (y >= static_cast<double>(fwidth)) return static_cast<double>(fwidth);
    if (y < 0) return 0;
    return y;
}

void AddNode()
{

    int sn, sm, sx, sy, d, mind, mindSegment = 0;
    double sm2, mx, my;

    if (SelectedCurve == -1 || CurrentMode != 2) return; //Not appropriate

    FilesDirty[CurrentFile] = true;
    mx = static_cast<double>(LastMouseX) / static_cast<double>(ColMonoScale);
    my = static_cast<double>(LastMouseY) / static_cast<double>(ColMonoScale);

    if (Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Count == 0)
    {
        //initial create - have to have 4 nodes - create then get out
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Count = 4;

        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X.append(mx);
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y.append(limity(my + 5));

        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X.append(mx);
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y.append(my);

        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X.append(limitx(mx + 5));
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y.append(my);

        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X.append(limitx(mx + 5));
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y.append(limity(my + 5));

        CurvesDirty = true;
        CurvesUndoDirty = true;

        return;
    }

    //first trick - find the line point which we are closest too...
    mind = 10000000;

    PointList *p = Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity];
    if (Curves[SelectedCurve]->Closed)   //closed loop
    {
        for (sn = 0; sn < p->Count; sn++)   //all the line segments
        {
            for (sm = 1; sm < 10; sm++)
            {
                sm2 = static_cast<double>(sm) / 10;

                sx = CatmullRomSpline(sm2, p->X[WrapSpline(sn - 1, p->Count)], p->X[WrapSpline(sn, p->Count)], p->X[WrapSpline(sn + 1, p->Count)], p->X[WrapSpline(sn + 2, p->Count)]);
                sy = CatmullRomSpline(sm2, p->Y[WrapSpline(sn - 1, p->Count)], p->Y[WrapSpline(sn, p->Count)], p->Y[WrapSpline(sn + 1, p->Count)], p->Y[WrapSpline(sn + 2, p->Count)]);

                d = static_cast<int>((sx - mx) * (sx - mx) + (sy - my) * (sy - my));
                if (d < mind)
                {
                    mind = d;
                    mindSegment = sn;
                }
            }
        }
    }
    else     //open curve
    {
        for (sn = 1; sn < (p->Count - 2); sn++)   //all the segments
        {
            for (sm = 1; sm < 10; sm++)
            {
                sm2 = double(sm) / 10;

                sx = CatmullRomSpline(sm2, p->X[WrapSpline(sn - 1, p->Count)], p->X[WrapSpline(sn, p->Count)], p->X[WrapSpline(sn + 1, p->Count)], p->X[WrapSpline(sn + 2, p->Count)]);
                sy = CatmullRomSpline(sm2, p->Y[WrapSpline(sn - 1, p->Count)], p->Y[WrapSpline(sn, p->Count)], p->Y[WrapSpline(sn + 1, p->Count)], p->Y[WrapSpline(sn + 2, p->Count)]);

                d = static_cast<int>((sx - mx) * (sx - mx) + (sy - my) * (sy - my));
                if (d < mind)
                {
                    mind = d;
                    mindSegment = sn;
                }
            }
        }
    }
    p->X.insert(mindSegment + 1, mx);
    p->Y.insert(mindSegment + 1, my);
    p->Count++;
    CurvesDirty = true;
    CurvesUndoDirty = true;
}

void KillNode(MainWindowImpl *th)
{
    //find node
    int node = FindClosestNode(static_cast<double>(LastMouseX), static_cast<double>(LastMouseY));
    FilesDirty[CurrentFile] = true;

    if (node >= 0)   //selected curve must be 0 or more for this to be the case
    {
        if (Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Count == 4)
        {
            if (QMessageBox::question(th, "Remove Nodes", "Minimum of four points required - do you want to remove all curve points from this image?", QMessageBox::Ok | QMessageBox::Cancel)
                    == QMessageBox::Ok)
            {
                Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Count = 0;
                Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X.clear();
                Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y.clear();
            }
            return;
        }

        //remove the specified node then redraw
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Count--;
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->X.removeAt(node);
        Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity]->Y.removeAt(node);
        CurvesDirty = true;
        CurvesUndoDirty = true;
        return;
    }
}


int FindClosestNode(double X, double Y)
{
    //Which of the spline points is closest?
    double d, d2, min;
    int n, which;
    if (SelectedCurve == -1) return -1;
    PointList *p = Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity];

    //Y=(double)cheight-Y;
    which = -1;
    min = 350;

    for (n = 0; n < p->Count; n++)
    {
        //get distance
        d = p->X[n] * ColMonoScale - X;
        d *= d;
        d2 = p->Y[n] * ColMonoScale - Y;
        d2 *= d2;
        d += d2;
        if (d < min)
        {
            which = n;
            min = d;
        }
    }
    return which;
}


void ScanlineFill(int c, int mycol, QImage *Thresh, class PointList *LPointList)
{
    //Direct conversion from VB

    Curve *cv = Curves[c];
    int n, m, apos;
    QList <double> intersections;
    int intercount;
    double acth, r;
    bool fill;
    int nextiter;
    uchar *data = Thresh->bits();

    uchar red = 0, g = 0, b = 0;
    if (mycol == 1 || cv->Segment == 0)
    {
        //use spline colours
        red = static_cast<uchar> (cv->Colour[0]);
        g = static_cast<uchar> (cv->Colour[1]);
        b = static_cast<uchar> (cv->Colour[2]);
    }
    else if (mycol == 0)     //seg colours
    {
        red = static_cast<uchar>(Segments[(cv->Segment) - 1]->Colour[0]);
        g = static_cast<uchar>(Segments[(cv->Segment) - 1]->Colour[1]);
        b = static_cast<uchar>(Segments[(cv->Segment) - 1]->Colour[2]);
    }

    if (LPointList->Count == 0) return;
    //This needs an explanation!
    //LpointList are the points plotted in the normal line mode
    // - forming a fairly closely packed set of straight lines defining the shape
    //For each horizontal scanline, we work out the points of intersection with each of these lines
    //and stick them in a list
    //The list is then sorted, giving a set of on-off points for each scanline
    //all pixels in the line are then read through (starting at first on-point) and points plotted
    //turning on/off plotting as we go

    //put wrap around point in
    LPointList->X.append(LPointList->X[0]);
    LPointList->Y.append(LPointList->Y[0]);
    LPointList->Count++;

    for (n = 0; n < fheight; n++)
    {
        //new scanline
        intersections.clear();
        acth = n + 0.499;  //this is line y: avoid hitting points exactly
        intercount = 0;
        for (m = 0; m < ((LPointList->Count) - 1); m++)
        {
            //for each point
            if (LPointList->Y[m] < acth)
            {
                if (LPointList->Y[m + 1] > acth)
                {
                    //yep
                    r = (acth - LPointList->Y[m]) / (LPointList->Y[m + 1] - LPointList->Y[m]);
                    intersections.append(r * (LPointList->X[m + 1] - LPointList->X[m]) + LPointList->X[m]);
                    intercount++;
                }
            }
            else if (LPointList->Y[m] > acth)
            {
                if (LPointList->Y[m + 1] < acth)
                {
                    //yep again, same code
                    r = (acth - LPointList->Y[m]) / (LPointList->Y[m + 1] - LPointList->Y[m]);
                    intersections.append(r * (LPointList->X[m + 1] - LPointList->X[m]) + LPointList->X[m]);
                    intercount++;
                }
            }
        }
        //sanity check
        if (intercount % 2 != 0) Error ("Error in draw scanline - hit odd number of edges (should be impossible)");

        if (intercount > 0)
        {
            //there are intersections!
            //sort them into order - this is insertion sort algorithm

            qSort(intersections.begin(), intersections.end());

            //and make sure they can't go off the left!
            //for (int i=0; i<intersections.count(); i++) if (intersections[i]<0) intersections[i]=0;

            //now we can fill these parts
            fill = true;
            nextiter = 1;
            for (m = qRound(intersections[0]); m < fwidth; m++)
            {
                //flick the fill flag if necessary
                if (m > qRound(intersections[nextiter]))
                {
                    fill = !fill;
                    nextiter++;
                    if (nextiter == intercount) goto nextrow;
                }
                //and draw if necessary
                if (fill)
                {
                    if (m >= 0)   //watch for 'off left' errors
                    {
                        apos = (n * fwidth + m) * 4; //offset for array
                        SegmentMap[n * fwidth + m] = cv->Segment - 1;
                        if (mycol < 2  || cv->Segment == 0)
                        {
                            //use spline colours
                            RED(data, apos) = red;
                            GREEN(data, apos) = g;
                            BLUE(data, apos) = b;
                        }
                        else     //mask colours
                        {
                            Mask *M = MasksSettings[static_cast<quint8>(Masks[(fheight - n - 1) * fwidth + m])];
                            RED(data, apos) = static_cast<uchar>(M->ForeColour[0]);
                            GREEN(data, apos) = static_cast<uchar>(M->ForeColour[1]);
                            BLUE(data, apos) = static_cast<uchar>(M->ForeColour[2]);
                        }
                    }
                }
            }
        }
nextrow:
        int tempM = m;
        m = tempM; //just a dummy so goto works
    }
}

void ScanlineFillOutput(int c, uchar *data, class PointList *LPointList, QList <bool> *UseMasks, uchar setval)
{
    Q_UNUSED(c);
    //qDebug()<<"In scanline - uchar is "<<setval;
    //Curve *cv = Curves[c];
    int n, m;
    QList <double> intersections;
    int intercount;
    double acth, r;
    bool fill;
    int nextiter;

    if (LPointList->Count == 0) return;

    //put wrap around point in
    LPointList->X.append(LPointList->X[0]);
    LPointList->Y.append(LPointList->Y[0]);
    LPointList->Count++;


    for (n = 0; n < fheight; n++)
    {
        //new scanline
        intersections.clear();
        acth = n + 0.499;  //this is line y: avoid hitting points exactly
        intercount = 0;
        for (m = 0; m < ((LPointList->Count) - 1); m++)
        {
            //for each point
            if (LPointList->Y[m] < acth)
            {
                if (LPointList->Y[m + 1] > acth)
                {
                    //yep
                    r = (acth - LPointList->Y[m]) / (LPointList->Y[m + 1] - LPointList->Y[m]);
                    intersections.append(r * (LPointList->X[m + 1] - LPointList->X[m]) + LPointList->X[m]);
                    intercount++;
                }
            }
            else if (LPointList->Y[m] > acth)
            {
                if (LPointList->Y[m + 1] < acth)
                {
                    //yep again, same code
                    r = (acth - LPointList->Y[m]) / (LPointList->Y[m + 1] - LPointList->Y[m]);
                    intersections.append(r * (LPointList->X[m + 1] - LPointList->X[m]) + LPointList->X[m]);
                    intercount++;
                }
            }
        }
        //sanity check
        if (intercount % 2 != 0) Error ("Error in draw scanline - hit odd number of edges (should be impossible)");

        if (intercount > 0)
        {
            //there are intersections!
            //sort them into order - this is insertion sort algorithm

            qSort(intersections.begin(), intersections.end());

            //and make sure they can't go off the left!
            //for (int i=0; i<intersections.count(); i++) if (intersections[i]<0) intersections[i]=0;

            //now we can fill these parts
            fill = true;
            nextiter = 1;
            for (m = qRound(intersections[0]); m < fwidth; m++)
            {
                //flick the fill flag if necessary
                if (m > qRound(intersections[nextiter]))
                {
                    fill = !fill;
                    nextiter++;
                    if (nextiter == intercount) goto nextrow;
                }
                //and draw if necessary
                if (fill)
                {
                    if (m >= 0) //watch for 'off left' errors
                        if ((*UseMasks)[static_cast<quint8>(Masks[(fheight - 1 - n)*fwidth + m])]) data[n * fwidth + m] = setval;
                }
            }
        }
nextrow:
        int tempM = m;
        m = tempM; //just a dummy so goto works
    }
}

void setcval(uchar *data, int offset, uchar setval)
{
    //if (offset>=fwidth*fheight) qDebug()<<"OOps - writing offset"<<offset;
    //if (offset<0) qDebug()<<"OOps - writing offset"<<offset;
    data[offset] = setval;
}

void DrawCurveOutput(int c, int file, uchar *data, QList <bool> *UseMasks, bool remove)
{
    //This modified from C++ DrawCurve rather than VB DrawCurveOutput

    double d, d2, dist, sm2;
    int sn, sm, sx, sy;
    class PointList LPointList; //the list for filling
    file *= zsparsity;

    uchar setval;
    if (remove) setval = 0;
    else setval = 255;

    Curve *cv = Curves[c];

    PointList *p;

    if (cv->SplinePoints[file]->Count == 0) return; //nothing to see here!

    p = cv->SplinePoints[file];
    if (cv->Closed)
    {
        LPointList.X.clear();
        LPointList.Y.clear();
        LPointList.Count = 0;
        for (sn = 0; sn < p->Count; sn++)
        {
            //work out distance
            d = p->X[WrapSpline(sn, p->Count)] - p->X[WrapSpline(sn + 1, p->Count)];

            d2 = p->Y[WrapSpline(sn, p->Count)] - p->Y[WrapSpline(sn + 1, p->Count)];
            d = qAbs(d);
            d2 = qAbs(d2);
            if (d2 > d) d = d2;
            d *= 2; // a fudge factor carried over from VB
            if (d != 0)
            {
                dist = 1 / d;
                for (sm = 1; sm <= d; sm++)
                {
                    sm2 = double(sm) * dist;

                    sx = CatmullRomSpline(sm2, p->X[WrapSpline(sn - 1, p->Count)], p->X[WrapSpline(sn, p->Count)],
                                          p->X[WrapSpline(sn + 1, p->Count)], p->X[WrapSpline(sn + 2, p->Count)]);
                    sy = CatmullRomSpline(sm2, p->Y[WrapSpline(sn - 1, p->Count)], p->Y[WrapSpline(sn, p->Count)],
                                          p->Y[WrapSpline(sn + 1, p->Count)], p->Y[WrapSpline(sn + 2, p->Count)]);

                    if (cv->Filled)   //store them for scanline stuff even if off edge
                    {
                        LPointList.X.append(sx);
                        LPointList.Y.append(sy);
                    }
                    if (sy >= 0 && sy < fheight && sx >= 0 && sx < fwidth)
                        if ((*UseMasks)[static_cast<quint8>(Masks[(fheight - 1 - sy)*fwidth + sx])]) data[sy * fwidth + sx] = setval;
                }
            }
        }
        LPointList.Count = LPointList.X.count();
        if (cv->Filled) ScanlineFillOutput(c, data, &LPointList, UseMasks, setval);
    }
    else     //open
    {
        for (sn = 1; sn <= (p->Count - 3); sn++)   //all the segments
        {
            //work out distance
            d = p->X[sn] - p->X[sn + 1];
            d2 = p->Y[sn] - p->Y[sn + 1];
            d = qAbs(d);
            d2 = qAbs(d2);
            if (d2 > d) d = d2;
            d *= 2; //fudge factor it says here!
            if (d != 0)
            {
                dist = 1 / d;
                for (sm = 1; sm <= d; sm++)
                {
                    sm2 = static_cast<double>(sm * dist);

                    sx = CatmullRomSpline(sm2, p->X[sn - 1], p->X[sn], p->X[sn + 1], p->X[sn + 2]);
                    sy = CatmullRomSpline(sm2, p->Y[sn - 1], p->Y[sn], p->Y[sn + 1], p->Y[sn + 2]);

                    if (sy >= 0 && sy < fheight && sx >= 0 && sx < fwidth)
                        if ((*UseMasks)[static_cast<quint8>(Masks[(fheight - 1 - sy)*fwidth + sx])]) data[sy * fwidth + sx] = setval;
                }
            }
        }
    }
}



void DrawCurve(int c, int mycol, int file, QImage *Thresh)
{


//Direct translation from old VB sub
    double d, d2, dist, sm2;
    int sn, sm, sx, sy, apos;
    uchar *data;
    class PointList LPointList; //the list for filling

    if (Thresh->width() == 0) return; //can get here with empty QImage apparently
    data = Thresh->bits();
    Curve *cv = Curves[c];
    file *= zsparsity;

    uchar r = 0, g = 0, b = 0;
    if (mycol == 1 || cv->Segment == 0)   //curve mode
    {
        //use spline colours
        r = static_cast<uchar>(cv->Colour[0]);
        g = static_cast<uchar>(cv->Colour[1]);
        b = static_cast<uchar>(cv->Colour[2]);
    }
    else if (mycol == 0)     //seg colours //seg mode or similar
    {
        r = static_cast<uchar>(Segments[(cv->Segment) - 1]->Colour[0]);
        g = static_cast<uchar>(Segments[(cv->Segment) - 1]->Colour[1]);
        b = static_cast<uchar>(Segments[(cv->Segment) - 1]->Colour[2]);
    }

    PointList *p;

    if (cv->SplinePoints[file]->Count == 0)
    {
        return;   //nothing to see here!
    }

    if (cv->Segment == 0 && (SelectedCurve != c || CurrentMode != 2)) return; //not in a segment - don't draw


    p = cv->SplinePoints[file];

    if (cv->Closed)
    {

        LPointList.X.clear();
        LPointList.Y.clear();
        LPointList.Count = 0;
        for (sn = 0; sn < p->Count; sn++)
        {
            //work out distance
            d = p->X[WrapSpline(sn, p->Count)] - p->X[WrapSpline(sn + 1, p->Count)];

            d2 = p->Y[WrapSpline(sn, p->Count)] - p->Y[WrapSpline(sn + 1, p->Count)];
            d = qAbs(d);
            d2 = qAbs(d2);
            if (d2 > d) d = d2;
            d *= 2; // a fudge factor carried over from VB
            if (d != 0)
            {
                dist = 1 / d;
                for (sm = 1; sm <= d; sm++)
                {
                    sm2 = double(sm) * dist;

                    sx = CatmullRomSpline(sm2, p->X[WrapSpline(sn - 1, p->Count)], p->X[WrapSpline(sn, p->Count)],
                                          p->X[WrapSpline(sn + 1, p->Count)], p->X[WrapSpline(sn + 2, p->Count)]);
                    sy = CatmullRomSpline(sm2, p->Y[WrapSpline(sn - 1, p->Count)], p->Y[WrapSpline(sn, p->Count)],
                                          p->Y[WrapSpline(sn + 1, p->Count)], p->Y[WrapSpline(sn + 2, p->Count)]);

                    if (cv->Filled)   //store them for scanline stuff even if off edge
                    {
                        LPointList.X.append(sx);
                        LPointList.Y.append(sy);
                    }
                    if (sy >= 0 && sy < fheight && sx >= 0 && sx < fwidth)
                    {
                        SegmentMap[sy * fwidth + sx] = cv->Segment - 1;
                        apos = (sy) * fwidth * 4 + sx * 4; //offset for array

                        if (mycol < 2  || cv->Segment == 0)
                        {
                            //use spline or segment colours
                            RED(data, apos) = r;
                            GREEN(data, apos) = g;
                            BLUE(data, apos) = b;
                        }
                        else     //mask colours
                        {
                            //if (SegsFlag) //this is 'show segs in masks mode
                            //{
                            //   Mask *M = MasksSettings[(quint8)Masks[(fheight-sy-1)*fwidth + sx]];
                            //  RED(data,apos) = (uchar)(r/2  + M->ForeColour[0] / 2);
                            //  GREEN(data,apos) = (uchar)(g/2  + M->ForeColour[1] / 2);
                            //  BLUE(data,apos) = (uchar)(b/2  + M->ForeColour[2] / 2);
                            //}
                            //else //background - no seg over 128
                            //{
                            Mask *M = MasksSettings[static_cast<quint8>(Masks[(fheight - sy - 1) * fwidth + sx])];
                            RED(data, apos) = static_cast<uchar>(M->ForeColour[0]);
                            GREEN(data, apos) = static_cast<uchar>(M->ForeColour[1]);
                            BLUE(data, apos) = static_cast<uchar>(M->ForeColour[2]);
                            //}
                        }
                    }
                }
            }
        }
        LPointList.Count = LPointList.X.count();
        if (cv->Filled) ScanlineFill(c, mycol, Thresh, &LPointList);
    }
    else     //open
    {

        for (sn = 1; sn <= (p->Count - 3); sn++)   //all the segments
        {
            //work out distance
            d = p->X[sn] - p->X[sn + 1];
            d2 = p->Y[sn] - p->Y[sn + 1];
            d = qAbs(d);
            d2 = qAbs(d2);
            if (d2 > d) d = d2;
            d *= 2; //fudge factor it says here!
            if (d != 0)
            {
                dist = 1 / d;
                for (sm = 1; sm <= d; sm++)
                {
                    sm2 = static_cast<double>(sm) * dist;

                    sx = CatmullRomSpline(sm2, p->X[sn - 1], p->X[sn], p->X[sn + 1], p->X[sn + 2]);
                    sy = CatmullRomSpline(sm2, p->Y[sn - 1], p->Y[sn], p->Y[sn + 1], p->Y[sn + 2]);

                    if (sy >= 0 && sy < fheight && sx >= 0 && sx < fwidth)
                    {
                        apos = (sy) * fwidth * 4 + sx * 4; //offset for array
                        SegmentMap[sy * fwidth + sx] = cv->Segment - 1;

                        if (mycol < 2  || cv->Segment == 0)
                        {
                            //use spline colours
                            RED(data, apos) = r;
                            GREEN(data, apos) = g;
                            BLUE(data, apos) = b;
                        }
                        else     //mask colours
                        {
                            Mask *M = MasksSettings[static_cast<quint8>(Masks[(fheight - sy - 1) * fwidth + sx])];
                            RED(data, apos) = static_cast<uchar>(M->ForeColour[0]);
                            GREEN(data, apos) = static_cast<uchar>(M->ForeColour[1]);
                            BLUE(data, apos) = static_cast<uchar>(M->ForeColour[2]);
                        }
                    }
                }
            }
        }
    }
}

QList <QGraphicsItem *> MarkerList;

void DrawCurveMarkers(QGraphicsScene *scene)
{
    if (MarkerList.count() > 0)
        foreach (QGraphicsItem *item, MarkerList)
        {
            scene->removeItem(item);
            delete item;
        }
    MarkerList.clear();

    //redraw all the markers if mode is appropriate
    if (CurrentMode != 2) return; //only draw in curve mode
    if (SelectedCurve < 0) return; //need a selected curve


    double size = 10. / (CurrentZoom);
    double size2 = size / 2.0;

    PointList *p = Curves[SelectedCurve]->SplinePoints[CurrentFile * zsparsity];
    QPen mypen = QPen(QBrush(QColor(0, 255, 0)), 1);
    mypen.setCosmetic(true);
    QPen redpen = QPen(QBrush(QColor(0, 100, 255)), 1);
    redpen.setCosmetic(true);


    //First job - delete all existing markers
    if (MarkerList.count() > 0)
        foreach (QGraphicsItem *item, MarkerList)
        {
            scene->removeItem(item);
            delete item;
        }
    MarkerList.clear();

    //OK, plan is - cycle through all the markers
    for (int i = 0; i < p->Count; i++)
    {
        if (CurveMarkersAsCrosses)
        {
            if (i == 0)
            {
                //first - green box AND cross
                QGraphicsRectItem *newitem = new QGraphicsRectItem((p->X[i])*ColMonoScale - size2, (p->Y[i])*ColMonoScale - size2, size, size);
                newitem->setPen(mypen);
                newitem->setZValue(2);
                MarkerList.append(static_cast<QGraphicsItem *>(newitem));
                scene->addItem(newitem);
            }
            else if (i == p->Count - 1)
            {
                //last - red box
                QGraphicsRectItem *newitem = new QGraphicsRectItem((p->X[i])*ColMonoScale - size2, (p->Y[i])*ColMonoScale - size2, size, size);
                newitem->setPen(redpen);
                newitem->setZValue(2);
                MarkerList.append(static_cast<QGraphicsItem *>(newitem));
                scene->addItem(newitem);
            }
            //always do a cross
            QGraphicsLineItem *newitem1 = new QGraphicsLineItem((p->X[i])*ColMonoScale - size2, (p->Y[i])*ColMonoScale, (p->X[i])*ColMonoScale + size2, (p->Y[i])*ColMonoScale);
            QGraphicsLineItem *newitem3 = new QGraphicsLineItem((p->X[i])*ColMonoScale, (p->Y[i])*ColMonoScale - size2, (p->X[i])*ColMonoScale, (p->Y[i])*ColMonoScale + size2);
            if (i >= p->Count - 2)
            {
                newitem1->setPen(redpen);
                newitem3->setPen(redpen);
            }
            else
            {
                newitem1->setPen(mypen);
                newitem3->setPen(mypen);
            }
            newitem1->setZValue(2);
            newitem3->setZValue(2);

            MarkerList.append(static_cast<QGraphicsItem *>(newitem1));
            MarkerList.append(static_cast<QGraphicsItem *>(newitem3));
            scene->addItem(newitem1);
            scene->addItem(newitem3);
        }
        else     //plain circle markers
        {
            if (i == 0)
            {
                //first - green box
                QGraphicsRectItem *newitem = new QGraphicsRectItem((p->X[i])*ColMonoScale - size2, (p->Y[i])*ColMonoScale - size2, size, size);
                newitem->setPen(mypen);
                newitem->setZValue(2);
                MarkerList.append(static_cast<QGraphicsItem *>(newitem));
                scene->addItem(newitem);
            }
            else if (i == p->Count - 1)
            {
                //last - red box
                QGraphicsRectItem *newitem = new QGraphicsRectItem((p->X[i])*ColMonoScale - size2, (p->Y[i])*ColMonoScale - size2, size, size);
                newitem->setPen(redpen);
                newitem->setZValue(2);
                MarkerList.append(static_cast<QGraphicsItem *>(newitem));
                scene->addItem(newitem);
            }
            else
            {
                //otherwise - plain old marker
                QGraphicsEllipseItem *newitem = new QGraphicsEllipseItem((p->X[i])*ColMonoScale - size2, (p->Y[i])*ColMonoScale - size2, size, size);
                if (i == p->Count - 2) newitem->setPen(redpen);
                else newitem->setPen(mypen);
                newitem->setZValue(2);
                MarkerList.append(static_cast<QGraphicsItem *>(newitem));
                scene->addItem(newitem);
            }
        }
    }

}



/* Now some function relating to direct curve surfacing. Directly translated from the VB */

double GetLinearSplineLength(int scurve, int filenum, QList <double> *NodeSpacings)
{
    //calc and return spline length ignoring curvature of segments
    double s1, s2;
    int i;

    double GetLinearSplineLength = 0;
    if (Curves[scurve]->Closed)
    {
        for (i = 1; i < Curves[scurve]->SplinePoints[filenum * zsparsity]->Count; i++)
        {
            s1 = Curves[scurve]->SplinePoints[filenum * zsparsity]->X[i] - Curves[scurve]->SplinePoints[filenum * zsparsity]->X[i - 1];
            s2 = Curves[scurve]->SplinePoints[filenum * zsparsity]->Y[i] - Curves[scurve]->SplinePoints[filenum * zsparsity]->Y[i - 1];
            GetLinearSplineLength += (*NodeSpacings)[i] = sqrt(s1 * s1 + s2 * s2);
        }
        //and the 'wrapround' one
        s1 = Curves[scurve]->SplinePoints[filenum * zsparsity]->X[i - 1] - Curves[scurve]->SplinePoints[filenum * zsparsity]->X[0];
        s2 = Curves[scurve]->SplinePoints[filenum * zsparsity]->Y[i - 1] - Curves[scurve]->SplinePoints[filenum * zsparsity]->Y[0];
        GetLinearSplineLength += (*NodeSpacings)[0] = sqrt(s1 * s1 + s2 * s2);
    }
    else
    {
        for (i = 2; i < Curves[scurve]->SplinePoints[filenum * zsparsity]->Count - 1; i++)
        {
            s1 = Curves[scurve]->SplinePoints[filenum * zsparsity]->X[i] - Curves[scurve]->SplinePoints[filenum * zsparsity]->X[i - 1];
            s2 = Curves[scurve]->SplinePoints[filenum * zsparsity]->Y[i] - Curves[scurve]->SplinePoints[filenum * zsparsity]->Y[i - 1];
            GetLinearSplineLength += (*NodeSpacings)[i] = sqrt(s1 * s1 + s2 * s2);
        }
    }
    return GetLinearSplineLength;
}

void WorkOutNewNodes(int scurve, int filenum, double SplineLength, QList <double> *NodeSpacings, int NodesToUse, QList <double> *NodeX, QList <double> *NodeY, QList <double> *NodeNormX,
                     QList <double> *NodeNormY, int ArrayBase)
{
    Q_UNUSED(NodeNormY);
    int nextpoint, i, v1, v2, v3, v4, currentpoint;
    double DistIncrement, currentpos, ratiopos,  ratiopos2, ratiopos1, scaledown;

    scaledown = XYDownsample;
    //work out X and Y for specified number of nodes along line
    //note fudge to get round issues from false rounding
    DistIncrement = (SplineLength / static_cast<double>(NodesToUse - 1)) - 0.00000001;
    PointList *p = Curves[scurve]->SplinePoints[filenum * zsparsity];

    currentpos = 0;
    if (Curves[scurve]->Closed)
    {
        currentpoint = 0;
        nextpoint = 1;

        for (i = 0; i < NodesToUse; i++)
        {
            if (currentpos < (*NodeSpacings)[nextpoint])
            {
                if (currentpoint == 0)
                {
                    v1 = p->Count - 1;
                    v2 = 0;
                    v3 = 1;
                    v4 = 2;
                }
                else if ( currentpoint == (p->Count - 1))
                {
                    v1 = p->Count - 2;
                    v2 = p->Count - 1;
                    v3 = 0;
                    v4 = 1;
                }
                else if (currentpoint == (p->Count - 2))
                {
                    v1 = p->Count - 3;
                    v2 = p->Count - 2;
                    v3 = p->Count - 1;
                    v4 = 0;
                }
                else
                {
                    v1 = currentpoint - 1;
                    v2 = currentpoint;
                    v3 = currentpoint + 1;
                    v4 = currentpoint + 2;
                }

                ratiopos = currentpos / (*NodeSpacings)[nextpoint];
                (*NodeX)[i + ArrayBase] = DCatmullRomSpline(ratiopos, p->X[v1], p->X[v2], p->X[v3], p->X[v4]) / scaledown;
                (*NodeY)[i + ArrayBase] = DCatmullRomSpline(ratiopos, p->Y[v1], p->Y[v2], p->Y[v3], p->Y[v4]) / scaledown;

                //and the gradient - store x increase and y increase in NodeNormX, NodeNormY
                ratiopos2 = ratiopos + 0.01;
                if (ratiopos2 > 1) ratiopos2 = 1;
                ratiopos1 = ratiopos2 - 0.02;
                (*NodeNormX)[i + ArrayBase] = DCatmullRomSpline(ratiopos2, p->X[v1], p->X[v2], p->X[v3], p->X[v4]) / scaledown
                                              - DCatmullRomSpline(ratiopos1, p->X[v1], p->X[v2], p->X[v3], p->X[v4]) / scaledown;
                (*NodeNormX)[i + ArrayBase] = DCatmullRomSpline(ratiopos2, p->Y[v1], p->Y[v2], p->Y[v3], p->Y[v4]) / scaledown
                                              - DCatmullRomSpline(ratiopos1, p->Y[v1], p->Y[v2], p->Y[v3], p->Y[v4]) / scaledown;

                if (i != NodesToUse - 1)   //skip for last iteration
                {
                    currentpos += DistIncrement;
                    while (currentpos > (*NodeSpacings)[nextpoint])
                    {
                        currentpos -= (*NodeSpacings)[nextpoint];
                        currentpoint++;
                        nextpoint = currentpoint + 1;
                        if (nextpoint > (p->Count - 1)) nextpoint = 0;
                    }
                }
            }
        }
    }
    else
    {
        currentpoint = 1;
        for (i = 0; i < NodesToUse; i++)
        {
            if (currentpos < (*NodeSpacings)[currentpoint + 1])
            {
                v1 = currentpoint - 1;
                v2 = currentpoint;
                v3 = currentpoint + 1;
                v4 = currentpoint + 2;
                ratiopos = currentpos / (*NodeSpacings)[currentpoint + 1];
                (*NodeX)[i + ArrayBase] = DCatmullRomSpline(ratiopos, p->X[v1], p->X[v2], p->X[v3], p->X[v4]) / scaledown;
                (*NodeY)[i + ArrayBase] = DCatmullRomSpline(ratiopos, p->Y[v1], p->Y[v2], p->Y[v3], p->Y[v4]) / scaledown;

                //and the gradient - store x increase and y increase in NodeNormX, NodeNormY
                ratiopos2 = ratiopos + 0.01;
                if (ratiopos2 > 1) ratiopos2 = 1;
                ratiopos1 = ratiopos2 - 0.02;
                (*NodeNormX)[i + ArrayBase] = DCatmullRomSpline(ratiopos2, p->X[v1], p->X[v2], p->X[v3], p->X[v4]) / scaledown
                                              - DCatmullRomSpline(ratiopos1, p->X[v1], p->X[v2], p->X[v3], p->X[v4]) / scaledown;
                (*NodeNormX)[i + ArrayBase] = DCatmullRomSpline(ratiopos2, p->Y[v1], p->Y[v2], p->Y[v3], p->Y[v4]) / scaledown
                                              - DCatmullRomSpline(ratiopos1, p->Y[v1], p->Y[v2], p->Y[v3], p->Y[v4]) / scaledown;


                if (i != NodesToUse - 1)   //skip for last iteration
                {
                    currentpos = currentpos + DistIncrement;
                    while (currentpos > (*NodeSpacings)[currentpoint + 1])
                        currentpos = currentpos - (*NodeSpacings)[currentpoint++ + 1];
                }
            }
        }
    }
}

double GetStretch(int Index, QList <double> *stretches, int Smax)
{
    if (Index < 0) return (*stretches)[0] - (*stretches)[1]  - (*stretches)[0];
    if (Index > Smax) return (*stretches)[Smax] + (*stretches)[Smax] - (*stretches)[Smax - 1];
    return (*stretches)[Index];
}


double GetZpos(int Index, QList <double> *stretches)
{
    Index++;
    if (Index >= (stretches->size() - 1))
    {
        if (Index == (stretches->size() - 1)) return (*stretches)[Index];
        else return 0;
    }

    return (((*stretches)[Index] + (*stretches)[Index + 1])) / 2;
}

void SurfaceCurve(int scurve, int firstfile, int lastfile, QList <double> *stretches, int resamps, QVector <double> *TrigArray, int *TrigCount)
{
    int n, i, m, NodesToUse, tpos = 0;
    double SplineLength;

    //find the biggest spline count
    int max = 0;
    for (i = firstfile; i <= lastfile; i++) if (max < Curves[scurve]->SplinePoints[i * zsparsity]->Count) max = Curves[scurve]->SplinePoints[i * zsparsity]->Count;
    QList <double> NodeSpacings;
    for (i = 0; i < max; i++) NodeSpacings.append(0); //initialise to 0's
    //should produce nodespacings of correct size

    QList <double> NodeX;
    QList <double> NodeY;
    QList <double> NodeNormX;
    QList <double> NodeNormY;
    QList <double> NodeNormZ;
    QList <bool> Present;

    double zposa, zposb;
    int NodeBase;
    bool running;
    double v1x, v1y, v1z, v3x, v3y, v3z, cx, cy, cz;

    int Arrayn, tmp, nstep, nmin, nmax, Smax;
    bool flag;
    double XYresamp;
    Q_UNUSED(XYresamp);

    XYresamp = XYDownsample;
    Smax = lastfile - firstfile;
    NodesToUse = resamps; //for now

    //set up full size for arrays
    //Qvector is a trick, as we can't initialise Qlists to a set size

    QVector <double> temp(NodesToUse * (lastfile - firstfile + 1));
    NodeX = QList<double>::fromVector(temp);
    NodeY = QList<double>::fromVector(temp);
    NodeNormX = QList<double>::fromVector(temp);
    NodeNormY = QList<double>::fromVector(temp);
    NodeNormZ = QList<double>::fromVector(temp);

    QVector <bool> temp2(lastfile - firstfile + 1);
    Present = QList<bool>::fromVector(temp2);



    //flip running order if mirroring is no

    nstep = 1;
    nmin = firstfile;
    nmax = lastfile + 1;
    if (OutputMirroring)
    {
        nmin = lastfile;
        nmax = firstfile - 1;
        nstep = -1;
    }

    //get all the data and work out new nodes
    Arrayn = 0;
    for (n = nmin; n != nmax; n += nstep)
    {
        Present[Arrayn] = false;
        if (Curves[scurve]->SplinePoints[n * zsparsity]->Count > 0)
        {
            Present[Arrayn] = true;
            SplineLength = GetLinearSplineLength(scurve, n, &NodeSpacings);  //needs fnum
            NodeBase = Arrayn * NodesToUse;  //first position in NodeX/Y arrays
            WorkOutNewNodes (scurve, n, SplineLength, &NodeSpacings, NodesToUse, &NodeX, &NodeY, &NodeNormX, &NodeNormY, NodeBase);
        }
        Arrayn++;
    }

    //OK, now we work out normals for each node
    running = false;
    for (n = firstfile + 1; n <= lastfile; n++)
    {
        Arrayn = n - firstfile - 1;
        if (Present[Arrayn])
        {
            flag = false;
            if (n != nmax)
                if (Present[Arrayn + 1])
                {
                    flag = true;
                    //there is one next too
                    if (running == false)
                        v1z = GetZpos(Arrayn + 1, stretches) - GetZpos(Arrayn, stretches);
                    else
                        v1z = GetZpos(Arrayn + 1, stretches) - GetZpos(Arrayn - 1, stretches);

                    for (m = 0; m < NodesToUse; m++)
                    {
                        if (running == false)
                        {
                            //first one - use vector from this to next
                            v1x = NodeX[(Arrayn + 1) * NodesToUse + m] - NodeX[Arrayn * NodesToUse + m];
                            v1y = NodeY[(Arrayn + 1) * NodesToUse + m] - NodeY[Arrayn * NodesToUse + m];
                        }
                        else
                        {
                            //from last to next
                            v1x = NodeX[(Arrayn + 1) * NodesToUse + m] - NodeX[(Arrayn - 1) * NodesToUse + m];
                            v1y = NodeY[(Arrayn + 1) * NodesToUse + m] - NodeY[(Arrayn - 1) * NodesToUse + m];
                        }
                        //now do x product
                        v3x = 0 - v1z * NodeNormY[Arrayn * NodesToUse + m];
                        v3y = v1z * NodeNormX[Arrayn * NodesToUse + m];
                        v3z = v1x * NodeNormY[Arrayn * NodesToUse + m] - v1y * NodeNormX[Arrayn * NodesToUse + m];

                        NodeNormX[Arrayn * NodesToUse + m] = v3x;
                        NodeNormY[Arrayn * NodesToUse + m] = v3y;
                        NodeNormZ[Arrayn * NodesToUse + m] = v3z;
                    }
                    running = true;
                }

            if (flag == false)   //this is just an else for the double if
            {
                //this is the last of a run, so
                v1z = (*stretches)[Arrayn] - GetStretch(Arrayn - 1, stretches, Smax);
                v1z = GetZpos(Arrayn, stretches) - GetZpos(Arrayn, stretches);
                if (running)
                {
                    for (m = 0; m < NodesToUse; m++)
                    {
                        v1x = NodeX[(Arrayn) * NodesToUse + m] - NodeX[(Arrayn - 1) * NodesToUse + m];
                        v1y = NodeY[(Arrayn) * NodesToUse + m] - NodeY[(Arrayn - 1) * NodesToUse + m];

                        //now do x product
                        v3x = 0 - v1z * NodeNormY[Arrayn * NodesToUse + m];
                        v3y = v1z * NodeNormX[Arrayn * NodesToUse + m];
                        v3z = v1x * NodeNormY[Arrayn * NodesToUse + m] - v1y * NodeNormX[Arrayn * NodesToUse + m];

                        NodeNormX[Arrayn * NodesToUse + m] = v3x;
                        NodeNormY[Arrayn * NodesToUse + m] = v3y;
                        NodeNormZ[Arrayn * NodesToUse + m] = v3z;
                    }
                }
                else
                {
                    //if last AND first do nothing much
                    running = true;
                }
            }
        }
        else
        {
            running = false;
        }
    }

    //OK, hopefully now have normals

    //generate triglist
    //output node, normal for all triangles in a strip
    //terminate with a BIG number in nodeX
    running = false;
    for (n = firstfile; n < lastfile; n++)
    {
        Arrayn = n - firstfile;
        if (Present[Arrayn] && Present[Arrayn + 1])
        {
            //is this the start? If so a cap may be needed
            if (Curves[scurve]->Filled && Curves[scurve]->Closed && running == false)
            {
                //start cap
                //find centroid of all points on first curve, cx, cy
                cx = 0;
                cy = 0;
                for (m = 0; m < NodesToUse; m++)
                {
                    tmp = Arrayn * NodesToUse + m;
                    cx += NodeX[tmp];
                    cy += NodeY[tmp];
                }
                cx /= NodesToUse;
                cy /= NodesToUse;
                cz = (GetZpos(Arrayn, stretches) + GetZpos(Arrayn - 1, stretches)) / 2;

                //now define a triangle strip with all a points as this
                TrigArray->resize(*TrigCount * 6 + (NodesToUse * 12) + 6); //resizes array
                //possible bug - do I need to initialise Tpos here?
                zposb = GetZpos(Arrayn, stretches);
                for (m = 0; m < NodesToUse; m++)
                {
                    tmp = Arrayn * NodesToUse + m;
                    (*TrigArray)[tpos] = 0;
                    (*TrigArray)[tpos + 1] = 0;
                    (*TrigArray)[tpos + 2] = 1;
                    (*TrigArray)[tpos + 3] = cx;
                    (*TrigArray)[tpos + 4] = cy;
                    (*TrigArray)[tpos + 5] = cz;

                    tmp = (Arrayn) * NodesToUse + m;
                    (*TrigArray)[tpos + 6] = NodeNormX[tmp];
                    (*TrigArray)[tpos + 7] = NodeNormY[tmp];
                    (*TrigArray)[tpos + 8] = NodeNormZ[tmp];
                    (*TrigArray)[tpos + 9] = NodeX[tmp];
                    (*TrigArray)[tpos + 10] = NodeY[tmp];
                    (*TrigArray)[tpos + 11] = zposb;
                    tpos += 12;
                }
                (*TrigArray)[tpos] = 10000000; //termination code
                tpos += 6;
                *TrigCount += NodesToUse * 2 + 1;
            }
            running = true;
            TrigArray->resize(*TrigCount * 6 + (NodesToUse * 12)  + 6);
            tpos = *TrigCount * 6;
            zposa = GetZpos(Arrayn, stretches);
            zposb = GetZpos(Arrayn + 1, stretches);
            for (m = 0; m < NodesToUse; m++)
            {
                tmp = Arrayn * NodesToUse + m;
                (*TrigArray)[tpos] = NodeNormX[tmp];
                (*TrigArray)[tpos + 1] = NodeNormY[tmp];
                (*TrigArray)[tpos + 2] = NodeNormZ[tmp];
                (*TrigArray)[tpos + 3] = NodeX[tmp];
                (*TrigArray)[tpos + 4] = NodeY[tmp];
                (*TrigArray)[tpos + 5] = zposa;

                tmp = (Arrayn + 1) * NodesToUse + m;
                (*TrigArray)[tpos + 6] = NodeNormX[tmp];
                (*TrigArray)[tpos + 7] = NodeNormY[tmp];
                (*TrigArray)[tpos + 8] = NodeNormZ[tmp];
                (*TrigArray)[tpos + 9] = NodeX[tmp];
                (*TrigArray)[tpos + 10] = NodeY[tmp];
                (*TrigArray)[tpos + 11] = zposb;
                tpos += 12;
            }
            (*TrigArray)[tpos] = 10000000; //termination code
            tpos += 6;
            *TrigCount += NodesToUse * 2 + 1;
        }
        if ((! (Present[Arrayn] && Present[Arrayn + 1]) || n == lastfile - 1) && running)
        {
            if ( n == lastfile - 1 && Present[Arrayn] && Present[Arrayn + 1]) Arrayn++;
            if ( Curves[scurve]->Filled && Curves[scurve]->Closed)
            {
                //end cap
                //find centroid of all points on last curve, cx, cy
                cx = 0;
                cy = 0;
                for (m = 0; m < NodesToUse; m++)
                {
                    tmp = Arrayn * NodesToUse + m;
                    cx += NodeX[tmp];
                    cy += NodeY[tmp];
                }
                cx /= NodesToUse;
                cy /= NodesToUse;
                cz = (GetZpos(Arrayn, stretches) + GetZpos(Arrayn + 1, stretches)) / 2;
                //cz = GetStretch(Arrayn + 1, stretches, Smax)

                //now define a triangle strip with all a points as this
                TrigArray->resize(*TrigCount * 6 + (NodesToUse * 12) + 6);
                zposb = GetZpos(Arrayn, stretches);
                for (m = 0; m < NodesToUse; m++)
                {
                    (*TrigArray)[tpos + 6] = 0;
                    (*TrigArray)[tpos + 7] = 0;
                    (*TrigArray)[tpos + 8] = -1;
                    (*TrigArray)[tpos + 9] = cx;
                    (*TrigArray)[tpos + 10] = cy;
                    (*TrigArray)[tpos + 11] = cz;

                    tmp = (Arrayn) * NodesToUse + m;
                    (*TrigArray)[tpos + 0] = NodeNormX[tmp];
                    (*TrigArray)[tpos + 1] = NodeNormY[tmp];
                    (*TrigArray)[tpos + 2] = NodeNormZ[tmp];
                    (*TrigArray)[tpos + 3] = NodeX[tmp];
                    (*TrigArray)[tpos + 4] = NodeY[tmp];
                    (*TrigArray)[tpos + 5] = zposb;
                    tpos += 12;
                }
                (*TrigArray)[tpos] = 10000000; //termination code
                tpos += 6;
                TrigCount += NodesToUse * 2 + 1;
            }
            running = false;
        }
    }
}

void PopulateTriangleList(int OutObject, int firstfile, int lastfile, QList<double> *stretches, int resamps, QVector<double> *TrigArray, int *TrigCount)
{
    int n;
    //ReDim OutTrigArray(0) As Double
    for (n = 0; n < OutputObjects[OutObject]->CurveComponents.count(); n++)
        SurfaceCurve (n, firstfile, lastfile, stretches, resamps, TrigArray, TrigCount);
}


int GetGradientXPos(double pos, PointList *p)
{
    //get int and double parts of pos
    int intPart = (int)pos;
    double doublePart = pos - (double)intPart;
    if (doublePart<0) doublePart = 0;
    if (doublePart>1) doublePart = 1;

    int i0 = intPart-1; if (i0<0) i0 += p->Count; if (i0>=p->Count) i0-=p->Count;
    int i1 = intPart; if (i1<0) i1 += p->Count; if (i1>=p->Count) i1-=p->Count;
    int i2 = intPart+1; if (i2<0) i2 += p->Count; if (i2>=p->Count) i2-=p->Count;
    int i3 = intPart+2; if (i3<0) i3 += p->Count; if (i3>=p->Count) i3-=p->Count;

    //points are, apparently, already in threshold space.
    return CatmullRomSpline(doublePart, (*p).X[i0], (*p).X[i1],(*p).X[i2],(*p).X[i3]);

}


int GetGradientYPos(double pos, PointList *p)
{
    //get int and double parts of pos
    int intPart = (int)pos;
    double doublePart = pos - (double)intPart;
    if (doublePart<0) doublePart = 0;
    if (doublePart>1) doublePart = 1;

    int i0 = intPart-1; if (i0<0) i0 += p->Count; if (i0>=p->Count) i0-=p->Count;
    int i1 = intPart; if (i1<0) i1 += p->Count; if (i1>=p->Count) i1-=p->Count;
    int i2 = intPart+1; if (i2<0) i2 += p->Count; if (i2>=p->Count) i2-=p->Count;
    int i3 = intPart+2; if (i3<0) i3 += p->Count; if (i3>=p->Count) i3-=p->Count;

    return CatmullRomSpline(doublePart, (*p).Y[i0], (*p).Y[i1],(*p).Y[i2],(*p).Y[i3]);

}


// for gradients system. Sample - in threshold image space - a series of positions
// Assumed lists are empty
void GetPointsOnSpline(int curveIndex, QList<int> *xPos, QList<int> *yPos)
{
    xPos->clear();
    yPos->clear();

    Curve *cv = Curves[curveIndex];
    PointList *p = cv->SplinePoints[CurrentFile];

    int pointCount = p->Count;
    double posBase = 0;
    if (!cv->Closed)
    {
        pointCount-=2;
        posBase = 1;
    }
    double posAdd;

    int numberOfPoints = GradientDensity * pointCount;
    if (numberOfPoints<2)
        posAdd = 0;
    else
        posAdd = (double) pointCount / (double)(numberOfPoints-1);

    for (int i=0; i<numberOfPoints; i++)
    {
        //qDebug()<<posBase + i * posAdd;
        xPos->append(GetGradientXPos(posBase + i * posAdd,p));
        yPos->append(GetGradientYPos(posBase + i * posAdd,p));
    }

}
