/**
 * @file
 * Source: ExportSPV
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "exportspv.h"
#include "globals.h"
#include "fileio.h"
#include "display.h"
#include "output.h"
#include "curves.h"
#include "brush.h"

#include <QtConcurrent/QtConcurrentMap>
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
#include <QShortcut>

// ============================================================
// Construction / dialog boilerplate
// ============================================================

ExportSPV::ExportSPV(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));
    CountMessage = "";
    copying = false;

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escape()));
}

void ExportSPV::closeEvent(QCloseEvent *event)
{
    if (copying)
        escapeFlag = true;
    else
        event->accept();
}

void ExportSPV::escape()
{
    escapeFlag = true;
}

// ============================================================
// Internal helpers shared by Run() and RunLegacy()
// ============================================================

void ExportSPV::WriteSPVData(int FullOutArrayCount, QByteArray FullOutArray, QVector<double> *TrigArray, int TrigCount, QDataStream *out)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    (*out) << FullOutArrayCount;
    out->writeRawData(FullOutArray.constData(), FullOutArray.size());

    (*out) << TrigCount;
    for (int z = 0; z < TrigCount; z++)
        (*out) << (*TrigArray)[z];
}

void ExportSPV::ReverseStretches(QList<double> *stretches, int Sstart, int Sstop)
{
    int n;
    QVector<double> thicks;
    thicks.resize(Sstop + 1);

    for (n = Sstart + 2; n <= Sstop; n++)
        thicks[n] = (*stretches)[n] - (*stretches)[n - 1];

    for (n = Sstart + 2; n <= Sstop; n++)
        (*stretches)[n] = (*stretches)[n - 1] + thicks[Sstop - (n - 2)];
}

bool ExportSPV::DoIHaveChildren(int parent)
{
    for (int i = 0; i < OutputObjectsCount; i++)
        if (OutputObjects[i]->IsGroup == false && OutputObjects[i]->Parent == parent && OutputObjects[i]->Show) return true;
        else
        {
            if (OutputObjects[i]->IsGroup && OutputObjects[i]->Parent == parent && OutputObjects[i]->Show) if (DoIHaveChildren(i)) return true;
        }
    return false;
}

bool ExportSPV::AmIMerged(int n)
{
    if (OutputObjects[n]->Parent == -1) return false;
    if (OutputObjects[OutputObjects[n]->Parent]->Merge == true) return true;
    return AmIMerged(OutputObjects[n]->Parent);
}

void ExportSPV::StupidBodgyFunction(QList<int> *tt, int parent)
{
    QList<bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++)
    {
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
        if (lowestindex == -1) return;
        int i = lowestindex;
        usedflags[i] = true;

        if (OutputObjects[i]->Show)
        {
            if (OutputObjects[i]->IsGroup)
            {
                if (OutputObjects[i]->Merge) tt->append(i);
                else StupidBodgyFunction(tt, i);
            }
            else
                tt->append(i);
        }
    }
}

QByteArray ExportSPV::ExpandGrid(QByteArray *grid, int awidth, int aheight)
{
    QByteArray NewGrid(grid->size(), '\0');

    int gxsize = (awidth / GRID_SCALE) + 1;
    int gysize = (aheight / GRID_SCALE) + 1;

    int Gpos = 0;
    for (int j = 0; j < gysize; j++)
        for (int i = 0; i < gxsize; i++)
        {
            if (grid->at(Gpos))
            {
                NewGrid[Gpos] = 255u;
                if (i > 0)
                {
                    NewGrid[Gpos - 1] = 255u;
                    if (j > 0) NewGrid[Gpos - 1 - gxsize] = 255u;
                    if (j < (gysize - 1)) NewGrid[Gpos - 1 + gxsize] = 255u;
                }
                if (i < (gxsize - 1))
                {
                    NewGrid[Gpos + 1] = 255u;
                    if (j > 0) NewGrid[Gpos + 1 - gxsize] = 255u;
                    if (j < (gysize - 1)) NewGrid[Gpos + 1 + gxsize] = 255u;
                }
                if (j > 0) NewGrid[Gpos - gxsize] = 255u;
                if (j < (gysize - 1)) NewGrid[Gpos + gxsize] = 255u;
            }
            Gpos++;
        }
    return NewGrid;
}

// ============================================================
// Legacy path helpers (used by RunLegacy / MeasureVols)
// ============================================================

void ExportSPV::MakeMergeObject(int *FullOutArrayCount, QByteArray *FullOutArray, QVector<double> *TrigArray, int *TrigCount, int parent, long long int asize, int filesused, int awidth,
                                int aheight, QDataStream *out, QList<double> *stretches)
{
    QList<bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++)
    {
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
        if (lowestindex == -1) return;
        int i = lowestindex;
        usedflags[i] = true;

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        if (OutputObjects[i]->Show)
        {
            if (OutputObjects[i]->IsGroup)
            {
                if (DoIHaveChildren(i))
                {
                    MakeMergeObject(FullOutArrayCount, FullOutArray, TrigArray, TrigCount, i,
                                    asize, filesused, awidth, aheight, out, stretches);
                }
            }
            else
            {
                PopulateOutputArray(FullOutArrayCount, FullOutArray, &Count, FirstOutputFile, LastOutputFile, i, awidth, aheight, label);
                if (out != nullptr) PopulateTriangleList(i, FirstOutputFile, LastOutputFile, stretches, OutputObjects[i]->Resample, TrigArray, TrigCount);
            }
        }
    }
}

void ExportSPV::MakeMergeObject_2(int parent, OutputObject *target)
{
    QList<bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++)
    {
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
        if (lowestindex == -1) return;
        int i = lowestindex;
        usedflags[i] = true;

        if (OutputObjects[i]->Show)
        {
            if (OutputObjects[i]->IsGroup)
            {
                if (DoIHaveChildren(i)) MakeMergeObject_2(i, target);
            }
            else
                target->MergeObjects.append(OutputObjects[i]);
        }
    }
}

void ExportSPV::DoOutputRecursive(QList<int> *translationtable, int *nexttransentry, QList<int> *translationtable2, int *nexttransentry2, int parent, long long int asize, int filesused,
                                  int awidth, int aheight, QDataStream *out, QList<double> *stretches)
{
    QList<bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++)
    {
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
        if (lowestindex == -1) return;
        int i = lowestindex;
        usedflags[i] = true;

        progressBar->setValue((*nexttransentry) + 1);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        if (OutputObjects[i]->Show)
        {
            if (OutputObjects[i]->IsGroup)
            {
                if (OutputObjects[i]->Merge)
                {
                    if (DoIHaveChildren(i))
                    {
                        asize = static_cast<long long int>(filesused) * static_cast<long long int>(awidth) * static_cast<long long int>(aheight);
                        QByteArray FullOutArray;
                        int FullOutArrayCount = 0;
                        QVector<double> TrigArray;
                        int TrigCount = 0;

                        if (out == nullptr) Count = 0;
                        else Count = -1;
                        MakeMergeObject(&FullOutArrayCount, &FullOutArray, &TrigArray, &TrigCount, i, asize, filesused, awidth, aheight, out, stretches);

                        (*translationtable)[(*nexttransentry)] = i;
                        (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;

                        if (out == nullptr)
                        {
                            QTextStream t(&CountMessage);
                            if (CountMessage.length() == 0) t << "Total voxels in volume: " << asize << "\n";
                            t << OutputObjects[i]->Name << ": " << Count << " voxels (" << (static_cast<double>((static_cast<long long int>(Count) * static_cast<long long int>(100000)) / asize)) / 1000 << "%)\n";
                        }
                        else
                        {
                            WriteSPVData(FullOutArrayCount, FullOutArray, &TrigArray, TrigCount, out);
                        }
                    }
                }
                else
                {
                    if (DoIHaveChildren(i))
                    {
                        (*translationtable)[(*nexttransentry)++] = i;
                        DoOutputRecursive(translationtable, nexttransentry, translationtable2, nexttransentry2, i,
                                          asize, filesused, awidth, aheight, out, stretches);
                    }
                }
            }
            else
            {
                asize = filesused * awidth * aheight;
                QByteArray FullOutArray;
                int FullOutArrayCount = 0;
                if (out == nullptr) Count = 0;
                else Count = -1;

                PopulateOutputArray(&FullOutArrayCount, &FullOutArray, &Count, FirstOutputFile, LastOutputFile, i, awidth, aheight, label);

                QVector<double> TrigArray;
                int TrigCount = 0;
                if (out != nullptr)
                    PopulateTriangleList(i, FirstOutputFile, LastOutputFile, stretches, OutputObjects[i]->Resample, &TrigArray, &TrigCount);

                if (out == nullptr)
                {
                    QTextStream t(&CountMessage);
                    if (CountMessage.length() == 0) t << "Total voxels in volume: " << asize << "\n";
                    t << OutputObjects[i]->Name << ": " << Count << " voxels (" << (static_cast<double>((static_cast<long long int>(Count) * static_cast<long long int>(100000)) / asize)) / 1000 << "%)\n";
                }
                else
                {
                    WriteSPVData(FullOutArrayCount, FullOutArray, &TrigArray, TrigCount, out);
                }

                (*translationtable)[(*nexttransentry)] = i;
                (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;
            }
        }
    }
}

void ExportSPV::GetOutputList(QList<OutputObject *> *outlist, QList<int> *translationtable, int *nexttransentry, QList<int> *translationtable2, int *nexttransentry2, int parent)
{
    QList<bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++)
    {
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
        if (lowestindex == -1) return;
        int i = lowestindex;
        usedflags[i] = true;

        if (OutputObjects[i]->Show)
        {
            if (OutputObjects[i]->IsGroup)
            {
                if (OutputObjects[i]->Merge)
                {
                    if (DoIHaveChildren(i))
                    {
                        (*translationtable)[(*nexttransentry)] = i;
                        (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;
                        outlist->append(OutputObjects[i]);

                        OutputObjects[i]->MergeObjects.clear();
                        MakeMergeObject_2(i, OutputObjects[i]);
                    }
                }
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
                (*translationtable)[(*nexttransentry)] = i;
                (*translationtable2)[(*nexttransentry)++] = (*nexttransentry2)++;
                outlist->append(OutputObjects[i]);
            }
        }
    }
}

// ============================================================
// MeasureVols
// ============================================================

void ExportSPV::MeasureVols()
{
    int n;
    long long int asize = 0;
    int flen, pos;
    Q_UNUSED(flen);
    Q_UNUSED(pos);
    int awidth, aheight;

    QList<int> translationtable1;
    StupidBodgyFunction(&translationtable1, -1);

    int ActualOutputObjectCount = translationtable1.count();
    if (ActualOutputObjectCount == 0)
    {
        Message("Nothing to Count!");
        return;
    }

    int filesused = LastOutputFile - FirstOutputFile + 1;
    pos = 0;
    flen = fwidth * fheight - 1;

    aheight = (fheight - fheight % XYDownsample) / XYDownsample;
    awidth = (fwidth - fwidth % XYDownsample) / XYDownsample;
    filesused = (filesused - filesused % ZDownsample) / ZDownsample;

    show();
    this->setWindowTitle("Counting voxels in volume...");
    copying = true;

    WriteAllData(CurrentFile);
    WriteSettings();
    progressBar->setMaximum(ActualOutputObjectCount);

    QList<int> translationtable;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable.append(0);
    int nexttransentry = 0, nexttransentry2 = 0;
    QList<int> translationtable2;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable2.append(0);

    QList<double> stretches;
    for (n = 0; n <= filesused + 1; n++) stretches.append(n);

    DoOutputRecursive(&translationtable, &nexttransentry, &translationtable2, &nexttransentry2, -1, asize, filesused, awidth, aheight, static_cast<QDataStream *>(nullptr), &stretches);

    LoadAllData(CurrentFile);
    copying = false;
    close();
    Message(CountMessage);
}

// ============================================================
// RunLegacy  (original ExportSPV — kept for comparison)
// ============================================================

void ExportSPV::RunLegacy(int flag)
{
    int n;
    long long int asize = 0;
    QString outputfile;
    QByteArray compressedbuffer;
    QList<double> stretches, stretches2;
    int flen, pos;
    Q_UNUSED(flen);
    int awidth, aheight;

    label->setFixedWidth(60);

    QList<int> translationtable1;
    StupidBodgyFunction(&translationtable1, -1);
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

    if (OutputMirroring) ReverseStretches(&stretches, 0, filesused + 1);

    if (flag < 2)
    {
        outputfile = QFileDialog::getSaveFileName(nullptr, "SPIERSview File Name", "", "SPIERSview files (*.spv)");
    }
    else
    {
        int lastsep, lastdot;
        QString Fname = Files.at(0);
        lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/"));
        lastdot = Fname.lastIndexOf(".");
        QString sfname = Fname.left(lastsep);
        QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
        outputfile = sfname + "/" + SettingsFileName + "/temp.spv";
    }
    if (outputfile == "") return;
    QFile File(outputfile);

    pos = 0;
    flen = fwidth * fheight - 1;

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
            tot = stretches[spos++];
            zpos++;
            if (zpos == ZDownsample)
            {
                stretches2.append(tot);
                zpos = 0;
            };
        }
        while (spos < stretches.count());
    }

    if (!File.open(QIODevice::WriteOnly))
    {
        Message("Sorry, can't open spv file for writing");
        return;
    }
    QDataStream out(&File);
    out.setVersion(QDataStream::Qt_4_3);
    out.setByteOrder(QDataStream::LittleEndian);

    out << static_cast<double>(-1);
    out << static_cast<int>(5);
    out << PixPerMM / (static_cast<double>(XYDownsample * ColMonoScale));
    out << SlicePerMM;
    out << SkewDown << SkewLeft;
    out << awidth << aheight;
    out << filesused + 2;
    out << ActualOutputObjectCount;

    QByteArray OutKeys(402, 0);
    pos = 0;
    for (n = 0; n < translationtable1.count(); n++)
        if (OutputObjects[translationtable1[n]]->Key != 0) OutKeys[(pos++ * 2)] = static_cast<char>(OutputObjects[translationtable1[n]]->Key);
        else OutKeys[(pos++ * 2)] = 1;
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
    for (n = 0; n < translationtable1.count(); n++) OutResamples[(pos++ * 2)] = OutputObjects[translationtable1[n]]->Resample;
    for (n = 0; n < 402; n++) out << static_cast<quint8>(OutResamples[n]);

    for (n = 0; n <= (filesused + 1); n++) out << stretches2[n];

    show();
    this->setWindowTitle("Creating SPIERSview file...");
    copying = true;

    WriteAllData(CurrentFile);
    WriteSettings();
    progressBar->setMaximum(ActualOutputObjectCount);

    QList<int> translationtable;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable.append(0);
    int nexttransentry = 0, nexttransentry2 = 0;
    QList<int> translationtable2;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable2.append(0);

    DoOutputRecursive(&translationtable, &nexttransentry, &translationtable2, &nexttransentry2, -1,
                      asize, filesused, awidth, aheight, &out, &stretches2);

    out << static_cast<int>(10000);
    out << nexttransentry;
    for (int i = 0; i < nexttransentry; i++) out << translationtable2[i];
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Parent == -1) out << static_cast<int>(-1);
        else out << translationtable.indexOf(OutputObjects[translationtable[i]]->Parent);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->IsGroup && OutputObjects[translationtable[i]]->Merge == false) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->IsGroup) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Key == 0) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(OutputObjects[translationtable[i]]->Key);
    for (int i = 0; i < nexttransentry; i++)
    {
        QString s;
        if (OutputObjects[translationtable[i]]->Key != 0)
        {
            s.append(QChar(static_cast<uchar>(OutputObjects[translationtable[i]]->Key)));
            s += " - ";
        }
        s += OutputObjects[translationtable[i]]->Name;
        out << static_cast<int>(s.size());
        out.writeRawData(s.toLatin1().constData(), s.size());
    }

    File.close();
    LoadAllData(CurrentFile);
    copying = false;
    close();

    qDebug() << "file:" + outputfile;

    if (flag > 0)
    {
#ifdef __APPLE__
        QString program = qApp->applicationFilePath();
        program.replace("/SPIERSedit", "/SPIERSview");
        QStringList arguments;
        arguments << outputfile;
        QProcess::startDetached(program, arguments, qApp->applicationDirPath());
#endif

#ifdef __linux__
        QString program = qApp->applicationFilePath();
        program.replace("/SPIERSedit", "/SPIERSview");
        QStringList arguments;
        arguments << outputfile;
        QProcess::startDetached(program, arguments, qApp->applicationDirPath());
#endif

#ifdef _WIN64
        QString program = "\"" + qApp->applicationDirPath() + "/SPIERSview64.exe" + "\"" + " \"" + outputfile + "\"";
        if (!QProcess::startDetached(program))
            QDesktopServices::openUrl(QUrl::fromLocalFile(outputfile));
#endif
    }
}

// ============================================================
// Run  (optimised export path)
//
// Optimisations vs the original ExportSPV_2:
//
//  1. zflag check folded into the grid-building loop — eliminates a
//     redundant full grid scan per slice per object.
//
//  2. Per-object pixel processing is parallelised with
//     QtConcurrent::blockingMap after each slice load.  GA[]/Masks[] are
//     loaded sequentially (they write to shared globals), then become
//     read-only while all objects process their pixels concurrently.
//     Each object writes only to its own Outputarray / temparray.
//     DrawCurveOutput reads Curves[] data (read-only during export) and
//     writes to the per-object output buffer — no shared mutable state.
//
//  3. Per-object post-processing (resample, pad, grid, compress) is also
//     parallelised across objects with QtConcurrent::blockingMap, giving
//     a proportional speedup for multi-object datasets.
// ============================================================

void ExportSPV::Run(int flag)
{
    int n;
    QString outputfile;
    QList<double> stretches, stretches2;
    int flen, pos;
    Q_UNUSED(flen);
    int awidth, aheight;

    escapeFlag = false;

    label->setFixedWidth(60);

    QList<int> translationtable1;
    StupidBodgyFunction(&translationtable1, -1);

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

    if (OutputMirroring) ReverseStretches(&stretches, 0, filesused + 1);

    if (flag < 2)
    {
        outputfile = QFileDialog::getSaveFileName(nullptr, "SPIERSview File Name", "", "SPIERSview files (*.spv)");
    }
    else
    {
        int lastsep, lastdot;
        QString Fname = Files.at(0);
        lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/"));
        lastdot = Fname.lastIndexOf(".");
        QString sfname = Fname.left(lastsep);
        QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
        outputfile = sfname + "/" + SettingsFileName + "/temp.spv";
    }
    if (outputfile == "") return;
    QFile File(outputfile);

    pos = 0;
    flen = fwidth * fheight - 1;

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
            tot = stretches[spos++];
            zpos++;
            if (zpos == ZDownsample)
            {
                stretches2.append(tot);
                zpos = 0;
            };
        }
        while (spos < stretches.count());

        while (stretches2.count() < filesused + 2)
        {
            int nc = stretches2.count();
            double diff = stretches2[nc - 1] - stretches2[nc - 2];
            stretches2.append(stretches2.last() + diff);
        }
    }

    if (!File.open(QIODevice::WriteOnly))
    {
        Message("Sorry, can't open spv file for writing");
        return;
    }
    QDataStream out(&File);
    out.setVersion(QDataStream::Qt_6_11);
    out.setByteOrder(QDataStream::LittleEndian);

    out << static_cast<double>(-1);
    out << static_cast<int>(SPVFILEVERSION);
    out << PixPerMM / (static_cast<double>(XYDownsample * ColMonoScale));
    out << SlicePerMM;
    out << SkewDown << SkewLeft;
    out << awidth << aheight;
    out << filesused + 2;
    out << ActualOutputObjectCount;

    QByteArray OutKeys(402, 0);
    pos = 0;
    for (n = 0; n < translationtable1.count(); n++)
        if (OutputObjects[translationtable1[n]]->Key != 0) OutKeys[(pos++ * 2)] = static_cast<char>(OutputObjects[translationtable1[n]]->Key);
        else OutKeys[(pos++ * 2)] = 1;
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

    for (n = 0; n <= (filesused + 1); n++) out << stretches2[n];

    WriteAllData(CurrentFile);
    WriteSettings();
    show();
    this->setWindowTitle("Creating SPIERSview file...");
    copying = true;

    QList<int> translationtable;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable.append(0);
    QList<int> translationtable2;
    for (int i = 0; i < OutputObjectsCount; i++) translationtable2.append(0);
    int nexttransentry = 0, nexttransentry2 = 0;

    QList<OutputObject *> outlist;
    GetOutputList(&outlist, &translationtable, &nexttransentry, &translationtable2, &nexttransentry2, -1);

    progressBar->setMaximum(filesused);

    // Set up per-object working arrays
    foreach (OutputObject *Object, outlist)
    {
        if (Object->Merge) {
            foreach (OutputObject *Object2, Object->MergeObjects) {
                Object2->SetUpForRender();
            }
        }
        Object->SetUpForRender();
        Object->Outputarray.resize(awidth * aheight);
        //qDebug()<<"Object name"<<Object->Name<<"MasksList"<<Object->UseMasks<<"SegList"<<Object->UseSegs<<"OA:"<<Object->Outputarray.size()<<fwidth<<fheight<<fwidth*fheight;
    }

    const int fmax = (fwidth * fheight) - 1;

    int loopfrom, loopto, loopstep;
    if (OutputMirroring)
    {
        loopto = FirstOutputFile;
        loopfrom = loopto;
        loopstep = 0 - ZDownsample;
        while ((loopfrom - loopstep) <= (LastOutputFile + loopstep + 1))
            loopfrom -= loopstep;
    }
    else
    {
        loopfrom = FirstOutputFile;
        loopto = LastOutputFile;
        loopstep = ZDownsample;
        loopto = loopto - (((loopto - loopfrom) + 1) % loopstep);
    }

    int count = 0;

    for (int f = loopfrom; freached(f, loopto, loopfrom); f += loopstep)
    {
        qApp->processEvents();
        if (escapeFlag) break;
        progressBar->setValue(count++);

        QString message;
        QTextStream ms(&message);
        ms << "File: " << f;
        label->setText(message);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Check whether any z-sub-slices for this output slice are dirty
        bool dflag = false;
        for (int i = 0; i < ZDownsample; i++) if (FilesDirty[i + f]) dflag = true;

        if (dflag)
        {
            // Reset per-object working arrays
            foreach (OutputObject *Object, outlist)
            {
                Object->bigpos = 0;
                Object->tpos = 0;
                if (XYDownsample * ZDownsample > 1)
                    Object->temparray.fill(0, (fmax + 1) * ZDownsample);
                Object->Outputarray.fill(0);
            }

            // -------------------------------------------------------
            // Z sub-slice loop: load slice data sequentially, then
            // process all objects in parallel.
            // -------------------------------------------------------
            for (int i = 0; i < ZDownsample; i++)
            {
                int usefile = f + i;
                if (usefile >= FileCount) usefile = FileCount - 1;

                // Sequential: writes to global GA[] and Masks[]
                for (int j = 0; j < SegmentCount; j++) LoadGreyData(usefile, j);
                LoadMasks(usefile);

                // Parallel: GA[] / Masks[] / Segments[] / Curves[] are now
                // read-only; each object writes only to its own arrays.
                QtConcurrent::blockingMap(outlist, [&](OutputObject *Object)
                {
                    QList<OutputObject *> ObsToUse;
                    if (Object->Merge) ObsToUse = Object->MergeObjects;
                    else ObsToUse.append(Object);

                    foreach (OutputObject *Obj, ObsToUse)
                    {
                        int bp = Object->bigpos;
                        int tp = Object->tpos;
                        QByteArray *outArr = &(Object->Outputarray);
                        QByteArray *tmpArr = &(Object->temparray);
                        QList<bool> *uMasks = &(Obj->UseMasks);
                        QList<bool> *uSegs  = &(Obj->UseSegs);

                        if (XYDownsample * ZDownsample == 1)
                        {
                            for (int y = fheight - 1; y >= 0; y--)
                                for (int x = 0; x < fwidth; x++)
                                {
                                    if ((*uMasks)[static_cast<quint8>(Masks.at(y * fwidth + x))])
                                    {
                                        int high = 128, seg = -1;
                                        for (int s = 0; s < SegmentCount; s++)
                                        {
                                            if (Segments[s]->Activated)
                                            {
                                                int pval = static_cast<int>(*((GA[s]->constBits()) + (fheight - 1 - y) * fwidth4 + x));
                                                if (pval >= high) { high = pval; seg = s; }
                                            }
                                        }
                                        if (seg != -1 && (*uSegs)[seg])
                                            (*outArr)[bp] = static_cast<char>(255);
                                    }
                                    bp++;
                                }

                            for (int sn = 0; sn < CurveCount; sn++)
                            {
                                if (Curves[sn]->Segment != 0)
                                {
                                    int segcv = Curves[sn]->Segment - 1;
                                    bool visible = Segments[segcv]->Activated && (*uSegs)[segcv];
                                    DrawCurveOutput(sn, usefile, (uchar *)outArr->data(), uMasks, !visible);
                                }
                            }
                        }
                        else
                        {
                            int mi = i * (fmax + 1);
                            for (int y = fheight - 1; y >= 0; y--)
                                for (int x = 0; x < fwidth; x++)
                                {
                                    if ((*uMasks)[static_cast<quint8>(Masks.at(y * fwidth + x))])
                                    {
                                        int high = 128, seg = -1;
                                        for (int s = 0; s < SegmentCount; s++)
                                        {
                                            if (Segments[s]->Activated)
                                            {
                                                int pval = static_cast<int>(*((GA[s]->constBits()) + (fheight - 1 - y) * fwidth4 + x));
                                                if (pval >= high) { high = pval; seg = s; }
                                            }
                                        }
                                        if (seg != -1 && (*uSegs)[seg])
                                            (*tmpArr)[tp] = static_cast<uchar>(255);
                                    }
                                    tp++;
                                }

                            for (int sn = 0; sn < CurveCount; sn++)
                            {
                                if (Curves[sn]->Segment != 0)
                                {
                                    int segcv = Curves[sn]->Segment - 1;
                                    bool visible = Segments[segcv]->Activated && (*uSegs)[segcv];
                                    DrawCurveOutput(sn, usefile, (uchar *)tmpArr->data() + mi, uMasks, !visible);
                                }
                            }
                        }

                        Object->bigpos = bp;
                        Object->tpos   = tp;
                    }
                });

                FilesDirty[f + i] = false;
            }

            // -------------------------------------------------------
            // Post-processing: resample, pad, grid, compress.
            // Each object is independent — run in parallel.
            // zflag check is folded into the grid loop.
            // -------------------------------------------------------
            QtConcurrent::blockingMap(outlist, [&](OutputObject *Object)
            {
                QByteArray *outArr = &(Object->Outputarray);
                QByteArray *tmpArr = &(Object->temparray);

                // Resample if needed
                if (XYDownsample * ZDownsample > 1)
                {
                    for (int pn = 0; pn < awidth; pn++)
                        for (int pm = 0; pm < aheight; pm++)
                            if (ResampleForLoRes(pn * XYDownsample, pm * XYDownsample, tmpArr))
                                (*outArr)[pm * awidth + pn] = 255u;
                }

                // Pad edges to zero so marching cubes produces closed surfaces
                int edgestart = 0;
                for (int pn = 0; pn < awidth; pn++)  (*outArr)[edgestart++] = 0;
                for (int pn = 1; pn < (aheight - 2); pn++)
                {
                    (*outArr)[edgestart] = 0;
                    edgestart += awidth;
                    (*outArr)[edgestart - 1] = 0;
                }
                for (int pn = 0; pn < awidth; pn++)  (*outArr)[edgestart++] = 0;

                // Build grid and check for empty slice in one pass
                bool zflag = true;
                const int gxsize = (awidth  / GRID_SCALE) + 1;
                const int gysize = (aheight / GRID_SCALE) + 1;
                QByteArray *GridArray = Object->GridArrays[f];
                GridArray->fill(0, gxsize * gysize);

                int Gpos = 0;
                for (int gm = 0; gm < gysize; gm++)
                {
                    int ymax = gm * GRID_SCALE + GRID_SCALE;
                    if (ymax > aheight) ymax = aheight;
                    for (int gn = 0; gn < gxsize; gn++)
                    {
                        int xmax = gn * GRID_SCALE + GRID_SCALE;
                        if (xmax > awidth) xmax = awidth;
                        for (int mm = gm * GRID_SCALE; mm < ymax; mm++)
                        {
                            int pixpos = mm * awidth + gn * GRID_SCALE;
                            for (int nn = gn * GRID_SCALE; nn < xmax; nn++)
                            {
                                if ((*outArr)[pixpos++])
                                {
                                    (*GridArray)[Gpos] = 255u;
                                    zflag = false;
                                }
                            }
                        }
                        Gpos++;
                    }
                }

                // Compress non-empty slices
                if (!zflag)
                    (*(Object->CompressedSPVarrays[f])) = qCompress(*outArr, 9);
            });
        }
    }

    // Restore slice data for the currently displayed file
    for (int j = 0; j < SegmentCount; j++) LoadGreyData(CurrentFile, j);
    LoadMasks(CurrentFile);

    // Write all compressed data per object
    foreach (OutputObject *Object, outlist)
    {
        out << count;

        for (int f = loopfrom; freached(f, loopto, loopfrom); f += loopstep)
        {
            QByteArray *carray = Object->CompressedSPVarrays[f];
            if (carray->isEmpty())
            {
                out << static_cast<int>(-1);
            }
            else
            {
                int outsize = carray->size() - 4;
                out << outsize;
                out.writeRawData(carray->constData() + 4, outsize);

                QByteArray OutputGrid = ExpandGrid(Object->GridArrays[f], awidth, aheight);
                out.writeRawData(OutputGrid.constData(), OutputGrid.size());
            }
        }
        out << static_cast<int>(0); // triangle count (none in this path)
    }

    // Footer
    out << static_cast<int>(10000);
    out << nexttransentry;
    for (int i = 0; i < nexttransentry; i++)
        out << translationtable2[i];
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Parent == -1) out << static_cast<int>(-1);
        else out << translationtable.indexOf(OutputObjects[translationtable[i]]->Parent);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->IsGroup && OutputObjects[translationtable[i]]->Merge == false) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->IsGroup) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(0);
    for (int i = 0; i < nexttransentry; i++)
        if (OutputObjects[translationtable[i]]->Key == 0) out << static_cast<uchar>(1);
        else out << static_cast<uchar>(OutputObjects[translationtable[i]]->Key);
    for (int i = 0; i < nexttransentry; i++)
    {
        QString s;
        if (OutputObjects[translationtable[i]]->Key != 0)
        {
            s.append(QChar(static_cast<uchar>(OutputObjects[translationtable[i]]->Key)));
            s += " - ";
        }
        s += OutputObjects[translationtable[i]]->Name;
        out << static_cast<int>(s.size());
        out.writeRawData(s.toLatin1().constData(), s.size());
    }

    File.close();
    LoadAllData(CurrentFile);
    copying = false;
    close();

    if (escapeFlag)
    {
        File.remove();
        return;
    }

    if (flag > 0)
    {
#ifdef __APPLE__
        QString program = qApp->applicationFilePath();
        program.replace("/SPIERSedit", "/SPIERSview");
        QStringList arguments;
        arguments << outputfile;
        QProcess::startDetached(program, arguments, qApp->applicationDirPath());
#endif

#ifdef __linux__
        QString program = qApp->applicationFilePath();
        program.replace("/SPIERSedit", "/SPIERSview");
        QStringList arguments;
        arguments << outputfile;
        QProcess::startDetached(program, arguments, qApp->applicationDirPath());
#endif

#ifdef _WIN64
        QString program = "\"" + qApp->applicationDirPath() + "/SPIERSview64.exe" + "\"" + " \"" + outputfile + "\"";
        if (!QProcess::startDetached(program))
            QDesktopServices::openUrl(QUrl::fromLocalFile(outputfile));
#endif
    }
}
