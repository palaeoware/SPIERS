/**
 * @file
 * Source: Output
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

#include "output.h"
#include "globals.h"
#include "curves.h"
#include "fileio.h"

uchar ResampleForLoRes(int X, int Y, QByteArray *a)
{
    int n, m, o, c = 0, max;

    int lrx = X + XYDownsample;
    int lry = Y + XYDownsample;
    int zr = ZDownsample;

    max = fheight * fwidth;

    for (n = X; n < lrx; n++)
        for (m = Y; m < lry; m++)
            for (o = 0; o < zr; o++)
                if (a->at(o * max + m * fwidth + n))
                    if (++c >= PixSens) return static_cast<uchar>(255);

    return static_cast<uchar>(0);
}

bool freached(int f, int to, int from)
{
    if (to == from)
    {
        if (f == to) return true;
        else return false;
    }
    if (to >= from)
    {
        if (f <= to) return true;
        else return false;
    }
    else
    {
        if (f >= to) return true;
        else return false;
    }
}


void PopulateOutputArray (int *FullOutArrayCount, QByteArray *FullOutArray, int *Count, int FirstOutputFile, int LastOutputFile, int Object, int awidth, int aheight, QLabel *pb)
{
    int f, bigpos, n, m, max, temp;
    Q_UNUSED(max);
    int seg, i, bigposfirst;
    Q_UNUSED(bigposfirst);
    int mi, loopfrom, loopto, loopstep, edgestart, fmax, tpos = 0;
    QByteArray temparray; //used for pre-resampling
    QByteArray OutputArray; //my actual target array
    OutputArray.resize(awidth * aheight);
    max = (awidth * aheight) - 1; //downsampled

    fmax = fwidth * fheight - 1;   //not downsampled


    //   qDebug()<<"Doing resample";

    //do mirroring by setting loop parameters
    if (OutputMirroring)
    {
        loopto = FirstOutputFile;
        loopfrom = loopto;
        loopstep = 0 - ZDownsample;
        while ((loopfrom - loopstep) <= (LastOutputFile + loopstep + 1))
        {
            loopfrom -= loopstep;
        }
    }
    else
    {
        loopfrom = FirstOutputFile;
        loopto = LastOutputFile;
        loopstep = ZDownsample;
        loopto = loopto - (((loopto - loopfrom) + 1) % loopstep); //avoid extras at end

    }

    QList <bool> UseMasks;
    QList <bool> UseSegs;

    for (i = 0; i <= MaxUsedMask; i++)
        if ((OutputObjects[Object]->ComponentMasks.indexOf(i)) >= 0)
            UseMasks.append(true);
        else UseMasks.append(false);

    for (i = 0; i < SegmentCount; i++)
        if ((OutputObjects[Object]->ComponentSegments.indexOf(i)) >= 0)
            UseSegs.append(true);
        else UseSegs.append(false);

    for (f = loopfrom; freached(f, loopto, loopfrom); f += loopstep) //loop through all files
    {
        QString message;
        QTextStream ms(&message);

        ms << "File: " << f;
        pb->setText(message);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        bigpos = 0;
        //get the files
        if (XYDownsample * ZDownsample > 1)
        {
            //we are downsampling, so need to blank the temp array
            temparray.fill(0, (fmax + 1) * ZDownsample);
            tpos = 0; //position in temp array
        }
        OutputArray.fill(0);

        for (i = 0; i < ZDownsample; i++)
        {
            int usefile = f + i;
            if ((usefile) >= FileCount) usefile = FileCount - 1; //off end! - can't go off start I hope. Just repeat last file
            {
                for (int j = 0; j < SegmentCount; j++) LoadGreyData(usefile, j);
                LoadMasks(usefile);

                if (XYDownsample * ZDownsample == 1)
                {
                    //no resample - build directly into fullarray


                    bigposfirst = bigpos;
                    for (int y = fheight - 1; y >= 0; y--)
                        for (int x = 0; x < fwidth; x++)
                        {
                            //work out segment - not in a function for speed
                            if ((UseMasks[static_cast<quint8>(Masks[y * fwidth + x])]))
                            {
                                int high = 128;
                                seg = -1;
                                for (int s = 0; s < SegmentCount; s++)
                                {
                                    if (Segments[s]->Activated)
                                    {
                                        temp = (int)  * ((GA[s]->bits()) + (fheight - 1 - y) * fwidth4 + x);
                                        if (temp >= high)
                                        {
                                            high = temp;
                                            seg = s;
                                        }
                                    }
                                }

                                if (seg != -1)
                                    if (UseSegs[seg]) //seg in list... is mask?
                                        OutputArray[bigpos] = static_cast<char>(255);

                                //no need to enter a 0 - array is initialised to 0
                            }
                            bigpos++;
                        }

                    //Now curves
                    for (int sn = 0; sn < CurveCount; sn++)
                    {
                        if (Curves[sn]->Segment != 0) //0 means not in any seg
                        {
                            int segcv = Curves[sn]->Segment - 1;
                            //qDebug()<<usefile<<segcv<< UseSegs[segcv];
                            if (Segments[segcv]->Activated && UseSegs[segcv])
                                DrawCurveOutput(sn, usefile, (uchar *) OutputArray.data(), &UseMasks, false);
                            else
                                //If curve is in a segment that makes it invisible - should REMOVE all the points!
                                DrawCurveOutput(sn, usefile, (uchar *) OutputArray.data(), &UseMasks, true);
                        }
                    }
                }
                else
                {
                    //Need to do resample, so work in temp array

                    mi = i * (fmax + 1);

                    for (int y = fheight - 1; y >= 0; y--)
                        for (int x = 0; x < fwidth; x++)
                            //work out segment - not in a function for speed
                        {
                            if ((UseMasks[static_cast<quint8>(Masks[y * fwidth + x])]))
                            {
                                int high = 128;
                                seg = -1;
                                for (int s = 0; s < SegmentCount; s++)
                                {
                                    if (Segments[s]->Activated)
                                    {
                                        //RJG - have left a few of these old style cast due to the fact that these are pointers and don't necessarily need a static cast
                                        temp = (int)  * ((GA[s]->bits()) + (fheight - 1 - y) * fwidth4 + x);
                                        if (temp >= high)
                                        {
                                            high = temp;
                                            seg = s;
                                        }
                                    }
                                }

                                if (seg != -1)
                                    if (UseSegs[seg]) //seg in list... is mask?
                                        temparray[tpos] = 255;

                                //no need to enter a 0 - array is initialised to 0
                            }
                            tpos++;
                        }

                    //Now the curves
                    for (int sn = 0; sn < CurveCount; sn++)
                    {
                        if (Curves[sn]->Segment != 0) //0 means not in any seg
                        {
                            int segcv = Curves[sn]->Segment - 1;
                            if (Segments[segcv]->Activated && UseSegs[segcv])
                                DrawCurveOutput(sn, usefile, (uchar *) temparray.data() + mi, &UseMasks, false);
                            else
                                //If curve is in a segment that makes it invisible - should REMOVE all the points!
                                DrawCurveOutput(sn, usefile, (uchar *) temparray.data() + mi, &UseMasks, true);
                        }
                    }

                }

            }
        }

        if (XYDownsample * ZDownsample > 1)
        {
            for (n = 0; n < awidth; n++) for (m = 0; m < aheight; m++)
                    if (ResampleForLoRes(n * XYDownsample, m * XYDownsample, &temparray)) OutputArray[m * awidth + n] = 255;
        }

        if (*Count == -1)
        {
            //NOT doing a count - so write a border of 0's round the edge to cap anything
            //top
            edgestart = 0;

            //PADDING

            for (n = 0; n < awidth; n++)   OutputArray[edgestart++] = 0;

            //sides
            for (n = 1; n < (aheight - 2); n++)
            {
                OutputArray[edgestart] = 0;
                edgestart += awidth;
                OutputArray[edgestart - 1] = 0;
            }

            //bottom
            for (n = 0; n < awidth; n++)   OutputArray[edgestart++] = 0;

            int gxsize = (awidth / GRID_SCALE) + 1;
            int gysize = (aheight / GRID_SCALE) + 1;
            QByteArray GridArray(gxsize * gysize, '\0'); //create 0 filled gridding array

            int Gpos = 0; //index into GridArray
            for (m = 0; m < gysize; m++) //for each row of grid
            {
                int ymax = gysize + GRID_SCALE;
                if (ymax > aheight) ymax = aheight; //height of cell (might be less than grid size at end)
                for (n = 0; n < gxsize; n++) //for each column
                {
                    int xmax = gxsize + GRID_SCALE;
                    if (xmax > awidth) xmax = awidth; //width of cell

                    //now within cell look at all pixels
                    for (int mm = gysize * GRID_SCALE; mm < ymax; mm++) //for each row
                    {
                        bigpos = m * GRID_SCALE * awidth + n * GRID_SCALE; //work out index into big array
                        for (int nn = gxsize * GRID_SCALE; nn < xmax; nn++) //row each column
                            if (OutputArray[bigpos++])
                            {
                                GridArray[Gpos] = 255;    //if I hit an on pixel turn grid cell on and stop
                                goto break2;
                            }
                    }
break2:
                    Gpos++;
                }
            }

            //Is whole grid array still 0?
            bool zflag = true;
            Q_UNUSED(zflag);
            Gpos = 0;
            for (m = 0; m < gysize; m++) for (n = 0; n < gxsize; n++) if (GridArray[Gpos++]) zflag = false;

            //Compression
            QByteArray comp1;
            //if (!zflag)
            comp1 = qCompress(OutputArray, 9);

            //TO DO
            //OUTPUT -1 only if zflag is -1
            //OTHERWISE CONTINUE AS NORMAL, BUT INSERT NEW ARRAY
            //snip off header, stick on a size

            //For some reason code below doesn't work

            int outsize = comp1.size() - 4;
            QByteArray size(4, '\0');
            int FAS = FullOutArray->size();
            FullOutArray->append(size);
            uchar *data = (uchar *)FullOutArray->data();
            data[FAS] = static_cast<uchar>(outsize % 256);
            outsize /= 256;
            data[FAS + 1] = static_cast<uchar> (outsize % 256);
            outsize /= 256;
            data[FAS + 2] = static_cast<uchar> (outsize % 256);
            outsize /= 256;
            data[FAS + 3] = static_cast<uchar>(outsize);

            //QDataStream ds(FullOutArray, QIODevice::WriteOnly);

            //ds.setByteOrder(QDataStream::LittleEndian);
            //ds<<((int)(comp1.size()-4));
            //ds<<(int)-1;

            FullOutArray->append(comp1.right(comp1.size() - 4));

            (*FullOutArrayCount)++;
        }
        else //doing a count not an output - no capping, no compressing
        {
            int lsize = OutputArray.size();
            uchar *d = (uchar *)OutputArray.data();
            for (int li = 0; li < lsize; li++)
                if  (d[li]) (*Count)++;
        }
    }

    //restore settings for current file
    for (int j = 0; j < SegmentCount; j++) LoadGreyData(CurrentFile, j);
    LoadMasks(CurrentFile);

    return;

}

/*QList <QByteArray *> PopulateOutputArray (int *Count, QList <QByteArray *> fullarray, int FirstOutputFile, int LastOutputFile, int Object, int awidth, int aheight, QLabel *pb)
{
        qDebug()<<"P1";

    int f, bigpos, n, m, max, temp;
    int seg, i, bigposfirst;
    int mi, loopfrom, loopto, loopstep, edgestart, fmax, tpos=0;
    QByteArray temparray; //used for pre-resampling
    QByteArray OutputArray; //my actual target array
    OutputArray.resize(awidth * aheight);
    max = (awidth * aheight) - 1; //downsampled
            //qDebug()<<"Writing"<<((int)(comp1.size()-4));
            //ds<<(int)-1;
    fmax = fwidth * fheight - 1;   //not downsampled


        qDebug()<<"Doing resample";

   //do mirroring by setting loop parameters
    if (OutputMirroring)
    {
        loopto=FirstOutputFile; loopfrom=loopto; loopstep=0-ZDownsample;
        while ((loopfrom-loopstep)<=(LastOutputFile+loopstep+1)) {loopfrom-=loopstep;}
    }
    else
    {
        loopfrom = FirstOutputFile;  loopto = LastOutputFile;  loopstep = ZDownsample;
        loopto = loopto - (((loopto - loopfrom) + 1) % loopstep); //avoid extras at end

    }

        qDebug()<<"P2";
    QList <bool> UseMasks;
    QList <bool> UseSegs;

    for (i=0; i<=MaxUsedMask; i++)
        if ((OutputObjects[Object]->ComponentMasks.indexOf(i))>=0)
            UseMasks.append(true); else UseMasks.append(false);

    for (i=0; i<SegmentCount; i++)
        if ((OutputObjects[Object]->ComponentSegments.indexOf(i))>=0)
            UseSegs.append(true); else UseSegs.append(false);
        qDebug()<<"P3";
    for (f = loopfrom; freached(f,loopto,loopfrom); f+=loopstep) //loop through all files
    {
        QString message;
        QTextStream ms(&message);
        ms<<"File: "<<f;
        pb->setText(message);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        bigpos=0;
        //get the files
        if (XYDownsample * ZDownsample > 1)
        {
            //we are downsampling, so need to blank the temp array
            temparray.fill(0,(fmax + 1) * ZDownsample);
            tpos=0; //position in temp array
        }
        OutputArray.fill(0);

        for (i = 0; i<ZDownsample; i++)
        {
            int usefile =f+i; if ((usefile)>=FileCount) usefile=FileCount-1; //off end! - can't go off start I hope. Just repeat last file
            {
                for (int j=0; j<SegmentCount; j++) LoadGreyData(usefile, j);
                            LoadMasks(usefile);
                        // qDebug()<<"Skipping processing"<<usefile;

                        if (XYDownsample * ZDownsample == 1)
                {   //no resample - build directly into fullarray

                    for (int sn=0; sn<CurveCount; sn++)
                    {
                        if (Curves[sn]->Segment!=0) //0 means not in any seg
                        {
                            int segcv=Curves[sn]->Segment-1;
                            if (Segments[segcv]->Activated && UseSegs[segcv])
                                DrawCurveOutput(sn, usefile, (uchar *) OutputArray.data(), &UseMasks);
                        }
                    }

                    bigposfirst = bigpos;
                    for (int y=fheight-1; y>=0; y--)
                    for (int x=0; x<fwidth; x++)
                    {
                        //work out segment - not in a function for speed
                        if ((UseMasks[(quint8)Masks[y*fwidth+x]]))
                        {
                            int high = 128;
                            seg = -1;
                            for (int s=0; s<SegmentCount; s++)
                            {
                                if (Segments[s]->Activated)
                                {
                                    temp=(int)  *((GA[s]->bits())+(fheight-1-y)*fwidth4 + x);
                                    if (temp >= high)  { high = temp; seg = s;}
                                }
                            }

                            if (seg != -1)
                                if (UseSegs[seg]) //seg in list... is mask?
                                     OutputArray[bigpos] = (char)255;

                            //no need to enter a 0 - array is initialised to 0
                        }
                       bigpos++;
                    }


                }
                        else
                {
                    //Need to do resample, so work in temp array

                    mi=i*(fmax+1);
                    for (int sn=0; sn<CurveCount; sn++)
                    {
                        if (Curves[sn]->Segment!=0) //0 means not in any seg
                        {
                            int segcv=Curves[sn]->Segment-1;
                            if (Segments[segcv]->Activated && UseSegs[segcv])
                                DrawCurveOutput(sn, usefile, (uchar *) temparray.data()+mi, &UseMasks);
                        }
                    }

                    for (int y=fheight-1; y>=0; y--)
                    for (int x=0; x<fwidth; x++)
                        //work out segment - not in a function for speed
                    {
                        if ((UseMasks[(quint8)Masks[y*fwidth+x]]))
                        {
                            int high = 128;
                            seg = -1;
                            for (int s=0; s<SegmentCount; s++)
                            {
                                if (Segments[s]->Activated)
                                {
                                    temp=(int)  *((GA[s]->bits())+(fheight-1-y)*fwidth4 + x);
                                    if (temp >= high)  { high = temp; seg = s;}
                                }
                            }

                            if (seg != -1)
                                if (UseSegs[seg]) //seg in list... is mask?
                                        temparray[tpos] = 255;

                            //no need to enter a 0 - array is initialised to 0
                        }
                        tpos++;
                    }
                }

            }
        }

        qDebug()<<"P4";
        if (XYDownsample * ZDownsample > 1)
        {
           for (n=0; n<awidth; n++) for (m=0; m<aheight; m++)
               if (ResampleForLoRes(n * XYDownsample, m * XYDownsample, &temparray)) OutputArray[m * awidth + n] =255;
        }

        qDebug()<<"P5";
        if (*Count==-1)
        {
            //qDebug()<<"In edgewrite";
            //NOT doing a count - so write a border of 0's round the edge to cap anything
            //top
            edgestart=0;

                //PADDING

            for (n = 0; n<awidth; n++)   OutputArray[edgestart++] = 0;

            //sides
            for (n=1; n<(aheight - 2); n++)
            {
                OutputArray[edgestart] = 0;
                edgestart += awidth;
                OutputArray[edgestart - 1] = 0;
                }

            //bottom
            for (n = 0; n<awidth; n++)   OutputArray[edgestart++] = 0;
                        qDebug()<<"P6";




                //Old Zlip approach

                QByteArray comp1=qCompress(OutputArray, 1);
                QByteArray *compressed=new QByteArray(comp1);
                //(*compressed)=qCompress(OutputArray, 1);

                //new LZO approach

            //Now done - can compress and add to the QList
                //QByteArray *compressed=new QByteArray(OutputArray.size() + OutputArray.size() / 16 + 64 + 3,0);
                //  debugstream<<"About to compress new algorithm, output array size is "<<OutputArray.size(); debugstream.flush();

                //  if (lzo_init() != LZO_E_OK)
                //    {debugstream<<"Failed to initialise lzo_lib"; debugstream.flush();}
                //      else
                //    {debugstream<<"Initialised lzo_lib"; debugstream.flush();}

                //  lzo_uint out_len;
                //    int r = lzo1x_1_compress((const unsigned char *)OutputArray.constData(),OutputArray.size(),(unsigned char *)compressed->data(),&out_len,wrkmem);
                //    if (r == LZO_E_OK)
                //    {debugstream<<"Compressed into "<<out_len<<" bytes\n"; debugstream.flush();}
                //     else
                //    {debugstream<<"Compression failed"; debugstream.flush();}
                //
                //    compressed->resize(out_len);
                //  debugstream<<"Done compress\n"; debugstream.flush();

                //DO THE APPEND
                fullarray.append(compressed);

                //int bytes=0;
                //for (int ii=0; ii<fullarray.count(); ii++) bytes+=fullarray[ii]->size();
                //qDebug()<<"Added "<<compressed->size()<<bytes<<" total of "<<fullarray.count()<<"objects, for "<<bytes;
                //delete compressed;

                qDebug()<<"End Sec";
        }
        else //doing a count not an output - no capping, no compressing
        {
                qDebug()<<"IN COUNT!!!";
            int lsize=OutputArray.size();
            uchar * d=(uchar *)OutputArray.data();
            for (int li=0; li<lsize; li++)
                if  (d[li]) (*Count)++;
        }
    }

    //restore settings for current file
    for (int j=0; j<SegmentCount; j++) LoadGreyData(CurrentFile, j);
     LoadMasks(CurrentFile);

    //and done!
        qDebug()<<"And Out";
        //qDebug()<<"P7";
    return fullarray;

}
*/
