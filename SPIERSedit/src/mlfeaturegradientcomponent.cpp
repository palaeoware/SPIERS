/**
 * @file
 * Source: Mlfeaturegradientcomponent
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
#include "mlfeaturegradientcomponent.h"

#include "globals.h"
#include "mlfeaturegaussian.h"
#include "mlroislice.h"
#include "mlupdateblockingdialog.h"

#include <cmath>

MLFeatureGradientComponent::MLFeatureGradientComponent(Channel channel, int arg1, GradientDirection dir)
    : MLFeature(FeatureType::Gradient_component, channel, false, arg1, static_cast<int>(dir))
{
}

MLFeatureGradientComponent::MLFeatureGradientComponent(Channel channel, int arg1, int arg2)
    : MLFeature(FeatureType::Gradient_component, channel, false, arg1, arg2)
{
}

MLFeatureGradientComponent::GradientDirection MLFeatureGradientComponent::GetDirection() const
{
    return static_cast<GradientDirection>(_arg2);
}

QString MLFeatureGradientComponent::GetDirectionString() const
{
    switch (GetDirection())
    {
    case GradientDirection::X:
        return "x";
    case GradientDirection::Y:
        return "y";
    case GradientDirection::Z:
        return "z";
    default:
        return "?";
    }
}

void MLFeatureGradientComponent::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, static_cast<float>(_arg1));
    float scaleFactor = sigma;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating %1")
            .arg(GetPrettyFullName())
        );

    switch (GetDirection())
    {
    case GradientDirection::X:
    {
        int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, 0);
        cv::Mat slice = data->GetWholeSliceFeature(sliceID, gauIndex);
        CalcFirstDerivativeX(mat, slice, scaleFactor);
        break;
    }

    case GradientDirection::Y:
    {
        int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, 0);
        cv::Mat slice = data->GetWholeSliceFeature(sliceID, gauIndex);
        CalcFirstDerivativeY(mat, slice, scaleFactor);
        break;
    }

    case GradientDirection::Z:
    {
        int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, true, _arg1, 0);

        int prevSlice = std::max(0, sliceID - 1);
        int nextSlice = std::min(FileCount - 1, sliceID + 1);

        QVector<cv::Mat> slices;
        slices.append(data->GetWholeSliceFeature(prevSlice, gauIndex));
        slices.append(data->GetWholeSliceFeature(sliceID, gauIndex));
        slices.append(data->GetWholeSliceFeature(nextSlice, gauIndex));

        CalcFirstDerivativeZ(mat, slices, 1, scaleFactor);
        break;
    }

    default:
        Q_ASSERT(false);
        break;
    }
}

bool MLFeatureGradientComponent::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    const float scaleFactor =
        std::pow(2.0f, static_cast<float>(_arg1));
    const GradientDirection direction = GetDirection();
    const bool zDirection = direction == GradientDirection::Z;
    const MLROISlice sourceROI =
        zDirection ? roi : roi.expandedByPixels(1);
    const int gaussianIndex = data->GetIndexForFeature(
        MLFeature::FeatureType::Gaussian,
        _channel,
        zDirection,
        _arg1,
        0);

    cv::Mat current;
    cv::Mat previous;
    cv::Mat next;
    if (zDirection)
    {
        const int previousSlice = qMax(0, sliceID - 1);
        const int nextSlice = qMin(FileCount - 1, sliceID + 1);
        previous = data->GetROISliceFeature(
            previousSlice,
            gaussianIndex,
            sourceROI);
        next = data->GetROISliceFeature(
            nextSlice,
            gaussianIndex,
            sourceROI);
    }
    else
    {
        current = data->GetROISliceFeature(
            sliceID,
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
                const float *currentRow =
                    zDirection ? nullptr : current.ptr<float>(y);
                const float *previousYRow =
                    direction == GradientDirection::Y
                        ? current.ptr<float>(previousY)
                        : nullptr;
                const float *nextYRow =
                    direction == GradientDirection::Y
                        ? current.ptr<float>(nextY)
                        : nullptr;
                const float *previousZRow =
                    zDirection ? previous.ptr<float>(y) : nullptr;
                const float *nextZRow =
                    zDirection ? next.ptr<float>(y) : nullptr;
                float *outputRow = mat.ptr<float>(y);

                for (int x = tile.left(); x <= tile.right(); x++)
                {
                    switch (direction)
                    {
                    case GradientDirection::X:
                    {
                        const int previousX = qMax(0, x - 1);
                        const int nextX = qMin(fwidth - 1, x + 1);
                        outputRow[x] =
                            scaleFactor * 0.5f
                            * (currentRow[nextX]
                               - currentRow[previousX]);
                        break;
                    }
                    case GradientDirection::Y:
                        outputRow[x] =
                            scaleFactor * 0.5f
                            * (nextYRow[x] - previousYRow[x]);
                        break;
                    case GradientDirection::Z:
                        outputRow[x] =
                            scaleFactor * 0.5f
                            * (nextZRow[x] - previousZRow[x]);
                        break;
                    }
                }
            }
        }
    }

    return false;
}

QList<MLFeature *> MLFeatureGradientComponent::GetDependencies()
{
    QList<MLFeature *> deps;

    switch (GetDirection())
    {
    case GradientDirection::X:
    case GradientDirection::Y:
        deps.append(new MLFeatureGaussian(_channel, false, _arg1));
        break;

    case GradientDirection::Z:
        deps.append(new MLFeatureGaussian(_channel, true, _arg1));
        break;

    default:
        Q_ASSERT(false);
        break;
    }

    return deps;
}

QString MLFeatureGradientComponent::GetPrettyName()
{
    return "grad. comp.";
}

QString MLFeatureGradientComponent::GetPrettyArgs()
{
    return QString("sigma=%1, %2")
        .arg(std::pow(2.0f, static_cast<float>(_arg1)))
        .arg(GetDirectionString());
}

QString MLFeatureGradientComponent::GetPretty3D()
{
    return "";
}

QString MLFeatureGradientComponent::GetTypeCodeForFile()
{
    return "grc";
}

QString MLFeatureGradientComponent::GetArgsForFile()
{
    return QString("@%1@%2").arg(_arg1).arg(_arg2);
}

QString MLFeatureGradientComponent::Get3DForFile()
{
    return "";
}

int MLFeatureGradientComponent::GetDependencyDepth()
{
    switch (GetDirection())
    {
    case GradientDirection::X:
    case GradientDirection::Y:
        return 2; // Gaussian2D -> gradient
    case GradientDirection::Z:
        return 3; // Gaussian3D -> gradient
    default:
        Q_ASSERT(false);
        return 0;
    }
}

QString MLFeatureGradientComponent::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(std::pow(2.0f, static_cast<float>(v)));
}

QString MLFeatureGradientComponent::GetArg2SetupString(int v)
{
    GradientDirection dir = static_cast<GradientDirection>(v);

    switch (dir)
    {
    case GradientDirection::X:
        return "x";
    case GradientDirection::Y:
        return "y";
    case GradientDirection::Z:
        return "z";
    default:
        return "";
    }
}


int MLFeatureGradientComponent::GetMinMaxForArgs(int arg, bool max)
{
    if (arg==1 && !max) return 0;
    if (arg==1 && max) return 6;
    if (arg==2 && !max) return 0;
    if (arg==2 && max) return 2;
    return 0;
}
