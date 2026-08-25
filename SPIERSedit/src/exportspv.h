/**
 * @file
 * Header: ExportSPV
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

#ifndef EXPORTSPV_H
#define EXPORTSPV_H

#include <QDialog>
#include <QList>
#include <QVector>
#include <QDataStream>

#include "ui_copying.h"

class OutputObject;

/**
 * @brief Handles all SPIERSview export operations.
 *
 * Run() is the optimised single-pass export path. RunLegacy() preserves the
 * original object-per-pass path (accessible via the "Use Old Exporting Code"
 * menu option). MeasureVols() counts voxels without writing output.
 *
 * Optimisations in Run() vs the original ExportSPV_2:
 *  - Per-object pixel processing is parallelised with QtConcurrent::blockingMap
 *    after each slice load (GA[]/Masks[] are read-only during that phase).
 *  - Per-object post-processing (resample, pad, grid, compress) is likewise
 *    parallelised across objects.
 *  - The zflag (empty-slice) check is folded into the grid-building loop,
 *    eliminating a redundant full-grid scan.
 */
class ExportSPV : public QDialog, public Ui::Copying
{
    Q_OBJECT
public:
    explicit ExportSPV(QWidget *parent = nullptr);

    void Run(int flag);        ///< Current optimised export path
    void RunLegacy(int flag);  ///< Original export path (kept for comparison)
    void MeasureVols();        ///< Count voxels without writing output

    bool copying;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void escape();

private:
    // Object-tree helpers
    bool AmIMerged(int n);
    void StupidBodgyFunction(QList<int> *tt, int parent);
    bool DoIHaveChildren(int parent);
    void MakeMergeObject(int *fullOutArrayCount, QByteArray *fullOutArray,
                         QVector<double> *trigArray, int *trigCount,
                         int parent, long long int asize, int filesused,
                         int awidth, int aheight, QDataStream *out,
                         QList<double> *stretches);
    void MakeMergeObject_2(int parent, OutputObject *target);
    void DoOutputRecursive(QList<int> *translationtable, int *nexttransentry,
                           QList<int> *translationtable2, int *nexttransentry2,
                           int parent, long long int asize, int filesused,
                           int awidth, int aheight, QDataStream *out,
                           QList<double> *stretches);
    void GetOutputList(QList<OutputObject *> *outlist,
                       QList<int> *translationtable, int *nexttransentry,
                       QList<int> *translationtable2, int *nexttransentry2,
                       int parent);

    // Export helpers
    void WriteSPVData(int count, QByteArray data, QVector<double> *trigArray,
                      int trigCount, QDataStream *out);
    QByteArray ExpandGrid(QByteArray *grid, int awidth, int aheight);
    void ReverseStretches(QList<double> *stretches, int Sstart, int Sstop);

    QString CountMessage;
    int Count;
};

#endif // EXPORTSPV_H
