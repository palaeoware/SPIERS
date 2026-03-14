#include "mlcachedslice.h"
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

MLCachedSlice::~MLCachedSlice()
{
    sourceImage.deallocate();
    for (int i=0; i<featureData.count(); i++)
        featureData[i].deallocate();
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
