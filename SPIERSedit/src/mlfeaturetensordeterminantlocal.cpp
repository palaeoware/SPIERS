/**
 * @file
 * Source: Mlfeaturetensordeterminantlocal
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
#include "mlfeaturetensordeterminantlocal.h"
#include "mlfeaturetensorcomponentlocal.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <cmath>

MLFeatureTensorDeterminantLocal::MLFeatureTensorDeterminantLocal(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Tensor_determinant_local, channel, is3D, arg1, 0)
{
}

void MLFeatureTensorDeterminantLocal::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    using TC = MLFeatureTensorComponentLocal::TensorComponent;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int xxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1, static_cast<int>(TC::XX));
    int yyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1, static_cast<int>(TC::YY));
    int xyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1, static_cast<int>(TC::XY));

    cv::Mat xx = data->GetWholeSliceFeature(sliceID, xxIndex);
    cv::Mat yy = data->GetWholeSliceFeature(sliceID, yyIndex);
    cv::Mat xy = data->GetWholeSliceFeature(sliceID, xyIndex);

    cv::Mat zz, xz, yz;
    if (_is3D)
    {
        int zzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1, static_cast<int>(TC::ZZ));
        int xzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1, static_cast<int>(TC::XZ));
        int yzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1, static_cast<int>(TC::YZ));
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

QList<MLFeature *> MLFeatureTensorDeterminantLocal::GetDependencies()
{
    using TC = MLFeatureTensorComponentLocal::TensorComponent;
    QList<MLFeature *> deps;
    deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::XX));
    deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::YY));
    deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::XY));
    if (_is3D)
    {
        deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::ZZ));
        deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::XZ));
        deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::YZ));
    }
    return deps;
}

QString MLFeatureTensorDeterminantLocal::GetPrettyName() { return "t-determinant local"; }
QString MLFeatureTensorDeterminantLocal::GetPrettyArgs() { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(_arg1))); }
QString MLFeatureTensorDeterminantLocal::GetPretty3D() { return _is3D ? "3D" : "2D"; }
int MLFeatureTensorDeterminantLocal::GetDependencyDepth() { return 5; }
int MLFeatureTensorDeterminantLocal::GetMinMaxForArgs(int arg, bool max) { if (arg == 1) return max ? 6 : 0; return 0; }
QString MLFeatureTensorDeterminantLocal::GetTypeCodeForFile() { return "tdl"; }
QString MLFeatureTensorDeterminantLocal::GetArgsForFile() { return QString("@%1").arg(_arg1); }
QString MLFeatureTensorDeterminantLocal::Get3DForFile() { return _is3D ? "3" : "2"; }
QString MLFeatureTensorDeterminantLocal::GetArg1SetupString(int v) { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(v))); }
QString MLFeatureTensorDeterminantLocal::GetArg2SetupString(int) { return ""; }
