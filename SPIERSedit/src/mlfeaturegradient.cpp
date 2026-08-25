/**
 * @file
 * Source: Mlfeaturegradient
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
#include "mlfeaturegradient.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeaturegaussian.h"
#include "mlroislice.h"

MLFeatureGradient::MLFeatureGradient(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Gradient_magnitude, channel, is3D, arg1, 0)
{}

void MLFeatureGradient::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureGradient3D(mat, sliceID, data);
    else
        CalcFeatureGradient2D(mat, sliceID, data);
}

bool MLFeatureGradient::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    const MLROISlice sourceROI = roi.expandedByPixels(1);
    const int gaussianIndex = data->GetIndexForFeature(
        MLFeature::FeatureType::Gaussian,
        _channel,
        _is3D,
        _arg1,
        0);

    const int previousSlice = qMax(0, sliceID - 1);
    const int nextSlice = qMin(FileCount - 1, sliceID + 1);
    const cv::Mat current = data->GetROISliceFeature(
        sliceID,
        gaussianIndex,
        sourceROI);
    cv::Mat previous;
    cv::Mat next;
    if (_is3D)
    {
        previous = data->GetROISliceFeature(
            previousSlice,
            gaussianIndex,
            sourceROI);
        next = data->GetROISliceFeature(
            nextSlice,
            gaussianIndex,
            sourceROI);
    }

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
                const int previousY = qMax(0, y - 1);
                const int nextY = qMin(fheight - 1, y + 1);
                const float *currentRow = current.ptr<float>(y);
                const float *previousYRow =
                    current.ptr<float>(previousY);
                const float *nextYRow = current.ptr<float>(nextY);
                const float *previousZRow =
                    _is3D ? previous.ptr<float>(y) : nullptr;
                const float *nextZRow =
                    _is3D ? next.ptr<float>(y) : nullptr;
                float *outputRow = mat.ptr<float>(y);

                for (int x = tile.left(); x <= tile.right(); x++)
                {
                    const int previousX = qMax(0, x - 1);
                    const int nextX = qMin(fwidth - 1, x + 1);
                    const float gradientX =
                        0.5f
                        * (currentRow[nextX]
                           - currentRow[previousX]);
                    const float gradientY =
                        0.5f
                        * (nextYRow[x] - previousYRow[x]);

                    float magnitudeSquared =
                        gradientX * gradientX
                        + gradientY * gradientY;
                    if (_is3D)
                    {
                        const float gradientZ =
                            0.5f
                            * (nextZRow[x] - previousZRow[x]);
                        magnitudeSquared += gradientZ * gradientZ;
                    }
                    outputRow[x] = std::sqrt(magnitudeSquared);
                }
            }
        }
    }

    return false;
}

QList<MLFeature *> MLFeatureGradient::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureGaussian(_channel, _is3D, _arg1));
    return deps;
}

QString MLFeatureGradient::GetPrettyName()
{
    return "gradient";
}

QString MLFeatureGradient::GetPrettyArgs()
{
    return QString("sigma=%1").arg(pow(2.0f, _arg1));
}

QString MLFeatureGradient::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureGradient::GetTypeCodeForFile()
{
    return "grd";
}

QString MLFeatureGradient::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureGradient::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureGradient::CalcFeatureGradient2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, _arg2);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, idx);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        float *outRow = mat.ptr<float>(y);

        int y0 = std::max(0, y - 1);
        int y1 = std::min(fheight - 1, y + 1);

        for (int x = 0; x < fwidth; ++x)
        {
            int x0 = std::max(0, x - 1);
            int x1 = std::min(fwidth - 1, x + 1);

            float gx = 0.5f * (slice.at<float>(y, x1) - slice.at<float>(y, x0));
            float gy = 0.5f * (slice.at<float>(y1, x) - slice.at<float>(y0, x));

            outRow[x] = std::sqrt(gx * gx + gy * gy);
        }
    }
}

void MLFeatureGradient::CalcFeatureGradient3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, true, _arg1, _arg2);

    int z0 = std::max(0, sliceID - 1);
    int z1 = std::min(FileCount - 1, sliceID + 1);

    cv::Mat slice0 = data->GetWholeSliceFeature(z0, idx);
    cv::Mat slice  = data->GetWholeSliceFeature(sliceID, idx);
    cv::Mat slice1 = data->GetWholeSliceFeature(z1, idx);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        float *outRow = mat.ptr<float>(y);

        int y0 = std::max(0, y - 1);
        int y1 = std::min(fheight - 1, y + 1);

        for (int x = 0; x < fwidth; ++x)
        {
            int x0 = std::max(0, x - 1);
            int x1 = std::min(fwidth - 1, x + 1);

            float gx = 0.5f * (slice.at<float>(y, x1) - slice.at<float>(y, x0));
            float gy = 0.5f * (slice.at<float>(y1, x) - slice.at<float>(y0, x));
            float gz = 0.5f * (slice1.at<float>(y, x) - slice0.at<float>(y, x));

            outRow[x] = std::sqrt(gx * gx + gy * gy + gz * gz);
        }
    }
}

int MLFeatureGradient::GetDependencyDepth()
{
    if (_is3D)
        return 4;
    else
        return 2;
}

QString MLFeatureGradient::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(pow(2.0f, v));
}

QString MLFeatureGradient::GetArg2SetupString(int)
{
    return "";
}
