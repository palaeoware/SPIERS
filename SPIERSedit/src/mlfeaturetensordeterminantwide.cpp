/**
 * @file
 * Source: Mlfeaturetensordeterminantwide
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
#include "mlfeaturetensordeterminantwide.h"
#include "mlfeaturetensorcomponentwide.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <cmath>

MLFeatureTensorDeterminantWide::MLFeatureTensorDeterminantWide(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Tensor_determinant_wide, channel, is3D, arg1, 0)
{
}

void MLFeatureTensorDeterminantWide::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    using TC = MLFeatureTensorComponentWide::TensorComponent;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int xxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1, static_cast<int>(TC::XX));
    int yyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1, static_cast<int>(TC::YY));
    int xyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1, static_cast<int>(TC::XY));

    cv::Mat xx = data->GetWholeSliceFeature(sliceID, xxIndex);
    cv::Mat yy = data->GetWholeSliceFeature(sliceID, yyIndex);
    cv::Mat xy = data->GetWholeSliceFeature(sliceID, xyIndex);

    cv::Mat zz, xz, yz;
    if (_is3D)
    {
        int zzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1, static_cast<int>(TC::ZZ));
        int xzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1, static_cast<int>(TC::XZ));
        int yzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1, static_cast<int>(TC::YZ));
        zz = data->GetWholeSliceFeature(sliceID, zzIndex);
        xz = data->GetWholeSliceFeature(sliceID, xzIndex);
        yz = data->GetWholeSliceFeature(sliceID, yzIndex);
    }

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1 %2%").arg(GetPrettyFullName()).arg((y * 100) / fheight));

        const float *xxRow = xx.ptr<float>(y);
        const float *yyRow = yy.ptr<float>(y);
        const float *xyRow = xy.ptr<float>(y);
        const float *zzRow = _is3D ? zz.ptr<float>(y) : nullptr;
        const float *xzRow = _is3D ? xz.ptr<float>(y) : nullptr;
        const float *yzRow = _is3D ? yz.ptr<float>(y) : nullptr;
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            if (_is3D)
            {
                float vxx = xxRow[x], vyy = yyRow[x], vzz = zzRow[x];
                float vxy = xyRow[x], vxz = xzRow[x], vyz = yzRow[x];
                outRow[x] = vxx * vyy * vzz
                            + 2.0f * vxy * vxz * vyz
                            - vxx * vyz * vyz
                            - vyy * vxz * vxz
                            - vzz * vxy * vxy;
            }
            else
            {
                outRow[x] = xxRow[x] * yyRow[x] - xyRow[x] * xyRow[x];
            }
        }
    }
}

bool MLFeatureTensorDeterminantWide::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    CalcTensorDeterminantROI(
        mat,
        sliceID,
        data,
        FeatureType::Tensor_component_wide,
        roi);
    return false;
}

QList<MLFeature *> MLFeatureTensorDeterminantWide::GetDependencies()
{
    using TC = MLFeatureTensorComponentWide::TensorComponent;
    QList<MLFeature *> deps;
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::XX));
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::YY));
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::XY));
    if (_is3D)
    {
        deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::ZZ));
        deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::XZ));
        deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::YZ));
    }
    return deps;
}

QString MLFeatureTensorDeterminantWide::GetPrettyName() { return "t-determinant wide"; }
QString MLFeatureTensorDeterminantWide::GetPrettyArgs() { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(_arg1))); }
QString MLFeatureTensorDeterminantWide::GetPretty3D() { return _is3D ? "3D" : "2D"; }
int MLFeatureTensorDeterminantWide::GetDependencyDepth() { return 5; }
int MLFeatureTensorDeterminantWide::GetMinMaxForArgs(int arg, bool max) { if (arg == 1) return max ? 6 : 0; return 0; }
QString MLFeatureTensorDeterminantWide::GetTypeCodeForFile() { return "tdw"; }
QString MLFeatureTensorDeterminantWide::GetArgsForFile() { return QString("@%1").arg(_arg1); }
QString MLFeatureTensorDeterminantWide::Get3DForFile() { return _is3D ? "3" : "2"; }
QString MLFeatureTensorDeterminantWide::GetArg1SetupString(int v) { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(v))); }
QString MLFeatureTensorDeterminantWide::GetArg2SetupString(int) { return ""; }
