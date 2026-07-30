/**
 * @file
 * Source: Mlcachedaccess
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
#include "mlcachedaccess.h"
#include "mlcachedslice.h"

#include "mlfileio.h"

#include "globals.h"

#include <functional>
#include <limits>
#include <QSet>
#include <stdexcept>

//Public API

MLCachedAccess::MLCachedAccess(int sliceCount, bool colourImages, int fwidth, int fheight, int _xyBin, int _zBin)
{

    cacheIndicesBySlice.clear();
    for (int i=0; i<sliceCount; i++)
        cacheIndicesBySlice.append(-1);


    xyBin = _xyBin;
    zBin = _zBin;
    zSize = sliceCount;
    xSize = fwidth;
    ySize = fheight;

    featureSize = fwidth * fheight * 4;
    sourceImageSize = featureSize;

    qDebug()<<"feature size is "<<featureSize;
    if (colourImages) sourceImageSize *=3;

    sourceImageRGB = colourImages;

    ResizeCache();
    timeStamp = 0;
}

MLCachedAccess::~MLCachedAccess()
{
    qDeleteAll(cachedSlices);
    qDeleteAll(features);
}

int MLCachedAccess::GetIndexForFeature(MLFeature::FeatureType type, MLFeature::Channel channel, bool is3D, int arg1, int arg2)
{
    int feature = -1;
    for (int i=0; i<features.count(); i++)
    {
        if (features[i]->Compare(type, channel, is3D, arg1, arg2))
            return i;
    }

    return feature;
}

int MLCachedAccess::GetIndexForFeature(MLFeature *feature)
{
    return GetIndexForFeature(feature->GetType(), feature->GetChannel(), feature->is3D(), feature->GetArg1(),feature->GetArg2());
}


void MLCachedAccess::ClearFeatures()
{
    for (int i=0; i<cachedSlices.count(); i++)
    {
        if (cachedSlices[i]!=nullptr)
        {
            cachedSlices[i]->RemoveAllFeatures();
        }
    }
    qDeleteAll(features);
    features.clear();
    featureIDsInUse.clear();

}

void MLCachedAccess::SetFeatures(QList<MLFeature *> newFeatures)
{
    ClearFeatures();
    for (int i=0; i<newFeatures.count(); i++)
    {
        features.append(newFeatures[i]);

        for (int j=0; j<cachedSlices.count(); j++)
        {
            if (cachedSlices[j]!=nullptr)
            {
                cachedSlices[j]->AddFeature();
            }
        }
        ResizeCache();
    }
    RebuildFeatureIDsInUse();
    qDebug()<<"Done Set Features "<<features.count();
}

void MLCachedAccess::Reset()
{
    qDebug()<<"RESET";
    //Called after a resample change
    ReleaseCacheMemoryForExclusiveOperation();
}

void MLCachedAccess::ReleaseCacheMemoryForExclusiveOperation()
{
    /**
     *
     * Release all resident matrices while retaining the feature configuration
     * and disk cache. Envelope generation uses this immediately before creating
     * its private MLCachedAccess instance, so that only one cache consumes the
     * user-configured CacheMemMLGb allowance at a time.
     *
     * WARNING: This is an exclusive, modal handover of the ML RAM budget. A
     * background envelope operation must not use this mechanism. Supporting
     * concurrent cache users requires a shared global budget manager first.
     *
     **/

    qDeleteAll(cachedSlices);
    cachedSlices.clear();
    slicesByCacheIndex.clear();
    cacheIndicesBySlice.fill(-1, zSize);
    ResizeCache();
    timeStamp = 0;
}

void MLCachedAccess::IncrementTimestamp()
{
    timeStamp++;
}

void MLCachedAccess::SetFeatureInUse(int featureID, bool inUse)
{
    bool oldSelected =  features[featureID]->IsSelected();
    if (oldSelected == inUse)
        return;

    features[featureID]->SetSelected(inUse);
    RebuildFeatureIDsInUse();
    mlInterface->ResetRFAndSample();
}

QList<int> MLCachedAccess::GetFeaturesInUse()
{
    return featureIDsInUse;
}

int MLCachedAccess::GetRequiredXYHalo()
{
    QHash<int, int> cachedRadii;
    QSet<int> visiting;

    std::function<int(int)> calculateRadius = [&](int featureID) -> int
    {
        if (cachedRadii.contains(featureID))
            return cachedRadii.value(featureID);

        if (featureID < 0 || featureID >= features.count()
            || visiting.contains(featureID))
        {
            return -1;
        }

        visiting.insert(featureID);
        MLFeature *feature = features.at(featureID);
        const int localRadius = feature->GetXYSupportRadius();
        if (localRadius < 0)
        {
            visiting.remove(featureID);
            cachedRadii.insert(featureID, -1);
            return -1;
        }

        int dependencyRadius = 0;
        QList<MLFeature *> dependencies = feature->GetDependencies();
        for (MLFeature *dependency : dependencies)
        {
            const int dependencyID = GetIndexForFeature(dependency);
            const int radius = calculateRadius(dependencyID);
            if (radius < 0)
            {
                dependencyRadius = -1;
                break;
            }
            dependencyRadius = qMax(dependencyRadius, radius);
        }
        qDeleteAll(dependencies);

        int totalRadius = -1;
        if (dependencyRadius >= 0
            && localRadius <= std::numeric_limits<int>::max() - dependencyRadius)
        {
            totalRadius = localRadius + dependencyRadius;
        }

        visiting.remove(featureID);
        cachedRadii.insert(featureID, totalRadius);
        return totalRadius;
    };

    int requiredRadius = 0;
    for (int featureID : featureIDsInUse)
    {
        const int radius = calculateRadius(featureID);
        if (radius < 0)
            return -1;
        requiredRadius = qMax(requiredRadius, radius);
    }
    return requiredRadius;
}

void MLCachedAccess::DumpFeatures()
{
    for (int i=0; i<features.count(); i++)
    {
        qDebug()<<"Feature "<<i<<":"<<features[i]->Dump();
    }
}
QString MLCachedAccess::GetSourceImageFeatureName()
{
    return "src";
}

MLFeature *MLCachedAccess::GetFeature(int featureID)
{
    return features[featureID];
}

int MLCachedAccess::GetFeatureCount()
{
    return features.count();
}

int MLCachedAccess::GetXSize()
{
    return xSize;
}

int MLCachedAccess::GetYSize()
{
    return ySize;
}

bool MLCachedAccess::GetSourceColour()
{
    return sourceImageRGB;
}

cv::Mat MLCachedAccess::GetWholeSliceFeature(int z, int featureIndex)
{
    MLCachedSlice *slice = GetSlice(z);
    slice->FetchFeatureIfNeeded(featureIndex);
    return slice->featureData[featureIndex];
}

void MLCachedAccess::CalculateFeature(cv::Mat &mat, int sliceIndex, int featureID)
{
    features[featureID]->CalculateFeature(mat, sliceIndex, this);
}

int MLCachedAccess::AddFeature(MLFeature *feature)
{
    //Already exist? If so just return the index
    for (int i=0; i<features.count(); i++)
        if (features[i]->Compare(feature))
        {
            delete feature; //will not be using it
            return i;
        }
    qDebug()<<"Adding feature "<<feature->GetPrettyFullName();

    auto dependencies = feature->GetDependencies();
    for (int i=0; i<dependencies.count(); i++)
    {
        AddFeature(dependencies[i]);
    }

    //No, I have to add it
    int nextFeatureIndex = features.count();
    features.append(feature);

    for (int i=0; i<cachedSlices.count(); i++)
    {
        if (cachedSlices[i]!=nullptr)
        {
            cachedSlices[i]->AddFeature();
        }
    }

    RebuildFeatureIDsInUse();
    ResizeCache();
    return nextFeatureIndex;
}

bool MLCachedAccess::IsFeatureADependency(MLFeature *feature)
{
    for (int i=0; i<features.count(); i++)
    {
        if (features[i]==feature)
            continue;

        auto depList = features[i]->GetDependencies();
        for (int j=0; j<depList.count(); j++)
        {
            if (feature->Compare(depList[j]))
            {
                qDeleteAll(depList);
                return true;
            }
        }
        qDeleteAll(depList);
    }
    return false;
}


bool MLCachedAccess::RemoveFeature(int featureIndex)
{
    if (featureIndex == -1)
        //it doesn't exist - return false (error)
        return false;

    if (IsFeatureADependency(features[featureIndex]))
        return false;

    features.removeAt(featureIndex);

    for (int i=0; i<cachedSlices.count(); i++)
    {
        if (cachedSlices[i]!=nullptr)
        {
            cachedSlices[i]->RemoveFeature(featureIndex);
        }
    }

    RebuildFeatureIDsInUse();
    ResizeCache();
    return true;
}

MLCachedSlice * MLCachedAccess::GetSlice(int sliceIndex)
{
    int cacheIndex = cacheIndicesBySlice[sliceIndex];
    if (cacheIndex!=-1)
    {
        return cachedSlices[cacheIndex];
    }
    else
    {
        //Assign one
        return cachedSlices[AssignCacheSlot(sliceIndex)];
    }
}

void MLCachedAccess::RebuildFeatureIDsInUse()
{
    featureIDsInUse.clear();
    for(int i=0; i<features.count(); i++)
        if (features[i]->IsSelected()) featureIDsInUse.append(i);
}

float MLCachedAccess::GetFeatureValueAt(int x, int y, int z, int featureID)
{
    MLCachedSlice *slice = GetSlice(z);
    return slice->GetFeatureData(x,y,featureID);
}

float MLCachedAccess::GetIntensityAsFloat(int x, int y, int z)
{
    MLCachedSlice *slice = GetSlice(z);
    return slice->GetIntensityGrey(x,y);
}

QColor MLCachedAccess::GetRGBFloat(int x, int y, int z)
{
    MLCachedSlice *slice = GetSlice(z);
    return slice->GetColor(x,y);
}


//Private
uint64_t MLCachedAccess::GetMemorySizeOfSlice()
{
    ulong size = (ulong) sourceImageSize;
    size += (ulong) features.count() * (ulong)featureSize;
    return size;
}

void MLCachedAccess::ResizeCache()
{
    int newCacheLength = (int)(((uint64_t) CacheMemMLGb * 1024ull * 1024ull * 1024ull)/ GetMemorySizeOfSlice());

    if (newCacheLength > zSize) newCacheLength = zSize; //no need for more!
    if (newCacheLength < 1 && zSize > 0) newCacheLength = 1;
    int oldCacheLength= cachedSlices.count();

    if (newCacheLength == oldCacheLength) //nothing to do
        return;


    qDebug()<<"Resizing cache to "<<newCacheLength<<"slices from "<<oldCacheLength<<" slices";

    if (newCacheLength>oldCacheLength)
    {

        //easy case - add new cache entries.
        int entriesToAdd = newCacheLength - oldCacheLength;
        //qDebug()<<"Adding "<<entriesToAdd<<" new entries";
        for (int i=0; i<entriesToAdd; i++)
        {
            cachedSlices.append(new MLCachedSlice(features.count(), -1, this));
            slicesByCacheIndex.append(-1); //points to no slice
        }

        return; //done
    }

    if (newCacheLength < oldCacheLength)
    {
        int entriesToRemove = oldCacheLength - newCacheLength;
        //qDebug()<<"Removing "<<entriesToRemove<<" new entries";
        for (int i=0; i<entriesToRemove; i++)
        {
            int removeIndex = FindReusableCacheSlot();

            //qDebug()<<"Removing index "<<removeIndex<< " for slice "<<slicesByCacheIndex[removeIndex];
            if (slicesByCacheIndex[removeIndex] !=-1)
            {
                //Shuffle all indices in the pointer array by one where needed

                for (int i=0; i<cacheIndicesBySlice.count();i++)
                {
                    if (cacheIndicesBySlice[i] > removeIndex)
                    {
                        cacheIndicesBySlice[i]--;
                    }
                    //and remove pointer to this slice if present
                    if (cacheIndicesBySlice[i] == removeIndex)
                    {
                        cacheIndicesBySlice[i] = -1;
                    }
                }
            }

            //and remove this slice from the other list
            slicesByCacheIndex.removeAt(removeIndex);
            delete cachedSlices[removeIndex];
            cachedSlices.removeAt(removeIndex);

        }

        //qDebug()<<"After reduction, slicesByCacheIndex count "<<slicesByCacheIndex.count();
        //qDebug()<<"After reduction, cacheIndicesBySlice count "<<cacheIndicesBySlice.count();

        return;
    }

}

int MLCachedAccess::FindReusableCacheSlot()
{
    uint64 oldest;
    int useCacheIndex = -1;

    for (int i=0; i<cachedSlices.count();i++)
    {
        if (cachedSlices[i]->activeFetchCount > 0)
        {
            continue;
        }

        if (slicesByCacheIndex[i]==-1)
        {
            useCacheIndex= i; //can stop - found an empty one
            break;
        }
        else
        {
            if (useCacheIndex>=0)
            {
                if (cachedSlices[i]->lastUsed < oldest)
                {
                    useCacheIndex = i;
                    oldest = cachedSlices[i]->lastUsed;
                }
            }
            else
            {
                useCacheIndex = i;
                oldest = cachedSlices[i]->lastUsed;
            }
        }
    }

    if (useCacheIndex < 0)
    {
        throw std::runtime_error(
            "The ML cache is too small for the active feature dependency chain. "
            "Increase the ML cache size or reduce the envelope smoothing and closing radii.");
    }

    if (slicesByCacheIndex[useCacheIndex]!=-1)
    {
        //re-using, so clear it
        qDebug()<<"Resuing cache index "<<useCacheIndex;
        cacheIndicesBySlice[cachedSlices[useCacheIndex]->sliceIndex]=-1;
        slicesByCacheIndex[useCacheIndex] = -1;
        cachedSlices[useCacheIndex]->Clear();
    }


    return useCacheIndex;
}

int MLCachedAccess::AssignCacheSlot(int sliceIndex)
{
    //find first available slot

    int useCacheIndex = FindReusableCacheSlot();

    //And assign
    cachedSlices[useCacheIndex]->sliceIndex = sliceIndex;
    cacheIndicesBySlice[sliceIndex] = useCacheIndex;
    slicesByCacheIndex[useCacheIndex] = sliceIndex;

    return useCacheIndex;
}

