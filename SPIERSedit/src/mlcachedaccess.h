/**
 * @file
 * Header: Mlcachedaccess
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
#pragma once

#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QColor>
#include "opencv2/core.hpp"
#include "mlfeature.h"

class MLCachedSlice;
class MLROISlice;

class MLCachedAccess
{
public:
    MLCachedAccess(int sliceCount, bool colourImages, int fwidth, int fheight, int _xyBin, int _zBin);
    ~MLCachedAccess();
    int GetIndexForFeature(MLFeature::FeatureType type, MLFeature::Channel channel, bool is3D, int arg1, int arg2);
    int AddFeature(MLFeature *feature);
    bool RemoveFeature(int featureIndex);  //should take an index
    float GetFeatureValueAt(int x, int y, int z, int featureID);
    float GetIntensityAsFloat(int x, int y, int z);
    QColor GetRGBFloat(int x, int y, int z);
    QString GetSourceImageFeatureName();
    MLFeature *GetFeature(int featureID);  //replace with either pretty or filename calls. Do need a
                                            //fn that returns the MLFeature itself though
    int GetFeatureCount();
    int GetXSize();
    int GetYSize();
    int GetFeatureTileCount();
    bool GetSourceColour();
    void CalculateFeature(cv::Mat &mat, int sliceIndex, int featureID);
    bool CalculateFeatureROI(
        cv::Mat &mat,
        int sliceIndex,
        int featureID,
        const MLROISlice &roi);

    cv::Mat GetWholeSliceFeature(int z, int featureIndex);
    cv::Mat GetROISliceFeature(
        int z,
        int featureIndex,
        const MLROISlice &roi);
    void SetFeatureInUse(int featureID, bool inUse);
    QList<int> GetFeaturesInUse();
    int GetRequiredXYHalo();
    uint64 timeStamp;
    void DumpFeatures();
    int GetIndexForFeature(MLFeature *feature);
    void ClearFeatures();
    void SetFeatures(QList<MLFeature *> newFeatures);
    void Reset();
    void ReleaseCacheMemoryForExclusiveOperation();
    void IncrementTimestamp();
    void ResizeCache();
private:
    uint64_t GetMemorySizeOfSlice();

    int AssignCacheSlot(int sliceIndex);
    MLCachedSlice * GetSlice(int sliceIndex);

    bool sourceImageRGB;
    QList<MLCachedSlice *> cachedSlices;
    QList<int> slicesByCacheIndex;
    QList<int> cacheIndicesBySlice;
    QList<MLFeature*> features;
    QList<int> featureIDsInUse;
    void RebuildFeatureIDsInUse();

    int xyBin, zBin, xSize, ySize, zSize;
    int featureSize, sourceImageSize;


    int FindReusableCacheSlot();
    bool IsFeatureADependency(MLFeature *feature);

};

