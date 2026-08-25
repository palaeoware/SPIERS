/**
 * @file
 * Header: Globals
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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <QString>
#include <QStringList>
#include <QList>
#include <QByteArray>
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
#include <QSurfaceFormat>
#include <QTime>
#include "mlinterface.h"
#include "mainwindow.h"

// ============================================================================
// COMPILE-TIME CONSTANTS
// ============================================================================

// Internal versions for input/output file purposes
#define SPEFILEVERSION 4
#define SPVFILEVERSION 1010

// Legal Stuff
#define COPYRIGHT "Copyright © 2008-2026 Mark D. Sutton, Russell J. Garwood, Alan R.T. Spencer"
#define LICENCE "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the GPL v3 license"

// Programme Name
#define PRODUCTNAME "SPIERSedit"

// Email
#define EMAIL "palaeoware@gmail.com"

// Github
#define GITURL "https://github.com/"
#define GITREPOSITORY "palaeoware/SPIERS"
#define GITISSUE "/issues"

// OpenGL
#define GL_MAJOR 4
#define GL_MINOR 1
#define GL_MAJOR_MAC 4
#define GL_MINOR_MAC 1

// Read the Docs
#define READTHEDOCS "https://spiersedit.readthedocs.io/"

// Pixel manipulation macros (endian-sensitive, not portable)
#define ALPHA(pointer, offset) pointer[offset+3]
#define RED(pointer, offset) pointer[offset+2]
#define GREEN(pointer, offset) pointer[offset+1]
#define BLUE(pointer, offset) pointer[offset]

// Export gridding table scale
#define GRID_SCALE 32

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/**
 * @brief The Segment class — represents a 3D reconstructed anatomical structure
 */
class Segment
{
public:
    Segment(QString name);
    ~Segment();

    QString Name;
    int Colour[3];                          /// RGB color
    int LinPercent[3];                      /// Linearization percentages (RGB)
    int LinGlobal;                          /// Global linearization percentage
    bool LinInvert;                         /// Invert linearization
    double NeighbourBright;                 /// Neighbor brightness parameter
    int NeighbourSparse;                    /// Neighbor sparsity
    bool NeighbourSingle;                   /// Single neighbor mode
    double PolyRedConsts[10];               /// Polynomial color constants (red)
    double PolyGreenConsts[10];             /// Polynomial color constants (green)
    double PolyBlueConsts[10];              /// Polynomial color constants (blue)
    double PolyKall;                        /// Polynomial overall constant
    int PolyOrder;                          /// Polynomial order
    int PolySparse;                         /// Polynomial sparsity
    int PolyRetries;                        /// Polynomial fit retries
    int PolyConverge;                       /// Polynomial convergence threshold
    double PolyScale;                       /// Polynomial scale factor
    int PolyContrast;                       /// Polynomial contrast
    int RangeBase;                          /// Range base value
    int RangeTop;                           /// Range top value
    double RangeGradient;                   /// Range gradient
    double RangeCenter;                     /// Range center
    int ListOrder;                          /// Display list position
    bool Dirty;                             /// Changed on current slice
    bool UndoDirty;                         /// Changed since last undo save
    bool Locked;                            /// Locked from editing
    bool Activated;                         /// Active/visible
    QTreeWidgetItem *widgetitem;            /// Tree widget item pointer
    QGraphicsRectItem *rectitem;            /// Graphics scene rect item
    QGraphicsTextItem *textitem;            /// Graphics scene text item
};

/**
 * @brief The Mask class — overlay mask/layer
 */
class Mask
{
public:
    Mask(QString name);

    QString Name;
    int ForeColour[3];                      /// Foreground RGB color
    int BackColour[3];                      /// Background RGB color
    bool Show;                              /// Visible
    bool Write;                             /// Editable
    int Contrast;                           /// Display contrast
    bool Lock;                              /// Locked from editing
    QTreeWidgetItem *widgetitem;            /// Tree widget item pointer
    int ListOrder;                          /// Display list position
};

/**
 * @brief The PointList class — points on a single slice for a curve
 */
class PointList
{
public:
    PointList();

    QList<double> X;                        /// X coordinates
    QList<double> Y;                        /// Y coordinates
    QByteArray Fixed;                       /// Per-node automatic-interpolation keyframes
    int Count;                              /// Number of points
};

/**
 * @brief The Curve class — annotation curve (spline) across slices
 */
class Curve
{
public:
    Curve(QString name);
    ~Curve();

    QString Name;
    int Colour[3];                          /// RGB color
    bool Closed;                            /// Closed curve
    bool Filled;                            /// Filled region
    int Segment;                            /// Associated segment (segment+1; 0 = none)
    bool AutomaticallyInterpolated;         /// Uses per-node interpolation keyframes
    int AutomaticStartSlice;                /// First managed SplinePoints index
    int AutomaticEndSlice;                  /// Last managed SplinePoints index
    QList<PointList *> SplinePoints;        /// Points on each file/slice
    int ListOrder;                          /// Display list position
    QTreeWidgetItem *widgetitem;            /// Tree widget item pointer
};

/**
 * @brief The OutputObject class — 3D object component for export
 */
class OutputObject
{
public:
    OutputObject(QString name);
    ~OutputObject();

    QString Name;
    int Resample;                           /// Resample percentage
    int Colour[3];                          /// RGB color
    QList<int> ComponentMasks;              /// Included masks
    QList<int> ComponentSegments;           /// Included segments
    QList<int> CurveComponents;             /// Included curves
    int ListOrder;                          /// Display list position
    QTreeWidgetItem *widgetitem;            /// Tree widget item pointer
    bool IsGroup;                           /// Is a group container
    int Parent;                             /// Parent object index (-1 = root)
    uchar Key;                              /// Hotkey
    bool Show;                              /// Visible in output
    bool Merge;                             /// Merge with other objects
    bool Expanded;                          /// Tree expanded state
    bool TempSelected;                      /// Temporary selection state

    // Output-only temporary data
    QByteArray temparray;                   /// Temporary work array
    QByteArray Outputarray;                 /// Output data array
    QList<QByteArray *> CompressedSPVarrays; /// Per-file compressed SPV data
    QList<QByteArray *> GridArrays;        /// Per-file grid arrays
    int bigposfirst;                        /// Output position start
    int bigpos;                             /// Current output position
    int tpos;                               /// Temporary position
    QList<bool> UseMasks;                   /// Per-mask usage flag
    QList<bool> UseSegs;                    /// Per-segment usage flag
    QList<OutputObject *> MergeObjects;     /// Objects to merge with

    void SetUpForRender();
};

/**
 * @brief The RecentFiles struct — entry in recent files list
 */
struct RecentFiles
{
    QString File;                           /// File path
    QString Notes;                          /// Associated notes
};

// ============================================================================
// GLOBAL POINTERS & CORE SERVICES
// ============================================================================

extern MainWindow *mainwin;
extern QTabWidget *tabwidget;
extern MLInterface *mlInterface;
extern QSurfaceFormat surfaceFormat;

// ============================================================================
// SYNCHRONIZATION & STATE
// ============================================================================

extern QRecursiveMutex mutex;
extern bool Active;                         /// Overall program active state
extern bool clearing;                       /// Currently clearing project data
extern bool pausetimers;                    /// Pause timer-based operations
extern bool temptestflag;                   /// Temporary test/debug flag
extern bool bodgeflag;                      /// Temporary bodge/hack flag
extern int Counter;                         /// General-purpose counter

// ============================================================================
// PROJECT FILE MANAGEMENT
// ============================================================================

extern QString openfile;                    /// Current working directory for file dialogs
extern QString currentOpenFileName;         /// Full path to currently open project file
extern QString SettingsFileName;            /// Settings file name (relative)
extern QString FullSettingsFileName;        /// Settings file full path
extern QString DefaultPath;                 /// Default file browser path
extern QList<RecentFiles> RecentFileList;   /// Recently opened projects

// ============================================================================
// PROJECT-SPECIFIC: FILE & IMAGE DATA
// (Should eventually migrate to Project class)
// ============================================================================

extern QStringList Files;                   /// Slice file paths (relative)
extern QStringList FullFiles;               /// Slice file full paths
extern int FileCount;                       /// Total number of slice files
extern int CurrentFile;                     /// Currently loaded slice index

extern QList<QImage *> GA;                  /// Grey array images (one per slice)
extern QImage ColArray;                     /// Current color source image
extern QByteArray GradientArray;            /// Gradient modifier array
extern QByteArray FeaturesByteArray;        /// ML-extracted features
extern QByteArray Masks;                    /// Binary mask data
extern QByteArray Locks;                    /// Binary lock pixel data
extern QByteArray dirty;                    /// Binary dirty pixel tracking
extern QByteArray SampleArray;              /// Per-project sample data buffer
extern int SampleArraySize;                 /// Size of sample array

extern int cwidth, cheight;                 /// Color source image dimensions
extern int fwidth, fheight;                 /// Mono output image dimensions
extern int cwidth4, fwidth4;                /// Width×4 (for pixel arithmetic)
extern int ColMonoScale;                    /// Resample/binning factor (source→output)

extern QList<double> Stretches;             /// Per-slice pixel stretch mapping
extern QList<double> FullStretches;         /// Full resolution stretch mapping
extern int zsparsity;                       /// Slice sampling sparsity factor
extern double SkewDown, SkewLeft;           /// Image alignment skew parameters
extern double PixPerMM;                     /// Pixels per millimeter calibration
extern double SlicePerMM;                   /// Slices per millimeter calibration

extern bool GreyImage;                      /// Flag: project is greyscale (not RGB)
extern QString Notes;                       /// Project notes/description
extern QString FileNotes;                   /// File-level notes
extern QVector<int> SegmentMap;             /// Per-project segment mapping table

// ============================================================================
// PROJECT-SPECIFIC: DISPLAY & RENDERING STATE
// (Should eventually migrate to Project class)
// ============================================================================

extern double CurrentZoom;                  /// Current zoom level of slice viewer
extern int CurrentMode;                     /// Editing mode (0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc)
extern int CurrentSegment;                  /// Currently selected segment index
extern int CurrentRSegment;                 /// Currently selected range segment
extern int Trans;                           /// Transparency for color source display
extern int CMin, CMax;                      /// Contrast min/max for display
extern int LastTrans;                       /// Last transparency value used
extern double CurrentPolyContrast;          /// Current polynomial contrast rendering param

extern QList<bool> FilesDirty;              /// Per-file dirty flag for re-rendering

// ============================================================================
// PROJECT-SPECIFIC: SEGMENT/MASK/CURVE/OUTPUT DATA
// (Should eventually migrate to Project class)
// ============================================================================

extern int SegmentCount;                    /// Total number of segments
extern int MaxUsedMask;                     /// Highest mask index in use
extern int SelectedMask;                    /// Currently selected mask index
extern int SelectedRMask;                   /// Currently selected range mask
extern int SelectedCurve;                   /// Currently selected curve index
extern int CurveCount;                      /// Total number of curves
extern int OutputObjectsCount;              /// Total number of output objects

extern QList<Segment *> Segments;           /// Segment objects with metadata
extern QList<Mask *> MasksSettings;         /// Mask settings objects
extern QList<Curve *> Curves;               /// Curve annotation objects
extern QList<OutputObject *> OutputObjects; /// 3D output objects for export

// ============================================================================
// PROJECT-SPECIFIC: DIRTY/UNDO TRACKING
// (Should eventually migrate to Project class)
// ============================================================================

extern bool MasksDirty;                     /// Masks changed since last file save
extern bool LocksDirty;                     /// Locks changed since last file save
extern bool CurvesDirty;                    /// Curves changed since last file save
extern bool MasksUndoDirty;                 /// Masks changed since last undo save
extern bool LocksUndoDirty;                 /// Locks changed since last undo save
extern bool CurvesUndoDirty;                /// Curves changed since last undo save
extern bool HorribleBodgeFlagDontStoreUndo; /// Prevent undo save for this operation

// ============================================================================
// PROJECT-SPECIFIC: OUTPUT GENERATION SETTINGS
// (Should eventually migrate to Project class)
// ============================================================================

extern bool RangeHardFill;                  /// Output: use hard fill mode
extern bool RangeSelectedOnly;              /// Output: only selected range
extern bool OutputMirroring;                /// Output: mirror geometry
extern int FirstOutputFile;                 /// Output file range start
extern int LastOutputFile;                  /// Output file range end
extern int PixSens;                         /// Output: pixel sensitivity
extern int XYDownsample, ZDownsample;       /// Output: downsampling factors
extern int MaxTriangles;                    /// Output: triangle limit

// ============================================================================
// EDITOR TOOL STATE
// ============================================================================

extern int Brush_Size;                      /// Current brush size
extern int BrushY, BrushZ;                  /// Brush position
extern bool SquareBrush;                    /// Brush shape: square vs circular
extern int BrightUp, BrightDown, BrightSoft; /// Brightness tool parameters
extern int LCE_Boost, LCE_Radius, LCE_Adjust; /// Local contrast enhancement params
extern bool HiddenMasksLockedForGeneration; /// Output: hidden masks locked
extern bool SegmentBrushAppliesMasks;       /// Segment brush affects masks too
extern bool SegmentBrushAppliesLocks;       /// Segment brush affects locks too
extern bool SegmentsLocked;                 /// Global segment lock
extern bool CurveMarkersAsCrosses;          /// Render curve markers as crosses
extern bool CurveShapeLocked;               /// Prevent curve shape modification
extern bool MasksMoveBack, MasksMoveForward; /// Mask navigation direction flags

// ============================================================================
// UI INTERACTION STATE
// ============================================================================

extern int LastMouseX, LastMouseY;          /// Last mouse position
extern bool ThreshFlag, MasksFlag, SegsFlag; /// UI display flags
extern bool MoveFlag, ChangeFlag;           /// Interaction state flags
extern bool NoUpdateSelectionFlag;          /// Suppress selection updates
extern bool escapeFlag;                     /// Escape key pressed
extern bool previewGradient;                /// Preview gradient before apply
extern int GradientDensity;                 /// Gradient preview density
extern int GradientMinDist, GradientMinDistValue; /// Gradient min distance settings
extern int GradientMaxDist, GradientMaxDistValue; /// Gradient max distance settings
extern bool OutputRegroupMode;              /// Output object regroup mode
extern bool ShowSlicePosition;              /// Show slice position display
extern bool ThreeDmode;                     /// 3D preview mode active
extern double yaw, pitch, roll;             /// 3D camera orientation
extern double k;                            /// Temporary calculation variable
extern double LastZoom;                     /// Last recorded zoom level

extern QGraphicsPixmapItem *MainImage;      /// Main canvas image item
extern QTreeWidgetItem *LastItemClicked;    /// Last clicked tree widget item
extern QTime LastTimeClicked;               /// Time of last click
extern int LastColumnClicked;               /// Last clicked column index

extern double RedConsts[10];                /// Red channel constants (historical)
extern double GreenConsts[10];              /// Green channel constants (historical)
extern double BlueConsts[10];               /// Blue channel constants (historical)

// ============================================================================
// PROGRAM SETTINGS (Persisted via QSettings)
// ============================================================================

extern int CacheCompressionLevel;           /// Cache compression (0-9)
extern int FileCompressionLevel;            /// File compression (0-9)
extern int CacheMem;                        /// Cache memory limit (MB)
extern int CacheMemMLGb;                    /// ML cache memory limit (GB)
extern int UndoMem;                         /// Undo memory limit (MB)
extern int UndoTimerSetting;                /// Auto-undo save timer (seconds)
extern int AutoSaveFrequency;               /// Auto-save frequency (minutes)
extern bool BackgroundCacheFilling;         /// Enable background cache generation
extern bool RenderCache;                    /// Enable render caching

// ============================================================================
// UI MENU STATE (Persisted via QSettings)
// ============================================================================

extern bool MenuHistSelectedOnly;
extern bool MenuHistChecked;
extern bool MenuInfoChecked;
extern bool MenuGenChecked;
extern bool MenuMasksChecked;
extern bool MenuSegsChecked;
extern bool MenuCurvesChecked;
extern bool MenuOutputChecked;
extern bool MenuToolboxChecked;
extern bool MenuSliceSelectorChecked;
extern bool Menu3DPreviewChecked;

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

extern void Error(QString message);
extern void Message(QString message);
extern void Message1(char *message, int x);
extern int randn(int n);

extern void WriteSuperGlobals();            /// Save app settings to QSettings
extern void ReadSuperGlobals();             /// Load app settings from QSettings
extern void RecentFile(QString fname);      /// Add file to recent list
extern bool IsDatasetLoaded();              /// Check if project data loaded
extern void ResetFilesDirty();              /// Reset per-file dirty flags

#endif // __GLOBALS_H__
