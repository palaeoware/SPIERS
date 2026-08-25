/**
 * @file
 * Header: Project data encapsulation class
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QByteArray>
#include <QRecursiveMutex>

class Segment;
class Mask;
class Curve;
class OutputObject;

/**
 *
 * Project encapsulates all per-project data that was previously stored in global
 * variables. This includes image data, segment/mask/curve definitions, project
 * metadata, and rendering state.
 *
 * Thread-safe: All data access is protected by QRecursiveMutex.
 *
 **/
class Project : public QObject
{
    Q_OBJECT

public:
    explicit Project(QObject *parent = nullptr);
    ~Project();

    // Prevent copying
    Project(const Project &) = delete;
    Project &operator=(const Project &) = delete;

    // ---- Image Data Accessors ----

    /// Get grey array images (shallow copy for thread safety)
    QList<QImage *> gaImages() const;
    /// Set single GA image at index
    void setGAImage(int index, QImage *image);
    /// Get single GA image at index (thread-safe)
    QImage *getGAImage(int index) const;
    /// Clear all GA images
    void clearGAImages();

    // ---- Segment/Mask/Curve/Output Object Accessors ----

    /// Get segments list
    QList<Segment *> segments() const;
    /// Get mutable segments list (caller responsible for thread safety)
    QList<Segment *> &segmentsRef();
    /// Add segment to list
    void addSegment(Segment *segment);
    /// Remove segment from list (does not delete)
    void removeSegment(Segment *segment);

    /// Get mask settings list
    QList<Mask *> maskSettings() const;
    /// Get mutable mask settings list
    QList<Mask *> &maskSettingsRef();
    /// Add mask to settings list
    void addMaskSetting(Mask *mask);
    /// Remove mask from settings list (does not delete)
    void removeMaskSetting(Mask *mask);

    /// Get curves list
    QList<Curve *> curves() const;
    /// Get mutable curves list
    QList<Curve *> &curvesRef();
    /// Add curve to list
    void addCurve(Curve *curve);
    /// Remove curve from list (does not delete)
    void removeCurve(Curve *curve);

    /// Get output objects list
    QList<OutputObject *> outputObjects() const;
    /// Get mutable output objects list
    QList<OutputObject *> &outputObjectsRef();
    /// Add output object to list
    void addOutputObject(OutputObject *obj);
    /// Remove output object from list (does not delete)
    void removeOutputObject(OutputObject *obj);

    // ---- File List Accessors ----

    /// Get slice file paths (relative)
    QStringList files() const;
    /// Set slice file paths
    void setFiles(const QStringList &files);
    /// Get full file paths
    QStringList fullFiles() const;
    /// Set full file paths
    void setFullFiles(const QStringList &fullFiles);

    // ---- Current File / Index Accessors ----

    /// Get current file index being edited
    int currentFile() const;
    /// Set current file index
    void setCurrentFile(int index);

    // ---- Count Accessors ----

    /// Get total segment count
    int segmentCount() const;
    /// Set total segment count
    void setSegmentCount(int count);

    /// Get total curve count
    int curveCount() const;
    /// Set total curve count
    void setCurveCount(int count);

    /// Get highest used mask index
    int maxUsedMask() const;
    /// Set highest used mask index
    void setMaxUsedMask(int index);

    // ---- Dirty/State Flags ----

    /// Get dirty flags for all files
    QList<bool> filesDirty() const;
    /// Set dirty flag for single file
    void setFileDirty(int fileIndex, bool dirty);
    /// Mark all files dirty
    void markAllFilesDirty();
    /// Clear all dirty flags
    void clearAllDirtyFlags();

    // ---- Rendering/Display Accessors ----

    /// Get current zoom level
    double currentZoom() const;
    /// Set current zoom level
    void setCurrentZoom(double zoom);

    /// Get current editing mode (0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc)
    int currentMode() const;
    /// Set current editing mode
    void setCurrentMode(int mode);

    /// Get current selected segment index
    int currentSegment() const;
    /// Set current selected segment index
    void setCurrentSegment(int index);

    /// Get transparency setting
    int transparency() const;
    /// Set transparency setting
    void setTransparency(int trans);

    /// Get contrast minimum
    int contrastMin() const;
    /// Set contrast minimum
    void setContrastMin(int cmin);

    /// Get contrast maximum
    int contrastMax() const;
    /// Set contrast maximum
    void setContrastMax(int cmax);

    // ---- Binary Data Accessors ----

    /// Get binary mask data
    QByteArray maskBinaryData() const;
    /// Set binary mask data
    void setMaskBinaryData(const QByteArray &data);

    /// Get binary lock data
    QByteArray lockBinaryData() const;
    /// Set binary lock data
    void setLockBinaryData(const QByteArray &data);

    /// Get binary dirty pixel tracking data
    QByteArray dirtyBinaryData() const;
    /// Set binary dirty pixel tracking data
    void setDirtyBinaryData(const QByteArray &data);

    // ---- Calibration Accessors ----

    /// Get pixels per millimeter
    double pixPerMM() const;
    /// Set pixels per millimeter
    void setPixPerMM(double pix);

    /// Get slices per millimeter
    double slicePerMM() const;
    /// Set slices per millimeter
    void setSlicePerMM(double slice);

    // ---- Metadata Accessors ----

    /// Get project notes
    QString notes() const;
    /// Set project notes
    void setNotes(const QString &notes);

    /// Get settings file name (relative)
    QString settingsFileName() const;
    /// Set settings file name
    void setSettingsFileName(const QString &filename);

    /// Get full settings file path
    QString fullSettingsFileName() const;
    /// Set full settings file path
    void setFullSettingsFileName(const QString &filename);

    // ---- Project Lifecycle ----

    /// Clear all project data to empty/null state (safe for reuse)
    void clear();

    /// Check if project has data
    bool isEmpty() const;

signals:
    /// Emitted when project data changes
    void projectChanged();
    /// Emitted when segment is added
    void segmentAdded(int index);
    /// Emitted when segment is removed
    void segmentRemoved(int index);
    /// Emitted when segment is modified
    void segmentModified(int index);

private:
    /// Protect all member data from concurrent access
    mutable QRecursiveMutex m_dataMutex;

    // ---- Project Data Members ----

    /// Grey array images - one per segment
    QList<QImage *> m_gaImages;
    /// Segment objects with metadata
    QList<Segment *> m_segments;
    /// Mask settings objects
    QList<Mask *> m_maskSettings;
    /// Curve annotation objects
    QList<Curve *> m_curves;
    /// 3D output objects for export
    QList<OutputObject *> m_outputObjects;

    /// Image slice file paths (relative)
    QStringList m_files;
    /// Full paths to image slice files
    QStringList m_fullFiles;


    /// Current slice being edited (index into Files list)
    int m_currentFile;
    /// Total number of segments
    int m_segmentCount;
    /// Total number of curves
    int m_curveCount;
    /// Highest mask index in use
    int m_maxUsedMask;

    /// Dirty flags for each slice file (true if needs re-rendering)
    QList<bool> m_filesDirty;

    /// Current zoom level of slice viewer
    double m_currentZoom;
    /// Current editing mode (0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc)
    int m_currentMode;
    /// Index of currently selected segment
    int m_currentSegment;

    /// Transparency setting for color images
    int m_trans;
    /// Contrast minimum for color images
    int m_cmin;
    /// Contrast maximum for color images
    int m_cmax;

    /// Binary mask data
    QByteArray m_maskBinaryData;
    /// Binary lock pixel data
    QByteArray m_lockBinaryData;
    /// Binary dirty pixel tracking
    QByteArray m_dirtyBinaryData;

    /// Pixels per millimeter calibration
    double m_pixPerMM;
    /// Slices per millimeter calibration
    double m_slicePerMM;

    /// Project notes/description
    QString m_notes;
    /// Current project settings file name (relative)
    QString m_settingsFileName;
    /// Full path to current project settings file
    QString m_fullSettingsFileName;
};

#endif // PROJECT_H
