/**
 * @file
 * Header: File IO
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

#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <QDateTime>
#include <QList>
#include <QImage>
#include <QByteArray>
#include <QDir>

extern int Errors;
extern void LoadAllData(int fnum);
extern void LoadColourData(int fnum);
extern void LoadGreyData(int fnum, int seg);
extern void SaveGreyData(int fnum, int seg);
extern void LoadMasks(int fnum);
extern void SaveMasks(int fnum);
extern void LoadLocks(int fnum);
extern void SaveLocks(int fnum);
extern void LoadCurves(int fnum);
extern void SaveCurves(int fnum);
extern bool GetSettings(QDir srcdir);
extern void WriteSettings();
extern void ReadSettings();
extern void DumpSettings();
extern void ApplyDefaultSettings();
extern void WriteAllData(int fnum);
extern void SaveMasks(int fnum);
extern void InvalidateAllGreyCaches(); //use after segment delete
extern void ClearCache();
extern int CacheIndex(int fnum);
extern int GetCacheIndex(int fnum);
extern int GetShort(QDataStream *in);
extern bool SimpleLoadLocks(int fnum, int expectedsize, QByteArray *array);
extern void SimpleSaveLocks(int fnum, QByteArray array);
extern bool SimpleLoadMasks(int fnum, int expectedsize, QByteArray *array);
extern void SimpleSaveMasks(int fnum, QByteArray array);
extern void SimpleSaveGreyData(int fnum, int seg, QImage greydata);
extern bool SimpleLoadGreyData(int fnum, int seg, QImage *greydata);
extern bool SimpleLoadColourData(QString fname);

/**
 * @brief The CacheGreyData class
 */
class CacheGreyData
{
public:
    CacheGreyData();
    ~CacheGreyData();
    QImage *Data;
    QByteArray *CompressedData;
};

/**
 * @brief The Cache class
 */
class Cache
{
public:
    QImage *ColData;
    QByteArray *ColDataCompressed;
    QByteArray *Locks;
    QByteArray *Masks;
    QList <CacheGreyData *> GreyData;
    int Filenum;
    QDateTime LastUsed;
    int Size;
    Cache();
    ~Cache();
    void CalcMySize();
    void Blank(int fnum);
};

extern QList <Cache *> Caches;

#endif // __FILEIO_H__
