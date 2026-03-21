#include "mlfeaturegradient.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeaturegaussian.h"

MLFeatureGradient::MLFeatureGradient(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Gradient_magnitude, channel, is3D, arg1, 0)
{}

void MLFeatureGradient::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureGradient3D(mat, sliceID, data);
    else
        CalcFeatureGradient2D(mat, sliceID, data);
}

QList<MLFeature *> MLFeatureGradient::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureGaussian(_channel, _is3D, _arg1));
    return deps;
}

QString MLFeatureGradient::GetPrettyName()
{
    return "gradient";
}

QString MLFeatureGradient::GetPrettyArgs()
{
    return QString("sigma=%1").arg(pow(2.0f, _arg1));
}

QString MLFeatureGradient::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureGradient::GetTypeCodeForFile()
{
    return "grd";
}

QString MLFeatureGradient::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureGradient::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureGradient::CalcFeatureGradient2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, _arg2);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, idx);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        float *outRow = mat.ptr<float>(y);

        int y0 = std::max(0, y - 1);
        int y1 = std::min(fheight - 1, y + 1);

        for (int x = 0; x < fwidth; ++x)
        {
            int x0 = std::max(0, x - 1);
            int x1 = std::min(fwidth - 1, x + 1);

            float gx = 0.5f * (slice.at<float>(y, x1) - slice.at<float>(y, x0));
            float gy = 0.5f * (slice.at<float>(y1, x) - slice.at<float>(y0, x));

            outRow[x] = std::sqrt(gx * gx + gy * gy);
        }
    }
}

void MLFeatureGradient::CalcFeatureGradient3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, true, _arg1, _arg2);

    int z0 = std::max(0, sliceID - 1);
    int z1 = std::min(FileCount - 1, sliceID + 1);

    cv::Mat slice0 = data->GetWholeSliceFeature(z0, idx);
    cv::Mat slice  = data->GetWholeSliceFeature(sliceID, idx);
    cv::Mat slice1 = data->GetWholeSliceFeature(z1, idx);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        float *outRow = mat.ptr<float>(y);

        int y0 = std::max(0, y - 1);
        int y1 = std::min(fheight - 1, y + 1);

        for (int x = 0; x < fwidth; ++x)
        {
            int x0 = std::max(0, x - 1);
            int x1 = std::min(fwidth - 1, x + 1);

            float gx = 0.5f * (slice.at<float>(y, x1) - slice.at<float>(y, x0));
            float gy = 0.5f * (slice.at<float>(y1, x) - slice.at<float>(y0, x));
            float gz = 0.5f * (slice1.at<float>(y, x) - slice0.at<float>(y, x));

            outRow[x] = std::sqrt(gx * gx + gy * gy + gz * gz);
        }
    }
}

int MLFeatureGradient::GetDependencyDepth()
{
    if (_is3D)
        return 4;
    else
        return 2;
}

QString MLFeatureGradient::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(pow(2.0f, v));
}

QString MLFeatureGradient::GetArg2SetupString(int v)
{
    return "";
}
