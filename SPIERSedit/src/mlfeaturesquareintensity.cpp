#include "mlfeaturesquareintensity.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeatureintensity.h"

MLFeatureSquareIntensity::MLFeatureSquareIntensity(Channel channel)
    : MLFeature(FeatureType::Square, channel, false, 0, 0)
{}

void MLFeatureSquareIntensity::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Intensity, _channel, false, 0, 0);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, idx);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1: %2%")
                    .arg(GetPrettyFullName())
                    .arg((y * 100) / fheight)
                );

        const float *inRow = slice.ptr<float>(y);
        float *outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            float v = inRow[x];
            outRow[x] = v * v;
        }
    }
}

QList<MLFeature *> MLFeatureSquareIntensity::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureIntensity(_channel));
    return deps;
}

QString MLFeatureSquareIntensity::GetPrettyName()
{
    return "square";
}

QString MLFeatureSquareIntensity::GetPrettyArgs()
{
    return "";
}

QString MLFeatureSquareIntensity::GetPretty3D()
{
    return "";
}

int MLFeatureSquareIntensity::GetDependencyDepth()
{
    return 1;
}

QString MLFeatureSquareIntensity::GetTypeCodeForFile()
{
    return "sqr";
}

QString MLFeatureSquareIntensity::GetArgsForFile()
{
    return "";
}

QString MLFeatureSquareIntensity::Get3DForFile()
{
    return "";
}

QString MLFeatureSquareIntensity::GetArg1SetupString(int)
{
    return "";
}

QString MLFeatureSquareIntensity::GetArg2SetupString(int)
{
    return "";
}


int MLFeatureSquareIntensity::GetMinMaxForArgs(int, bool)
{
    return 0;
}
