#include "mlcachedaccess.h"

#include "opencvfileio.h"
#include "globals.h"

#include "mlupdateblockingdialog.h"
//Slice data class constructor
MLCachedSlice::MLCachedSlice(int featureCount, int zIndex, MLCachedAccess *parent)
{
    cache = parent;
    featureData.clear();
    featuresValid.clear();
    for (int i=0;i< featureCount; i++)
    {
        featureData.append(cv::Mat()); //an empty Mat
        featuresValid.append(false);
    }
    sliceIndex = zIndex;
    lastUsed = QDateTime::currentDateTime();
    sourceValid = false;

}



void MLCachedSlice::AddFeature()
{
    featuresValid.append(false);
    featureData.append(cv::Mat()); //an empty Mat
}

void MLCachedSlice::RemoveFeature(int index)
{
    featuresValid.removeAt(index);
    featureData.removeAt(index);
}

void MLCachedSlice::Clear()
{
    if (sourceValid)
        sourceImage.deallocate();

    sourceValid=false;

    for (int i=0;i< featuresValid.count(); i++)
    {
        if (featuresValid[i])
        {
            featureData[i].deallocate(); //probably done automatically, but just in case!
        }
        featuresValid[i] = false;
    }
}

float MLCachedSlice::GetFeatureData(int x, int y, int feature)
{
    FetchFeatureIfNeeded(feature);
    return featureData[feature].at<float>(y,x);
}

void MLCachedSlice::FetchSourceDataIfNeeded()
{
    if (!sourceValid) FetchSourceData();
}

void MLCachedSlice::FetchFeatureIfNeeded(int featureIndex)
{
    if (!featuresValid[featureIndex])
        FetchFeatureData(featureIndex);
}



void MLCachedSlice::FetchFeatureData(int feature)
{
    bool ok;
    cv::Mat loadedMat = openCVFileIO::LoadMatBinary(cache->GetFeature(feature)->GetEncodedNameForFile(),
                                                    cache->GetXSize(), cache->GetYSize(),
                                                    sliceIndex, ok);

    if (ok)
    {
        //file loading worked OK
        featureData[feature] = loadedMat;
        MLUpdateBlockingDialog::updateDetailText(
            QString("Loaded feature %1 for slice %2 from file cache")
                .arg(cache->GetFeature(feature)->GetPrettyFullName())
                .arg(sliceIndex));
    }
    else
    {
        cv::Mat mat;
        mat.create(cache->GetYSize(), cache->GetXSize(), CV_32F);
        MLUpdateBlockingDialog::updateDetailText(
            QString("Calculating feature %1 for slice %2")
                .arg(cache->GetFeature(feature)->GetPrettyFullName())
                .arg(sliceIndex));
        cache->CalculateFeature(mat, sliceIndex, feature);
        featureData[feature] = mat;
        openCVFileIO::SaveMatBinary(cache->GetFeature(feature)->GetEncodedNameForFile(),
                                    featureData[feature], sliceIndex);
    }
    featuresValid[feature] = true;
}

void MLCachedSlice::FetchSourceData()
{
    bool ok;
    cv::Mat loadedMat = openCVFileIO::LoadMatBinary(cache->GetSourceImageFeatureName(),
                                                    cache->GetXSize(), cache->GetYSize(),
                                                    sliceIndex, ok);

    if (ok)
    {
        //file loading worked OK
        sourceImage = loadedMat;
    }
    else
    {
        //Load image from source file
        MLUpdateBlockingDialog::updateDetailText(
            QString("Fetching raw data slice %1 from source image")
                .arg(sliceIndex));
        sourceImage = openCVFileIO::LoadMatFromImageFile(sliceIndex, cache->GetSourceColour());

        if (ColMonoScale==1)
        {
            openCVFileIO::SaveMatBinary(cache->GetSourceImageFeatureName(),
                                    sourceImage, sliceIndex);
        }
        else
        {
            //Do downsampling
            MLUpdateBlockingDialog::updateDetailText(
                QString("Binning raw data"));
            cv::Mat binned;

            cv::resize(sourceImage, binned,
                       cv::Size(sourceImage.cols/ColMonoScale, sourceImage.rows/ColMonoScale),
                       0, 0,
                       cv::INTER_AREA);
            sourceImage.deallocate();
            sourceImage = binned;
        }
    }
    sourceValid = true;
}

float MLCachedSlice::GetIntensityGrey(int x, int y)
{
    FetchSourceDataIfNeeded();
    if (cache->GetSourceColour())
    {
        cv::Vec3f p = sourceImage.at<cv::Vec3f>(y, x);

        float b = p[0];
        float g = p[1];
        float r = p[2];
        return (r+g+b)/3.0f;
    }
    else
        return sourceImage.at<float>(y,x);
}

QColor MLCachedSlice::GetColor(int x, int y)
{
    FetchSourceDataIfNeeded();
    if (cache->GetSourceColour())
    {
        cv::Vec3f p = sourceImage.at<cv::Vec3f>(y, x);

        float b = p[0];
        float g = p[1];
        float r = p[2];
        return QColor::fromRgbF(r, g, b);
    }
    else
    {
        float intensity  = sourceImage.at<float>(y,x);
        return QColor::fromRgbF(intensity, intensity, intensity);
    }

}

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

    if (colourImages) sourceImageSize *=3;

    sourceImageRGB = colourImages;

    SetMaxMemoryUsage(1024ul *1024ul *1024ul * 2ul); //2Gb default
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

void MLCachedAccess::SetFeatureInUse(int featureID, bool inUse)
{
    features[featureID]->SetSelected(inUse);
    RebuildFeatureIDsInUse();
}

QList<int> MLCachedAccess::GetFeaturesInUse()
{
    return featureIDsInUse;
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

cv::Mat MLCachedAccess::GetWholeSliceIntensity(int z)
{
    MLCachedSlice *slice = GetSlice(z);
    slice->FetchSourceDataIfNeeded();

    if (sourceImageRGB)
    {
        cv::Mat mat32;
        cv::cvtColor(sourceImageRGB, mat32, cv::COLOR_BGR2GRAY);
        mat32.convertTo(mat32, CV_32F, 1.0 / 255.0);
        return mat32;
    }
    else
    {
        //qDebug()<<"Inside: "<<slice->sourceImage.rows;
        return slice->sourceImage;
    }
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

void MLCachedAccess::SetMaxMemoryUsage(uint64 size)
{
    maxMemoryUsage = size;
    ResizeCache();
}


//Private
ulong MLCachedAccess::GetMemorySizeOfSlice()
{
    ulong size = (ulong) sourceImageSize;
    size += (ulong) features.count() * (ulong)featureSize;
    return size;
}

void MLCachedAccess::ResizeCache()
{
    int newCacheLength = maxMemoryUsage / GetMemorySizeOfSlice();

    if (newCacheLength > zSize) newCacheLength = zSize; //no need for more!
    int oldCacheLength= cachedSlices.count();

    if (newCacheLength == oldCacheLength) //nothing to do
        return;

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

            qDebug()<<"Removing index "<<removeIndex<< " for slice "<<slicesByCacheIndex[removeIndex];
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
        }
        return;
    }

}

int MLCachedAccess::FindReusableCacheSlot()
{
    QDateTime oldest;
    int useCacheIndex = -1;

    for (int i=0; i<cachedSlices.count();i++)
    {
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

    if (slicesByCacheIndex[useCacheIndex]!=-1)
    {
        //re-using, so clear it
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

