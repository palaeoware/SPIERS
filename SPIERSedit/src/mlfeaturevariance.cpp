/**
 * @file
 * Source: Mlfeaturevariance
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
#include "mlfeaturevariance.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeatureintensity.h"
#include "mlfeaturesquareintensity.h"

#include <cmath>

MLFeatureVariance::MLFeatureVariance(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Local_variance, channel, is3D, arg1, 0)
{}

void MLFeatureVariance::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureVariance3D(mat, sliceID, data);
    else
        CalcFeatureVariance2D(mat, sliceID, data);
}

QList<MLFeature *> MLFeatureVariance::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureIntensity(_channel));
    deps.append(new MLFeatureSquareIntensity(_channel));
    return deps;
}

QString MLFeatureVariance::GetPrettyName()
{
    return "variance";
}

QString MLFeatureVariance::GetPrettyArgs()
{
    return QString("Radius=%1").arg(pow(2.0f, _arg1));
}

QString MLFeatureVariance::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

int MLFeatureVariance::GetDependencyDepth()
{
    if (_is3D)
        return 3;
    else
        return 2;
}

QString MLFeatureVariance::GetTypeCodeForFile()
{
    return "var";
}

QString MLFeatureVariance::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureVariance::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureVariance::CalcFeatureVariance2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idxIntensity = data->GetIndexForFeature(MLFeature::FeatureType::Intensity, _channel, false, 0, 0);
    int idxSquare    = data->GetIndexForFeature(MLFeature::FeatureType::Square, _channel, false, 0, 0);

    cv::Mat intensity = data->GetWholeSliceFeature(sliceID, idxIntensity);
    cv::Mat square    = data->GetWholeSliceFeature(sliceID, idxSquare);

    cv::Mat meanI(fheight, fwidth, CV_32F);
    cv::Mat meanI2(fheight, fwidth, CV_32F);

    CalcLocalMean2D(meanI, intensity, _arg1);
    CalcLocalMean2D(meanI2, square, _arg1);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        const float *rowMeanI  = meanI.ptr<float>(y);
        const float *rowMeanI2 = meanI2.ptr<float>(y);
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            float m  = rowMeanI[x];
            float m2 = rowMeanI2[x];

            float var = m2 - m * m;
            if (var < 0.0f)
                var = 0.0f;

            outRow[x] = var;
        }
    }
}

void MLFeatureVariance::CalcFeatureVariance3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idxIntensity = data->GetIndexForFeature(MLFeature::FeatureType::Intensity, _channel, false, 0, 0);
    int idxSquare    = data->GetIndexForFeature(MLFeature::FeatureType::Square, _channel, false, 0, 0);

    int r = static_cast<int>(pow(2.0f, _arg1));



    QVector<cv::Mat> intensitySlices2DMean;
    QVector<cv::Mat> squareSlices2DMean;

    intensitySlices2DMean.reserve(r*2+1);
    squareSlices2DMean.reserve(r*2+1);

    for (int i = -r; i <= r; ++i)
    {
        int z = sliceID + i;
        if (z<0) z=0;
        if (z>FileCount - 1) z = FileCount - 1;
        cv::Mat intensity = data->GetWholeSliceFeature(z, idxIntensity);
        cv::Mat square    = data->GetWholeSliceFeature(z, idxSquare);

        cv::Mat meanI2D(fheight, fwidth, CV_32F);
        cv::Mat meanI22D(fheight, fwidth, CV_32F);

        CalcLocalMean2D(meanI2D, intensity, _arg1);
        CalcLocalMean2D(meanI22D, square, _arg1);

        intensitySlices2DMean.append(meanI2D);
        squareSlices2DMean.append(meanI22D);
    }

    cv::Mat meanI3D(fheight, fwidth, CV_32F);
    cv::Mat meanI23D(fheight, fwidth, CV_32F);

    CalcZMean(meanI3D, intensitySlices2DMean, r, _arg1);
    CalcZMean(meanI23D, squareSlices2DMean, r, _arg1);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        const float *rowMeanI  = meanI3D.ptr<float>(y);
        const float *rowMeanI2 = meanI23D.ptr<float>(y);
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            float m  = rowMeanI[x];
            float m2 = rowMeanI2[x];

            float var = m2 - m * m;
            if (var < 0.0f)
                var = 0.0f;

            outRow[x] = var;
        }
    }
}

QString MLFeatureVariance::GetArg1SetupString(int v)
{
    return QString("radius=%1")
        .arg(pow(2.0f, v));
}

QString MLFeatureVariance::GetArg2SetupString(int)
{
    return "";
}
