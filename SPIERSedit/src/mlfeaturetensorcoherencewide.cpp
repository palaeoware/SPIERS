/**
 * @file
 * Source: Mlfeaturetensorcoherencewide
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
#include "mlfeaturetensorcoherencewide.h"
#include "mlfeaturetensorcomponentwide.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <cmath>

MLFeatureTensorCoherenceWide::MLFeatureTensorCoherenceWide(Channel channel, int arg1)
    : MLFeature(FeatureType::Tensor_coherence_wide, channel, false, arg1, 0)
{
}

void MLFeatureTensorCoherenceWide::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
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

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1 %2%").arg(GetPrettyFullName()).arg((y * 100) / fheight));

        const float *xxRow = xx.ptr<float>(y);
        const float *yyRow = yy.ptr<float>(y);
        const float *xyRow = xy.ptr<float>(y);
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            float a = xxRow[x];
            float b = yyRow[x];
            float c = xyRow[x];
            float denom = a + b;

            if (denom < 1e-6f)
            {
                outRow[x] = 0.0f;
            }
            else
            {
                float num = std::sqrt((a - b)*(a - b) + 4.0f*c*c);
                outRow[x] = std::min(1.0f, num / denom);
            }
        }
    }
}

QList<MLFeature *> MLFeatureTensorCoherenceWide::GetDependencies()
{
    using TC = MLFeatureTensorComponentWide::TensorComponent;
    QList<MLFeature *> deps;
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::XX));
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::YY));
    deps.append(new MLFeatureTensorComponentWide(_channel, _arg1, TC::XY));
    return deps;
}

QString MLFeatureTensorCoherenceWide::GetPrettyName() { return "t-coherence wide"; }
QString MLFeatureTensorCoherenceWide::GetPrettyArgs() { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(_arg1))); }
QString MLFeatureTensorCoherenceWide::GetPretty3D() { return ""; }
int MLFeatureTensorCoherenceWide::GetDependencyDepth() { return 5; }
int MLFeatureTensorCoherenceWide::GetMinMaxForArgs(int arg, bool max) { if (arg == 1) return max ? 6 : 0; return 0; }
QString MLFeatureTensorCoherenceWide::GetTypeCodeForFile() { return "tow"; }
QString MLFeatureTensorCoherenceWide::GetArgsForFile() { return QString("@%1").arg(_arg1); }
QString MLFeatureTensorCoherenceWide::Get3DForFile() { return "2"; }
QString MLFeatureTensorCoherenceWide::GetArg1SetupString(int v) { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(v))); }
QString MLFeatureTensorCoherenceWide::GetArg2SetupString(int) { return ""; }
