/**
 * @file
 * Source: Mlcachedslice
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
#include "mlcachedslice.h"
#include "mlcachedaccess.h"
#include "mlroislice.h"

#include "mlfileio.h"
#include "globals.h"

#include "mlupdateblockingdialog.h"
//Slice data class constructor
MLCachedSlice::MLCachedSlice(int featureCount, int zIndex, MLCachedAccess *parent)
{
    cache = parent;
    featureData.clear();
    featuresValid.clear();
    featureValidTiles.clear();
    featureTileCacheLoaded.clear();
    for (int i=0;i< featureCount; i++)
    {
        featureData.append(cv::Mat()); //an empty Mat
        featuresValid.append(false);
        featureValidTiles.append(
            QByteArray(parent->GetFeatureTileCount(), 0));
        featureTileCacheLoaded.append(false);
    }
    sliceIndex = zIndex;
    lastUsed = parent->timeStamp;
    activeFetchCount = 0;
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
    featureValidTiles.append(
        QByteArray(cache->GetFeatureTileCount(), 0));
    featureTileCacheLoaded.append(false);
}

void MLCachedSlice::RemoveFeature(int index)
{
    featuresValid.removeAt(index);
    featureData.removeAt(index);
    featureValidTiles.removeAt(index);
    featureTileCacheLoaded.removeAt(index);
}

void MLCachedSlice::Clear()
{
    sourceImage.release();

    sourceValid=false;

    for (int i=0;i< featuresValid.count(); i++)
    {
        featureData[i].release(); //probably done automatically, but just in case!
        featuresValid[i] = false;
        featureValidTiles[i].fill(0);
        featureTileCacheLoaded[i] = false;
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
    featureValidTiles.clear();
    featureTileCacheLoaded.clear();
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

void MLCachedSlice::FetchFeatureTilesIfNeeded(
    int featureIndex,
    const MLROISlice &roi)
{
    if (FeatureTilesAreValid(featureIndex, roi))
        return;

    const MLROISlice missingROI =
        roi.requiringInvalidTiles(featureValidTiles.at(featureIndex));
    if (missingROI.isValid() && missingROI.requiredTileCount() > 0)
        FetchFeatureData(featureIndex, &missingROI);
    else
        FetchFeatureData(featureIndex);
}

bool MLCachedSlice::FeatureTilesAreValid(
    int feature,
    const MLROISlice &roi) const
{
    if (featuresValid.at(feature))
        return true;

    if (!roi.isValid()
        || roi.width() != cache->GetXSize()
        || roi.height() != cache->GetYSize()
        || roi.totalTileCount() != featureValidTiles.at(feature).size())
    {
        return false;
    }

    const QByteArray &validTiles = featureValidTiles.at(feature);
    for (int tileY = 0; tileY < roi.tileRows(); tileY++)
    {
        for (int tileX = 0; tileX < roi.tileColumns(); tileX++)
        {
            if (roi.tileState(tileX, tileY)
                == MLROISlice::TileState::Inactive)
            {
                continue;
            }

            const int tile = tileY * roi.tileColumns() + tileX;
            if (validTiles.at(tile) == 0)
                return false;
        }
    }
    return true;
}

void MLCachedSlice::SetFeatureFullyValid(int feature)
{
    featuresValid[feature] = true;
    featureValidTiles[feature].fill(1);
}

void MLCachedSlice::SetFeatureTilesValid(
    int feature,
    const MLROISlice &roi)
{
    if (!roi.isValid()
        || roi.width() != cache->GetXSize()
        || roi.height() != cache->GetYSize()
        || roi.totalTileCount() != featureValidTiles.at(feature).size())
    {
        return;
    }

    QByteArray &validTiles = featureValidTiles[feature];
    for (int tileY = 0; tileY < roi.tileRows(); tileY++)
    {
        for (int tileX = 0; tileX < roi.tileColumns(); tileX++)
        {
            if (roi.tileState(tileX, tileY)
                != MLROISlice::TileState::Inactive)
            {
                validTiles[tileY * roi.tileColumns() + tileX] = 1;
            }
        }
    }

    featuresValid[feature] =
        !validTiles.contains(static_cast<char>(0));
}



void MLCachedSlice::FetchFeatureData(
    int feature,
    const MLROISlice *roi)
{
    /**
     *
     * Feature calculation can recursively request neighbouring slices. Keep
     * this cache slot pinned until the calculation completes, otherwise an LRU
     * eviction can reuse the slot and assign the result to the wrong slice.
     *
     **/
    activeFetchCount++;

    try
    {
        cache->IncrementTimestamp();
        lastUsed = cache->timeStamp;
        const QString featureName =
            cache->GetFeature(feature)
                ->GetEncodedNameForFile();

        if (!featureTileCacheLoaded.at(feature))
        {
            bool wholeCacheLoaded = false;
            cv::Mat loadedMat = MLFileIO::LoadMatBinary(
                featureName,
                cache->GetXSize(),
                cache->GetYSize(),
                sliceIndex,
                wholeCacheLoaded);
            featureTileCacheLoaded[feature] = true;

            if (wholeCacheLoaded)
            {
                featureData[feature] = loadedMat;
                SetFeatureFullyValid(feature);
                MLFileIO::RemoveMatTiles(
                    featureName,
                    sliceIndex);
                MLUpdateBlockingDialog::updateDetailText(
                    QString(
                        "Loaded feature %1 for slice %2 "
                        "from file cache")
                        .arg(
                            cache->GetFeature(feature)
                                ->GetPrettyFullName())
                        .arg(sliceIndex));
            }
            else
            {
                cv::Mat tiledMat;
                QByteArray tiledValidTiles(
                    cache->GetFeatureTileCount(),
                    0);
                if (MLFileIO::LoadMatTiles(
                        featureName,
                        cache->GetXSize(),
                        cache->GetYSize(),
                        sliceIndex,
                        MLROISlice::adaptiveTileSize(
                            cache->GetXSize(),
                            cache->GetYSize(),
                            MLROISlice::TARGET_TILE_COUNT),
                        tiledMat,
                        tiledValidTiles))
                {
                    featureData[feature] = tiledMat;
                    featureValidTiles[feature] =
                        tiledValidTiles;
                    featuresValid[feature] =
                        !tiledValidTiles.contains(
                            static_cast<char>(0));

                    int loadedTileCount = 0;
                    for (char valid : tiledValidTiles)
                    {
                        if (valid != 0)
                            loadedTileCount++;
                    }
                    if (loadedTileCount > 0
                        && cache
                               ->IsPartialFeatureTileLoggingEnabled())
                    {
                        qDebug()
                            << "ML loaded feature tiles:"
                            << cache->GetFeature(feature)
                                   ->GetPrettyFullName()
                            << "- slice" << sliceIndex
                            << "-" << loadedTileCount
                            << "of"
                            << tiledValidTiles.size();
                    }

                    if (featuresValid.at(feature))
                    {
                        MLFileIO::SaveMatBinary(
                            featureName,
                            featureData[feature],
                            sliceIndex);
                        MLFileIO::RemoveMatTiles(
                            featureName,
                            sliceIndex);
                    }
                }
            }
        }

        if (featuresValid.at(feature))
        {
            activeFetchCount--;
            return;
        }

        MLROISlice missingROI;
        const MLROISlice *calculationROI = roi;
        if (roi != nullptr)
        {
            missingROI = roi->requiringInvalidTiles(
                featureValidTiles.at(feature));
            if (missingROI.isValid()
                && missingROI.requiredTileCount() > 0)
            {
                calculationROI = &missingROI;
            }
            else
            {
                activeFetchCount--;
                return;
            }
        }

        cv::Mat mat = featureData[feature];
        if (mat.empty())
        {
            mat = cv::Mat::zeros(
                cache->GetYSize(),
                cache->GetXSize(),
                CV_32F);
        }
        MLUpdateBlockingDialog::updateDetailText(
            QString("Calculating feature %1 for slice %2")
                .arg(
                    cache->GetFeature(feature)
                        ->GetPrettyFullName())
                .arg(sliceIndex));

        bool calculatedWholeSlice = true;
        if (calculationROI == nullptr)
        {
            cache->CalculateFeature(
                mat,
                sliceIndex,
                feature);
        }
        else
        {
            calculatedWholeSlice =
                cache->CalculateFeatureROI(
                    mat,
                    sliceIndex,
                    feature,
                    *calculationROI);
        }

        if (!featureData[feature].empty()
            && calculatedWholeSlice)
        {
            qDebug() << "Replacing a feature matrix of size"
                     << featureData[feature].total()
                            * featureData[feature].elemSize();
        }
        featureData[feature] = mat;

        if (calculatedWholeSlice)
        {
            MLFileIO::SaveMatBinary(
                featureName,
                featureData[feature],
                sliceIndex);
            MLFileIO::RemoveMatTiles(
                featureName,
                sliceIndex);
            SetFeatureFullyValid(feature);
        }
        else
        {
            MLFileIO::SaveMatTiles(
                featureName,
                featureData[feature],
                sliceIndex,
                *calculationROI);
            SetFeatureTilesValid(
                feature,
                *calculationROI);
            if (cache
                    ->IsPartialFeatureTileLoggingEnabled())
            {
                qDebug()
                    << "ML partial feature tiles:"
                    << cache->GetFeature(feature)
                           ->GetPrettyFullName()
                    << "- slice" << sliceIndex
                    << "-"
                    << calculationROI
                           ->requiredTileCount()
                    << "of"
                    << calculationROI->totalTileCount();
            }
            if (featuresValid.at(feature))
            {
                MLFileIO::SaveMatBinary(
                    featureName,
                    featureData[feature],
                    sliceIndex);
                MLFileIO::RemoveMatTiles(
                    featureName,
                    sliceIndex);
            }
        }
    }
    catch (...)
    {
        activeFetchCount--;
        throw;
    }

    activeFetchCount--;
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
