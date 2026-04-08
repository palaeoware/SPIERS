/**
 * @file
 * Source: Mlfeaturehessian
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
#include "mlfeaturehessian.h"

#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeaturegaussian.h"

#include <cmath>

MLFeatureHessian::MLFeatureHessian(Channel channel, bool is3D, int arg1, HessianMode mode)
    : MLFeature(FeatureType::Hessian, channel, is3D, arg1, static_cast<int>(mode))
{
}

MLFeatureHessian::MLFeatureHessian(Channel channel, bool is3D, int arg1, int arg2)
    : MLFeature(FeatureType::Hessian, channel, is3D, arg1, arg2)
{
}

MLFeatureHessian::HessianMode MLFeatureHessian::GetMode() const
{
    return static_cast<HessianMode>(_arg2);
}

bool MLFeatureHessian::IsValidMode() const
{
    HessianMode mode = GetMode();

    if (_is3D)
    {
        switch (mode)
        {
        case HessianMode::XX:
        case HessianMode::YY:
        case HessianMode::ZZ:
        case HessianMode::XY:
        case HessianMode::XZ:
        case HessianMode::YZ:
        case HessianMode::Determinant:
            return true;
        default:
            return false;
        }
    }
    else
    {
        switch (mode)
        {
        case HessianMode::XX:
        case HessianMode::YY:
        case HessianMode::XY:
        case HessianMode::Determinant:
            return true;
        case HessianMode::ZZ:
        case HessianMode::XZ:
        case HessianMode::YZ:
        default:
            return false;
        }
    }
}

QString MLFeatureHessian::GetModeString() const
{
    switch (GetMode())
    {
    case HessianMode::XX:
        return "xx";
    case HessianMode::YY:
        return "yy";
    case HessianMode::XY:
        return "xy";
    case HessianMode::ZZ:
        return "zz";
    case HessianMode::XZ:
        return "xz";
    case HessianMode::YZ:
        return "yz";
    case HessianMode::Determinant:
        return "det";
    default:
        return "unknown";
    }
}

void MLFeatureHessian::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(IsValidMode());

    HessianMode mode = GetMode();

    if (mode == HessianMode::Determinant)
    {
        if (_is3D)
            CalcFeatureDeterminant3D(mat, sliceID, data);
        else
            CalcFeatureDeterminant2D(mat, sliceID, data);
    }
    else
    {
        if (_is3D)
            CalcFeaturePrimitive3D(mat, sliceID, data);
        else
            CalcFeaturePrimitive2D(mat, sliceID, data);
    }
}

QList<MLFeature *> MLFeatureHessian::GetDependencies()
{
    QList<MLFeature *> deps;

    HessianMode mode = GetMode();

    if (mode == HessianMode::Determinant)
    {
        if (_is3D)
        {
            deps.append(new MLFeatureHessian(_channel, true, _arg1, HessianMode::XX));
            deps.append(new MLFeatureHessian(_channel, true, _arg1, HessianMode::YY));
            deps.append(new MLFeatureHessian(_channel, true, _arg1, HessianMode::ZZ));
            deps.append(new MLFeatureHessian(_channel, true, _arg1, HessianMode::XY));
            deps.append(new MLFeatureHessian(_channel, true, _arg1, HessianMode::XZ));
            deps.append(new MLFeatureHessian(_channel, true, _arg1, HessianMode::YZ));
        }
        else
        {
            deps.append(new MLFeatureHessian(_channel, false, _arg1, HessianMode::XX));
            deps.append(new MLFeatureHessian(_channel, false, _arg1, HessianMode::YY));
            deps.append(new MLFeatureHessian(_channel, false, _arg1, HessianMode::XY));
        }
    }
    else
    {
        deps.append(new MLFeatureGaussian(_channel, _is3D, _arg1));
    }

    return deps;
}

QString MLFeatureHessian::GetPrettyName()
{
    return "hessian";
}

QString MLFeatureHessian::GetPrettyArgs()
{
    return QString("Sigma=%1, %2")
        .arg(std::pow(2.0f, static_cast<float>(_arg1)))
        .arg(GetModeString());
}

QString MLFeatureHessian::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureHessian::GetTypeCodeForFile()
{
    return "hes";
}

QString MLFeatureHessian::GetArgsForFile()
{
    return QString("@%1@%2").arg(_arg1).arg(_arg2);
}

QString MLFeatureHessian::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureHessian::CalcFeaturePrimitive2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, static_cast<float>(_arg1));
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

    switch (GetMode())
    {
    case HessianMode::XX:
        CalcSecondDerivativeXX(mat, slice, scaleFactor);
        break;

    case HessianMode::YY:
        CalcSecondDerivativeYY(mat, slice, scaleFactor);
        break;

    case HessianMode::XY:
        CalcSecondDerivativeXY(mat, slice, scaleFactor);
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

void MLFeatureHessian::CalcFeaturePrimitive3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, static_cast<float>(_arg1));
    float scaleFactor = sigma * sigma;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, true, _arg1, 0);

    int prevSlice = std::max(0, sliceID - 1);
    int nextSlice = std::min(FileCount - 1, sliceID + 1);

    QVector<cv::Mat> slices;
    slices.append(data->GetWholeSliceFeature(prevSlice, gauIndex));
    slices.append(data->GetWholeSliceFeature(sliceID, gauIndex));
    slices.append(data->GetWholeSliceFeature(nextSlice, gauIndex));

    const cv::Mat &cur = slices[1];

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating %1")
            .arg(GetPrettyFullName())
        );

    switch (GetMode())
    {
    case HessianMode::XX:
        CalcSecondDerivativeXX(mat, cur, scaleFactor);
        break;

    case HessianMode::YY:
        CalcSecondDerivativeYY(mat, cur, scaleFactor);
        break;

    case HessianMode::ZZ:
        CalcSecondDerivativeZZ(mat, slices, 1, scaleFactor);
        break;

    case HessianMode::XY:
        CalcSecondDerivativeXY(mat, cur, scaleFactor);
        break;

    case HessianMode::XZ:
        CalcSecondDerivativeXZ(mat, slices, 1, scaleFactor);
        break;

    case HessianMode::YZ:
        CalcSecondDerivativeYZ(mat, slices, 1, scaleFactor);
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

void MLFeatureHessian::CalcFeatureDeterminant2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int xxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, false, _arg1,
                                           static_cast<int>(HessianMode::XX));
    int yyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, false, _arg1,
                                           static_cast<int>(HessianMode::YY));
    int xyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, false, _arg1,
                                           static_cast<int>(HessianMode::XY));

    cv::Mat xx = data->GetWholeSliceFeature(sliceID, xxIndex);
    cv::Mat yy = data->GetWholeSliceFeature(sliceID, yyIndex);
    cv::Mat xy = data->GetWholeSliceFeature(sliceID, xyIndex);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1 %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        const float *xxRow = xx.ptr<float>(y);
        const float *yyRow = yy.ptr<float>(y);
        const float *xyRow = xy.ptr<float>(y);
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
            outRow[x] = xxRow[x] * yyRow[x] - xyRow[x] * xyRow[x];
    }
}

void MLFeatureHessian::CalcFeatureDeterminant3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int xxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, true, _arg1,
                                           static_cast<int>(HessianMode::XX));
    int yyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, true, _arg1,
                                           static_cast<int>(HessianMode::YY));
    int zzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, true, _arg1,
                                           static_cast<int>(HessianMode::ZZ));
    int xyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, true, _arg1,
                                           static_cast<int>(HessianMode::XY));
    int xzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, true, _arg1,
                                           static_cast<int>(HessianMode::XZ));
    int yzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Hessian, _channel, true, _arg1,
                                           static_cast<int>(HessianMode::YZ));

    cv::Mat xx = data->GetWholeSliceFeature(sliceID, xxIndex);
    cv::Mat yy = data->GetWholeSliceFeature(sliceID, yyIndex);
    cv::Mat zz = data->GetWholeSliceFeature(sliceID, zzIndex);
    cv::Mat xy = data->GetWholeSliceFeature(sliceID, xyIndex);
    cv::Mat xz = data->GetWholeSliceFeature(sliceID, xzIndex);
    cv::Mat yz = data->GetWholeSliceFeature(sliceID, yzIndex);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1 %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        const float *xxRow = xx.ptr<float>(y);
        const float *yyRow = yy.ptr<float>(y);
        const float *zzRow = zz.ptr<float>(y);
        const float *xyRow = xy.ptr<float>(y);
        const float *xzRow = xz.ptr<float>(y);
        const float *yzRow = yz.ptr<float>(y);
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            float vxx = xxRow[x];
            float vyy = yyRow[x];
            float vzz = zzRow[x];
            float vxy = xyRow[x];
            float vxz = xzRow[x];
            float vyz = yzRow[x];

            outRow[x] =
                vxx * vyy * vzz
                + 2.0f * vxy * vxz * vyz
                - vxx * vyz * vyz
                - vyy * vxz * vxz
                - vzz * vxy * vxy;
        }
    }
}

int MLFeatureHessian::GetDependencyDepth()
{
    HessianMode mode = GetMode();

    if (mode == HessianMode::Determinant)
    {
        if (_is3D)
            return 4;
        else
            return 3;
    }
    else
    {
        if (_is3D)
            return 3;
        else
            return 2;
    }
}

QString MLFeatureHessian::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(std::pow(2.0f, static_cast<float>(v)));
}

QString MLFeatureHessian::GetArg2SetupString(int v)
{
    HessianMode mode = static_cast<HessianMode>(v);

    switch (mode)
    {
    case HessianMode::XX:
        return "xx";
    case HessianMode::YY:
        return "yy";
    case HessianMode::XY:
        return "xy";
    case HessianMode::ZZ:
        return "zz";
    case HessianMode::XZ:
        return "xz";
    case HessianMode::YZ:
        return "yz";
    case HessianMode::Determinant:
        return "determinant";
    default:
        return "";
    }
}


int MLFeatureHessian::GetMinMaxForArgs(int arg, bool max)
{
    if (arg==1 && !max) return 0;
    if (arg==1 && max) return 6;
    if (arg==2 && !max) return 0;
    if (arg==2 && max) return 6;
    return 0;
}
