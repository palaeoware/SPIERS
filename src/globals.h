/**
 * @file
 * Header: Globals
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <QString>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QImage>
#include <QMessageBox>
#include <QColor>
#include <QtDebug>
#include <QTreeWidgetItem>
#include <QTabWidget>
#include <QMutex>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

#include "mainwindowimpl.h"

// internal version for file purposes - take as SPIERSedit 2.this
#define SPIERS_VERSION 2

//Legal Stuff
#define COPYRIGHT "Copyright © 2018 Mark D. Sutton, Russell J. Garwood, Alan R.T.Spencer"
#define LICENCE "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the GPL v3 license"

//Programme Name
#define PRODUCTNAME "SPIERSedit"

//Email
#define EMAIL "palaeoware@gmail.com"

//Github
#define GITURL "https://github.com/"
#define GITREPOSITORY "palaeoware/SPIERSedit"
#define GITISSUE "/issues"

//Readthedocs
#define READTHEDOCS "https://spiersedit.readthedocs.io/en/latest/"

extern QList<bool> FilesDirty; //for re-rendering.
extern QString openfile;
extern bool bodgeflag;
extern bool ThreeDmode;
extern int BrushY, BrushZ;
extern double yaw, pitch, roll;
extern bool pausetimers;
extern QString Notes;
extern int zsparsity;
extern int CacheCompressionLevel;
extern int FileCompressionLevel;
extern QVector<int> SegmentMap;
extern MainWindowImpl *mainwin;
extern bool MasksMoveBack, MasksMoveForward;
extern QMutex mutex;
extern QList<double> Stretches;
extern QList<double> FullStretches;
extern bool ShowSlicePosition;

/**
 * @brief The Segment class
 */
class Segment
{
public:

    Segment(QString name);
    ~Segment();

    QString Name;
    int Colour[3]; //r,g,b
    int LinPercent[3]; //r,g,b
    int LinGlobal;
    bool LinInvert;
    double NeighbourBright;
    int NeighbourSparse;
    bool NeighbourSingle;
    double PolyRedConsts[10];
    double PolyGreenConsts[10];
    double PolyBlueConsts[10];
    double PolyKall;
    int PolyOrder;
    int PolySparse, PolyRetries, PolyConverge;
    double PolyScale;
    int PolyContrast;
    int RangeBase;
    int RangeTop;
    double RangeGradient;
    double RangeCenter;
    int ListOrder; //position in display list
    bool Dirty;  //changed on this slice
    bool UndoDirty; //changed since last undo save
    bool Locked;
    bool Activated;
    QTreeWidgetItem *widgetitem;
    QGraphicsRectItem *rectitem;
    QGraphicsTextItem *textitem;
};

/**
 * @brief The Mask class
 */
class Mask
{
public:
    Mask(QString name);
    QString Name;
    int ForeColour[3]; //r,g,b
    int BackColour[3];
    bool Show;
    bool Write;
    int Contrast;
    bool Lock;
    QTreeWidgetItem *widgetitem;
    int ListOrder; //position in display list
};

/**
 * @brief The PointList class
 */
class PointList
{
public:
    PointList();
    QList <double> X;
    QList <double> Y;
    int Count;
};

/**
 * @brief The Curve class
 */
class Curve
{
public:
    Curve(QString name);
    ~Curve();
    QString Name;
    int Colour[3]; //r,g,b
    bool Closed;
    bool Filled;
    int Segment; //This is segment +1, as '0' reserved for 'none'
    QList <class PointList *> SplinePoints; // list for each file
    int ListOrder; //position in display list
    QTreeWidgetItem *widgetitem;
};

/**
 * @brief The OutputObject class
 */
class OutputObject
{
public:
    OutputObject(QString name);
    ~OutputObject();
    QString Name; //not implemented in VB - use for SView compatibility one day
    int Resample;
    int Colour[3]; //r,g,b
    QList <int> ComponentMasks;
    QList <int> ComponentSegments;
    QList <int> CurveComponents;
    int ListOrder; //position in display list
    QTreeWidgetItem *widgetitem;
    bool IsGroup;
    int Parent;
    uchar Key;
    bool Show;
    bool Merge;
    bool Expanded;
    bool TempSelected;

    //some stuff only used in output
    QByteArray temparray, Outputarray;
    QList <QByteArray *> CompressedSPVarrays;        //Equiv to old 'fullarrays
    QList <QByteArray *> GridArrays;
    int bigposfirst, bigpos, tpos;
    QList <bool> UseMasks;
    QList <bool> UseSegs;
    QList <OutputObject *> MergeObjects;

    void SetUpForRender();
};

extern int fwidth4;
extern int cwidth4;
extern int AutoSaveFrequency;
extern bool NoUpdateSelectionFlag;
extern bool escapeFlag;
extern bool OutputRegroupMode;
extern QList <class Segment *> Segments;
extern QList <class Mask *> MasksSettings;
extern QList <class Curve *> Curves;
extern QList <class OutputObject *> OutputObjects;
extern bool CurveShapeLocked;
extern QStringList Files;
extern QStringList FullFiles;
extern int FileCount, CurrentFile, cwidth, cheight, ColMonoScale, fwidth, fheight;
extern double CurrentZoom;
extern bool Active;
extern bool clearing;
extern int Trans, CMin, CMax;
extern int SegmentCount;
extern QList <QImage *> GA; //this is the grey arrays
extern QImage ColArray; //other data
extern QByteArray Masks, Locks;
extern QByteArray dirty;
extern void Error(QString);
extern void Message(QString);
extern void Message1(char *message, int x);
extern int CurrentMode;
extern int Brush_Size;
extern int BrightUp;
extern int BrightDown;
extern int BrightSoft;
extern int LastTrans;
extern int CurrentSegment, CurrentRSegment;
extern bool ThreshFlag, MasksFlag, SegsFlag;
extern bool MoveFlag, ChangeFlag;
extern QList <class Segment *> Segments;
extern int MaxUsedMask, SelectedMask, SelectedRMask, SelectedCurve, CurveCount, OutputObjectsCount;
extern int SampleArraySize;
extern QByteArray SampleArray;
extern double PixPerMM, SlicePerMM, SkewDown, SkewLeft;
extern int  FirstOutputFile, LastOutputFile;
extern int MaxTriangles;
extern bool RangeHardFill, RangeSelectedOnly, OutputMirroring;
extern int  PixSens, XYDownsample, ZDownsample;
extern bool HiddenMasksLockedForGeneration, SegmentBrushAppliesMasks;
extern bool SegmentsLocked, CurveMarkersAsCrosses, CurveShapeLocked;
extern int LastMouseX, LastMouseY;
extern bool SquareBrush;
extern QString SettingsFileName, FileNotes;
extern QString FullSettingsFileName;
extern bool MasksDirty, LocksDirty, CurvesDirty; //for file writing
extern bool MasksUndoDirty, LocksUndoDirty, CurvesUndoDirty; //for file writing
extern bool HorribleBodgeFlagDontStoreUndo;

// 'superglobals' - written to registry or equivalent
extern bool BackgroundCacheFilling;
extern bool RenderCache;
extern bool MenuHistSelectedOnly, MenuHistChecked, MenuInfoChecked, MenuGenChecked, MenuMasksChecked, MenuSegsChecked, MenuCurvesChecked, MenuOutputChecked, MenuToolboxChecked,
       MenuSliceSelectorChecked;

/**
 * @brief The RecentFiles struct
 */
struct RecentFiles
{
    QString File;
    QString Notes;
};

extern QList<RecentFiles> RecentFileList;
extern int CacheMem;
extern int UndoMem;
extern int UndoTimerSetting;

extern void WriteSuperGlobals();
extern void ReadSuperGlobals();
extern void RecentFile(QString fname);
extern void ResetFilesDirty();

extern QTabWidget *tabwidget;
extern bool GreyImage; // flag for current colour image is actually a greyscale image

// Macros for reading/writing RGB from arrays - these are endian sensitive and NOT portable (though easily tweaked)
extern int randn(int n);

#define ALPHA(pointer, offset) pointer[offset+3]
#define RED(pointer, offset) pointer[offset+2]
#define GREEN(pointer, offset) pointer[offset+1]
#define BLUE(pointer, offset) pointer[offset]
//size for export gridding table - high values give smaller SPVs, lower performance of SPIERSview
#define GRID_SCALE 32


#endif // __GLOBALS_H__
