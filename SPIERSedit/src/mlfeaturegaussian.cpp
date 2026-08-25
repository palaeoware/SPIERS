/**
 * @file
 * Source: Mlfeaturegaussian
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
#include "mlfeaturegaussian.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeatureintensity.h"
#include "mlroislice.h"

MLFeatureGaussian::MLFeatureGaussian(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Gaussian, channel, is3D, arg1, 0)
{}

void MLFeatureGaussian::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureGaussian3D(mat, sliceID, data);
    else
        CalcFeatureGaussian2D(mat, sliceID, data);
}

bool MLFeatureGaussian::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    if (_is3D)
        CalcFeatureGaussian3DROI(mat, sliceID, data, roi);
    else
        CalcFeatureGaussian2DROI(mat, sliceID, data, roi);

    return false;
}

QList<MLFeature *> MLFeatureGaussian::GetDependencies()
{
    QList<MLFeature *> deps;
    if (_is3D)
        deps.append(new MLFeatureGaussian(_channel, false, _arg1));
    else
        deps.append(new MLFeatureIntensity(_channel));

    return deps;
}

QString MLFeatureGaussian::GetPrettyName()
{
    return "gaussian";
}

QString MLFeatureGaussian::GetPrettyArgs()
{
    return QString("Sigma=%1").arg(pow(2.0f,_arg1));
}

QString MLFeatureGaussian::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureGaussian::GetTypeCodeForFile()
{
    return "gau";
}

QString MLFeatureGaussian::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureGaussian::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureGaussian::CalcFeatureGaussian2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{

    float sigma = std::pow(2.0f,_arg1);

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Intensity, _channel, false, 0, 0);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, idx);

    int r = static_cast<int>(ceil(3.0 * sigma));
    int ksize = 2 * r + 1;

    std::vector<float> kernel(ksize);

    double sum = 0.0;
    for (int k = -r; k <= r; ++k)
    {
        double v = exp(-(k*k) / (2.0 * sigma * sigma));
        kernel[k + r] = static_cast<float>(v);
        sum += v;
    }

    for (float &v : kernel)
        v /= sum;

    cv::Mat temp(fheight, fwidth, CV_32F);

    // --- horizontal pass ---

    for (int y = 0; y < fheight; ++y)
    {
        float *inRow = slice.ptr<float>(y);
        float *outRow = temp.ptr<float>(y);

        if (y%150==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y*50)/fheight)
                );

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int xx = std::max(0, std::min(fwidth - 1, x + k));
                acc += kernel[k + r] * inRow[xx];
            }

            outRow[x] = static_cast<float>(acc);
        }
    }

    // --- vertical pass ---

    for (int y = 0; y < fheight; ++y)
    {

        if (y%150==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y*50)/fheight)
                );

        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int yy = std::max(0, std::min(fheight - 1, y + k));
                float *row = temp.ptr<float>(yy);

                acc += kernel[k + r] * row[x];
            }

            outRow[x] = static_cast<float>(acc);
        }
    }
}


void MLFeatureGaussian::CalcFeatureGaussian3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{

    float sigma = std::pow(2.0f,_arg1);

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int gau2index = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, _arg2);

    int r = static_cast<int>(ceil(3.0 * sigma));
    int ksize = 2 * r + 1;

    std::vector<float> kernel(ksize);

    double sum = 0.0;
    for (int k = -r; k <= r; ++k)
    {
        double v = exp(-(k * k) / (2.0 * sigma * sigma));
        kernel[k + r] = static_cast<float>(v);
        sum += v;
    }

    for (float &v : kernel)
        v /= sum;

    int z0 = std::max(0, sliceID - r);
    int z1 = std::min(FileCount - 1, sliceID + r);

    QVector<cv::Mat> slices;
    QVector<float> zweights;

    for (int i = z0; i <= z1; i++)
    {
        slices.append(data->GetWholeSliceFeature(i, gau2index));

        int dz = i - sliceID;
        zweights.append(kernel[dz + r]);
    }

    double wsum = 0.0;
    for (float w : zweights)
        wsum += w;

    for (int y = 0; y < fheight; ++y)
    {
        if (y%150==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y*100)/fheight)
                );

        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int z = z0; z <= z1; ++z)
            {
                float w = zweights[z - z0];
                acc += w * slices[z - z0].at<float>(y, x);
            }

            outRow[x] = (wsum > 0.0) ? static_cast<float>(acc / wsum) : 0.0f;
        }
    }
}

void MLFeatureGaussian::CalcFeatureGaussian2DROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    const float sigma = std::pow(2.0f, _arg1);
    const int radius = static_cast<int>(std::ceil(3.0 * sigma));
    const int kernelSize = 2 * radius + 1;

    std::vector<float> kernel(kernelSize);
    double kernelSum = 0.0;
    for (int k = -radius; k <= radius; k++)
    {
        const double value =
            std::exp(-(k * k) / (2.0 * sigma * sigma));
        kernel[k + radius] = static_cast<float>(value);
        kernelSum += value;
    }
    for (float &value : kernel)
        value /= kernelSum;

    const MLROISlice sourceROI = roi.expandedByPixels(radius);
    const int intensityIndex = data->GetIndexForFeature(
        MLFeature::FeatureType::Intensity,
        _channel,
        false,
        0,
        0);
    const cv::Mat source =
        data->GetROISliceFeature(sliceID, intensityIndex, sourceROI);
    cv::Mat horizontal = cv::Mat::zeros(
        data->GetYSize(),
        data->GetXSize(),
        CV_32F);

    for (int tileY = 0; tileY < sourceROI.tileRows(); tileY++)
    {
        for (int tileX = 0; tileX < sourceROI.tileColumns(); tileX++)
        {
            if (sourceROI.tileState(tileX, tileY)
                == MLROISlice::TileState::Inactive)
            {
                continue;
            }

            const QRect tile = sourceROI.tileRect(tileX, tileY);
            for (int y = tile.top(); y <= tile.bottom(); y++)
            {
                const float *sourceRow = source.ptr<float>(y);
                float *horizontalRow = horizontal.ptr<float>(y);
                for (int x = tile.left(); x <= tile.right(); x++)
                {
                    double value = 0.0;
                    for (int k = -radius; k <= radius; k++)
                    {
                        const int sourceX =
                            qBound(0, x + k, data->GetXSize() - 1);
                        value +=
                            kernel[k + radius] * sourceRow[sourceX];
                    }
                    horizontalRow[x] = static_cast<float>(value);
                }
            }
        }
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
                float *outputRow = mat.ptr<float>(y);
                for (int x = tile.left(); x <= tile.right(); x++)
                {
                    double value = 0.0;
                    for (int k = -radius; k <= radius; k++)
                    {
                        const int sourceY =
                            qBound(0, y + k, data->GetYSize() - 1);
                        value += kernel[k + radius]
                                 * horizontal.at<float>(sourceY, x);
                    }
                    outputRow[x] = static_cast<float>(value);
                }
            }
        }
    }
}

void MLFeatureGaussian::CalcFeatureGaussian3DROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    const float sigma = std::pow(2.0f, _arg1);
    const int radius = static_cast<int>(std::ceil(3.0 * sigma));
    const int kernelSize = 2 * radius + 1;

    std::vector<float> kernel(kernelSize);
    double kernelSum = 0.0;
    for (int k = -radius; k <= radius; k++)
    {
        const double value =
            std::exp(-(k * k) / (2.0 * sigma * sigma));
        kernel[k + radius] = static_cast<float>(value);
        kernelSum += value;
    }
    for (float &value : kernel)
        value /= kernelSum;

    const int gaussian2DIndex = data->GetIndexForFeature(
        MLFeature::FeatureType::Gaussian,
        _channel,
        false,
        _arg1,
        0);
    const int firstSlice = qMax(0, sliceID - radius);
    const int lastSlice = qMin(FileCount - 1, sliceID + radius);

    QVector<cv::Mat> slices;
    QVector<float> weights;
    double weightSum = 0.0;
    for (int sourceSlice = firstSlice;
         sourceSlice <= lastSlice;
         sourceSlice++)
    {
        slices.append(
            data->GetROISliceFeature(
                sourceSlice,
                gaussian2DIndex,
                roi));
        const float weight =
            kernel[sourceSlice - sliceID + radius];
        weights.append(weight);
        weightSum += weight;
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
                float *outputRow = mat.ptr<float>(y);
                for (int x = tile.left(); x <= tile.right(); x++)
                {
                    double value = 0.0;
                    for (int slice = 0; slice < slices.size(); slice++)
                    {
                        value += weights.at(slice)
                                 * slices.at(slice).at<float>(y, x);
                    }
                    outputRow[x] =
                        weightSum > 0.0
                            ? static_cast<float>(value / weightSum)
                            : 0.0f;
                }
            }
        }
    }
}

int MLFeatureGaussian::GetDependencyDepth()
{
    if (_is3D)
        return 2;
    else
        return 1;
}


QString MLFeatureGaussian::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(pow(2.0f,v));
}

QString MLFeatureGaussian::GetArg2SetupString(int)
{
    return "";
}
