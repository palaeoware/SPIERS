#pragma once

#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QColor>
#include "opencv2/core.hpp"
#include "mlfeature.h"

class MLCachedSlice;

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
    bool GetSourceColour();
    void CalculateFeature(cv::Mat &mat, int sliceIndex, int featureID);
    cv::Mat GetWholeSliceIntensity(int sliceIndex);
    cv::Mat GetWholeSliceFeature(int z, int featureIndex);
    void SetFeatureInUse(int featureID, bool inUse);
    QList<int> GetFeaturesInUse();
    uint64 timeStamp;
    void DumpFeatures();
    int GetIndexForFeature(MLFeature *feature);
    void ClearFeatures();
    void SetFeatures(QList<MLFeature *> newFeatures);
    void Reset();
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

