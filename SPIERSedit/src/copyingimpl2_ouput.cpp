/**
 * @file
 * Source: CopyingImpl 2
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

#include "copyingimpl.h"
#include "globals.h"
#include "fileio.h"
#include "display.h"
#include "output.h"
#include "curves.h"
#include "brush.h"

#include <QProcess>
#include <QMutexLocker>
#include <QVarLengthArray>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QEventLoop>
#include <QCloseEvent>
#include <QtGlobal>
#include <QListWidget>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>


void CopyingImpl::WriteSPVData(int FullOutArrayCount, QByteArray FullOutArray, QVector<double> *TrigArray, int TrigCount, QDataStream *out)
//output data to the file
{

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    (*out) << FullOutArrayCount; //commence with number of fullarrays to decompress - though should know this already
    out->writeRawData(FullOutArray.constData(), FullOutArray.size());

    //and output any triangles
    (*out) << TrigCount;
    for (int z = 0; z < TrigCount; z++)
        (*out) << (*TrigArray)[z];
}

void CopyingImpl::escape()
{
    escapeFlag = true;
}


void CopyingImpl::MakeMergeObject(int *FullOutArrayCount, QByteArray *FullOutArray, QVector <double> *TrigArray, int *TrigCount, int parent, long long int asize, int filesused, int awidth,
                                  int aheight, QDataStream *out, QList <double> *stretches)
{
//A cut down version of the output recursive function, putting all daughter objects together into one massive lump
    QList <bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++) //this can probably be an infinite loop - should normally get out in middle - but leave!
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < OutputObjectsCount; j++)
        {
            if (OutputObjects[j]->ListOrder < lowestval && usedflags[j] == false && OutputObjects[j]->Parent == parent)
            {
                lowestval = OutputObjects[j]->ListOrder;
                lowestindex = j;
            }
        }
        if (lowestindex == -1) //didn't find any- get out
            return;
        int i = lowestindex;
        usedflags[i] = true;

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        if (OutputObjects[i]->Show)
        {
            // for each object to be output;
            if (OutputObjects[i]->IsGroup) //merged or not!
            {
                if (DoIHaveChildren(i))
                {
                    MakeMergeObject(FullOutArrayCount, FullOutArray, TrigArray, TrigCount, i,
                                    asize,  filesused,  awidth,  aheight,  out, stretches);
                }
            }
            else
            {
                //normal obj
                //call function to make the array
                PopulateOutputArray (FullOutArrayCount, FullOutArray, &Count, FirstOutputFile, LastOutputFile, i, awidth, aheight, label);
                if (out != nullptr) PopulateTriangleList (i, FirstOutputFile, LastOutputFile, stretches, OutputObjects[i]->Resample, TrigArray, TrigCount);
            }
        }
    }
}

void CopyingImpl::MakeMergeObject_2(int parent, OutputObject *target)
{
    //doesn't need the ordering but does no harmn, leave
    //A cut down version of the output recursive function, putting all daughter objects together into one massive lump
    QList <bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++) //this can probably be an infinite loop - should normally get out in middle - but leave!
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < OutputObjectsCount; j++)
        {
            if (OutputObjects[j]->ListOrder < lowestval && usedflags[j] == false && OutputObjects[j]->Parent == parent)
            {
                lowestval = OutputObjects[j]->ListOrder;
                lowestindex = j;
            }
        }
        if (lowestindex == -1) //didn't find any- get out
            return;
        int i = lowestindex;
        usedflags[i] = true;

        if (OutputObjects[i]->Show)
        {
            // for each object to be output;
            if (OutputObjects[i]->IsGroup) //merged or not!
            {
                if (DoIHaveChildren(i))  MakeMergeObject_2(i, target);
            }
            else
                target->MergeObjects.append(OutputObjects[i]);
        }
    }
}

void CopyingImpl::DoOutputRecursive(QList <int> *translationtable, int *nexttransentry, QList <int> *translationtable2, int *nexttransentry2, int parent, long long int asize, int filesused,
                                    int awidth, int aheight, QDataStream *out, QList <double> *stretches)
{
    //now work through output items in display order, noting merges and groups

    //Summary of current algorithm

    //Loop round output objects in ListOrder
    //For each:
    //Update progress bar
    //If it's a group:
    //Do If it's not a merged group, call this function on my children
    //If it IS a merged group, and has some children -
    //Make a uncompressed array big enough to hold everything (files*height*width)
    //Set up compressed buffer too - a qbytearray
    //Run MakeMergeObject [come back to this!]
    //Output the data
    //Otherwise:
    //Set up compressed buffer too - a qbytearray
    //Run PopulateOutputArray function (in output.cpp)
    //Loop round all files
    //Next Job - understand this!
    //Write the compressed data
    //Sort out translation entries

    QList <bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++) //this can probably be an infinite loop - should normally get out in middle - but leave!
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < OutputObjectsCount; j++)
        {
            if (OutputObjects[j]->ListOrder < lowestval && usedflags[j] == false && OutputObjects[j]->Parent == parent)
            {
                lowestval = OutputObjects[j]->ListOrder;
                lowestindex = j;
            }
        }
        if (lowestindex == -1) //didn't find any- get out
            return;
        int i = lowestindex;
        usedflags[i] = true;

        progressBar->setValue((*nexttransentry) + 1);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        if (OutputObjects[i]->Show)
        {
            // for each object to be output;
            if (OutputObjects[i]->IsGroup)
            {
                if  (OutputObjects[i]->Merge) //merged group - all children merged into this one object
                {
                    if (DoIHaveChildren(i))
                    {
                        asize = static_cast<long long int>(filesused ) * static_cast<long long int>(awidth) * static_cast<long long int>(aheight); //this is size of output buffer
                        QByteArray FullOutArray; //initialised - yes, we need this!
                        int FullOutArrayCount = 0;
                        QVector <double> TrigArray;
                        int TrigCount = 0;

                        if (out == nullptr) Count = 0;
                        else Count = -1;
                        MakeMergeObject(&FullOutArrayCount, &FullOutArray, &TrigArray, &TrigCount, i, asize,  filesused,  awidth,  aheight,  out, stretches);

                        //do trans table entry
                        (*translationtable)[(*nexttransentry)] = i;
                        (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;

                        if (out == nullptr)
                        {
                            QTextStream t(&CountMessage);
                            if (CountMessage.length() == 0) t << "Total voxels in volume: " << asize << "\n";
                            t << OutputObjects[i]->Name << ": " << Count << " voxels (" << (static_cast<double>((static_cast<long long int>(Count) * static_cast<long long int>(100000)) / asize )) / 1000 << "%)\n";
                        }
                        else
                        {
                            WriteSPVData(FullOutArrayCount, FullOutArray, &TrigArray, TrigCount, out);
                            //qDeleteAll(fullarray.begin(), fullarray.end()); fullarray.clear();
                        }
                    }
                }
                //otherwise just call this function on the children
                else
                {
                    if (DoIHaveChildren(i))
                    {
                        (*translationtable)[(*nexttransentry)++] = i;
                        DoOutputRecursive(translationtable, nexttransentry, translationtable2, nexttransentry2, i,
                                          asize,  filesused,  awidth,  aheight,  out, stretches);
                    }
                }
            }
            else
            {
                //normal object
                //redim inside to clear it - + 2 is for the end caps
                asize = filesused * awidth * aheight; //this is size of output buffer
                QByteArray FullOutArray;

                int FullOutArrayCount = 0;
                //call function to make the array
                if (out == nullptr) Count = 0;
                else Count = -1;
                //qDebug()<<"about to POA";

                PopulateOutputArray (&FullOutArrayCount, &FullOutArray, &Count, FirstOutputFile, LastOutputFile, i, awidth, aheight, label);

                //qDebug()<<"done POA\n";
                QVector <double> TrigArray;
                int TrigCount = 0;
                if (out != nullptr)
                {
                    PopulateTriangleList (i, FirstOutputFile, LastOutputFile, stretches, OutputObjects[i]->Resample, &TrigArray, &TrigCount);
                }

                if (out == nullptr)
                {
                    QTextStream t(&CountMessage);
                    if (CountMessage.length() == 0) t << "Total voxels in volume: " << asize << "\n";
                    t << OutputObjects[i]->Name << ": " << Count << " voxels (" << (static_cast<double>((static_cast<long long int>(Count) * static_cast<long long int>(100000)) / asize )) / 1000 << "%)\n";
                }
                else
                {
                    WriteSPVData(FullOutArrayCount, FullOutArray, &TrigArray, TrigCount, out);
                    //qDeleteAll(fullarray.begin(), fullarray.end()); fullarray.clear();
                }

                //do trans table entry
                (*translationtable)[(*nexttransentry)] = i;
                (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;
            }
        }
    }
}

void CopyingImpl::GetOutputList(QList <OutputObject *> *outlist, QList <int> *translationtable, int *nexttransentry, QList <int> *translationtable2, int *nexttransentry2, int parent)
{
    //Based on old DoOutputRecursive - loops round all objects building tables for output basically,
    QList <bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++) //this can probably be an infinite loop - should normally get out in middle - but leave!
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < OutputObjectsCount; j++)
        {
            if (OutputObjects[j]->ListOrder < lowestval && usedflags[j] == false && OutputObjects[j]->Parent == parent)
            {
                lowestval = OutputObjects[j]->ListOrder;
                lowestindex = j;
            }
        }
        if (lowestindex == -1) //Didn't find any- get out
            return;
        int i = lowestindex;
        usedflags[i] = true;

        if (OutputObjects[i]->Show)
        {
            // for each object to be output;
            if (OutputObjects[i]->IsGroup)
            {
                if  (OutputObjects[i]->Merge) //merged group - all children merged into this one object
                {
                    if (DoIHaveChildren(i))
                    {
                        //do trans table entry
                        (*translationtable)[(*nexttransentry)] = i;
                        (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;
                        outlist->append(OutputObjects[i]);

                        OutputObjects[i]->MergeObjects.clear();
                        MakeMergeObject_2(i, OutputObjects[i]); //build the merge object list
                    }
                }
                //otherwise just call this function on the children
                else
                {
                    if (DoIHaveChildren(i))
                    {
                        (*translationtable)[(*nexttransentry)++] = i;
                        GetOutputList(outlist, translationtable, nexttransentry, translationtable2, nexttransentry2, i);
                    }
                }
            }
            else
            {
                //normal object
                //do trans table entry
                (*translationtable)[(*nexttransentry)] = i;
                (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;
                outlist->append(OutputObjects[i]);
            }
        }
    }
}

void CopyingImpl::StupidBodgyFunction(QList <int> *tt, int parent)
//Just don't ask
{
    QList <bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++) //this can probably be an infinite loop - should normally get out in middle - but leave!
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < OutputObjectsCount; j++)
        {
            if (OutputObjects[j]->ListOrder < lowestval && usedflags[j] == false && OutputObjects[j]->Parent == parent)
            {
                lowestval = OutputObjects[j]->ListOrder;
                lowestindex = j;
            }
        }
        if (lowestindex == -1) //didn't find any- get out
            return;
        int i = lowestindex;
        usedflags[i] = true;

        if (OutputObjects[i]->Show)
        {
            // for each object to be output;
            if (OutputObjects[i]->IsGroup)
            {
                if  (OutputObjects[i]->Merge) //merged group - all children merged into this one object
                    tt->append(i);
                else StupidBodgyFunction(tt, i);
            }
            else //normal object, can't be merged
                tt->append(i);
        }
    }
}

bool CopyingImpl::AmIMerged(int n)
{
    if (OutputObjects[n]->Parent == -1) return false;
    if (OutputObjects[OutputObjects[n]->Parent]->Merge == true) return true;
    return AmIMerged(OutputObjects[n]->Parent);
}

void CopyingImpl::MeasureVols()
//Adapted from ExportSPV - does same work, but doesn't export arrays, just counts contents
{
    //Export to SPIERSview
    //Adapted from old VB function
    //mutex.lock(); //don't even thinkk about background cacheing while I'm doing this!
    int n;
    long long int asize = 0;
    QString outputfile;
    QByteArray compressedbuffer;

    int flen, pos;
    Q_UNUSED(flen);
    Q_UNUSED(pos);
    int awidth, aheight;

    QList <int> translationtable1; //tt1[myobjectindex] = index of object in output list.

    StupidBodgyFunction(&translationtable1, -1); //Stupid function to get correct order for object data

    int ActualOutputObjectCount = translationtable1.count();
    if (ActualOutputObjectCount == 0)
    {
        Message("Nothing to Count!");
        return;
    }

    int filesused = LastOutputFile - FirstOutputFile + 1;


    pos = 0;
    flen = fwidth * fheight - 1;

    //work out reduced height/width
    aheight = (fheight - fheight % XYDownsample) / XYDownsample;
    awidth = (fwidth - fwidth % XYDownsample) / XYDownsample;
    filesused = (filesused - filesused % ZDownsample) / ZDownsample;


    show();
    this->setWindowTitle("Counting voxels in volume...");
    copying = true;

    WriteAllData(CurrentFile);

    WriteSettings();
    progressBar->setMaximum(ActualOutputObjectCount);

    QList <int> translationtable;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable.append(0);
    int nexttransentry = 0, nexttransentry2 = 0;
    QList <int> translationtable2;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable2.append(0);
    //these are only needed so I can use same functions as normal output

    QList <double> stretches;
    for (n = 0; n <= filesused + 1; n++) stretches.append(n);

    DoOutputRecursive(&translationtable, &nexttransentry, &translationtable2, &nexttransentry2, -1, asize,  filesused,  awidth,  aheight,  static_cast<QDataStream *>(nullptr), &stretches);

    LoadAllData(CurrentFile);
    copying = false;
    close();
    Message (CountMessage);
}

void CopyingImpl::ExportSPV(int flag)  //0 for export, 1 for export and launch, 2 for export to temp file and launch
{
    //Export to SPIERSview
    int n;
    long long int asize = 0;
    QString outputfile;

    QByteArray compressedbuffer;
    QList <double> stretches, stretches2;
    int flen, pos;
    Q_UNUSED(flen);
    int awidth, aheight;

    label->setFixedWidth(60);

    QList <int> translationtable1; //tt1[myobjectindex] = index of object in output list.

    StupidBodgyFunction(&translationtable1, -1); //Stupid function to get correct order for object data
    int ActualOutputObjectCount = translationtable1.count();
    if (ActualOutputObjectCount == 0)
    {
        Message("Nothing to Export!");
        return;
    }

    int filesused = LastOutputFile - FirstOutputFile + 1;

    //a default stretch array
    //for (n=0; n<=filesused+1; n++) stretches.append(n);
    stretches.append(0);
    if (FirstOutputFile == 0) stretches.append(0);
    else stretches.append(Stretches[FirstOutputFile - 1]);
    for (n = FirstOutputFile; n <= LastOutputFile; n++) stretches.append(Stretches[n]);
    //stretches.append(Stretches[n-1]+1);

    //now reverse stretches array if mirror is on
    if (OutputMirroring) ReverseStretches(&stretches, 0, filesused + 1);
    //qDebug()<<"2";
    if (flag < 2)
    {
        outputfile = QFileDialog::getSaveFileName(
                         nullptr,
                         "SPIERSview File Name",
                         "",
                         "SPIERSview files (*.spv)");
        /*
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setNameFilter("SPIERSview files (*.spv)");
        dialog.setViewMode(QFileDialog::Detail);

        QStringList files;
        if (dialog.exec())
            files = dialog.selectedFiles();
        else return;
        outputfile=files[0];
        */
    }
    else
        //temp file
    {
        //work out path
        int lastsep, lastdot;
        QString Fname = Files.at(0);
        lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
        lastdot = Fname.lastIndexOf(".");
        QString sfname = Fname.left(lastsep);
        QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
        outputfile = sfname + "/" + SettingsFileName + "/temp.spv";
        //Message("File is " + outputfile);
    }
    //outputfile="c:/test.spv";
    if (outputfile == "") return;
    QFile File(outputfile);

    pos = 0;
    flen = fwidth * fheight - 1;

    //work out reduced height/width
    aheight = (fheight - fheight % XYDownsample) / XYDownsample;
    awidth = (fwidth - fwidth % XYDownsample) / XYDownsample;
    filesused = (filesused - filesused % ZDownsample) / ZDownsample;

    if (ZDownsample == 1) stretches2 = stretches;
    else
    {
        stretches2.append(0);
        int spos = 0;
        int zpos = 0;
        double tot = 0;
        do
        {
            tot = stretches[spos++]; //actually a max
            zpos++;
            if (zpos == ZDownsample)
            {
                stretches2.append(tot);
                zpos = 0;
            };
        }
        while (spos < stretches.count());
    }
    //sort out downsampled stretches
    //stretches2.append(0); //so it DOES start at 1!
    //for (n=1; n<stretches.count(); n++)
    //{
    //    stretches2.append(stretches[(n - 1) * ZDownsample + 1] / ZDownsample);
    //}
    // stretches2=stretches;
    if (!File.open(QIODevice::WriteOnly))
    {
        Message("Sorry, can't open spv file for writing");
        return;
    }
    QDataStream out(&File);
    out.setVersion(QDataStream::Qt_4_3);
    out.setByteOrder(QDataStream::LittleEndian);


    out << static_cast<double>(-1); //start with -1 - historical reasons
    out << static_cast<int>(5); //file version - 4 was last one from VB, 3 is last Mac one.

    out << PixPerMM / (static_cast<double>(XYDownsample * ColMonoScale));
    out << SlicePerMM; // ((double) ZDownsample);
    out << SkewDown << SkewLeft;
    out << awidth << aheight;
    out << filesused + 2;
    out << ActualOutputObjectCount;

    //now some VB simulation - need to put out
    //Put #3, , OutKeys  'this is an array of   Public OutKeys(0 To 200) As Integer - i.e. 201 shorts
    //Put #3, , OutColours   'Public OutColours(0 To 200, 0 To 2) As Integer
    //Put #3, , OutResamples  'Public OutResamples(0 To 200) As Integer
    QByteArray OutKeys(402, 0);
    pos = 0;


    //OK, translationtable1 now has a list in order of the objects which will be output

    //int nextkey=1;
    for (n = 0; n < translationtable1.count(); n++)
        if (OutputObjects[translationtable1[n]]->Key != 0) OutKeys[(pos++ * 2)] = static_cast<char>(OutputObjects[translationtable1[n]]->Key);
        else  OutKeys[(pos++ * 2)] = 1;
    for (n = 0; n < 402; n++) out << static_cast<quint8>(OutKeys[n]);

    QByteArray OutColours(402 * 3, 0);
    for (int i = 0; i < 3; i++)
    {
        pos = 0;
        for (n = 0; n < translationtable1.count(); n++)
            OutColours[(pos++ * 2 + 402 * i)] = static_cast<char>(OutputObjects[translationtable1[n]]->Colour[i]);
    }

    for (n = 0; n < 402 * 3; n++) out << static_cast<quint8>(OutColours[n]);

    QByteArray OutResamples(402, 0);
    pos = 0;
    for (n = 0; n < translationtable1.count(); n++)OutResamples[(pos++ * 2)] = OutputObjects[translationtable1[n]]->Resample;
    for (n = 0; n < 402; n++) out << static_cast<quint8>(OutResamples[n]);

    //stretches
    //had a reverse here - I think it was superflous?

    for (n = 0; n <= (filesused + 1); n++)
        out << stretches2[n];

    show();
    this->setWindowTitle("Creating SPIERSview file...");
    copying = true;

    WriteAllData(CurrentFile);
    WriteSettings();
    progressBar->setMaximum(ActualOutputObjectCount);

    QList <int> translationtable;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable.append(0);
    int nexttransentry = 0, nexttransentry2 = 0;
    QList <int> translationtable2;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable2.append(0);
    //trans table is a conversion between my numbers and the index numbers for SPV output - i.e. translationtable[SPVnumber] gives my index number
    //trans table2 is a conversion between SPV index of objects and SPV phyiscal objects (not the same - some of the above may be groups)
    //the next entries are next number available in SPV numbers

    //qDebug()<<"3\n";
    DoOutputRecursive(&translationtable, &nexttransentry, &translationtable2, &nexttransentry2, -1,
                      asize,  filesused,  awidth,  aheight,  &out, &stretches2);
    //qDebug()<<"4\n";

    //now output the SV footer

    out << static_cast<int>(10000);
    out << nexttransentry; //count of objects including groups
    for (int i = 0; i < nexttransentry; i++)out << translationtable2[i];
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Parent == -1) out << static_cast<int>(-1);
        else out << translationtable.indexOf(OutputObjects[translationtable[i]]->Parent);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->IsGroup && OutputObjects[translationtable[i]]->Merge == false ) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);

    for (int i = 0; i < nexttransentry; i++)
        //these are the 'show groups' output - leave as per isgroup for now
        if (OutputObjects[translationtable[i]]->IsGroup) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);

    //keys
    //nextkey=1;
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Key == 0) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(OutputObjects[translationtable[i]]->Key);

    //names
    //nextkey=1;
    for (int i = 0; i < nexttransentry; i++)
    {
        QString s;
        if (OutputObjects[translationtable[i]]->Key != 0)
        {
            s.append(static_cast<uchar>(OutputObjects[translationtable[i]]->Key));
            s += " - ";
        }
        s += OutputObjects[translationtable[i]]->Name;
        out << s.size();
        out.writeRawData(s.toLatin1().constData(), s.size());
    }
    File.close();
    LoadAllData(CurrentFile);
    copying = false;
    close();
    //mutex.unlock();
    //launch external file - temp debug measure!
    qDebug() << "file:" + outputfile;

    if (flag > 0)
    {

#ifdef __APPLE__
        //qDebug()<<"file:" + outputfile;
        //QMessageBox::information(0,"","file:" + outputfile);
        QDesktopServices::openUrl(QUrl("file:" + outputfile, QUrl::TolerantMode));
        //QString program = qApp->applicationFilePath();
        //program.replace("edit2","view2");
        //QStringList arguments;
        //arguments << outputfile;
        //QMessageBox::warning(this,"",program);
        //QProcess::startDetached (program, arguments, qApp->applicationDirPath());
#endif

#ifndef __APPLE__
        QString program = "\"" + qApp->applicationDirPath() + "/SPIERSview2.exe" + "\"" + " \"" + outputfile + "\"";
        QProcess::startDetached(program);
#endif
    }
}



QByteArray CopyingImpl::ExpandGrid(QByteArray *grid, int awidth, int aheight)
{
    //Make sure that if we have a 255 anywhere that all 8 surrounding grid squares also do

    QByteArray NewGrid(grid->size(), '\0');

    //GRID
    int gxsize = (awidth / GRID_SCALE) + 1;
    int gysize = (aheight / GRID_SCALE) + 1;

    int Gpos = 0; //index into GridArray
    for (int j = 0; j < gysize; j++) //for each row of grid
        for (int i = 0; i < gxsize; i++)
        {
            if (grid->at(Gpos))
            {
                NewGrid[Gpos] = 255;
                //set all 8 surroundings in NewGrid
                if (i > 0)
                {
                    NewGrid[Gpos - 1] = 255;
                    if (j > 0) NewGrid[Gpos - 1 - gxsize] = 255;
                    if (j < (gysize - 1)) NewGrid[Gpos - 1 + gxsize] = 255;
                }
                if (i < (gxsize - 1))
                {
                    NewGrid[Gpos + 1] = 255;
                    if (j > 0) NewGrid[Gpos + 1 - gxsize] = 255;
                    if (j < (gysize - 1)) NewGrid[Gpos + 1 + gxsize] = 255;
                }
                if (j > 0) NewGrid[Gpos - gxsize] = 255;
                if (j < (gysize - 1)) NewGrid[Gpos + gxsize] = 255;

            }
            Gpos++;
        }

    //Dump old and new grids for debugging
    /*
        qDebug()<<"Old";
        Gpos=0;
        for (int j=0; j<gysize; j++)  //for each row of grid
        {
            QString o;
            for (int i=0; i<gxsize; i++)
            {
                if ((*grid)[Gpos++]) o.append("1"); else o.append("0");
            }
            qDebug()<<o;
        }


        qDebug()<<"New";
        Gpos=0;
        for (int j=0; j<gysize; j++)  //for each row of grid
        {
            QString o;
            for (int i=0; i<gxsize; i++)
            {
                if (NewGrid[Gpos++]) o.append("1"); else o.append("0");
            }
            qDebug()<<o;
        }
    */
    return NewGrid;
}


void CopyingImpl::ExportSPV_2(int flag)  //0 for export, 1 for export and launch, 2 for export to temp file and launch
//2 is new version that does optimised output with gridded table of no contents
{
    //Export to SPIERSview

    int n;
    QString outputfile;

    QByteArray compressedbuffer;
    QList <double> stretches, stretches2;
    int flen, pos;
    Q_UNUSED(flen);
    int awidth, aheight;

    escapeFlag = false;


    //qDebug()<<"isFileDirty"<<FilesDirty;

    label->setFixedWidth(60);

    QList <int> translationtable1; //tt1[myobjectindex] = index of object in output list.

    StupidBodgyFunction(&translationtable1, -1); //Stupid function to get correct order for object data

    int ActualOutputObjectCount = translationtable1.count();
    if (ActualOutputObjectCount == 0)
    {
        Message("Nothing to Export!");
        return;
    }

    int filesused = LastOutputFile - FirstOutputFile + 1;

    stretches.append(0);
    if (FirstOutputFile == 0) stretches.append(0);
    else stretches.append(Stretches[FirstOutputFile - 1]);
    for (n = FirstOutputFile; n <= LastOutputFile; n++) stretches.append(Stretches[n]);

    //now reverse stretches array if mirror is on
    if (OutputMirroring) ReverseStretches(&stretches, 0, filesused + 1);

    //Set up the SPV file
    //qDebug()<<"Here";
    if (flag < 2)
    {
        outputfile = QFileDialog::getSaveFileName(nullptr, "SPIERSview File Name", "", "SPIERSview files (*.spv)");
    }
    else //temp file
    {
        //work out path
        int lastsep, lastdot;
        QString Fname = Files.at(0);
        lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
        lastdot = Fname.lastIndexOf(".");
        QString sfname = Fname.left(lastsep);
        QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
        outputfile = sfname + "/" + SettingsFileName + "/temp.spv";
    }
    if (outputfile == "") return;
    QFile File(outputfile);

    pos = 0;
    flen = fwidth * fheight - 1;

    //work out reduced height/width
    aheight = (fheight - fheight % XYDownsample) / XYDownsample;
    awidth = (fwidth - fwidth % XYDownsample) / XYDownsample;
    filesused = (filesused - filesused % ZDownsample) / ZDownsample;

    if (ZDownsample == 1) stretches2 = stretches;
    else //fix stretches if zdownsampled
    {
        stretches2.append(0);
        int spos = 0;
        int zpos = 0;
        double tot = 0;
        do
        {
            tot = stretches[spos++]; //actually a max
            zpos++;
            if (zpos == ZDownsample)
            {
                stretches2.append(tot);
                zpos = 0;
            };
        }
        while (spos < stretches.count());

        //check have right number
        while (stretches2.count() < filesused + 2)
        {
            int n = stretches2.count();
            double diff = stretches2[n - 1] - stretches2[n - 2];
            stretches2.append(stretches2.last() + diff);
        }
    }


    //Start file, do headers
    if (!File.open(QIODevice::WriteOnly))
    {
        Message("Sorry, can't open spv file for writing");
        return;
    }
    QDataStream out(&File);
    out.setVersion(QDataStream::Qt_4_3);
    out.setByteOrder(QDataStream::LittleEndian);

    out << static_cast<double>(-1); //start with -1 - historical reasons
    out << static_cast<int>(SPVFILEVERSION); //file version
    //- 5 was first QT one, 4 was last one from VB, 3 is last Mac one. 6 includes grid/flag support.
    //Now - 1000+ are exports from edit only
    out << PixPerMM / (static_cast<double>(XYDownsample * ColMonoScale));
    out << SlicePerMM; // ((double) ZDownsample);
    out << SkewDown << SkewLeft;
    out << awidth << aheight;
    out << filesused + 2;
    out << ActualOutputObjectCount;

    QByteArray OutKeys(402, 0);
    pos = 0;

    //OK, translationtable1 now has a list in order of the objects which will be output
    for (n = 0; n < translationtable1.count(); n++)
        if (OutputObjects[translationtable1[n]]->Key != 0) OutKeys[(pos++ * 2)] = static_cast<char>(OutputObjects[translationtable1[n]]->Key);
        else  OutKeys[(pos++ * 2)] = 1;
    for (n = 0; n < 402; n++) out << static_cast<quint8>(OutKeys[n]);


    QByteArray OutColours(402 * 3, 0);
    for (int i = 0; i < 3; i++)
    {
        pos = 0;
        for (n = 0; n < translationtable1.count(); n++)
            OutColours[(pos++ * 2 + 402 * i)] = static_cast<uchar>(OutputObjects[translationtable1[n]]->Colour[i]);
    }
    for (n = 0; n < 402 * 3; n++) out << static_cast<quint8>(OutColours[n]);
    QByteArray OutResamples(402, 0);
    pos = 0;
    for (n = 0; n < translationtable1.count(); n++)
        OutResamples[(pos++ * 2)] = OutputObjects[translationtable1[n]]->Resample;
    for (n = 0; n < 402; n++) out << static_cast<quint8>(OutResamples[n]);
//stretches
//had a reverse here - I think it was superflous?

    for (n = 0; n <= (filesused + 1); n++)
        out << stretches2[n];

    //Save anything we are working on
    WriteAllData(CurrentFile);
    WriteSettings();
    //headers written - setup progress bar properly
    show();
    this->setWindowTitle("Creating SPIERSview file...");
    copying = true;

    //set up blank translation tables
    //trans table is a conversion between my numbers and the index numbers for SPV output - i.e. translationtable[SPVnumber] gives my index number
    //trans table2 is a conversion between SPV index of objects and SPV phyiscal objects (not the same - some of the above may be groups)
    QList <int> translationtable;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable.append(0);
    QList <int> translationtable2;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable2.append(0);
//the next entries are next number available in SPV numbers
    int nexttransentry = 0, nexttransentry2 = 0;

    QList <OutputObject *> outlist;

    GetOutputList(&outlist, &translationtable, &nexttransentry, &translationtable2, &nexttransentry2, -1); //replaces old do out recursive.
//outlist is list of the items to be output. Note that some might be merged groups!

//OK, now we get onto the new code!

//Reset PB to be based on the file count
    progressBar->setMaximum(filesused);
//First, set up lists of compressed arrays that we will be generating


//Now code originally from Populate Output Array

//some initialisation
    int f, bigpos = 0, m, max, temp;
    Q_UNUSED(max);
    int seg, i;
    int mi, loopfrom, loopto, loopstep, edgestart, fmax, tpos = 0;

    OutputObject *Object;

//Some pointers into objects in classes for speed
    QByteArray *temparray, *OutputArray;
    QList <bool> *UseMasks, *UseSegs;

//set up the arrays in the output objects appropriated. Will clear all these at end.
    foreach (Object, outlist)
    {
        if (Object->Merge) foreach (OutputObject * Object2, Object->MergeObjects) Object2->SetUpForRender();

        Object->SetUpForRender();
        Object->Outputarray.resize(awidth * aheight);
        //            qDebug()<<"Object name"<<Object->Name<<"MasksList"<<Object->UseMasks<<"SegList"<<Object->UseSegs<<"OA:"<<Object->Outputarray.size()<<fwidth<<fheight<<fwidth*fheight;
    }

    max = (awidth * aheight) - 1; //downsampled
    fmax = (fwidth * fheight) - 1;   //not downsampled

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

    int count = 0;

    for (f = loopfrom; freached(f, loopto, loopfrom); f += loopstep) //loop through all files
    {
//        qDebug()<<"Start loop";
        qApp->processEvents();
        if (escapeFlag)break;
        progressBar->setValue(count++);
        QString message;
        QTextStream ms(&message);
        ms << "File: " << f;
        label->setText(message);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        //Check - are all of these flagged as NOT dirty? if so we don't need to do anything!
        bool dflag = false;
        for (i = 0; i < ZDownsample; i++) if (FilesDirty[i + f]) dflag = true;


        if (dflag)
        {
            //Now loop round all the objects and clear their arrays as needbe - this is NOT for subobjects in a merge
            foreach (Object, outlist)
            {
                //Blank the temp array if needbe
                Object->bigpos = 0;
                Object->tpos = 0;
                if (XYDownsample * ZDownsample > 1)
                {
                    //we are downsampling, so need to blank the temp array
                    Object->temparray.fill(0, (fmax + 1) * ZDownsample);
                    Object->tpos = 0; //position in temp array
                }
                //Blank the output array
                Object->Outputarray.fill(0);
            }

            //Now can loop through each file if zdownsample>1 and build arrays for all object
            for (i = 0; i < ZDownsample; i++)
            {
                int usefile = f + i;
                if ((usefile) >= FileCount) usefile = FileCount - 1; //off end! - can't go off start I hope. Just repeat last file

                //Load the file data
                for (int j = 0; j < SegmentCount; j++) LoadGreyData(usefile, j);
                LoadMasks(usefile);

//                qDebug()<<"H3b";
                foreach (Object, outlist)
                {
                    //Merge handling. If object is a merge, need to do all the following for all subobjects. Otherwise just this one.

                    OutputArray = &(Object->Outputarray);
                    temparray = &(Object->temparray);

                    QList <OutputObject *> ObsToUse;
                    ObsToUse.clear(); //ensure empty
                    if (Object->Merge) ObsToUse = Object->MergeObjects; //copy the list from the object, build during first pass
                    else ObsToUse.append(Object); //list of one item if not a merge

                    foreach (OutputObject *Obj, ObsToUse)
                    {
                        //HERE - for each object do all the below - need to get all the pointers local etc for speed...
                        bigpos = Object->bigpos; //reset to start of parent object array
                        tpos = Object->tpos; //reset to start of parent object array
                        UseMasks = &(Obj->UseMasks);
                        UseSegs = &(Obj->UseSegs);

                        //int segcounts[9];
                        //for (int iii=0; iii<9; iii++) segcounts[iii]=0;

                        if (XYDownsample * ZDownsample == 1)
                        {
                            //no resample - build directly

                            //qDebug()<<"H3d";

                            //int countme=0, countme2=0;
                            for (int y = fheight - 1; y >= 0; y--)
                                for (int x = 0; x < fwidth; x++)
                                {
                                    //work out segment - not in a function for speed
                                    if (((*UseMasks)[static_cast<quint8>(Masks[y * fwidth + x])]))
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
                                        //segcounts[seg+1]++;
                                        if (seg != -1)
                                            if ((*UseSegs)[seg]) //seg in list?
                                                (*OutputArray)[bigpos] = static_cast<char>(255);
                                        //no need to enter a 0 - array is initialised to 0
                                    }
                                    bigpos++;
                                }

                            //Now curves/
                            for (int sn = 0; sn < CurveCount; sn++)
                            {
                                if (Curves[sn]->Segment != 0) //0 means not in any seg
                                {
                                    int segcv = Curves[sn]->Segment - 1;
                                    if (Segments[segcv]->Activated && (*UseSegs)[segcv])
                                        DrawCurveOutput(sn, usefile, (uchar *) OutputArray->data(), UseMasks, false);
                                    else
                                        //If curve is in a segment that makes it invisible - should REMOVE all the points!
                                        DrawCurveOutput(sn, usefile, (uchar *) OutputArray->data(), UseMasks, true);
                                }
                            }

                        }
                        else //Need to do resample, so work in temp array
                        {
                            mi = i * (fmax + 1);

                            for (int y = fheight - 1; y >= 0; y--)
                                for (int x = 0; x < fwidth; x++)
                                    //work out segment - not in a function for speed
                                {
                                    if (((*UseMasks)[static_cast<quint8>(Masks[y * fwidth + x])]))
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
                                            if ((*UseSegs)[seg]) //seg in list... is mask?
                                                (*temparray)[tpos] = static_cast<uchar>(255);
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
                                    if (Segments[segcv]->Activated && (*UseSegs)[segcv])
                                        DrawCurveOutput(sn, usefile, (uchar *) temparray->data() + mi, UseMasks, false);
                                    else
                                        //If curve is in a segment that makes it invisible - should REMOVE all the points!
                                        DrawCurveOutput(sn, usefile, (uchar *) temparray->data() + mi, UseMasks, true);
                                }
                            }


                        }

                        ;
                    }
                    //copy the pointers back
                    Object->bigpos = bigpos;
                    Object->tpos = tpos;
                }
                FilesDirty[f + i] = false;
            }

            foreach (Object, outlist)
            {
                //Do resampling, padding, compression, store compressed array
                OutputArray = &(Object->Outputarray);
                temparray = &(Object->temparray);

                //Resample it if needbe
                if (XYDownsample * ZDownsample > 1)
                {
                    for (n = 0; n < awidth; n++) for (m = 0; m < aheight; m++)
                            if (ResampleForLoRes(n * XYDownsample, m * XYDownsample, temparray)) (*OutputArray)[m * awidth + n] = 255;
                }

                //PADDING

                //top
                edgestart = 0;

                for (n = 0; n < awidth; n++)   (*OutputArray)[edgestart++] = 0;

                //sides
                for (n = 1; n < (aheight - 2); n++)
                {
                    (*OutputArray)[edgestart] = 0;
                    edgestart += awidth;
                    (*OutputArray)[edgestart - 1] = 0;
                }

                //bottom
                for (n = 0; n < awidth; n++)   (*OutputArray)[edgestart++] = 0;


                bool zflag = true;

                //GRID
                int gxsize = (awidth / GRID_SCALE) + 1;
                int gysize = (aheight / GRID_SCALE) + 1;
                QByteArray *GridArray = Object->GridArrays[f];

                GridArray->fill(0, gxsize * gysize);
                //qDebug()<<"Grid Size "<<Object->GridArrays[f]->size();
                int Gpos = 0; //index into GridArray
                for (m = 0; m < gysize; m++) //for each row of grid
                {
                    int ymax = m * GRID_SCALE + GRID_SCALE;
                    if (ymax > aheight) ymax = aheight; //height of cell (might be less than grid size at end)
                    for (n = 0; n < gxsize; n++) //for each column
                    {
                        int xmax = n * GRID_SCALE + GRID_SCALE;
                        if (xmax > awidth) xmax = awidth; //width of cell

                        //now within cell look at all pixels
                        for (int mm = m * GRID_SCALE; mm < ymax; mm++) //for each row
                        {
                            bigpos = mm * awidth + n * GRID_SCALE; //work out index into big array
                            for (int nn = n * GRID_SCALE; nn < xmax; nn++) //row each column
                                if ((*OutputArray)[bigpos++]) (*GridArray)[Gpos] = 255;
                        }
                        //break2:
                        Gpos++;
                    }

                }


                //Is whole grid array still 0?
                Gpos = 0;
                for (m = 0; m < gysize; m++) for (n = 0; n < gxsize; n++) if ((*GridArray)[Gpos++]) zflag = false;

                //COMPRESSION
                //QByteArray *comp1 = new QByteArray();

                if (!zflag)
                {

                    (*(Object->CompressedSPVarrays[f])) = qCompress((*OutputArray), 9);
                    //delete Object->CompressedSPVarrays[f];
                    //Object->CompressedSPVarrays[f]=t;
                    //qDebug()<<"Storing compression in "<<f<<"size is"<<Object->CompressedSPVarrays[f]->size();
                }

            }
        }
    }

    //restore settings for current file
    for (int j = 0; j < SegmentCount; j++) LoadGreyData(CurrentFile, j);
    LoadMasks(CurrentFile);

//Now write out all the data I've generated

    foreach (Object, outlist)
    {
        //format is:
        //CompressedArrayCount
        out << count; //commence with number of fullarrays to decompress

        //Then for every compressedarray, write a size then the data
        for (f = loopfrom; freached(f, loopto, loopfrom); f += loopstep) //loop through all files
            //foreach (QByteArray *carray, Object->CompressedSPVarrays)
        {
            QByteArray *carray = Object->CompressedSPVarrays[f];

            if (carray->isEmpty())
            {
                out << static_cast<int>(-1);
            }
            else
            {
                //qDebug()<<"Outputting a non-empty slice"<<f<<carray->size()-4;
                int outsize = carray->size() - 4;
                out << outsize;
                out.writeRawData(carray->constData() + 4, outsize);

                //qDebug()<<"Outputting grid"<<Object->GridArrays[f]->size();
                QByteArray OutputGrid = ExpandGrid(Object->GridArrays[f], awidth, aheight);


                out.writeRawData(OutputGrid.constData(), OutputGrid.size());
            }
        }
        //and output any triangles - which there won't be
        out << static_cast<int>(0);

    }

    //now output the SV footer
    out << static_cast<int>(10000);
    out << nexttransentry; //count of objects including groups
    for (int i = 0; i < nexttransentry; i++)
        out << translationtable2[i];
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Parent == -1) out << static_cast<int>(-1);
        else out << translationtable.indexOf(OutputObjects[translationtable[i]]->Parent);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->IsGroup && OutputObjects[translationtable[i]]->Merge == false ) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);

    for (int i = 0; i < nexttransentry; i++)
//these are the 'show groups' output - leave as per isgroup for now
        if (OutputObjects[translationtable[i]]->IsGroup) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);

    //keys
    //nextkey=1;
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Key == 0) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(OutputObjects[translationtable[i]]->Key);

    //names
    //nextkey=1;
    for (int i = 0; i < nexttransentry; i++)
    {
        QString s;
        if (OutputObjects[translationtable[i]]->Key != 0)
        {
            s.append(static_cast<uchar>(OutputObjects[translationtable[i]]->Key));
            s += " - ";
        }
        s += OutputObjects[translationtable[i]]->Name;
        out << s.size();
        out.writeRawData(s.toLatin1().constData(), s.size());
    }


    File.close();
    LoadAllData(CurrentFile);
    copying = false;

    close(); //done, close the window

    if (escapeFlag)
    {
        File.remove();
        return;
    }

    if (flag > 0) //launch viewer
    {
//RJG conditional defines to launch SPIERSview on different operating systems
#ifdef __APPLE__
        //qDebug()<<"file:" + outputfile;
        //QMessageBox::information(0,"","file:" + outputfile);
        //QDesktopServices::openUrl(QUrl("file:" + outputfile, QUrl::TolerantMode));

        //RJG - sorting luanch spiersview on MAC export.
        //Old code above - new approach below is same as Linux, and works just fine
        QString program = qApp->applicationFilePath();
        program.replace("/SPIERSedit", "/SPIERSview");
        QStringList arguments;
        arguments << outputfile;
        QProcess::startDetached (program, arguments, qApp->applicationDirPath());

#endif

#ifdef __linux__
        //RJG - sorting luanch spiersview on linux export.
        QString program = qApp->applicationFilePath();
        program.replace("/SPIERSedit", "/SPIERSview");
        QStringList arguments;
        arguments << outputfile;
        QProcess::startDetached (program, arguments, qApp->applicationDirPath());
#endif
//RJG - Windows launch SPIERSview
#ifdef _WIN64
        QString program = "\"" + qApp->applicationDirPath() + "/SPIERSview64.exe" + "\"" + " \"" + outputfile + "\"";
        QProcess::startDetached(program);
#endif
    }
}
