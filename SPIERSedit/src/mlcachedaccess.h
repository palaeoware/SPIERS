#pragma once

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
    cv::Mat sourceImage;
    QList<cv::Mat> featureData;
    QDateTime lastUsed;
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

class MLCachedAccess
{
public:
    MLCachedAccess(int sliceCount, bool colourImages, int fwidth, int fheight, int _xyBin, int _zBin);
    int GetIndexForFeature(MLFeature::FeatureType type, MLFeature::Channel channel, bool is3D, int arg1, int arg2);
    int AddFeature(MLFeature *feature);
    bool RemoveFeature(int featureIndex);  //should take an index
    float GetFeatureValueAt(int x, int y, int z, int featureID);
    float GetIntensityAsFloat(int x, int y, int z);
    QColor GetRGBFloat(int x, int y, int z);
    void SetMaxMemoryUsage(uint64 size);
    QString GetSourceImageFeatureName();
    MLFeature *GetFeature(int featureID);  //replace with either pretty or filename calls. Do need a
                                            //fn that returns the MLFeature itself though
    int GetFeatureCount();
    int GetXSize();
    int GetYSize();
    bool GetSourceColour();
    void CalculateFeature(cv::Mat &mat, int sliceIndex, int featureID);
    cv::Mat GetWholeSliceIntensity(int sliceIndex);
    cv::Mat GetWholeSliceFeature(int z, int featureIndex);
    void SetFeatureInUse(int featureID, bool inUse);
    QList<int> GetFeaturesInUse();
    void DumpFeatures();
    int GetIndexForFeature(MLFeature *feature);
private:
    ulong GetMemorySizeOfSlice();
    void ResizeCache();
    int AssignCacheSlot(int sliceIndex);
    MLCachedSlice * GetSlice(int sliceIndex);
    uint64 maxMemoryUsage;

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

