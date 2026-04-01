/**
 * @file
 * Header: Copying Impl.
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef COPYINGIMPL_H
#define COPYINGIMPL_H

#include <QDir>
#include <QList>
#include <QListWidget>

#include "ui_copying.h"
#include "mainwindow.h"
#include "beamhardening.h"

/**
 * @brief The CopyingImpl class
 */
class CopyingImpl : public QDialog, public Ui::Copying
{
    Q_OBJECT
public:
    CopyingImpl( QWidget *parent = nullptr, Qt::WindowFlags f = {} );
    void Copy(QDir source, QDir dest);
    void MakeNewSegFiles(int snum);
    void DeleteSegments(QList <int> list);
    void GenerateLCE(QListWidget *SliceSelectorList);
    void GenerateGradient(QListWidget *SliceSelectorList);
    void GenerateLinear(QListWidget *SliceSelectorList);
    void GenerateML(QListWidget *SliceSelectorList);
    void GenerateRange(QListWidget *SliceSelectorList);
    void GenerateAllLinear();
    void GenerateAllBlank();
    void MaskCopy(int fromfile, MainWindow *mw);
    void MaskCopy2(int fromfile, MainWindow *mw);
    void CurvesToMasks(MainWindow *mw);
    void CompressAllWorkingFiles(int level);
    void CompressAllSourceFiles(int level);
    void Apply3DBrush(int button);
    bool copying;

    void GenerateRadial(QListWidget *SliceSelectorList, BeamHardening *bh);
protected:
    void closeEvent(QCloseEvent *event);

private:
    QVector <uchar> LCE_sample;
private slots:
    void escape();

};

#endif
