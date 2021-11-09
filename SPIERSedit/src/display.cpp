/**
 * @file
 * Source: Display
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

#include "display.h"
#include "globals.h"
#include "fileio.h"
#include "mainwindowimpl.h"
#include "histogram.h"

#include <QGraphicsPixmapItem>
#include <QImage>
#include <QColor>
#include <QTime>
#include <math.h>
#include "curves.h"
#include "myscene.h"
#include "brush.h"

QGraphicsPixmapItem *MainImage;
double LastZoom;
double CurrentPolyContrast;

//add these to scene at some point!

void SaveMainImage(QString fname)
{
    MainImage->pixmap().save(fname);
}

QImage GenerateThresh()
{
    //Using current settings generate the threshold file for output

//        qDebug()<<"In GT";
//        qDebug()<<GA[0]->size();
    QImage RetThresh;
    uchar *data;
    int temp;
    int sn, n, m, invertedpos, max;
    Q_UNUSED(max);
    int x, y;
    int high, seg, i;
    bool lockmode, maskmode, greymode, MergeMasks, MergeMasks2;

    //First - work out segments at each point
    SegmentMap.resize(fwidth * fheight);

    QList <uchar *> GApointers;
    //set up my pointers - an optimisation to point straight to data
    for (n = 0; n < SegmentCount; n++)
        GApointers.append(GA[n]->bits());

    for (int jx = 0; jx < fwidth; jx++)
        for (int jy = 0; jy < fheight; jy++)
        {
            high = 128;
            seg = -1;
            //n=j*4;
            for (i = 0; i < SegmentCount; i++)
            {
                if (Segments[i]->Activated)
                {
                    temp = (int)  * ((GA[i]->bits()) + jy * fwidth4 + jx);
                    if (temp >= high)
                    {
                        high = temp;
                        seg = i;
                    }
                }
            }
            SegmentMap[jy * fwidth + jx] = seg;
        }


    //Code here is direct VB translation - hence superflous copying of bools!
    MergeMasks = SegsFlag; //bool for show segs in mask mode
    MergeMasks2 = MasksFlag; //bool for show masks in seg mode

    max = fwidth * fheight;

    if (CurrentMode == 3) lockmode = true;
    else lockmode = false;
    if (CurrentMode == 1) maskmode = true;
    else maskmode = false;
    if (ThreshFlag) greymode = false;
    else greymode = true;

    if (greymode)   // just copy the image
    {
//      qDebug()<<"Format of GA"<<GA[CurrentSegment]->format();
        //RetThresh = GA[CurrentSegment]->scaled(QSize(fwidth*ColMonoScale, fheight*ColMonoScale), Qt::IgnoreAspectRatio,Qt::FastTransformation);
//      qDebug()<<"Format of GA rescales"<<RetThresh.format();
        RetThresh = GA[CurrentSegment]->convertToFormat(QImage::Format_RGB32); //make sure it's in 32 bit ARGB
//      qDebug()<<"Format of rescaled and converted"<<RetThresh.format();
        //RetThresh = RetThresh.convertToFormat(QImage::Format_RGB888); //make sure it's in 32 bit ARGB
    }
    else
    {
        //OK, algorithm is as follows
        //for each point, go through all GA arrays
        //find highest value > 128 (if nothing >= 128 its background)

        //First - set up Thresh to correct size
        QImage Thresh(fwidth, fheight, QImage::Format_RGB32); //set up buffer

        data = Thresh.bits(); //pointer to data

        n = 0;
        for (y = 0; y < fheight; y++)
            for (x = 0; x < fwidth; x++)
            {
                invertedpos = (fheight - 1 - y) * fwidth + x;
                if (!(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->Show))
                {
                    m = n * 4;
                    if (maskmode)
                    {
                        RED(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[0]);
                        GREEN(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[1]);
                        BLUE(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[2]);
                    }
                    else
                    {
                        RED(data, m) = static_cast<uchar>(0);
                        GREEN(data, m) = static_cast<uchar>(0);
                        BLUE(data, m) = static_cast<uchar>(0);
                    }
                }
                else
                {
                    m = n * 4;
                    seg = SegmentMap[n];
                    if (lockmode)
                    {
                        if (Locks[invertedpos * 2])   //*2 as these are qint16's. Just look at one byte is fine though - bools!
                        {
                            //qDebug()<<"Drawinglock";
                            if (seg >= 0)
                            {
                                RED(data, m) = static_cast<uchar>((Segments[seg]->Colour[0]) / 3);
                                GREEN(data, m) = static_cast<uchar>((Segments[seg]->Colour[1]) / 3);
                                BLUE(data, m) = static_cast<uchar>((Segments[seg]->Colour[2]));
                            }
                            else     //'background - no seg over 128
                            {
                                RED(data, m) = static_cast<uchar>(0);
                                GREEN(data, m) = static_cast<uchar>(0);
                                BLUE(data, m) = static_cast<uchar>(100);
                            }
                        }
                        else
                        {
                            if (seg >= 0)
                            {
                                RED(data, m) = static_cast<uchar>(Segments[seg]->Colour[0]);
                                GREEN(data, m) = static_cast<uchar>(Segments[seg]->Colour[1]);
                                BLUE(data, m) = static_cast<uchar>(Segments[seg]->Colour[2]);
                            }
                            else     //background - no seg over 128
                            {
                                RED(data, m) = static_cast<uchar>(0);
                                GREEN(data, m) = static_cast<uchar>(0);
                                BLUE(data, m) = static_cast<uchar>(0);
                            }
                        }
                    }
                    else if (maskmode)
                    {
                        if (MergeMasks)
                        {
                            if (seg >= 0)
                            {
                                RED(data, m) = static_cast<uchar>((Segments[seg]->Colour[0]) / 2  + (MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[0] / 2));
                                GREEN(data, m) = static_cast<uchar>((Segments[seg]->Colour[1]) / 2  + (MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[1] / 2));
                                BLUE(data, m) = static_cast<uchar>((Segments[seg]->Colour[2]) / 2  + (MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[2] / 2));
                            }
                            else     //background - no seg over 128
                            {
                                RED(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[0]);
                                GREEN(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[1]);
                                BLUE(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[2]);
                            }
                        }
                        else
                        {
                            if (seg >= 0)
                            {
                                RED(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[0]);
                                GREEN(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[1]);
                                BLUE(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[2]);
                            }
                            else     //background - no seg over 128
                            {
                                RED(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[0]);
                                GREEN(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[1]);
                                BLUE(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[2]);
                            }
                        }
                    }
                    else
                    {
                        if (MergeMasks2)   // show masks in seg
                        {
                            if (seg >= 0)
                            {
                                RED(data, m) = static_cast<uchar>((Segments[seg]->Colour[0]) / 2  + (MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[0] / 2));
                                GREEN(data, m) = static_cast<uchar>((Segments[seg]->Colour[1]) / 2  + (MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[1] / 2));
                                BLUE(data, m) = static_cast<uchar>((Segments[seg]->Colour[2]) / 2  + (MasksSettings[static_cast<quint8>(Masks[invertedpos])]->ForeColour[2] / 2));
                            }
                            else     //background - no seg over 128
                            {
                                RED(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[0]);
                                GREEN(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[1]);
                                BLUE(data, m) = static_cast<uchar>(MasksSettings[static_cast<quint8>(Masks[invertedpos])]->BackColour[2]);
                            }
                        }
                        else
                        {
                            //This one is 'normal'
                            if (seg >= 0)
                            {
                                RED(data, m) = static_cast<uchar>(Segments[seg]->Colour[0]);
                                GREEN(data, m) = static_cast<uchar>(Segments[seg]->Colour[1]);
                                BLUE(data, m) = static_cast<uchar>(Segments[seg]->Colour[2]);
                            }
                            else     //background - no seg over 128
                            {
                                RED(data, m) = static_cast<uchar>(0);
                                GREEN(data, m) = static_cast<uchar>(0);
                                BLUE(data, m) = static_cast<uchar>(0);
                            }
                        }
                    }
                }
                n++;
            }


        //Curves now
        if (CurrentMode == 2)  for (sn = 0; sn < CurveCount; sn++) DrawCurve(sn, 1, CurrentFile, &Thresh);
        else if (CurrentMode == 1 || MergeMasks2)  for (sn = 0; sn < CurveCount; sn++) DrawCurve(sn, 2, CurrentFile, &Thresh);
        else for (sn = 0; sn < CurveCount; sn++) DrawCurve(sn, 0, CurrentFile, &Thresh);


        return Thresh;
        //RetThresh = Thresh.scaled(fwidth*ColMonoScale, fheight*ColMonoScale, Qt::IgnoreAspectRatio, Qt::FastTransformation);

    }
    return RetThresh;

}


void ClearImages()
{
    //called to reset on a reload
    delete MainImage;
    delete scene;
}

void InitImage(QGraphicsView *gv)
{
    //Set up all display objects after a load
    int i;

//  if (Active==false) return;

    MainImage = new QGraphicsPixmapItem(Files.at(0));
    scene = new myscene;

    //if (Active) qDeleteAll(GA.begin(), GA.end());
    //check for 0 as I do add blanks in loadgreyimage now
    //for (int i=0; i<GA.count(); i++) if (GA[i]!=(QImage *)0) delete GA[i];
    GA.clear();

    if (Active) ClearCache();
    for (i = 0; i < SegmentCount; i++) GA.append(nullptr); //sets up segment arrays

    //also set up dirty array

    cheight = (MainImage->pixmap()).height();
    cwidth = (MainImage->pixmap()).width();

    MainImage->setCursor(Qt::BlankCursor);
    MainImage->setZValue(0);
    scene->addItem(MainImage);

    gv->setScene(scene);
    gv->show();

}


void AlterImage(QImage *myimage)
{
//Does ALL preprocessing of image, which has already been loaded from source data
    int i, j, c, c2;
    int minval, maxval;
    int tcwidth, tcheight;
    double addon;
    double subtract;
    double Col;
    double Col2;
    double  Transf, Transf2;
    QImage Thresh;
    QString Info;

    minval = CMin;
    maxval = CMax;

    if (minval == 255)  minval = 254;

    if (maxval == 0)  maxval = 1;

    if (minval >= maxval)  minval = maxval - 1;


    addon = static_cast<double>(255.) / (static_cast<double>(maxval - minval));

    subtract = static_cast<double>(minval);


    Transf = static_cast<double>(Trans) / static_cast<double>(15);

    Transf2 = static_cast<double>(15 - Trans) / static_cast<double>(15);

    Thresh = GenerateThresh(); //get threshold image generated and returned

    uchar *colpointer = ColArray.bits();
    uchar *outpointer = myimage->bits();
    uchar *threshpointer = Thresh.bits();
    c = 0;
    c2 = 0;
    int c3 = 0;
    Q_UNUSED(c3);

    tcwidth = fwidth * ColMonoScale;
    tcheight = fheight * ColMonoScale; //now use fwidth, fheight

    if (GreyImage)   //code for GS/Col copied and altered for speed
    {
        for (i = 0; i < tcheight; i++)
        {
            for (j = 0; j < cwidth; j++)
            {
                c2 = 4 * ((i / ColMonoScale) * fwidth + j / ColMonoScale);
                c = 4 * (i * cwidth + j);

                Col = ((static_cast<double>(colpointer[i * cwidth4 + j])) - subtract) * addon;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                Col *= Transf;
                if (j < tcwidth) //deal with edge effects
                    Col2 = static_cast<double>(RED(threshpointer, c2)); //threshold array
                else
                    Col2 = 0;
                Col2 = Col + Col2 * Transf2;
                if (Col2 > 255) Col2 = 255;
                if (Col2 < 0) Col2 = 0;
                RED(outpointer, c) = static_cast<uchar>(Col2);

                if (j < tcwidth) //deal with edge effects
                    Col2 = static_cast<double>(GREEN(threshpointer, c2)); //threshold array
                else
                    Col2 = 0;
                Col2 = Col + Col2 * Transf2;
                if (Col2 > 255) Col2 = 255;
                if (Col2 < 0) Col2 = 0;
                GREEN(outpointer, c) = static_cast<uchar>(Col2);

                if (j < tcwidth)   //deal with edge effects
                {
                    Col2 = static_cast<double>(BLUE(threshpointer, c2));   //threshold array
                }
                else
                    Col2 = 0;
                Col2 = Col + Col2 * Transf2;
                if (Col2 > 255) Col2 = 255;
                if (Col2 < 0) Col2 = 0;
                BLUE(outpointer, c) = static_cast<uchar>(Col2);

                ALPHA(outpointer, c) = 255;
            }
            //now do any extra rows of colour only
        }
        c3 = 0;
        for (i = tcheight; i < cheight; i++)
            for (j = 0; j < cwidth; j++)
            {
                c = 4 * (i * cwidth + j);
                Col = ((static_cast<double>(colpointer[i * cwidth4 + j])) - subtract) * addon;
                Col *= Transf;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                RED(outpointer, c) = static_cast<uchar>(Col);
                GREEN(outpointer, c) = static_cast<uchar>(Col);
                BLUE(outpointer, c) = static_cast<uchar>(Col);
                ALPHA(outpointer, c) = 255;
            }
    }
    else
    {
        for (i = 0; i < tcheight; i++)
        {
            for (j = 0; j < cwidth; j++)
            {
                c2 = 4 * ((i / ColMonoScale) * fwidth + j / ColMonoScale);
                c = (i * cwidth + j) * 4;
                Col = ((static_cast<double>(RED(colpointer, c))) - subtract) * addon;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                if (j < tcwidth) //deal with edge effects
                    Col2 = static_cast<double>(RED(threshpointer, c2)); //threshold array
                else
                    Col2 = 0;
                Col = Col * Transf + Col2 * Transf2;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                RED(outpointer, c) = static_cast<uchar>(Col);

                Col = ((static_cast<double>(GREEN(colpointer, c))) - subtract) * addon;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                if (j < tcwidth) //deal with edge effects
                    Col2 = static_cast<double>(GREEN(threshpointer, c2)); //threshold array
                else
                    Col2 = 0;
                Col = Col * Transf + Col2 * Transf2;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                GREEN(outpointer, c) = static_cast<uchar>(Col);

                Col = ((static_cast<double>(BLUE(colpointer, c))) - subtract) * addon;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                if (j < tcwidth)   //deal with edge effects
                {
                    Col2 = static_cast<double>(BLUE(threshpointer, c2));   //threshold array
                }
                else
                    Col2 = 0;
                Col = Col * Transf + Col2 * Transf2;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                BLUE(outpointer, c) = static_cast<uchar>(Col);

                ALPHA(outpointer, c) = 255;

            }
            //now do any extra rows of colour only
        }
        for (i = tcheight; i < cheight; i++)
            for (j = 0; j < cwidth; j++)
            {
                c = (i * cwidth + j) * 4;
                Col = ((static_cast<double>(RED(colpointer, c))) - subtract) * addon;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                Col = Col * Transf;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                RED(outpointer, c) = static_cast<uchar>(Col);

                Col = ((static_cast<double>(GREEN(colpointer, c))) - subtract) * addon;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                Col = Col * Transf;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                GREEN(outpointer, c) = static_cast<uchar>(Col);

                Col = ((static_cast<double>(BLUE(colpointer, c))) - subtract) * addon;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                Col = Col * Transf;
                if (Col > 255) Col = 255;
                if (Col < 0) Col = 0;
                BLUE(outpointer, c) = static_cast<uchar>(Col);

                ALPHA(outpointer, c) = 255;

            }
    }
    //myimage->save("d:/test/threstest-myimage.bmp");
    return;
}


void ShowImage(QGraphicsView *gv)
{
//        qDebug()<<"Here in Graphics View";
//        qDebug()<<GA.count();
//        for (int i=0; i<GA.count(); i++) qDebug()<<GA[i];
    QMatrix identity; //will default to this
    if (Active == false) return;
    QImage myimage(cwidth, cheight, QImage::Format_RGB32);


    AlterImage(&myimage); //Preprocess original image (contrast, transparency), add new one
    MainImage->setPixmap(QPixmap::fromImage(myimage));

    gv->setSceneRect(myimage.rect()); //keep scene size to this image - in case of curve markers for instance dragging it out
    if (LastZoom != CurrentZoom)
    {
        gv->setMatrix(identity);
        gv->scale(CurrentZoom, CurrentZoom);
        LastZoom = CurrentZoom;
    }


    GVHist->Refresh();

    //Do curve markers

    DrawCurveMarkers(gv->scene());

    MainWindowImpl *mw = static_cast<MainWindowImpl *>(gv->parent()->parent());
    if (GreyImage)
    {
        mw->LinearRedSpinBox->setVisible(false);
        mw->LinearGreenSpinBox->setVisible(false);
        mw->LinearBlueSpinBox->setVisible(false);
    }
    else
    {
        mw->LinearRedSpinBox->setVisible(true);
        mw->LinearGreenSpinBox->setVisible(true);
        mw->LinearBlueSpinBox->setVisible(true);
    }

}


void DeleteDisplayObjects()
//call from main window destructor
{
    int i;
    delete scene;
    delete MainImage;
    for (i = 0; i < SegmentCount; i++) delete (GA.at(i)); //delete segment arrays
}

QByteArray DoMaskLocking()
{
    //This needs to look through all masks and segments. Any hidden masks and any locked segments shouldn't be written - write in
    //a 1 to the relevent lock entry
    QByteArray newlocks(fwidth * fheight, 0);

    for (int x = 0; x < fwidth; x++)
        for (int y = 0; y < fheight; y++)
        {
            int hpos = (fheight - y - 1) * fwidth + x;
            if (Locks[hpos * 2]) newlocks[fwidth * y + x] = static_cast<uchar>(255);
            if (HiddenMasksLockedForGeneration) if (!MasksSettings[static_cast<quint8>(Masks[hpos])]->Show) newlocks[fwidth * y + x] = 255;
        }
    return newlocks;
}

//Do LCE - based on MakeLinearGreyScale
void ApplyLCE(int seg, int fnum, bool flag = false)
{
    //load data for file - can and should assume existing data is safe
    if (!flag) LoadAllData(fnum);

    if (Segments[seg]->Locked) return;
    uchar *data= GA[seg]->bits(); // get data from GA array

    //make a copy of underlying data
    QVector<uchar> data_original_vector(fwidth4 * fheight);
    uchar *data_original = data_original_vector.data();
    memcpy(data_original,data,fwidth4*fheight);

    QByteArray NewLocks = DoMaskLocking();

    //ignore inversion - doesn't make sense for LCE
    for (int h = 0; h < fheight; h++)
        for (int w = 0; w < fwidth; w++)
        {
            if (!(NewLocks[(fwidth * h + w)]))
                *(data + (fwidth4 * h + w))
                    = LCEPixel(w, h, data_original, &NewLocks);
        }
    if (!flag) SaveGreyData(fnum, seg);


}

void MakeLinearGreyScale(int seg, int fnum, bool flag = false)
{
    //load data for file - can and should assume existing data is safe
    if (!flag) LoadAllData(fnum);

    int r = Segments[seg]->LinPercent[0];
    int g = Segments[seg]->LinPercent[1];
    int b = Segments[seg]->LinPercent[2];
    int glob = Segments[seg]->LinGlobal;
    if (Segments[seg]->Locked) return;
    uchar *data;
    data = GA[seg]->bits();

    QByteArray NewLocks = DoMaskLocking();


    if (Segments[seg]->LinInvert)
    {

        for (int h = 0; h < fheight; h++)
            for (int w = 0; w < fwidth; w++)
            {
                if (!(NewLocks[(fwidth * h + w)])) *(data + (fwidth4 * h + w)) = static_cast<uchar>(255) - GreyScalePixel(w, h, r, g, b, glob);
            }
    }
    else
    {
        for (int h = 0; h < fheight; h++)
            for (int w = 0; w < fwidth; w++)
            {
                if (!(NewLocks[(fwidth * h + w)])) *(data + (fwidth4 * h + w)) = GreyScalePixel(w, h, r, g, b, glob);
            }
    }

    if (!flag) SaveGreyData(fnum, seg);

}

void MakeBlankGreyScale(int seg, int fnum, bool flag = false)
{
    Q_UNUSED(flag);
    //load data for file - can and should assume existing data is safe
    LoadGreyData(fnum, seg);
    uchar *data;
    data = GA[seg]->bits();


    for (int h = 0; h < fheight; h++)
        for (int w = 0; w < fwidth; w++)
        {
            *(data + (fwidth4 * h + w)) = 0;
        }

    SaveGreyData(fnum, seg);
}

void MakeRangeGreyScale(int seg, int fnum, bool flag = false)
{
    if (!flag) LoadAllData(fnum);


    if (Segments[seg]->Locked) return;
    uchar *data;
    data = GA[seg]->bits();

    QByteArray NewLocks = DoMaskLocking();

    int b = Segments[seg]->RangeBase;
    int t = Segments[seg]->RangeTop;

    //work out centers and gradients
    double c = static_cast<double>(b) + ((static_cast<double>(t) - static_cast<double>(b)) / 2.0);
    double g;
    if (b == t) g = 500;
    else g = (127.0) / ((static_cast<double>(t - b)) / 2.0);

    //ignore invert for range
    for (int h = 0; h < fheight; h++)
        for (int w = 0; w < fwidth; w++)
        {
            if (!(NewLocks[(fwidth * h + w)])) *(data + (fwidth4 * h + w)) = RangePixel(w, h, b, t, c, g, seg);
        }
    if (!flag) SaveGreyData(fnum, seg);

}

double CalcPoly(unsigned char r, unsigned char g, unsigned char b, Segment *seg)
{
    //Work out result of polynomial on given RGB!

    double result;
    double red, green, blue;
    int n;

    double redp, greenp, bluep; //will be the powers - mult up each time!

    redp = red = static_cast<double>(r);
    greenp = green = static_cast<double>(g);
    bluep = blue = static_cast<double>(b);

    result = seg->PolyKall; //start with constant

    double *rc = seg->PolyRedConsts;
    double *gc = seg->PolyRedConsts;
    double *bc = seg->PolyRedConsts;

    int check = seg->PolyOrder;
    for (n = 0; n < check; n++)
    {
        result += (rc[n] * redp + gc[n] * greenp + bc[n] * bluep);
        //raise powers for next iteration
        greenp *= green;
        redp *= red;
        bluep *= blue;
    }

    return result;
}

uchar PolyPixel(int w, int h, int s)
{
    w *= ColMonoScale;
    h *= ColMonoScale;
    int rtot = 0, gtot = 0, btot = 0;
    Segment *seg = Segments[s];
    uchar *data = ColArray.bits();

    if (GreyImage)
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                rtot += static_cast<int>(data[n + m * cwidth4]);
            }
        rtot /= (ColMonoScale * ColMonoScale);
        gtot = rtot;
        btot = rtot;
    }
    else
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                int p = 4 * (n + m * cwidth);
                rtot += static_cast<int>(RED(data, p));
                gtot += static_cast<int>(GREEN(data, p));
                btot += static_cast<int>(BLUE(data, p));
            }
        rtot /=  (ColMonoScale * ColMonoScale);
        gtot /=  (ColMonoScale * ColMonoScale);
        btot /=  (ColMonoScale * ColMonoScale);
    }

    //have my average RGB values  - work out function!
    int temp = static_cast<int>(128 + CalcPoly(static_cast<uchar>(rtot), static_cast<uchar>(gtot), static_cast<uchar>(btot), seg) * CurrentPolyContrast);

    if (temp < 0) return 0;
    if (temp > 255) return 255;
    return static_cast<uchar>(temp);
}

uchar RangePixel(int w, int h, int bot, int top, double cen, double gra, int seg)
{
    Q_UNUSED(bot);
    Q_UNUSED(top);

    w *= ColMonoScale;
    h *= ColMonoScale;
    int rtot = 0, gtot = 0, btot = 0;
    int r;
    uchar *data = ColArray.bits();

    if (GreyImage)
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                rtot += static_cast<int>(data[n + m * cwidth4]);
            }
        rtot /= (ColMonoScale * ColMonoScale);
        r = rtot;
    }
    else
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                int p = 4 * (n + m * cwidth);
                rtot += static_cast<int>(RED(data, p));
                gtot += static_cast<int>(GREEN(data, p));
                btot += static_cast<int>(BLUE(data, p));
            }
        rtot /=  (ColMonoScale * ColMonoScale);
        gtot /=  (ColMonoScale * ColMonoScale);
        btot /=  (ColMonoScale * ColMonoScale);
        r = (rtot + gtot + btot) / 3;
    }

    //have my average RGB value  - work out function!
    if (RangeHardFill)
    {
        if (r >= Segments[seg]->RangeBase && r <= Segments[seg]->RangeTop)
            //Fix up any other segs
            return 255;
        else
            return 0;
    }
    else
    {
        double val = 255.0 - gra * qAbs(static_cast<double>(r) - cen);
        if (val < 0) return 0;
        else return static_cast<uchar>(val + .5);
    }
}


void MakePolyGreyScale(int seg, int fnum, bool flag = false)
{
    CurrentPolyContrast = pow(static_cast<double>(2), Segments[seg]->PolyContrast) / Segments[seg]->PolyScale;

    //load data for file - can and should assume existing data is safe
    if (!flag) LoadAllData(fnum);

    if (Segments[seg]->Locked) return;
    uchar *data;
    data = GA[seg]->bits();

    QByteArray NewLocks = DoMaskLocking();

    if (Segments[seg]->LinInvert)
    {

        for (int h = 0; h < fheight; h++)
            for (int w = 0; w < fwidth; w++)
            {
                if (!(NewLocks[(fwidth * h + w)])) *(data + (fwidth4 * h + w)) = static_cast<uchar>(255) - PolyPixel(w, h, seg);
            }
    }
    else
    {
        for (int h = 0; h < fheight; h++)
            for (int w = 0; w < fwidth; w++)
            {
                if (!(NewLocks[(fwidth * h + w)])) *(data + (fwidth4 * h + w)) = PolyPixel(w, h, seg);
            }
    }
    if (!flag) SaveGreyData(fnum, seg);
}



uchar GenPixel(int x, int y, int s, QVector<uchar> *sample, QByteArray *locks)
{
    CurrentPolyContrast = pow(static_cast<double>(2), Segments[s]->PolyContrast) / Segments[s]->PolyScale;
    //generate a pixel using whatever method
    if (tabwidget->currentIndex() == 0)
    {
        uchar t = GreyScalePixel( x,  y,  Segments[s]->LinPercent[0],
                                  Segments[s]->LinPercent[1],  Segments[s]->LinPercent[2],  Segments[s]->LinGlobal);

        if (Segments[s]->LinInvert) return 255 - t;
        else return t;
    }

    if (tabwidget->currentIndex() == 1)
    {
        uchar t = PolyPixel(x, y, s);
        if (Segments[s]->LinInvert) return 255 - t;
        else return t;
    }

    if (tabwidget->currentIndex() == 2)
    {
        int b = Segments[s]->RangeBase;
        int t = Segments[s]->RangeTop;
        //work out centers and gradients
        double c = static_cast<double>(b) + ((static_cast<double>(t) - static_cast<double>(b)) / 2.0);
        double g;
        if (b == t) g = 500;
        else g = (127.0) / ((static_cast<double>(t - b)) / 2.0);
        uchar t2 = RangePixel(x, y, b, t, c, g, s);
        return t2;
    }

    if (tabwidget->currentIndex() == 3)
    {
        return LCEPixel(x,y,sample->data(),locks);
    }

    return 0;
}

uchar LCEPixel(int w, int h, uchar *original_data, QByteArray *new_locks)
{

    int val = (int)original_data[w + fwidth4*h];


    //work out min, max h and w for loop
    int minw = w-LCE_Radius;
    if (minw<0) minw=0;
    int maxw = w+LCE_Radius;
    if (maxw>=fwidth) maxw = fwidth-1;
    int minh = h-LCE_Radius;
    if (minh<0) minh=0;
    int maxh = h+LCE_Radius;
    if (maxh>=fheight) maxh = fheight-1;

    int radius_squared = LCE_Radius*LCE_Radius;
    quint64 count=0;
    quint64 sum=0;
    for (int ypos = minh; ypos<=maxh; ypos++)
    for (int xpos = minw; xpos<=maxw; xpos++)
    {
        //Exclude outside radius
        int d0 = ypos-h;
        int d1 = xpos-w;
        if (d0*d0+d1*d1<=radius_squared)
        {   
            if (!(new_locks->at(ypos*fwidth+xpos)))  // exclude lock-excluded pixels
            {
                sum+=static_cast<quint64>(original_data[ypos*fwidth4+xpos]);
                count++;
            }
        }
    }
    if (count==0) return static_cast<uchar>(val); //probably impossible?
    int mean = static_cast<int>(sum/count);

    int diff = static_cast<int>(val) - mean;
    int boost = (diff * LCE_Boost) / 5 + LCE_Adjust;
    val += boost;
    if (val<0) return 0;
    if (val>255) return 255;
    return static_cast<uchar>(val);
}

uchar GreyScalePixel(int w, int h, int r, int g, int b, int glob)
{
    w *= ColMonoScale;
    h *= ColMonoScale;
    int rtot = 0;
    uchar *data = ColArray.bits();
    int temp;
    if (GreyImage)
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                rtot += static_cast<int>(data[n + m * cwidth4]) * (glob - 1) / 50;
            }
        temp = rtot / (ColMonoScale * ColMonoScale);
    }
    else
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                int p = 4 * (n + m * cwidth);
                rtot += static_cast<int>(RED(data, p)) * r;
                rtot += static_cast<int>(GREEN(data, p)) * g;
                rtot += static_cast<int>(BLUE(data, p)) * b;
            }
        rtot *= glob;
        temp = rtot / (ColMonoScale * ColMonoScale * 10000);
    }


    if (temp < 0) temp= 0;
    if (temp > 255) temp= 255;
    return static_cast<uchar>(temp);
}
