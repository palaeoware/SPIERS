/**
 * @file
 * Header: Mlinterface
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
#ifndef MLINTERFACE_H
#define MLINTERFACE_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include "mlcachedaccess.h"
#include "mainwindow.h"

class MLFeatureUIManager;
class MLAddFeature;
class LabelledPoint;
class MLParallelForest;
class MLROISlice;

class MLInterface
{
public:
    MLInterface();
    static bool TestML();
    static bool enabled;
    void SampleAndTrain(bool autoGen = false);
    void CalculateFeatureData();
    void GetProbabilitiesAllSegments(int x, int y, int z, int *segBuffer);
    void Generate(QListWidget *SliceSelectorList);
    void UIActivateSelectedFeatures(bool activate);
    void UIDeleteSelectedFeatures();
    void UIAddFeature();
    void SetSamplePercent(int v);
    void SetMinSampleCount(int v);
    void SetTreeCount(int v);
    void SetTreeDepth(int v);
    int GetSamplePercent();
    int GetMinSampleCount();
    int GetTreeCount();
    int GetTreeDepth();

    void Initialise(MainWindow *mw,QLabel *statusLabel);
    void RemoveAllCacheFiles(bool override);
    QByteArray DumpFeaturesToByteArray();
    void RetrieveFeaturesFromByteArray(QByteArray &byteArray);
    void SaveFeaturesToFile();
    void LoadFeaturesFromFile();
    void ResetRFAndSample();
    void ResetCachedData();
    void MaskDataChanged();
    int RetargetMaskFeatures(const QVector<int> &maskMap, const QList<int> &deletedMaskIds);
    void AutoSampleTrainAndGenerate();
    void ResizeCache();
    void DoPreset(int presetCode);
    bool FloodFillMask(
        int x,
        int y,
        int maskId,
        int seedRadius,
        int segmentationInfluencePercent,
        int grabCutIterations,
        bool fillHoles,
        QString *errorMessage);
private:
    bool dataComputed;
    int currentSlice;
    std::unique_ptr<MLParallelForest> rf;
    MLCachedAccess *data;
    void CreateSingletonsIfNeeded();
    void ComputeSliceProbabilitiesFromVotes(int sliceID);
    MLFeatureUIManager *uiManager;
    MLAddFeature *addFeatureDialog;
    int samplePercent, minSampleCount, treeCount, treeDepth;
    void UpdateStatusLabel();

    QLabel *lblStatus;
    MainWindow *mainWin;
    QString DescribeSample();
    QVector<LabelledPoint> labels;
    bool Sample(bool incremental, bool noMessages);
    bool Train(bool noMessages);
    void DoImportances();


    //Probabity cache system for recalc brush
    cv::Mat cachedSliceProbabilities;
    QByteArray cachedProbabilityExcludedPixels;
    int cachedProbabilitySliceID = -1;
    bool cachedProbabilitySliceValid = false;
    bool cachedProbabilityRestricted = false;

    void InvalidateProbabilityCache();
    void RemoveDistanceToMaskCacheFiles();
    bool BuildSliceSampleMatrix(
        int sliceID,
        cv::Mat &samples,
        const MLROISlice *roi = nullptr);
    bool EnsureSliceProbabilityCache(
        int sliceID,
        const MLROISlice *roi = nullptr);
};

#endif // MLINTERFACE_H
