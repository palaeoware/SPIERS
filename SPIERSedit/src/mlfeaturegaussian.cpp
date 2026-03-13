#include "mlfeaturegaussian.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeatureintensity.h"

MLFeatureGaussian::MLFeatureGaussian(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Gaussian, channel, is3D, arg1, 0)
{}

void MLFeatureGaussian::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureGaussian3D(mat, sliceID, data);
    else
        CalcFeatureGaussian2D(mat, sliceID, data);
}

QList<MLFeature *> MLFeatureGaussian::GetDependencies()
{
    QList<MLFeature *> deps;
    if (_is3D)
        deps.append(new MLFeatureGaussian(_channel, false, _arg1));
    else
        deps.append(new MLFeatureIntensity(_channel));

    return deps;
}

QString MLFeatureGaussian::GetPrettyName()
{
    return "gaussian";
}

QString MLFeatureGaussian::GetPrettyArgs()
{
    return QString("Sigma=%1").arg(pow(2,_arg1));
}

QString MLFeatureGaussian::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureGaussian::GetTypeCodeForFile()
{
    return "gau";
}

QString MLFeatureGaussian::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureGaussian::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureGaussian::CalcFeatureGaussian2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{

    float sigma = std::pow(2,_arg1);

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Intensity, _channel, false, 0, 0);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, idx);

    int r = static_cast<int>(ceil(3.0 * sigma));
    int ksize = 2 * r + 1;

    std::vector<float> kernel(ksize);

    double sum = 0.0;
    for (int k = -r; k <= r; ++k)
    {
        double v = exp(-(k*k) / (2.0 * sigma * sigma));
        kernel[k + r] = static_cast<float>(v);
        sum += v;
    }

    for (float &v : kernel)
        v /= sum;

    cv::Mat temp(fheight, fwidth, CV_32F);

    // --- horizontal pass ---

    for (int y = 0; y < fheight; ++y)
    {
        float *inRow = slice.ptr<float>(y);
        float *outRow = temp.ptr<float>(y);

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y*50)/fheight)
                );

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int xx = std::max(0, std::min(fwidth - 1, x + k));
                acc += kernel[k + r] * inRow[xx];
            }

            outRow[x] = static_cast<float>(acc);
        }
    }

    // --- vertical pass ---

    for (int y = 0; y < fheight; ++y)
    {

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y*50)/fheight)
                );

        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int yy = std::max(0, std::min(fheight - 1, y + k));
                float *row = temp.ptr<float>(yy);

                acc += kernel[k + r] * row[x];
            }

            outRow[x] = static_cast<float>(acc);
        }
    }
}


void MLFeatureGaussian::CalcFeatureGaussian3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{

    float sigma = std::pow(2,_arg1);

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int gau2index = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, _arg2);

    int r = static_cast<int>(ceil(3.0 * sigma));
    int ksize = 2 * r + 1;

    std::vector<float> kernel(ksize);

    double sum = 0.0;
    for (int k = -r; k <= r; ++k)
    {
        double v = exp(-(k * k) / (2.0 * sigma * sigma));
        kernel[k + r] = static_cast<float>(v);
        sum += v;
    }

    for (float &v : kernel)
        v /= sum;

    int z0 = std::max(0, sliceID - r);
    int z1 = std::min(FileCount - 1, sliceID + r);

    QVector<cv::Mat> slices;
    QVector<float> zweights;

    for (int i = z0; i <= z1; i++)
    {
        slices.append(data->GetWholeSliceFeature(i, gau2index));

        int dz = i - sliceID;
        zweights.append(kernel[dz + r]);
    }

    double wsum = 0.0;
    for (float w : zweights)
        wsum += w;

    for (int y = 0; y < fheight; ++y)
    {
        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y*100)/fheight)
                );

        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int z = z0; z <= z1; ++z)
            {
                float w = zweights[z - z0];
                acc += w * slices[z - z0].at<float>(y, x);
            }

            outRow[x] = (wsum > 0.0) ? static_cast<float>(acc / wsum) : 0.0f;
        }
    }
}

int MLFeatureGaussian::GetDependencyDepth()
{
    if (_is3D)
        return 2;
    else
        return 1;
}


QString MLFeatureGaussian::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(pow(2,v));
}

QString MLFeatureGaussian::GetArg2SetupString(int v)
{
    return "";
}
