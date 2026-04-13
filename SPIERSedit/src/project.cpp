/**
 * @file
 * Implementation: Project data encapsulation class
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

#include "project.h"

#include <QMutexLocker>

/**
 *
 * Constructor: Initialize all data members to null/empty state.
 *
 **/
Project::Project(QObject *parent)
    : QObject(parent),
      m_currentFile(0),
      m_segmentCount(0),
      m_curveCount(0),
      m_maxUsedMask(-1),
      m_currentZoom(1.0),
      m_currentMode(0),
      m_currentSegment(-1),
      m_trans(255),
      m_cmin(0),
      m_cmax(255),
      m_pixPerMM(1.0),
      m_slicePerMM(1.0)
{
}

/**
 *
 * Destructor: Clean up all owned resources.
 *
 **/
Project::~Project()
{
    clear();
}

// ---- Image Data Accessors ----

QList<QImage *> Project::gaImages() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_gaImages;  // Return by-value for thread safety
}

void Project::setGAImage(int index, QImage *image)
{
    QMutexLocker locker(&m_dataMutex);
    if (index >= 0 && index < m_gaImages.size())
    {
        if (m_gaImages[index] != nullptr)
        {
            delete m_gaImages[index];
        }
        m_gaImages[index] = image;
    }
}

QImage *Project::getGAImage(int index) const
{
    QMutexLocker locker(&m_dataMutex);
    if (index >= 0 && index < m_gaImages.size())
    {
        return m_gaImages[index];
    }
    return nullptr;
}

void Project::clearGAImages()
{
    QMutexLocker locker(&m_dataMutex);
    // Delete all QImage pointers before clearing
    for (QImage *img : m_gaImages)
    {
        if (img != nullptr)
        {
            delete img;
        }
    }
    m_gaImages.clear();
}

// ---- Segment Accessors ----

QList<Segment *> Project::segments() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_segments;
}

QList<Segment *> &Project::segmentsRef()
{
    // Return mutable reference - caller must hold lock or ensure single-threaded access
    return m_segments;
}

void Project::addSegment(Segment *segment)
{
    QMutexLocker locker(&m_dataMutex);
    if (segment != nullptr)
    {
        m_segments.append(segment);
    }
}

void Project::removeSegment(Segment *segment)
{
    QMutexLocker locker(&m_dataMutex);
    m_segments.removeAll(segment);
}

// ---- Mask Settings Accessors ----

QList<Mask *> Project::maskSettings() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_maskSettings;
}

QList<Mask *> &Project::maskSettingsRef()
{
    return m_maskSettings;
}

void Project::addMaskSetting(Mask *mask)
{
    QMutexLocker locker(&m_dataMutex);
    if (mask != nullptr)
    {
        m_maskSettings.append(mask);
    }
}

void Project::removeMaskSetting(Mask *mask)
{
    QMutexLocker locker(&m_dataMutex);
    m_maskSettings.removeAll(mask);
}

// ---- Curve Accessors ----

QList<Curve *> Project::curves() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_curves;
}

QList<Curve *> &Project::curvesRef()
{
    return m_curves;
}

void Project::addCurve(Curve *curve)
{
    QMutexLocker locker(&m_dataMutex);
    if (curve != nullptr)
    {
        m_curves.append(curve);
    }
}

void Project::removeCurve(Curve *curve)
{
    QMutexLocker locker(&m_dataMutex);
    m_curves.removeAll(curve);
}

// ---- Output Object Accessors ----

QList<OutputObject *> Project::outputObjects() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_outputObjects;
}

QList<OutputObject *> &Project::outputObjectsRef()
{
    return m_outputObjects;
}

void Project::addOutputObject(OutputObject *obj)
{
    QMutexLocker locker(&m_dataMutex);
    if (obj != nullptr)
    {
        m_outputObjects.append(obj);
    }
}

void Project::removeOutputObject(OutputObject *obj)
{
    QMutexLocker locker(&m_dataMutex);
    m_outputObjects.removeAll(obj);
}

// ---- File List Accessors ----

QStringList Project::files() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_files;
}

void Project::setFiles(const QStringList &files)
{
    QMutexLocker locker(&m_dataMutex);
    m_files = files;
}

QStringList Project::fullFiles() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_fullFiles;
}

void Project::setFullFiles(const QStringList &fullFiles)
{
    QMutexLocker locker(&m_dataMutex);
    m_fullFiles = fullFiles;
}

// ---- Current File Accessors ----

int Project::currentFile() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentFile;
}

void Project::setCurrentFile(int index)
{
    QMutexLocker locker(&m_dataMutex);
    m_currentFile = index;
}

// ---- Count Accessors ----

int Project::segmentCount() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_segmentCount;
}

void Project::setSegmentCount(int count)
{
    QMutexLocker locker(&m_dataMutex);
    m_segmentCount = count;
}

int Project::curveCount() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_curveCount;
}

void Project::setCurveCount(int count)
{
    QMutexLocker locker(&m_dataMutex);
    m_curveCount = count;
}

int Project::maxUsedMask() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_maxUsedMask;
}

void Project::setMaxUsedMask(int index)
{
    QMutexLocker locker(&m_dataMutex);
    m_maxUsedMask = index;
}

// ---- Dirty/State Flags ----

QList<bool> Project::filesDirty() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_filesDirty;
}

void Project::setFileDirty(int fileIndex, bool dirty)
{
    QMutexLocker locker(&m_dataMutex);
    if (fileIndex >= 0 && fileIndex < m_filesDirty.size())
    {
        m_filesDirty[fileIndex] = dirty;
    }
}

void Project::markAllFilesDirty()
{
    QMutexLocker locker(&m_dataMutex);
    for (int i = 0; i < m_filesDirty.size(); ++i)
    {
        m_filesDirty[i] = true;
    }
}

void Project::clearAllDirtyFlags()
{
    QMutexLocker locker(&m_dataMutex);
    for (int i = 0; i < m_filesDirty.size(); ++i)
    {
        m_filesDirty[i] = false;
    }
}

// ---- Rendering/Display Accessors ----

double Project::currentZoom() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentZoom;
}

void Project::setCurrentZoom(double zoom)
{
    QMutexLocker locker(&m_dataMutex);
    m_currentZoom = zoom;
}

int Project::currentMode() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentMode;
}

void Project::setCurrentMode(int mode)
{
    QMutexLocker locker(&m_dataMutex);
    m_currentMode = mode;
}

int Project::currentSegment() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentSegment;
}

void Project::setCurrentSegment(int index)
{
    QMutexLocker locker(&m_dataMutex);
    m_currentSegment = index;
}

int Project::transparency() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_trans;
}

void Project::setTransparency(int trans)
{
    QMutexLocker locker(&m_dataMutex);
    m_trans = trans;
}

int Project::contrastMin() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_cmin;
}

void Project::setContrastMin(int cmin)
{
    QMutexLocker locker(&m_dataMutex);
    m_cmin = cmin;
}

int Project::contrastMax() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_cmax;
}

void Project::setContrastMax(int cmax)
{
    QMutexLocker locker(&m_dataMutex);
    m_cmax = cmax;
}

// ---- Binary Data Accessors ----

QByteArray Project::maskBinaryData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_maskBinaryData;
}

void Project::setMaskBinaryData(const QByteArray &data)
{
    QMutexLocker locker(&m_dataMutex);
    m_maskBinaryData = data;
}

QByteArray Project::lockBinaryData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_lockBinaryData;
}

void Project::setLockBinaryData(const QByteArray &data)
{
    QMutexLocker locker(&m_dataMutex);
    m_lockBinaryData = data;
}

QByteArray Project::dirtyBinaryData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_dirtyBinaryData;
}

void Project::setDirtyBinaryData(const QByteArray &data)
{
    QMutexLocker locker(&m_dataMutex);
    m_dirtyBinaryData = data;
}

// ---- Calibration Accessors ----

double Project::pixPerMM() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_pixPerMM;
}

void Project::setPixPerMM(double pix)
{
    QMutexLocker locker(&m_dataMutex);
    m_pixPerMM = pix;
}

double Project::slicePerMM() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_slicePerMM;
}

void Project::setSlicePerMM(double slice)
{
    QMutexLocker locker(&m_dataMutex);
    m_slicePerMM = slice;
}

// ---- Metadata Accessors ----

QString Project::notes() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_notes;
}

void Project::setNotes(const QString &notes)
{
    QMutexLocker locker(&m_dataMutex);
    m_notes = notes;
}

QString Project::settingsFileName() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_settingsFileName;
}

void Project::setSettingsFileName(const QString &filename)
{
    QMutexLocker locker(&m_dataMutex);
    m_settingsFileName = filename;
}

QString Project::fullSettingsFileName() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_fullSettingsFileName;
}

void Project::setFullSettingsFileName(const QString &filename)
{
    QMutexLocker locker(&m_dataMutex);
    m_fullSettingsFileName = filename;
}

// ---- Project Lifecycle ----

void Project::clear()
{
    QMutexLocker locker(&m_dataMutex);

    // Delete all dynamically-allocated objects
    // (lists own the memory; loop and delete before clearing)
    for (QImage *img : m_gaImages)
    {
        if (img != nullptr)
        {
            delete img;
        }
    }
    m_gaImages.clear();

    // Do NOT delete Segment/Mask/Curve/OutputObject pointers here
    // The lists are cleared, but caller is responsible for deletion
    // (This matches current behavior where qDeleteAll is called elsewhere)
    m_segments.clear();
    m_maskSettings.clear();
    m_curves.clear();
    m_outputObjects.clear();

    // Clear file lists
    m_files.clear();
    m_fullFiles.clear();

    // Reset state to defaults
    m_currentFile = 0;
    m_segmentCount = 0;
    m_curveCount = 0;
    m_maxUsedMask = -1;
    m_filesDirty.clear();

    m_currentZoom = 1.0;
    m_currentMode = 0;
    m_currentSegment = -1;

    m_trans = 255;
    m_cmin = 0;
    m_cmax = 255;

    // Clear binary data
    m_maskBinaryData.clear();
    m_lockBinaryData.clear();
    m_dirtyBinaryData.clear();

    // Reset calibration
    m_pixPerMM = 1.0;
    m_slicePerMM = 1.0;

    // Clear metadata
    m_notes.clear();
    m_settingsFileName.clear();
    m_fullSettingsFileName.clear();
}

bool Project::isEmpty() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_gaImages.isEmpty() && m_segments.isEmpty() && m_files.isEmpty();
}
