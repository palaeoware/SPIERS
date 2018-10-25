/*********************************************

SPIERSedit 2: globals.cpp

- Defines for global variables (of which there are lots - so sue me)
- Constructors for four primary classes (Masks, Segs, OutObjects, Curves)
- A few global service functions, e.g. output errors and messages in messageboxes

**********************************************/


#include "globals.h"
#include <stdio.h>
#include <math.h>
#include <QSettings>
#include "myrangescene.h"

bool ThreeDmode;
int BrushY, BrushZ;
double yaw, pitch, roll;
bool ShowSlicePosition;
QList<double> Stretches;
QList<double> FullStretches;
QVector<int> SegmentMap; //used for info control
bool bodgeflag = false;
int zsparsity;
bool pausetimers;
QString Notes;

QList<bool> FilesDirty; //for re-rendering.

MainWindowImpl *mainwin;
int CacheCompressionLevel = 1; //this is default
int FileCompressionLevel = 9; //this is default
bool MasksMoveBack, MasksMoveForward;
bool RenderCache;
bool NoUpdateSelectionFlag;
bool CurveShapeLocked;
bool GreyImage;
QMutex mutex(QMutex::Recursive);
QStringList Files;
QStringList FullFiles;
int FileCount, CurrentFile;
bool Active;
bool clearing;
int cwidth, cheight; //width/height of colour (source) files
int cwidth4;
int fwidth, fheight; //width/heigh of mono images (might not be col/scale exactly)
int fwidth4;
double CurrentZoom;
int ColMonoScale; //This is the resample factor - i.e. the binning - name is historical!
int Trans, CMin, CMax; //settings for colour (source) files
bool HiddenMasksLockedForGeneration, SegmentBrushAppliesMasks;
bool SegmentsLocked, CurveMarkersAsCrosses;
QString FullSettingsFileName;

int SegmentCount;
QList <QImage *> GA; //this is the grey arrays
QImage ColArray; //other data
QByteArray Masks, Locks;
QByteArray dirty;  //will hold flags for brighten mode - avoid doing same pixel twice in one stroke.

QString openfile;
bool MasksDirty, LocksDirty, CurvesDirty;
bool MasksUndoDirty, LocksUndoDirty, CurvesUndoDirty;
int CurrentMode; //0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc
int CurrentSegment, CurrentRSegment;
int Brush_Size;
int BrightUp;
int BrightDown;
int BrightSoft;
int LastTrans;
bool ThreshFlag;
bool MasksFlag;
bool SegsFlag;
int SampleArraySize;
QByteArray SampleArray;
int MaxUsedMask, SelectedMask, SelectedRMask, SelectedCurve, CurveCount, OutputObjectsCount;
double PixPerMM, SlicePerMM, SkewDown, SkewLeft;
int  FirstOutputFile, LastOutputFile;
int MaxTriangles;
bool RangeHardFill, RangeSelectedOnly, OutputMirroring;
int PixSens, XYDownsample, ZDownsample;
int LastMouseX, LastMouseY;
bool MoveFlag, ChangeFlag;
QList <struct Segment *> Segments;
QList <struct Mask *> MasksSettings;
QList <struct Curve *> Curves;
QList <struct OutputObject *> OutputObjects;
bool HorribleBodgeFlagDontStoreUndo;
bool MenuHistSelectedOnly;
bool SquareBrush;
bool escapeFlag;

QString SettingsFileName;
QString FileNotes;
QTabWidget *tabwidget;

bool MenuHistChecked, MenuInfoChecked, MenuGenChecked, MenuMasksChecked, MenuSegsChecked, MenuCurvesChecked, MenuOutputChecked, MenuToolboxChecked, MenuSliceSelectorChecked;

Mask::Mask(QString name)
//Constructor for a mask - set defaults
{
    int n, m;
    Name = name;
    n = (int((double)50 * qrand()));
    ForeColour[0] = 128 + int( (double)126 * sin((double)n));
    ForeColour[1] = 128 + int( (double)126 * cos((double)n));
    ForeColour[2] = 128 + int( (double)127 * (qrand() / RAND_MAX));
    Contrast = 2;

    for (m = 0; m < 3; m++) BackColour[m] = ForeColour[m] / 3;

    Show = true;
    Write = true;
    Lock = false;
    widgetitem = 0; //no pointer
    ListOrder = MasksSettings.count();
}

Segment::Segment(QString name)
//Constructor for a segment - set defaults
{
    Name = name;
    int n = (int((double)50 * qrand()));
    Colour[0] = 128 + int( (double)126 * sin((double)n));
    Colour[1] = 128 + int( (double)126 * cos((double)n));
    Colour[2] = 128 + int( (double)127 * (qrand() / RAND_MAX));
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

    NeighbourBright = 1;
    NeighbourSparse = 1;
    NeighbourSingle = false;
    /*PolyRedConsts[10];
    PolyGreenConsts[10];
    PolyBlueConsts[10];
    PolyKall;*/
    PolyOrder = 4;
    PolySparse = 1;
    PolyRetries = 10;
    PolyConverge = 3;
    PolyScale = -1; //Calced not set apparently
    PolyContrast = 3;

    //these are probably wrong - deal with on re-implement
    RangeBase = 0;
    RangeTop = 255;
    RangeGradient = 1;
    RangeCenter = 1;

    Dirty = false;
    UndoDirty = false;
    ListOrder = Segments.count();
    Locked = false;
    widgetitem = 0; //no pointer
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
    //remove from list if necessary
    QList <QGraphicsItem *> list = rangescene->items();
    if (list.indexOf(rectitem) != -1) rangescene->removeItem(rectitem);
    if (list.indexOf(textitem) != -1) rangescene->removeItem(textitem);
    delete rectitem;
    delete textitem;
}

Curve::Curve(QString name)
{
    Name = name;
    int n = (int((double)50 * qrand()));
    Colour[0] = 128 + int( (double)126 * sin((double)n));
    Colour[1] = 128 + int( (double)126 * cos((double)n));
    Colour[2] = 128 + int( (double)127 * qrand() / RAND_MAX);
    Closed = false;
    Filled = false;
    Segment = 0;
//  qDebug()<<"Constructing curve "<<name<<"Files.count is "<<Files.count();
//  qDebug()<<Files;

    for (int i = 0; i < FullFiles.count(); i++)
    {
        //set up all the lists
        SplinePoints.append(new PointList());
    }
    ListOrder = Curves.count();
    widgetitem = 0; //no pointer
}

Curve::~Curve()
{
    if (Segment != 0) for (int j = 0; j < Files.count(); j++) if (SplinePoints[j * zsparsity]->Count > 0) FilesDirty[j] = true;
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
    Name = name; //not implemented in VB - use for SView compatibility one day
    Resample = 100;
    Colour[0] = 255;
    Colour[1] = 255;
    Colour[2] = 255; //r,g,b
    ListOrder = OutputObjects.count();
    IsGroup = false;
    Parent = -1; //root
    Show = true;
    Merge = false;
    widgetitem = 0; //no pointer
    Expanded = false;
    ComponentMasks.clear();
    ComponentSegments.clear();
    CurveComponents.clear();
    MergeObjects.clear();
};

OutputObject::~OutputObject()
{
    //clear memory
    qDeleteAll(CompressedSPVarrays.begin(), CompressedSPVarrays.end());
    qDeleteAll(GridArrays.begin(), GridArrays.end());
}

void OutputObject::SetUpForRender()
{
    temparray.clear();

    if  (CompressedSPVarrays.isEmpty())
    {
        //create an entry for file
        for (int i = 0; i < Files.count(); i++)
        {
            CompressedSPVarrays.append(new QByteArray);
            GridArrays.append(new QByteArray);
        }
    }
    //qDeleteAll(CompressedSPVarrays.begin(), CompressedSPVarrays.end());
    //qDeleteAll(GridArrays.begin(), GridArrays.end());
    //CompressedSPVarrays.clear();
    //GridArrays.clear();

    UseMasks.clear();
    for (int i = 0; i <= MaxUsedMask; i++) if ((ComponentMasks.indexOf(i)) >= 0) UseMasks.append(true);
        else UseMasks.append(false);

    UseSegs.clear();
    for (int i = 0; i < SegmentCount; i++) if ((ComponentSegments.indexOf(i)) >= 0) UseSegs.append(true);
        else UseSegs.append(false);
}

void Message(QString message)
{
    QMessageBox::information(0, "Message", message, QMessageBox::Ok);
}

void Message1(char *message, int x)
{
    QString temp;
    temp.sprintf(message, x);
    QMessageBox::information(0, "Message", message, QMessageBox::Ok);
}


void Error(QString message)
{
    QMessageBox::critical(0, "ERROR", message, QMessageBox::Ok);
    exit(0);
}

bool OutputRegroupMode;

//Now the 'superglobals - these are to be written into system settings, not .spe files
QList<RecentFiles> RecentFileList;
int CacheMem;
int UndoMem;
int UndoTimerSetting;
bool BackgroundCacheFilling;
int AutoSaveFrequency;
QString DefaultPath;

void WriteSuperGlobals()
{
    QSettings settings("Mark Sutton", "SPIERSedit 2.0");
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
    settings.setValue("DefaultFilePath", RenderCache);
}

void ReadSuperGlobals()
{
    RecentFileList.clear(); //just in case
    QString t1;
    QSettings settings("Mark Sutton", "SPIERSedit 2.0");
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
    DefaultPath = settings.value("DefaultFilePath", "c:/").toString();
}

void RecentFile(QString fname)
{
    int n;
    //move this file to the top of the list
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

int randn(int n)
{
    //random 0 - n-1
    long r;

    r = (long)rand();
    r *= (long)n;
    return r / RAND_MAX;
}

void ResetFilesDirty()
{
    FilesDirty.clear();
    for (int i = 0; i < Files.count(); i++) FilesDirty.append(true);
}
