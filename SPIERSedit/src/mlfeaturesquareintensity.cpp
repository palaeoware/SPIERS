/**
 * @file
 * Source: Mlfeaturesquareintensity
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
#include "mlfeaturesquareintensity.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeatureintensity.h"
#include "mlroislice.h"

MLFeatureSquareIntensity::MLFeatureSquareIntensity(Channel channel)
    : MLFeature(FeatureType::Square, channel, false, 0, 0)
{}

void MLFeatureSquareIntensity::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Intensity, _channel, false, 0, 0);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, idx);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        const float *inRow = slice.ptr<float>(y);
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            float v = inRow[x];
            outRow[x] = v * v;
        }
    }
}

bool MLFeatureSquareIntensity::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);

    const int intensityIndex = data->GetIndexForFeature(
        MLFeature::FeatureType::Intensity,
        _channel,
        false,
        0,
        0);
    const cv::Mat intensity =
        data->GetROISliceFeature(sliceID, intensityIndex, roi);

    for (int tileY = 0; tileY < roi.tileRows(); tileY++)
    {
        for (int tileX = 0; tileX < roi.tileColumns(); tileX++)
        {
            if (roi.tileState(tileX, tileY)
                == MLROISlice::TileState::Inactive)
            {
                continue;
            }

            const QRect tile = roi.tileRect(tileX, tileY);
            for (int y = tile.top(); y <= tile.bottom(); y++)
            {
                const float *inRow = intensity.ptr<float>(y);
                float *outRow = mat.ptr<float>(y);
                for (int x = tile.left(); x <= tile.right(); x++)
                {
                    const float value = inRow[x];
                    outRow[x] = value * value;
                }
            }
        }
    }

    return false;
}

QList<MLFeature *> MLFeatureSquareIntensity::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureIntensity(_channel));
    return deps;
}

QString MLFeatureSquareIntensity::GetPrettyName()
{
    return "square";
}

QString MLFeatureSquareIntensity::GetPrettyArgs()
{
    return "";
}

QString MLFeatureSquareIntensity::GetPretty3D()
{
    return "";
}

int MLFeatureSquareIntensity::GetDependencyDepth()
{
    return 1;
}

QString MLFeatureSquareIntensity::GetTypeCodeForFile()
{
    return "sqr";
}

QString MLFeatureSquareIntensity::GetArgsForFile()
{
    return "";
}

QString MLFeatureSquareIntensity::Get3DForFile()
{
    return "";
}

QString MLFeatureSquareIntensity::GetArg1SetupString(int)
{
    return "";
}

QString MLFeatureSquareIntensity::GetArg2SetupString(int)
{
    return "";
}


int MLFeatureSquareIntensity::GetMinMaxForArgs(int, bool)
{
    return 0;
}
