#include "mlcachedslice.h"
#include "mlcachedaccess.h"

#include "mlfileio.h"
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
    lastUsed = parent->timeStamp;
    sourceValid = false;

}

MLCachedSlice::~MLCachedSlice()
{
    sourceImage.release();
    for (int i=0; i<featureData.count(); i++)
        featureData[i].release();
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
    sourceImage.release();

    sourceValid=false;

    for (int i=0;i< featuresValid.count(); i++)
    {
        featureData[i].release(); //probably done automatically, but just in case!
        featuresValid[i] = false;
    }
}

void MLCachedSlice::RemoveAllFeatures()
{
    for (int i=0;i< featuresValid.count(); i++)
    {
        featureData[i].release(); //probably done automatically, but just in case!
    }
    featureData.clear();
    featuresValid.clear();
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
    cache->IncrementTimestamp();
    lastUsed = cache->timeStamp;
    bool ok;
    cv::Mat loadedMat = MLFileIO::LoadMatBinary(cache->GetFeature(feature)->GetEncodedNameForFile(),
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
        if (!featureData[feature].empty())
        {
            qDebug()<<"Overwriting a mat of size "<<featureData[feature].total() * featureData[feature].elemSize();
        }
        featureData[feature] = mat;
        MLFileIO::SaveMatBinary(cache->GetFeature(feature)->GetEncodedNameForFile(),
                                    featureData[feature], sliceIndex);
    }
    featuresValid[feature] = true;
}

void MLCachedSlice::FetchSourceData()
{
    cache->IncrementTimestamp();
    lastUsed = cache->timeStamp;
    bool ok;
    cv::Mat loadedMat = MLFileIO::LoadMatBinary(cache->GetSourceImageFeatureName(),
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
        sourceImage = MLFileIO::LoadMatFromImageFile(sliceIndex, cache->GetSourceColour());

        if (ColMonoScale==1)
        {
            MLFileIO::SaveMatBinary(cache->GetSourceImageFeatureName(),
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
            sourceImage.release();
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
    lastUsed = cache->timeStamp;
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
