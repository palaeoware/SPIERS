/*********************************************

SPIERSedit 2: brush.cpp

Some OO programming originating from me, unforced!
Class brush - handles brush pixel arrays, and all brush-related drawing
- Will only have one object - resize function will redo all the internal arrays

**********************************************/


#include "brush.h"
#include "globals.h"
#include "display.h"
#include <math.h>
#include "myscene.h"  //for the scene
#include <QPen>
#include <QGraphicsView>
#include <QInputDialog>

Brush_class Brush;

Brush_class::Brush_class()
{
    resize(5, 0, 0);
}

Brush_class::~Brush_class()
//destructor - probably not needed
{
    return;
}

void Brush_class::resize(int size, int shape, double o)
//set up the brush - size is radius
//shape - 0=square, 1=circle, 2+ reserved
// 2 is now 3d square (cuboid) brush
// 3 is 3d 3d ellipsoid brush
{
    int x, y;
    int d = size * size;
    int d2;
    bool flag;
    Radius = size;
    //clear everything
    Xpos.clear();
    Ypos.clear();
    Soft.clear();
    PixelCount = 0;
    LeftLinesX.clear();
    RightLinesX.clear();
    TopLinesX.clear();
    BottomLinesX.clear();
    LeftLinesY.clear();
    RightLinesY.clear();
    TopLinesY.clear();
    BottomLinesY.clear();
    LeftLinesCount = 0;
    RightLinesCount = 0;
    TopLinesCount = 0;
    BottomLinesCount = 0;

    if (shape == 0)   //square
    {
        for (x = 0 - (size - 1); x < size; x++)
            for (y = 0 - (size - 1); y < size; y++)
            {
                if (x == (0 - (size - 1)))
                {
                    LeftLinesX << x;
                    LeftLinesY << y;
                    LeftLinesCount++;
                }
                if (x >= (size - 1))
                {
                    RightLinesX << x;
                    RightLinesY << y;
                    RightLinesCount++;
                }
                Xpos << x;
                Ypos << y;
                PixelCount++;
            }

        //now put in tops and bottoms
        for (x = 0 - (size - 1); x < size; x++)
        {
            TopLinesX << x;
            TopLinesY << (0 - (size - 1));
            TopLinesCount++;
            BottomLinesX << x;
            BottomLinesY << (size - 1);
            BottomLinesCount++;
        }

        //soft edge dist array
        Dist.clear();
        for (int i = 0; i < PixelCount; i++)
            Dist.append(qMax(qAbs(Xpos[i]), qAbs(Ypos[i])));

    }
    else if (shape == 1)     //circle
    {
        //do special cases
        if (size == 2)
        {
            //cross
            LeftLinesX << -1;
            LeftLinesY << 0;
            LeftLinesCount++;
            LeftLinesX << 0;
            LeftLinesY << 1;
            LeftLinesCount++;
            LeftLinesX << 0;
            LeftLinesY << -1;
            LeftLinesCount++;
            RightLinesX << 1;
            RightLinesY << 0;
            RightLinesCount++;
            RightLinesX << 0;
            RightLinesY << 1;
            RightLinesCount++;
            RightLinesX << 0;
            RightLinesY << -1;
            RightLinesCount++;
            TopLinesX << 0;
            TopLinesY << -1;
            TopLinesCount++;
            TopLinesX << -1;
            TopLinesY << 0;
            TopLinesCount++;
            TopLinesX << 1;
            TopLinesY << 0;
            TopLinesCount++;
            BottomLinesX << 0;
            BottomLinesY << 1;
            BottomLinesCount++;
            BottomLinesX << -1;
            BottomLinesY << 0;
            BottomLinesCount++;
            BottomLinesX << 1;
            BottomLinesY << 0;
            BottomLinesCount++;
            Xpos << 0;
            Ypos << -1;
            Xpos << 0;
            Ypos << 0;
            Xpos << 0;
            Ypos << 1;
            Xpos << -1;
            Ypos << 0;
            Xpos << 1;
            Ypos << 0;
            PixelCount = 5;
        }
        else if (size == 3)
        {
            //cross
            LeftLinesX << -1;
            LeftLinesY << -2;
            LeftLinesCount++;
            LeftLinesX << -2;
            LeftLinesY << -1;
            LeftLinesCount++;
            LeftLinesX << -2;
            LeftLinesY << 0;
            LeftLinesCount++;
            LeftLinesX << -2;
            LeftLinesY << 1;
            LeftLinesCount++;
            LeftLinesX << -1;
            LeftLinesY << 2;
            LeftLinesCount++;

            RightLinesX << 1;
            RightLinesY << -2;
            RightLinesCount++;
            RightLinesX << 2;
            RightLinesY << -1;
            RightLinesCount++;
            RightLinesX << 2;
            RightLinesY << 0;
            RightLinesCount++;
            RightLinesX << 2;
            RightLinesY << 1;
            RightLinesCount++;
            RightLinesX << 1;
            RightLinesY << 2;
            RightLinesCount++;

            TopLinesX << -2;
            TopLinesY << -1;
            TopLinesCount++;
            TopLinesX << -1;
            TopLinesY << -2;
            TopLinesCount++;
            TopLinesX << 0;
            TopLinesY << -2;
            TopLinesCount++;
            TopLinesX << 1;
            TopLinesY << -2;
            TopLinesCount++;
            TopLinesX << 2;
            TopLinesY << -1;
            TopLinesCount++;

            BottomLinesX << -2;
            BottomLinesY << 1;
            BottomLinesCount++;
            BottomLinesX << -1;
            BottomLinesY << 2;
            BottomLinesCount++;
            BottomLinesX << 0;
            BottomLinesY << 2;
            BottomLinesCount++;
            BottomLinesX << 1;
            BottomLinesY << 2;
            BottomLinesCount++;
            BottomLinesX << 2;
            BottomLinesY << 1;
            BottomLinesCount++;

            Xpos << -2;
            Ypos << -1;
            Xpos << -2;
            Ypos << 0;
            Xpos << -2;
            Ypos << 1;
            Xpos << 2;
            Ypos << -1;
            Xpos << 2;
            Ypos << 0;
            Xpos << 2;
            Ypos << 1;
            for (int i = 0; i < 5; i++)
            {
                Xpos << 1;
                Ypos << i - 2;
                Xpos << 0;
                Ypos << i - 2;
                Xpos << -1;
                Ypos << i - 2;
            }
            PixelCount = 21;
        }

        else
        {
            for (y = 0 - (size - 1); y < size; y++)
            {
                flag = false;
                for (x = 0 - (size - 1); x < size; x++)
                {
                    //work out distance
                    d2 = y * y + x * x;
                    if (d2 < d)   //inside
                    {
                        if (!flag)   //first one
                        {
                            LeftLinesX << x;
                            LeftLinesY << y;
                            LeftLinesCount++;
                            flag = true;
                        }
                        Xpos << x;
                        Ypos << y;
                        PixelCount++;
                    }
                    else if (flag)     //run off end - LAST pixel should be edge
                    {
                        flag = false;
                        RightLinesX << (x - 1);
                        RightLinesY << y;
                        RightLinesCount++;
                    }
                }
                //if flag is still true need to set right still
                if (flag)
                {
                    RightLinesX << (x - 1);
                    RightLinesY << y;
                    RightLinesCount++;
                    flag = true;
                }

            }

            //run loop again other way round to do top and bottom
            for (x = 0 - (size - 1); x < size; x++)
            {
                flag = false;
                for (y = 0 - (size - 1); y < size; y++)
                {
                    //work out distance
                    d2 = y * y + x * x;
                    if (d2 < d)   //inside
                    {
                        if (!flag)   //first one
                        {
                            TopLinesX << x;
                            TopLinesY << y;
                            TopLinesCount++;
                            flag = true;
                        }
                    }
                    else if (flag)     //run off end - LAST pixel should be edge
                    {
                        flag = false;
                        BottomLinesX << x;
                        BottomLinesY << (y - 1);
                        BottomLinesCount++;
                    }
                }
                //if flag is still true need to set right still
                if (flag)
                {
                    BottomLinesX << x;
                    BottomLinesY << (y - 1);
                    BottomLinesCount++;
                    flag = true;
                }

            }
        }

        //soft edge dist array
        Dist.clear();
        for (int i = 0; i < PixelCount; i++)
            Dist.append(sqrt(static_cast<double>(Xpos[i]*Xpos[i] + Ypos[i]*Ypos[i])));

    }
    else if (shape == 2 || shape == 3)
    {
        //3d shapes
        ThreeDBrushPixels(&Xpos, &Ypos, &PixelCount, o, shape);

        int max = qMax(qMax(Brush_Size, BrushY), BrushZ);

        //do vertical sweeps first
        for (int x = 0 - max; x < max; x++)
        {
            QList <int> TY;
            int i = -1;
            while ((i = Xpos.indexOf(x, i + 1)) != -1) TY.append(Ypos[i]);

            if (TY.count())
            {
                qSort(TY); //now ordered
                bool on = false;
                int last = TY[0] - 2;
                foreach (int y, TY)
                {
                    if (y != last + 1)
                    {
                        //top line
                        TopLinesX << x;
                        TopLinesY << y;
                        TopLinesCount++;
                        if (on)   //also need to close last one
                        {
                            BottomLinesX << x;
                            BottomLinesY << last;
                            BottomLinesCount++;
                        }
                    }
                    last = y;
                    on = true;
                }
                //and closing line
                BottomLinesX << x;
                BottomLinesY << last;
                BottomLinesCount++;
            }
        }

        //Now horizontal
        for (int y = 0 - max; y < max; y++)
        {
            QList <int> TX;
            int i = -1;
            while ((i = Ypos.indexOf(y, i + 1)) != -1) TX.append(Xpos[i]);

            if (TX.count())
            {
                qSort(TX); //now ordered
                bool on = false;
                int last = TX[0] - 2;
                foreach (int x, TX)
                {
                    if (x != last + 1)
                    {
                        //left line
                        LeftLinesX << x;
                        LeftLinesY << y;
                        LeftLinesCount++;
                        if (on)   //also need to close last one
                        {
                            RightLinesX << last;
                            RightLinesY << y;
                            RightLinesCount++;
                        }
                    }
                    last = x;
                    on = true;
                }
                //and closing
                RightLinesX << last;
                RightLinesY << y;
                RightLinesCount++;
            }
        }

        //soft edge dist array
        Dist.clear();
        for (int i = 0; i < PixelCount; i++)
            Dist.append(0); //might have to be a high number - but essentially we want to disable soft brushing

    }
    else Error("Unknown shape in Resize Brush");

}

void Brush_class::ThreeDBrushPixels(QList <int> *X, QList <int> *Y, int *PointCount, double offset, int mode)
{

    Q_UNUSED(mode);
    //returns pixel list of 3D slice, offset is slice offset - in slices (i.e. multiply by 1/slicepermm to get dist)
    //mode - 2 is cuboid, 3 is ellipsoid

//  bool ok;
//  offset=QInputDialog::getDouble(mainwin, "Test - offset in slices",
//                                        "Slices:", 0, -10000, 10000, 2, &ok);
    //for now assume it's a circle
    //qDebug()<<"offset is "<<offset;
    double R = static_cast<double>(Brush_Size) * PixPerMM * static_cast<double>(ColMonoScale);
    double O = offset * (1.0 / (SlicePerMM));
    //O and R are in mm.
    if (O < R)
    {
        double LR = sqrt(R * R - O * O);
        LR /= (PixPerMM * static_cast<double>(ColMonoScale));
        int s = static_cast<int>(LR);
        double LR2 = LR * LR;
        //now fill circle with LR
        for (int y = 0 - (s); y < s; y++)
        {
            for (int x = 0 - (s); x < s; x++)
            {
                //work out distance
                double d2 = y * y + x * x;
                if (d2 < LR2)   //inside
                {
                    *X << x;
                    *Y << y;
                }
            }
        }
    }

    *PointCount = X->count();
    return;
}


void Brush_class::Brush_Flag_Restart()
{
    RestartFlag = true;
}

bool Brush_class::draw(int x, int y)
//Draw the lines onto the graphicsscene
//returns true if anything drawn, otherwise false (allows cursor to change)
{
    int n;
    bool NoRecreateFlag = false;
    QPen MyPen;
    qreal rcms = static_cast<qreal>(ColMonoScale);
    qreal rx1, rx2, ry1, ry2;
    int lx, ly;

    if (Active == false) return false;
    QGraphicsLineItem *temp;
    MyPen.setCosmetic(true);
    MyPen.setColor(QColor(255, 0, 0));
    MyPen.setWidth(1);

    //first check to see if we need to recreate or just move lines
    if (LineList.count() == TopLinesCount + BottomLinesCount + LeftLinesCount + RightLinesCount) NoRecreateFlag = true;
    if (RestartFlag) NoRecreateFlag = false; //on a reload you need to redo it all - no lines left!

    if (NoRecreateFlag) for (n = 0; n < LineList.count(); n++) LineList[n]->setVisible(false);


    //remove list
    if (!NoRecreateFlag)
    {
        if (!RestartFlag) qDeleteAll(LineList.begin(), LineList.end());
        LineList.clear();
        //remove all line items from scene
        if (!RestartFlag) foreach (temp, LineList)
            {
                scene->removeItem(temp);
            }
    }
    RestartFlag = false;

    int counter = 0;
    for (n = 0; n < LeftLinesCount; n++)
    {
        lx = LeftLinesX[n] + (x / ColMonoScale);
        ly = LeftLinesY[n] + (y / ColMonoScale);

        if (lx >= 0 && lx < (fwidth) && ly >= 0 && ly < (fheight))   //pixel is in range
        {
            rx1 = static_cast<qreal>(lx) * rcms;
            rx2 = rx1;
            ry1 = static_cast<qreal>(ly) * rcms;
            ry2 = (static_cast<qreal>(ly) + 1) * rcms;
            if (NoRecreateFlag)
            {
                LineList[counter]->setLine (rx1, ry1, rx2, ry2);
                LineList[counter++]->setVisible(true);
            }
            else
            {
                temp = scene->addLine( rx1, ry1, rx2, ry2, MyPen);
                temp->setZValue(1);
                LineList.append(temp);
            }
        }
    }

    for (n = 0; n < RightLinesCount; n++)
    {
        lx = RightLinesX[n] + (x / ColMonoScale);
        ly = RightLinesY[n] + (y / ColMonoScale);

        if (lx >= 0 && lx < (fwidth) && ly >= 0 && ly < (fheight))   //pixel is in range
        {
            rx1 = (static_cast<qreal>(lx) + 1) * rcms;
            rx2 = rx1;
            ry1 = static_cast<qreal>(ly) * rcms;
            ry2 = (static_cast<qreal>(ly) + 1) * rcms;
            if (NoRecreateFlag)
            {
                LineList[counter]->setLine (rx1, ry1, rx2, ry2);
                LineList[counter++]->setVisible(true);
            }
            else
            {
                temp = scene->addLine( rx1, ry1, rx2, ry2, MyPen);
                temp->setZValue(1);
                LineList.append(temp);
            }
        }
    }

    for (n = 0; n < BottomLinesCount; n++)
    {
        lx = BottomLinesX[n] + (x / ColMonoScale);
        ly = BottomLinesY[n] + (y / ColMonoScale);

        if (lx >= 0 && lx < (fwidth) && ly >= 0 && ly < (fheight))   //pixel is in range
        {
            rx1 = static_cast<qreal>(lx) * rcms;
            rx2 = (static_cast<qreal>(lx) + 1) * rcms;
            ry1 = (static_cast<qreal>(ly) + 1) * rcms;
            ry2 = ry1;
            if (NoRecreateFlag)
            {
                LineList[counter]->setLine (rx1, ry1, rx2, ry2);
                LineList[counter++]->setVisible(true);
            }
            else
            {
                temp = scene->addLine( rx1, ry1, rx2, ry2, MyPen);
                temp->setZValue(1);
                LineList.append(temp);
            }
        }
    }

    for (n = 0; n < TopLinesCount; n++)
    {
        lx = TopLinesX[n] + (x / ColMonoScale);
        ly = TopLinesY[n] + (y / ColMonoScale);

        if (lx >= 0 && lx < (fwidth) && ly >= 0 && ly < (fheight))   //pixel is in range
        {
            rx1 = static_cast<qreal>(lx) * rcms;
            rx2 = (static_cast<qreal>(lx) + 1) * rcms;
            ry1 = (static_cast<qreal>(ly)) * rcms;
            ry2 = ry1;
            if (NoRecreateFlag)
            {
                LineList[counter]->setLine (rx1, ry1, rx2, ry2);
                LineList[counter++]->setVisible(true);
            }
            else
            {
                temp = scene->addLine( rx1, ry1, rx2, ry2, MyPen);
                temp->setZValue(1);
                LineList.append(temp);
            }
        }
    }

    return true;

}

void Brush_class::brighten(int x, int y, int segment, int effect)
{
    //qDebug()<<x<<y<<segment<<effect;
    int n;
    int soft_effect = 10 - mainwin->SpinSoft->value();
    //work out real values for this
    int soft_start = static_cast<int>((static_cast<double>(soft_effect)   * static_cast<double>(Radius) / 10.0));
    //soft_start is distance out if pixels for softing to start
    uchar *data;
    int pos;
    int val;
    int xoff, yoff;
    int ax, ay;
    int high, seg, temp;
    double divisor = static_cast<double>(Radius) - static_cast<double>(soft_start);
    int act_effect = effect; // won't get changed if no soft-brushing

    if (Segments[segment]->Locked) return;
    xoff = x / ColMonoScale;
    yoff = y / ColMonoScale;
    data = GA[segment]->bits();
    bool flag = false;
    for (n = 0; n < SegmentCount; n++) if (Segments[n]->Locked)flag = true;
    for (n = 0; n < PixelCount; n++)
    {
        ay = Ypos[n] + yoff;
        if (ay >= 0)
            if (ay < fheight)
            {
                ax = Xpos[n] + xoff;
                if (ax >= 0)
                    if (ax < fwidth)
                    {
                        pos = ay * fwidth + ax;
                        int pos4 = ay * fwidth4 + ax;
                        //work out distance

                        if (soft_effect != 10)
                        {
                            if (Dist[n] > soft_start) //Will be a soft_effect
                                act_effect = static_cast<int>(static_cast<double>(effect) * (static_cast<double>(divisor - (Dist[n] - soft_start))) / divisor);
                            else
                                act_effect = effect;
                        }

                        if (dirty.at(pos) < qAbs(act_effect))
                        {
                            if (flag)
                            {
                                //this stuff checks for locked segments - can't do brightening if there's a locked segment assigned to here!
                                high = 128;
                                seg = -1;
                                bool sflag = false;
                                for (int i = 0; i < SegmentCount; i++)
                                {
                                    if (Segments[i]->Activated)
                                    {
                                        temp = (int)  * ((GA[i]->bits()) + pos4);
                                        if (temp >= high)
                                        {
                                            high = temp;
                                            seg = i;
                                            sflag = Segments[seg]->Locked;
                                        }
                                    }
                                }
                                if (sflag) dirty[pos] = effect;
                                else
                                {
                                    val = static_cast<int>(data[pos4]);
                                    if (act_effect < 0)
                                    {
                                        val += act_effect + dirty.at(pos);
                                        if (val < 0) val = 0;
                                    }
                                    else
                                    {
                                        val += act_effect - dirty.at(pos);
                                        if (val > 255) val = 255;
                                    }
                                    data[pos4] = static_cast<uchar>(val);
                                    dirty[pos] = static_cast<uchar>(qAbs(act_effect));
                                }
                            }
                            else
                            {
                                val = static_cast<int>(data[pos4]);
                                if (act_effect < 0)
                                    val += act_effect + dirty.at(pos);
                                else
                                    val += act_effect - dirty.at(pos);
                                if (val > 255) val = 255;
                                else if (val < 0) val = 0;
                                data[pos4] = (uchar) val;
                                dirty[pos] = (uchar) qAbs(act_effect);
                            }
                        }
                    }
            }
    }
    Segments[CurrentSegment]->Dirty = true;
    Segments[CurrentSegment]->UndoDirty = true;

    //qDebug()<<"Dirtied file"<<CurrentFile;
    //FilesDirty[CurrentFile]=true;
}

void Brush_class::recalc(int x, int y, int segment)
{
    int n;
    uchar *data;
    int pos;
    int xoff, yoff;
    int ax, ay;

    if (Segments[segment]->Locked) return;
    xoff = x / ColMonoScale;
    yoff = y / ColMonoScale;
    data = GA[segment]->bits();
    //bool flag=false; - don't do this exception for recalc
    //for (n=0; n<SegmentCount; n++) if (Segments[n]->Locked) flag=true;
    for (n = 0; n < PixelCount; n++)
    {
        ay = Ypos[n] + yoff;
        if (ay >= 0)
            if (ay < fheight)
            {
                ax = Xpos[n] + xoff;
                if (ax >= 0)
                    if (ax < fwidth)
                    {
                        pos = ay * fwidth + ax;
                        int pos4 = ay * fwidth4 + ax;
                        data[pos4] = GenPixel(ax, ay, segment);

                        dirty[pos] = 1;
                    }
            }
    }
    Segments[segment]->Dirty = true;
    Segments[segment]->UndoDirty = true;
    //FilesDirty[CurrentFile]=true;
}


void Brush_class::mask(int x, int y, int mask)
{
    int n;
    uchar *data;
    int pos;
    int xoff, yoff;
    int ax, ay;
    uchar m = static_cast<uchar>(mask);
    uchar tmp;

    xoff = x / ColMonoScale;
    yoff = y / ColMonoScale;
    data = (uchar *) Masks.data();
    for (n = 0; n < PixelCount; n++)
    {
        ay = Ypos[n] + yoff;
        if (ay >= 0)
            if (ay < fheight)
            {
                ax = Xpos[n] + xoff;
                if (ax >= 0)
                    if (ax < fwidth)
                    {
                        pos = ((fheight - ay - 1) * fwidth + ax);
                        tmp = data[pos];
                        if (tmp <= MaxUsedMask) if ((MasksSettings[(int)tmp]->Lock) == false) data[pos] = m;
                    }
            }
    }
    MasksDirty = true;
    MasksUndoDirty = true;
    //FilesDirty[CurrentFile]=true;
}


void Brush_class::lock(int x, int y, int effect)
{
    int n;
    uchar *data;
    int pos;
    int xoff, yoff;
    int ax, ay;

    xoff = x / ColMonoScale;
    yoff = y / ColMonoScale;
    data = (uchar *) Locks.data();

    for (n = 0; n < PixelCount; n++)
    {
        ay = Ypos[n] + yoff;
        if (ay >= 0)
            if (ay < fheight)
            {
                ax = Xpos[n] + xoff;
                if (ax >= 0)
                    if (ax < fwidth)
                    {
                        pos = ((fheight - ay - 1) * fwidth + ax) * 2;
                        if (effect) data[pos] = 255;
                        else data[pos] = 0;
                    }
            }
    }
    LocksDirty = true;
    LocksUndoDirty = true;
}


void Brush_class::segment(int x, int y, int effect)
{
    int n, i, smax;
    uchar max;
    int pos;
    int xoff, yoff;
    int ax, ay;
    uchar tmp;
    uchar *mdata = (uchar *) Masks.data(); //for seg applies mask case
    int m, s; //will be mask to apply
    QList <uchar *> data;

    for (n = 0; n < SegmentCount; n++) data.append(GA[n]->bits()); //get pointers to grey arrays

    if (effect) m = SelectedMask;
    else m = SelectedRMask;
    xoff = x / ColMonoScale;
    yoff = y / ColMonoScale;

    if (effect) s = CurrentSegment;
    else s = CurrentRSegment;

    for (i = 0; i < PixelCount; i++)
    {
        ay = Ypos[i] + yoff;
        if (ay >= 0)
            if (ay < fheight)
            {
                ax = Xpos[i] + xoff;
                if (ax >= 0)
                    if (ax < fwidth)
                    {
                        if (SegmentBrushAppliesMasks)
                        {
                            pos = ((fheight - ay - 1) * fwidth + ax);
                            tmp = mdata[pos];
                            if (tmp <= MaxUsedMask) if ((MasksSettings[static_cast<int>(tmp)]->Lock) == false) mdata[pos] = static_cast<uchar>(m);
                        }
                        //now do actual segmentation effect

                        if (s == -2)   //delete mode
                        {
                            pos = (ay * fwidth4 + ax);
                            for (n = 0; n < SegmentCount; n++) if (!(Segments[n]->Locked))(data[n])[pos] = 0; // all segments blank
                        }
                        else     //not delete - normal segment mode
                        {
                            pos = (ay * fwidth4 + ax);
                            max = 128;
                            smax = 0;
                            bool flag = false;
                            for (n = 0; n < SegmentCount; n++)
                                if (Segments[n]->Activated)
                                {
                                    if ((data[n])[pos] >= max)
                                    {
                                        smax = n + 1;
                                        max = (data[n])[pos];
                                        flag = Segments[n]->Locked;
                                    }
                                }
                            //max now has maximum, smax the segment (+1 - 0 means none), flag is true if our max seg is locked
                            if (!flag)
                            {
                                if (!(Segments[s]->Locked))
                                {
                                    if (smax == 0)
                                    {
                                        (data[s])[pos] = static_cast<uchar>(255);
                                    }
                                    else
                                    {
                                        for (n = 0; n < SegmentCount; n++) if ((data[n])[pos] == 255) if (!(Segments[n]->Locked)) (data[n])[pos] = static_cast<uchar>(254); // all segments capped at 254
                                        (data[s])[pos] = static_cast<uchar>(255);
                                    }
                                }
                            }
                        }
                    }
            }
    }
    foreach (Segment *s, Segments)
    {
        s->Dirty = true;
        s->UndoDirty = true;
    }
    if (SegmentBrushAppliesMasks)   MasksDirty = true;
    //FilesDirty[CurrentFile]=true;
}

