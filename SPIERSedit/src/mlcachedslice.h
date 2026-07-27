/**
 * @file
 * Header: Mlcachedslice
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
#ifndef MLCACHEDSLICE_H
#define MLCACHEDSLICE_H

#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QColor>
#include "opencv2/core.hpp"
#include "mlfeature.h"

class MLCachedAccess;

class MLCachedSlice
{
public:
    MLCachedSlice(int featureCount, int zIndex, MLCachedAccess *parent);
    ~MLCachedSlice();
    cv::Mat sourceImage;
    QList<cv::Mat> featureData;
    uint64 lastUsed;
    int activeFetchCount;
    bool sourceValid;
    int sliceIndex;
    MLCachedAccess *cache;
    QList<bool> featuresValid;
    void AddFeature();
    void RemoveFeature(int index);
    void Clear();
    float GetFeatureData(int x, int y, int feature);
    float GetIntensityGrey(int x, int y);
    QColor GetColor(int x, int y);
    void FetchSourceDataIfNeeded();
    void FetchFeatureIfNeeded(int featureIndex);

    void RemoveAllFeatures();
private:
    void FetchSourceData();
    void FetchFeatureData(int feature);


};
#endif // MLCACHEDSLICE_H
