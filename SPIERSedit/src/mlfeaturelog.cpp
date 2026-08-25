/**
 * @file
 * Source: Mlfeaturelog
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
#include "mlfeaturelog.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeaturegaussian.h"
#include "mlroislice.h"

MLFeatureLoG::MLFeatureLoG(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Laplacian_of_gaussian, channel, is3D, arg1, 0)
{
}

void MLFeatureLoG::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureLoG3D(mat, sliceID, data);
    else
        CalcFeatureLoG2D(mat, sliceID, data);
}

bool MLFeatureLoG::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    const float sigma = std::pow(2.0f, _arg1);
    const float scaleFactor = sigma * sigma;
    const MLROISlice sourceROI = roi.expandedByPixels(1);
    const int gaussianIndex = data->GetIndexForFeature(
        MLFeature::FeatureType::Gaussian,
        _channel,
        _is3D,
        _arg1,
        0);

    const cv::Mat current = data->GetROISliceFeature(
        sliceID,
        gaussianIndex,
        sourceROI);
    cv::Mat previous;
    cv::Mat next;
    if (_is3D)
    {
        previous = data->GetROISliceFeature(
            qMax(0, sliceID - 1),
            gaussianIndex,
            sourceROI);
        next = data->GetROISliceFeature(
            qMin(FileCount - 1, sliceID + 1),
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
                    const float centre = currentRow[x];
                    float laplacian =
                        currentRow[previousX]
                        + currentRow[nextX]
                        + previousYRow[x]
                        + nextYRow[x]
                        - 4.0f * centre;
                    if (_is3D)
                    {
                        laplacian +=
                            nextZRow[x]
                            - 2.0f * centre
                            + previousZRow[x];
                    }
                    outputRow[x] = scaleFactor * laplacian;
                }
            }
        }
    }

    return false;
}

QList<MLFeature *> MLFeatureLoG::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureGaussian(_channel, _is3D, _arg1));
    return deps;
}

QString MLFeatureLoG::GetPrettyName()
{
    return "lapl. gauss.";
}

QString MLFeatureLoG::GetPrettyArgs()
{
    return QString("sigma=%1").arg(std::pow(2.0f, _arg1));
}

QString MLFeatureLoG::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureLoG::GetTypeCodeForFile()
{
    return "log";
}

QString MLFeatureLoG::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureLoG::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureLoG::CalcFeatureLoG2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, _arg1);
    float scaleFactor = sigma * sigma;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, 0);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, gauIndex);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating %1")
            .arg(GetPrettyFullName())
        );

    CalcLaplacian2D(mat, slice, scaleFactor);
}

void MLFeatureLoG::CalcFeatureLoG3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, _arg1);
    float scaleFactor = sigma * sigma;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, true, _arg1, 0);

    int z0 = std::max(0, sliceID - 1);
    int z1 = std::min(FileCount - 1, sliceID + 1);

    QVector<cv::Mat> slices;
    for (int z = z0; z <= z1; ++z)
        slices.append(data->GetWholeSliceFeature(z, gauIndex));

    int centralSliceIndex = sliceID - z0;

    cv::Mat xyTerm(fheight, fwidth, CV_32F);
    cv::Mat zTerm(fheight, fwidth, CV_32F);

    CalcLaplacian2D(xyTerm, slices[centralSliceIndex], scaleFactor);
    CalcSecondDerivativeZZ(zTerm, slices, centralSliceIndex, scaleFactor);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Combining %1")
            .arg(GetPrettyFullName())
        );

    cv::add(xyTerm, zTerm, mat);
}

int MLFeatureLoG::GetDependencyDepth()
{
    if (_is3D)
        return 3;
    else
        return 2;
}

QString MLFeatureLoG::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(std::pow(2.0f, v));
}

QString MLFeatureLoG::GetArg2SetupString(int)
{
    return "";
}
