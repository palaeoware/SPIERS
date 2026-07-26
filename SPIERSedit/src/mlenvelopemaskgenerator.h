/**
 * @file
 * Header: Mlenvelopemaskgenerator
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
#ifndef MLENVELOPEMASKGENERATOR_H
#define MLENVELOPEMASKGENERATOR_H

#include <QImage>
#include <QList>
#include <QString>

#include <memory>

class MLCachedAccess;
namespace cv
{
class Mat;
}

struct MLEnvelopeMaskParameters
{
    int smoothingRadius = 2;
    int closingRadius = 5;
    int sensitivity = 80;
    int expansionRadius = 1;
};

class MLEnvelopeMaskGenerator
{
public:
    MLEnvelopeMaskGenerator(MLCachedAccess *normalCache, int segmentId, const MLEnvelopeMaskParameters &parameters);
    ~MLEnvelopeMaskGenerator();

    bool generatePreview(int sliceId, QImage &evidenceImage, QImage &boundaryImage, QImage &filledImage);
    bool stageSlices(const QList<int> &sliceIds, const QString &directoryPath);
    QString errorMessage() const;

    static QString stagedSliceFileName(const QString &directoryPath, int sliceId);

private:
    bool createFilledSlice(int sliceId, cv::Mat &filled);
    void cleanCacheFiles();
    void setError(const QString &message);

    QString cachePrefix;
    QString lastError;
    int closedFeatureIndex = -1;
    int membershipFeatureIndex = -1;
    int runToken = 0;
    MLEnvelopeMaskParameters parameters;
    std::unique_ptr<MLCachedAccess> processingCache;
    bool ownsExclusiveCacheBudget = false;
};

#endif // MLENVELOPEMASKGENERATOR_H
