/**
 * @file
 * Source: Resample
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

#include "resampleimpl.h"
#include "globals.h"
#include "fileio.h"
#include "display.h"
#include "mainwindowimpl.h"
#include "brush.h"

#include <QMutexLocker>

resampleImpl::resampleImpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    QString t = label->text();
    QTextStream ts(&t);
    ts << ColMonoScale << ")";
    label->setText(t);

    t = label2->text();
    QTextStream ts2(&t);
    ts2 << zsparsity << ")";
    label2->setText(t);

    spinBox->setValue(ColMonoScale);
    SpinBoxZ->setValue(zsparsity);
}

int resampleImpl::ClosestTo(QString NewFileString)
{
    //find the closest entry in Files to this
    int i = FullFiles.indexOf(NewFileString);

    if (i == -1) Error("Error in ClosestTo");

    bool up = false;
    int posup = i;
    int posdown = i;
    do
    {
        if (up)
        {
            posup++;
            if (posup < FullFiles.count())
            {
                if (Files.indexOf(FullFiles[posup]) != -1) return posup;
            }
        }
        else
        {
            posdown--;
            if (posdown >= 0)
            {
                if (Files.indexOf(FullFiles[posdown]) != -1) return posdown;
            }
        }
        up = !up;
    }
    while (posup < FullFiles.count() || posdown >= 0);
    Error ("Fell off while loop in ClosestTo");
    return 0;
}
void resampleImpl::on_buttonBox_accepted()
{
    pausetimers = true;
    QMutexLocker locker(&mutex);
    QString mess;
    bool restartflag = false;
    //sort out zsparsity
    if (zsparsity != SpinBoxZ->value())
    {
        WriteAllData(CurrentFile);
        int oldzsparsity = zsparsity;
        zsparsity = SpinBoxZ->value();
        CurrentFile = (CurrentFile * oldzsparsity) / zsparsity;
        restartflag = true;

        QStringList NewFiles;
        if (zsparsity == 1) NewFiles = FullFiles;
        else if (zsparsity > 1)
        {
            NewFiles.clear();
            for (int i = 0; i < FullFiles.count(); i += zsparsity)
            {
                NewFiles.append(FullFiles[i]);
            }
            FileCount = Files.count();
        }

        progressBar->setMaximum(NewFiles.count());
        //Blank masks and locks - this is so generation works properly
        for (int i = 0; i < fheight * fwidth; i++)
            Masks[i] = 0;
        for (int i = 0; i < fheight * fwidth * 2; i++)
            Locks[i] = 0;

        for (int i = 0; i < NewFiles.count(); i++)
        {
//          qDebug()<<"Start loop"<<i;
            progressBar->setValue(i);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

//          qDebug()<<Files.indexOf(NewFiles[i]);
            if (Files.indexOf(NewFiles[i]) == -1)
            {
//              qDebug()<<"In";
                int c = ClosestTo(NewFiles[i]); //c is index of correct file in FullFiles

//              qDebug()<<"Checking file"<<i<<CheckBoxInterpolateCurves->isChecked();
                if (CheckBoxInterpolateCurves->isChecked())
                {

                    //OK, curves first. ONly task here is interpolation
                    for (int cu = 0; cu < CurveCount; cu++)
                    {
                        Curves[cu]->SplinePoints[i]->X = Curves[cu]->SplinePoints[c]->X;
                        Curves[cu]->SplinePoints[i]->Y = Curves[cu]->SplinePoints[c]->Y;
                        Curves[cu]->SplinePoints[i]->Count = Curves[cu]->SplinePoints[c]->Count;
                    }
                }
//              qDebug()<<"Done curves";
                //This NewFile is NOT currently in use
                if (CheckBoxInterpolate->isChecked())
                    //can do nothing otherwise - keep existing, or if no existing a blank will be created
                {
                    //Need  to copy both locks, masks and curves from the closest entry in Files

                    //work out lock name of FROM file
                    QString Fname = FullFiles.at(c);
                    int lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
                    int lastdot = Fname.lastIndexOf(".");
                    QString sfname = Fname.left(lastsep);
                    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
                    QString temp;
                    temp.append("/" + SettingsFileName + "/l_");
                    temp.append(actfn);
                    temp.append(".bmp");
                    sfname.append(temp);

                    QFile file(sfname);
                    if (file.exists() && file.size() > 0)
                    {
                        //There is a file to copy, so work out TO name
                        QString Fname2 = NewFiles[i];
                        int lastsep2 = qMax(Fname2.lastIndexOf("\\"), Fname2.lastIndexOf("/")); //this is last separator in path
                        int lastdot2 = Fname2.lastIndexOf(".");
                        QString sfname2 = Fname2.left(lastsep);
                        QString actfn2 = Fname2.mid(lastsep2 + 1, lastdot2 - lastsep2 - 1);
                        QString temp2;
                        temp2.append("/" + SettingsFileName + "/l_");
                        temp2.append(actfn2);
                        temp2.append(".bmp");
                        sfname2.append(temp2);

                        QFile file2(sfname2);
                        file2.remove(); //kill any existing lock file

                        file.copy(sfname2); //copy
                    }

                    //now repeat for masks
                    Fname = FullFiles.at(c);
                    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
                    lastdot = Fname.lastIndexOf(".");
                    sfname = Fname.left(lastsep);
                    actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
                    temp = "";
                    temp.append("/" + SettingsFileName + "/m_");
                    temp.append(actfn);
                    temp.append(".bmp");
                    sfname.append(temp);

                    QFile file3(sfname);
                    if (file3.exists() && file3.size() > 0)
                    {
                        //There is a file to copy, so work out TO name - can redeclare as in an IF
                        QString Fname2 = NewFiles[i];
                        int lastsep2 = qMax(Fname.lastIndexOf("\\"), Fname2.lastIndexOf("/")); //this is last separator in path
                        int lastdot2 = Fname2.lastIndexOf(".");
                        QString sfname2 = Fname2.left(lastsep);
                        QString actfn2 = Fname2.mid(lastsep2 + 1, lastdot2 - lastsep2 - 1);
                        QString temp2;
                        temp2.append("/" + SettingsFileName + "/m_");
                        temp2.append(actfn2);
                        temp2.append(".bmp");
                        sfname2.append(temp2);

                        QFile file2(sfname2);
                        file2.remove(); //kill any existing mask file

                        file3.copy(sfname2);    //copy
                    }
                }
                for (int seg = 0; seg < SegmentCount; seg++)
                {
//                  qDebug()<<"Making seg file";
                    int lastsep, lastdot;
                    QString Fname = NewFiles.at(i);
                    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
                    lastdot = Fname.lastIndexOf(".");
                    QString sfname = Fname.left(lastsep);
                    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
                    QString temp = "/" + SettingsFileName + "/" + "s";
                    QString t2;
                    t2.sprintf("%d_", seg + 1);
                    temp.append(t2);
                    temp.append(actfn);
                    sfname.append(temp);
                    if (QFile(sfname + ".bmp").exists()) sfname += ".bmp";
                    else if (QFile(sfname + ".png").exists()) sfname += ".png";
                    //Load it from disk

                    //Now we get bodgy!
                    QFile f(sfname);
                    if (!(f.exists()))
                    {
                        // bodging - make backups of current GA pointer, files list
//                      qDebug()<<"File"<<sfname;
                        QStringList OldFiles = Files;
                        Files = NewFiles;
                        QImage *backup = GA[seg];
                        //create NEW GA...
                        GA[seg] = new QImage(fwidth, fheight, QImage::Format_Indexed8);
                        QVector <QRgb> clut(256);
                        for (int ic = 0; ic < 256; ic++)
                            clut[ic] = qRgb(ic, ic, ic);
                        GA[seg]->setColorTable(clut);
                        bool OldRangeSelectedOnly = RangeSelectedOnly;
                        RangeSelectedOnly = true;

//                      qDebug()<<"About to load";
                        SimpleLoadColourData(Files[i]);

                        if (tabwidget->currentIndex() == 0) MakeLinearGreyScale(seg, i, true);
                        if (tabwidget->currentIndex() == 1) MakePolyGreyScale(seg, i, true);
                        if (tabwidget->currentIndex() == 2) MakeRangeGreyScale(seg, i, true);

                        SimpleSaveGreyData(FullFiles.indexOf(NewFiles[i]), seg, *GA[seg]);
//                      qDebug()<<"Saved";

                        //debodge!
                        delete GA[seg];
                        GA[seg] = backup;
                        Files = OldFiles;
                        RangeSelectedOnly = OldRangeSelectedOnly;
//                      qDebug()<<"Debodged";
                    }
                }
            }
        }
        WriteSettings();
        ResetFilesDirty();
    }

    if (ColMonoScale != spinBox->value())
    {

        if (spinBox->value() < ColMonoScale) mess = "This will increase reconstruction resolution, but reduce performance and increase disk-space usage. Are you sure?";
        if (spinBox->value() > ColMonoScale) mess =
                "This will decrease reconstruction resolution, but increase performance and decrease disk-space usage.\nResolution from downsampling is NOT recoverable by later upsampling. Are you sure you want to continue?";

        if (QMessageBox::question(this, "Resample files", mess, QMessageBox::Yes | QMessageBox::Cancel)
                == QMessageBox::Yes)
        {
            int OldColMonoScale = ColMonoScale;
            int oldfwidth = fwidth;
            int oldfwidth4 = fwidth4;
            int oldfheight = fheight;
            ColMonoScale = spinBox->value();
            fheight = cheight / ColMonoScale;
            fwidth = cwidth / ColMonoScale;
            if ((fwidth % 4) == 0) fwidth4 = fwidth; // no problem with width
            else fwidth4 = (fwidth / 4) * 4 + 4;
            uchar *data1, *data2;


            //Right, let's get on with it
            //Plan:
            //1. Save all data
            WriteAllData(CurrentFile);

            //2. Go through each lock, mask and greyscale file, load, resample, write back (in accordance with current compression settings)
            //Will first upsample to 1:1, then downsample if necessary again
            progressBar->setMaximum(Files.count());

            for (int i = 0; i < FullFiles.count(); i++)
            {
                progressBar->setValue(i);
                qApp->processEvents(QEventLoop::ExcludeUserInputEvents);


                //First up -Locks
                QByteArray locks;
                QByteArray biglocks(cwidth * cheight * 2, 0);
                QByteArray newlocks(fwidth * fheight * 2, 0);

                if (SimpleLoadLocks(i, oldfwidth * oldfheight * 2, &locks))
                {
                    //set up biglocks
                    if (OldColMonoScale == 1) biglocks = locks;
                    else
                    {
                        //upsampling needed
                        data1 = (uchar *) biglocks.data();
                        data2  = (uchar *) locks.data();
                        for (int x = 0; x < oldfwidth; x++)
                            for (int y = 0; y < oldfheight; y++)
                            {
                                for (int a = 0; a < OldColMonoScale; a++)
                                    for (int b = 0; b < OldColMonoScale; b++)
                                    {
                                        data1[2 * (x * OldColMonoScale + a + (y * OldColMonoScale + b)*cwidth)] = data2[2 * (y * oldfwidth + x)];
                                    }
                            }
                    }



                    if (ColMonoScale == 1) newlocks = biglocks;
                    else
                    {
                        //downsampling needed

                        //now downsample into newlocks
                        data1 = (uchar *) biglocks.data();
                        data2  = (uchar *) newlocks.data();
                        for (int x = 0; x < fwidth; x++)
                            for (int y = 0; y < fheight; y++)
                            {
                                int count = 0;
                                for (int a = 0; a < ColMonoScale; a++)
                                    for (int b = 0; b < ColMonoScale; b++)
                                    {
                                        if (data1[2 * (x * ColMonoScale + a + (y * ColMonoScale + b)*cwidth)]) count++;
                                    }
                                if (count >= (ColMonoScale * ColMonoScale) / 2) data2[x * fwidth + y] = 255;
                                else data2[x * fwidth + y] = 0;
                            }
                    }


                    SimpleSaveLocks(i, newlocks);
                }
                //Now masks

                QByteArray masks;
                QByteArray bigmasks(cwidth * cheight, 0);
                QByteArray newmasks(fwidth * fheight, 0);
                if (SimpleLoadMasks(i, oldfwidth * oldfheight, &masks))
                {
                    //set up biglocks
                    if (OldColMonoScale == 1) bigmasks = masks;
                    else
                    {
                        //upsampling needed
                        data1 = (uchar *) bigmasks.data();
                        data2  = (uchar *) masks.data();
                        for (int x = 0; x < oldfwidth; x++)
                            for (int y = 0; y < oldfheight; y++)
                            {
                                for (int a = 0; a < OldColMonoScale; a++)
                                    for (int b = 0; b < OldColMonoScale; b++)
                                    {
                                        data1[(x * OldColMonoScale + a + (y * OldColMonoScale + b)*cwidth)] = data2[(y * oldfwidth + x)];
                                    }
                            }
                    }


                    if (ColMonoScale == 1) newmasks = bigmasks;
                    else
                    {
                        //downsampling needed

                        //now downsample into newlocks
                        data1 = (uchar *) bigmasks.data();
                        data2  = (uchar *) newmasks.data();
                        int mcount[255]; //nice and simple!
                        for (int x = 0; x < fwidth; x++)
                            for (int y = 0; y < fheight; y++)
                            {
                                for (int j = 0; j <= MaxUsedMask; j++)
                                    mcount[j] = 0;

                                for (int a = 0; a < ColMonoScale; a++)
                                    for (int b = 0; b < ColMonoScale; b++)
                                    {
                                        mcount[data1[(x * ColMonoScale + a + (y * ColMonoScale + b)*cwidth)]]++;
                                    }

                                int best = -1;
                                int max = -1;
                                for (int j = 0; j <= MaxUsedMask; j++)
                                {
                                    if (mcount[j] > max)
                                    {
                                        max = mcount[j];
                                        best = j;
                                    }
                                }
                                //best is now mask to use - the most used in this square
                                data2[(y * fwidth + x)] = best;
                            }
                    }


                    SimpleSaveMasks(i, newmasks);
                }

                //Greyscales can be resampled using QImage resize?
                //Unfortunately they can't - can't handle edge padding properly - so need to do pix-by-pix
                for (int s = 0; s < SegmentCount; s++)
                {
                    QVector <QRgb> clut(256);
                    for (int ic = 0; ic < 256; ic++)
                        clut[ic] = qRgb(ic, ic, ic);

                    int bigwidth = oldfwidth * OldColMonoScale;
                    int bigwidth4;
                    if ((bigwidth % 4) == 0) bigwidth4 = bigwidth;
                    else bigwidth4 = (bigwidth / 4) * 4 + 4;
                    int bigheight = oldfheight * OldColMonoScale;


                    QImage greydataold(oldfwidth, oldfheight, QImage::Format_Indexed8);
                    if (SimpleLoadGreyData(i, s, &greydataold))
                    {
                        //Resample up to full size - this should be fine
                        //Actually seems that rescale is subtly bugged, so am writing my own
                        QImage greydata(oldfwidth * OldColMonoScale, oldfheight * OldColMonoScale, QImage::Format_Indexed8);
                        greydata.setColorTable(clut);

                        data1 = (uchar *) greydataold.bits();
                        data2 = (uchar *) greydata.bits();
                        for (int x = 0; x < oldfwidth * OldColMonoScale; x++)
                            for (int y = 0; y < oldfheight * OldColMonoScale; y++)
                            {
                                data2[x + y * bigwidth4] = data1[x / OldColMonoScale + (y / OldColMonoScale) * oldfwidth4];
                            }

                        //greydata=greydata.scaled(oldfwidth*OldColMonoScale, oldfheight*OldColMonoScale);//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);


                        //Do resample even if new scale is 1 - will add the 0's to pad on edge!
                        //set up target image
                        QImage greydata2(fwidth, fheight, QImage::Format_Indexed8);

                        //give it a clut
                        greydata2.setColorTable(clut);


                        //now do resample bearing in mind possible overflows and all my *4 problems! May get messy.
                        data1 = (uchar *) greydata.bits();
                        data2  = (uchar *) greydata2.bits();
                        for (int x = 0; x < fwidth; x++)
                            for (int y = 0; y < fheight; y++)
                            {
                                int tot = 0;

                                for (int a = 0; a < ColMonoScale; a++)
                                    for (int b = 0; b < ColMonoScale; b++)
                                    {
                                        if (x < bigwidth && y < bigheight) tot += (uchar)data1[(x * ColMonoScale + a + (y * ColMonoScale + b) * bigwidth4)];
                                    }

                                tot /= (ColMonoScale * ColMonoScale);

                                data2[(y * fwidth4 + x)] = (uchar) tot;
                            }
                        SimpleSaveGreyData(i, s, greydata2);
                    }
                }
            }

            //2b. Do I need to redo curves? Yes!
            double factor = static_cast<double>(OldColMonoScale) / static_cast<double>(ColMonoScale);
            for (int i = 0; i < FullFiles.count(); i++)
                for (int j = 0; j < CurveCount; j++)
                {
                    PointList *p = Curves[j]->SplinePoints[i];

                    for (int k = 0; k < p->Count; k++)
                    {
                        p->X[k] *= factor;
                        p->Y[k] *= factor;
                    }
                }

            //3. Write my settings to disk - will have new ColMonoScale, fwidth, fheight
            WriteSettings();

            //4. Reload the dataset - do same as recent file, which is:
            restartflag = true;
            ResetFilesDirty();
        }
    }

    if (restartflag)
    {
        close();
        ReadSettings();
        Active = false;
        Brush.Brush_Flag_Restart();
        ClearImages();
        mainwin->SetUpGUIFromSettings();
        mainwin->Start();
    }

    pausetimers = false;
}

void resampleImpl::on_buttonBox_rejected()
{
    close();
}

