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

private:
    void FetchSourceData();
    void FetchFeatureData(int feature);


};
#endif // MLCACHEDSLICE_H
