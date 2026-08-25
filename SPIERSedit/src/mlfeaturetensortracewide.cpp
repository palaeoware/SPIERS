/**
 * @file
 * Source: Mlfeaturetensortracewide
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
#include "mlfeaturetensortracewide.h"
#include "mlfeaturetensorcomponentwide.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <cmath>

MLFeatureTensorTraceWide::MLFeatureTensorTraceWide(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Tensor_trace_wide, channel, is3D, arg1, 0)
{
}

void MLFeatureTensorTraceWide::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    using TC = MLFeatureTensorComponentWide::TensorComponent;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int xxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1,
                                           static_cast<int>(TC::XX));
    int yyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1,
                                           static_cast<int>(TC::YY));

    cv::Mat xx = data->GetWholeSliceFeature(sliceID, xxIndex);
    cv::Mat yy = data->GetWholeSliceFeature(sliceID, yyIndex);

    cv::Mat zz;
    if (_is3D)
    {
        int zzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_wide, _channel, false, _arg1,
                                               static_cast<int>(TC::ZZ));
        zz = data->GetWholeSliceFeature(sliceID, zzIndex);
    }

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1 %2%").arg(GetPrettyFullName()).arg((y * 100) / fheight));

        const float *xxRow = xx.ptr<float>(y);
        const float *yyRow = yy.ptr<float>(y);
        const float *zzRow = _is3D ? zz.ptr<float>(y) : nullptr;
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
            outRow[x] = _is3D ? (xxRow[x] + yyRow[x] + zzRow[x]) : (xxRow[x] + yyRow[x]);
    }
}

bool MLFeatureTensorTraceWide::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    CalcTensorTraceROI(
        mat,
        sliceID,
        data,
        FeatureType::Tensor_component_wide,
        roi);
    return false;
}

QList<MLFeature *> MLFeatureTensorTraceWide::GetDependencies()
{
    using TC = MLFeatureTensorComponentWide::TensorComponent;
    QList<MLFeature *> deps;
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::XX));
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::YY));
    if (_is3D)
        deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::ZZ));
    return deps;
}

QString MLFeatureTensorTraceWide::GetPrettyName() { return "t-trace wide"; }
QString MLFeatureTensorTraceWide::GetPrettyArgs() { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(_arg1))); }
QString MLFeatureTensorTraceWide::GetPretty3D() { return _is3D ? "3D" : "2D"; }
int MLFeatureTensorTraceWide::GetDependencyDepth() { return 5; }
int MLFeatureTensorTraceWide::GetMinMaxForArgs(int arg, bool max) { if (arg == 1) return max ? 6 : 0; return 0; }
QString MLFeatureTensorTraceWide::GetTypeCodeForFile() { return "ttw"; }
QString MLFeatureTensorTraceWide::GetArgsForFile() { return QString("@%1").arg(_arg1); }
QString MLFeatureTensorTraceWide::Get3DForFile() { return _is3D ? "3" : "2"; }
QString MLFeatureTensorTraceWide::GetArg1SetupString(int v) { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(v))); }
QString MLFeatureTensorTraceWide::GetArg2SetupString(int) { return ""; }
