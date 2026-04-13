/**
 * @file
 * Source: Globals
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

#include "globals.h"

#include <math.h>
#include <QRandomGenerator>
#include <QSettings>

#include "myrangescene.h"

// ============================================================================
// GLOBAL POINTERS & CORE SERVICES
// ============================================================================

MainWindow *mainwin;
QTabWidget *tabwidget;
MLInterface *mlInterface;
QSurfaceFormat surfaceFormat;

// ============================================================================
// SYNCHRONIZATION & STATE
// ============================================================================

QRecursiveMutex mutex;
bool Active = false;
bool clearing = false;
bool pausetimers = false;
bool temptestflag = false;
bool bodgeflag = false;
int Counter = 0;

// ============================================================================
// PROJECT FILE MANAGEMENT
// ============================================================================

QString openfile;
QString currentOpenFileName;
QString SettingsFileName;
QString FullSettingsFileName;
QString DefaultPath;
QList<RecentFiles> RecentFileList;

// ============================================================================
// PROJECT-SPECIFIC: FILE & IMAGE DATA
// ============================================================================

QStringList Files;
QStringList FullFiles;
int FileCount = 0;
int CurrentFile = 0;

QList<QImage *> GA;
QImage ColArray;
QByteArray GradientArray;
QByteArray FeaturesByteArray;
QByteArray Masks;
QByteArray Locks;
QByteArray dirty;
QByteArray SampleArray;
int SampleArraySize = 0;

int cwidth = 0;
int cheight = 0;
int fwidth = 0;
int fheight = 0;
int cwidth4 = 0;
int fwidth4 = 0;
int ColMonoScale = 0;

QList<double> Stretches;
QList<double> FullStretches;
int zsparsity = 0;
double SkewDown = 0.0;
double SkewLeft = 0.0;
double PixPerMM = 1.0;
double SlicePerMM = 1.0;

bool GreyImage = false;
QString Notes;
QString FileNotes;
QVector<int> SegmentMap;

// ============================================================================
// PROJECT-SPECIFIC: DISPLAY & RENDERING STATE
// ============================================================================

double CurrentZoom = 1.0;
int CurrentMode = 0;                        /// 0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc
int CurrentSegment = -1;
int CurrentRSegment = 0;
int Trans = 255;
int CMin = 0;
int CMax = 255;
int LastTrans = 255;
double CurrentPolyContrast = 0.0;

QList<bool> FilesDirty;

// ============================================================================
// PROJECT-SPECIFIC: SEGMENT/MASK/CURVE/OUTPUT DATA
// ============================================================================

int SegmentCount = 0;
int MaxUsedMask = -1;
int SelectedMask = 0;
int SelectedRMask = 0;
int SelectedCurve = 0;
int CurveCount = 0;
int OutputObjectsCount = 0;

QList<Segment *> Segments;
QList<Mask *> MasksSettings;
QList<Curve *> Curves;
QList<OutputObject *> OutputObjects;

// ============================================================================
// PROJECT-SPECIFIC: DIRTY/UNDO TRACKING
// ============================================================================

bool MasksDirty = false;
bool LocksDirty = false;
bool CurvesDirty = false;
bool MasksUndoDirty = false;
bool LocksUndoDirty = false;
bool CurvesUndoDirty = false;
bool HorribleBodgeFlagDontStoreUndo = false;

// ============================================================================
// PROJECT-SPECIFIC: OUTPUT GENERATION SETTINGS
// ============================================================================

bool RangeHardFill = false;
bool RangeSelectedOnly = false;
bool OutputMirroring = false;
int FirstOutputFile = 0;
int LastOutputFile = 0;
int PixSens = 0;
int XYDownsample = 0;
int ZDownsample = 0;
int MaxTriangles = 0;

// ============================================================================
// EDITOR TOOL STATE
// ============================================================================

int Brush_Size = 0;
int BrushY = 0;
int BrushZ = 0;
bool SquareBrush = false;
int BrightUp = 0;
int BrightDown = 0;
int BrightSoft = 0;
int LCE_Boost = 0;
int LCE_Radius = 0;
int LCE_Adjust = 0;
bool HiddenMasksLockedForGeneration = false;
bool SegmentBrushAppliesMasks = false;
bool SegmentBrushAppliesLocks = false;
bool SegmentsLocked = false;
bool CurveMarkersAsCrosses = false;
bool CurveShapeLocked = false;
bool MasksMoveBack = false;
bool MasksMoveForward = false;

// ============================================================================
// UI INTERACTION STATE
// ============================================================================

int LastMouseX = 0;
int LastMouseY = 0;
bool ThreshFlag = false;
bool MasksFlag = false;
bool SegsFlag = false;
bool MoveFlag = false;
bool ChangeFlag = false;
bool NoUpdateSelectionFlag = false;
bool escapeFlag = false;
bool previewGradient = false;
int GradientDensity = 0;
int GradientMinDist = 0;
int GradientMinDistValue = 0;
int GradientMaxDist = 0;
int GradientMaxDistValue = 0;
bool OutputRegroupMode = false;
bool ShowSlicePosition = false;
bool ThreeDmode = false;
double yaw = 0.0;
double pitch = 0.0;
double roll = 0.0;
double k = 0.0;
double LastZoom = 1.0;

QGraphicsPixmapItem *MainImage = nullptr;
QTreeWidgetItem *LastItemClicked = nullptr;
QTime LastTimeClicked;
int LastColumnClicked = 0;

double RedConsts[10] = {0};
double GreenConsts[10] = {0};
double BlueConsts[10] = {0};

// ============================================================================
// PROGRAM SETTINGS (Persisted via QSettings)
// ============================================================================

int CacheCompressionLevel = 1;
int FileCompressionLevel = 9;
int CacheMem = 512;
int CacheMemMLGb = 2;
int UndoMem = 512;
int UndoTimerSetting = 4;
int AutoSaveFrequency = 5;
bool BackgroundCacheFilling = false;
bool RenderCache = false;

// ============================================================================
// UI MENU STATE (Persisted via QSettings)
// ============================================================================

bool MenuHistSelectedOnly = false;
bool MenuHistChecked = false;
bool MenuInfoChecked = false;
bool MenuGenChecked = false;
bool MenuMasksChecked = false;
bool MenuSegsChecked = false;
bool MenuCurvesChecked = false;
bool MenuOutputChecked = false;
bool MenuToolboxChecked = false;
bool MenuSliceSelectorChecked = false;
bool Menu3DPreviewChecked = false;

// ============================================================================
// CLASS CONSTRUCTORS & DESTRUCTORS
// ============================================================================

Mask::Mask(QString name)
{
    Name = name;

    quint64 n = static_cast<quint64>(50.0 * static_cast<double>(QRandomGenerator::global()->generate()));

    ForeColour[0] = 128 + static_cast<int>(static_cast<double>(126) * sin(static_cast<double>(n)));
    ForeColour[1] = 128 + static_cast<int>(static_cast<double>(126) * cos(static_cast<double>(n)));
    ForeColour[2] = 128 + static_cast<int>(static_cast<double>(127) * (static_cast<double>(QRandomGenerator::global()->generate()) / static_cast<double>(UINT_MAX)));

    Contrast = 2;

    for (int m = 0; m < 3; m++)
    {
        BackColour[m] = ForeColour[m] / 3;
    }

    Show = true;
    Write = true;
    Lock = false;
    widgetitem = nullptr;
    ListOrder = MasksSettings.count();
}

Segment::Segment(QString name)
{
    Name = name;
    quint64 n = static_cast<quint64>(50.0 * static_cast<double>(QRandomGenerator::global()->generate()));
    Colour[0] = 128 + int(static_cast<double>(126) * sin(static_cast<double>(n)));
    Colour[1] = 128 + int(static_cast<double>(126) * cos(static_cast<double>(n)));
    Colour[2] = 128 + int(static_cast<double>(127) * (static_cast<double>(QRandomGenerator::global()->generate()) / static_cast<double>(UINT_MAX)));

    LinPercent[0] = 100;
    LinPercent[1] = 100;
    LinPercent[2] = 100;
    LinGlobal = 100;
    LinInvert = false;

    PolyOrder = 4;
    PolySparse = 1;
    PolyRetries = 10;
    PolyConverge = 4;
    PolyContrast = 3;

    NeighbourBright = 1.0;
    NeighbourSparse = 1;
    NeighbourSingle = false;

    PolyScale = -1.0;

    RangeBase = 0;
    RangeTop = 255;
    RangeGradient = 1.0;
    RangeCenter = 1.0;

    Dirty = false;
    UndoDirty = false;
    ListOrder = Segments.count();
    Locked = false;
    widgetitem = nullptr;
    Activated = true;

    rectitem = new QGraphicsRectItem;
    textitem = new QGraphicsTextItem;
    rectitem->setVisible(false);
    textitem->setVisible(false);
    rangescene->addItem(rectitem);
    rangescene->addItem(textitem);
}

Segment::~Segment()
{
    QList<QGraphicsItem *> list = rangescene->items();
    if (list.indexOf(rectitem) != -1)
    {
        rangescene->removeItem(rectitem);
    }
    if (list.indexOf(textitem) != -1)
    {
        rangescene->removeItem(textitem);
    }
    delete rectitem;
    delete textitem;
}

Curve::Curve(QString name)
{
    Name = name;
    quint64 n = static_cast<quint64>(50.0 * static_cast<double>(QRandomGenerator::global()->generate()));
    Colour[0] = 128 + int(static_cast<double>(126) * sin(static_cast<double>(n)));
    Colour[1] = 128 + int(static_cast<double>(126) * cos(static_cast<double>(n)));
    Colour[2] = 128 + int(static_cast<double>(127) * (static_cast<double>(QRandomGenerator::global()->generate()) / static_cast<double>(UINT_MAX)));

    Closed = false;
    Filled = false;
    Segment = 0;

    for (int i = 0; i < FullFiles.count(); i++)
    {
        SplinePoints.append(new PointList());
    }
    ListOrder = Curves.count();
    widgetitem = nullptr;
}

Curve::~Curve()
{
    if (Segment != 0)
    {
        for (int j = 0; j < Files.count(); j++)
        {
            if (SplinePoints[j * zsparsity]->Count > 0)
            {
                FilesDirty[j] = true;
            }
        }
    }
    qDeleteAll(SplinePoints.begin(), SplinePoints.end());
}

PointList::PointList()
{
    X.clear();
    Y.clear();
    Count = 0;
}

OutputObject::OutputObject(QString name)
{
    Name = name;
    Resample = 100;
    Colour[0] = 255;
    Colour[1] = 255;
    Colour[2] = 255;
    ListOrder = OutputObjects.count();
    IsGroup = false;
    Parent = -1;
    Show = true;
    Merge = false;
    widgetitem = nullptr;
    Expanded = false;
    ComponentMasks.clear();
    ComponentSegments.clear();
    CurveComponents.clear();
    MergeObjects.clear();
}

OutputObject::~OutputObject()
{
    qDeleteAll(CompressedSPVarrays.begin(), CompressedSPVarrays.end());
    qDeleteAll(GridArrays.begin(), GridArrays.end());
}

void OutputObject::SetUpForRender()
{
    temparray.clear();

    if (CompressedSPVarrays.isEmpty())
    {
        for (int i = 0; i < Files.count(); i++)
        {
            CompressedSPVarrays.append(new QByteArray);
            GridArrays.append(new QByteArray);
        }
    }

    UseMasks.clear();
    for (int i = 0; i <= MaxUsedMask; i++)
    {
        if (ComponentMasks.indexOf(i) >= 0)
        {
            UseMasks.append(true);
        }
        else
        {
            UseMasks.append(false);
        }
    }

    UseSegs.clear();
    for (int i = 0; i < SegmentCount; i++)
    {
        if (ComponentSegments.indexOf(i) >= 0)
        {
            UseSegs.append(true);
        }
        else
        {
            UseSegs.append(false);
        }
    }
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void Message(QString message)
{
    QMessageBox::information(nullptr, "Message", message, QMessageBox::Ok);
}

void Message1(char *message, int x)
{
    QString temp = QString::asprintf(message, x);
    QMessageBox::information(nullptr, "Message", message, QMessageBox::Ok);
}

void Error(QString message)
{
    QMessageBox::critical(nullptr, "ERROR", message, QMessageBox::Ok);
    exit(0);
}

int randn(int n)
{
    long r = static_cast<long>(rand());
    r *= static_cast<long>(n);
    return static_cast<int>(r / RAND_MAX);
}

void ResetFilesDirty()
{
    FilesDirty.clear();
    for (int i = 0; i < Files.count(); i++)
    {
        FilesDirty.append(true);
    }
}

bool IsDatasetLoaded()
{
    return Files.count() > 0;
}

void RecentFile(QString fname)
{
    int n;
    for (n = 0; n < RecentFileList.count(); n++)
    {
        if (RecentFileList[n].File == fname)
        {
            RecentFileList.removeAt(n);
            break;
        }
    }

    RecentFiles rf;
    rf.File = fname;
    rf.Notes = FileNotes;
    RecentFileList.prepend(rf);
}

// ============================================================================
// QSETTINGS PERSISTENCE (Program-wide settings)
// ============================================================================

void WriteSuperGlobals()
{
    QSettings settings("Palaeoware", "SPIERSedit");

    settings.beginWriteArray("RecentFiles");
    for (int i = 0; i < RecentFileList.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("FileName", RecentFileList.at(i).File);
        settings.setValue("FileNotes", RecentFileList.at(i).Notes);
    }
    settings.endArray();

    settings.setValue("Cache Memory (Mb)", CacheMem);
    settings.setValue("Undo Memory (Mb)", UndoMem);
    settings.setValue("Undo Timer (Secs)", UndoTimerSetting);
    settings.setValue("Background Cacheing", BackgroundCacheFilling);
    settings.setValue("AutoSaveFrequency", AutoSaveFrequency);
    settings.setValue("CacheCompressionLevel", CacheCompressionLevel);
    settings.setValue("FileCompressionLevel", FileCompressionLevel);
    settings.setValue("RenderCache", RenderCache);
    settings.setValue("DefaultFilePath", DefaultPath);
    settings.setValue("CacheMemML(Gb)", CacheMemMLGb);
}

void ReadSuperGlobals()
{
    RecentFileList.clear();

    QSettings settings("Palaeoware", "SPIERSedit");

    int size = settings.beginReadArray("RecentFiles");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        RecentFiles rf;
        rf.File = settings.value("FileName").toString();
        rf.Notes = settings.value("FileNotes").toString();
        RecentFileList.append(rf);
    }
    settings.endArray();

    CacheMem = settings.value("Cache Memory (Mb)", 512).toInt();
    UndoMem = settings.value("Undo Memory (Mb)", 512).toInt();
    UndoTimerSetting = settings.value("Undo Timer (Secs)", 4).toInt();
    BackgroundCacheFilling = settings.value("Background Cacheing", false).toBool();
    AutoSaveFrequency = settings.value("AutoSaveFrequency", 5).toInt();
    CacheCompressionLevel = settings.value("CacheCompressionLevel", 0).toInt();
    FileCompressionLevel = settings.value("FileCompressionLevel", 0).toInt();
    RenderCache = settings.value("RenderCache", false).toBool();
    DefaultPath = settings.value("DefaultFilePath", QDir::homePath()).toString();
    CacheMemMLGb = settings.value("CacheMemML(Gb)", 2).toInt();

    qDebug() << "Read CacheMemMLGb" << CacheMemMLGb;
}
