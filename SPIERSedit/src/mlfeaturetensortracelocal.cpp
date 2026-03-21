#include "mlfeaturetensortracelocal.h"
#include "mlfeaturetensorcomponentlocal.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <cmath>

MLFeatureTensorTraceLocal::MLFeatureTensorTraceLocal(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Tensor_trace_local, channel, is3D, arg1, 0)
{
}

void MLFeatureTensorTraceLocal::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    using TC = MLFeatureTensorComponentLocal::TensorComponent;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int xxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1,
                                           static_cast<int>(TC::XX));
    int yyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1,
                                           static_cast<int>(TC::YY));

    cv::Mat xx = data->GetWholeSliceFeature(sliceID, xxIndex);
    cv::Mat yy = data->GetWholeSliceFeature(sliceID, yyIndex);

    cv::Mat zz;
    if (_is3D)
    {
        int zzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Tensor_component_local, _channel, false, _arg1,
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

QList<MLFeature *> MLFeatureTensorTraceLocal::GetDependencies()
{
    using TC = MLFeatureTensorComponentLocal::TensorComponent;
    QList<MLFeature *> deps;
    deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::XX));
    deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::YY));
    if (_is3D)
        deps.append(new MLFeatureTensorComponentLocal(_channel, _arg1, TC::ZZ));
    return deps;
}

QString MLFeatureTensorTraceLocal::GetPrettyName() { return "t-trace local"; }
QString MLFeatureTensorTraceLocal::GetPrettyArgs() { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(_arg1))); }
QString MLFeatureTensorTraceLocal::GetPretty3D() { return _is3D ? "3D" : "2D"; }
int MLFeatureTensorTraceLocal::GetDependencyDepth() { return 5; }
int MLFeatureTensorTraceLocal::GetMinMaxForArgs(int arg, bool max) { if (arg == 1) return max ? 6 : 0; return 0; }
QString MLFeatureTensorTraceLocal::GetTypeCodeForFile() { return "ttl"; }
QString MLFeatureTensorTraceLocal::GetArgsForFile() { return QString("@%1").arg(_arg1); }
QString MLFeatureTensorTraceLocal::Get3DForFile() { return _is3D ? "3" : "2"; }
QString MLFeatureTensorTraceLocal::GetArg1SetupString(int v) { return QString("sigma=%1").arg(std::pow(2.0f, static_cast<float>(v))); }
QString MLFeatureTensorTraceLocal::GetArg2SetupString(int) { return ""; }
