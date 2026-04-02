#include "mlfeaturelog.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeaturegaussian.h"

MLFeatureLoG::MLFeatureLoG(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Laplacian_of_gaussian, channel, is3D, arg1, 0)
{
}

void MLFeatureLoG::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureLoG3D(mat, sliceID, data);
    else
        CalcFeatureLoG2D(mat, sliceID, data);
}

QList<MLFeature *> MLFeatureLoG::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureGaussian(_channel, _is3D, _arg1));
    return deps;
}

QString MLFeatureLoG::GetPrettyName()
{
    return "lapl. gauss.";
}

QString MLFeatureLoG::GetPrettyArgs()
{
    return QString("sigma=%1").arg(std::pow(2.0f, _arg1));
}

QString MLFeatureLoG::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureLoG::GetTypeCodeForFile()
{
    return "log";
}

QString MLFeatureLoG::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureLoG::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}

void MLFeatureLoG::CalcFeatureLoG2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, _arg1);
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

    CalcLaplacian2D(mat, slice, scaleFactor);
}

void MLFeatureLoG::CalcFeatureLoG3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, _arg1);
    float scaleFactor = sigma * sigma;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, true, _arg1, 0);

    int z0 = std::max(0, sliceID - 1);
    int z1 = std::min(FileCount - 1, sliceID + 1);

    QVector<cv::Mat> slices;
    for (int z = z0; z <= z1; ++z)
        slices.append(data->GetWholeSliceFeature(z, gauIndex));

    int centralSliceIndex = sliceID - z0;

    cv::Mat xyTerm(fheight, fwidth, CV_32F);
    cv::Mat zTerm(fheight, fwidth, CV_32F);

    CalcLaplacian2D(xyTerm, slices[centralSliceIndex], scaleFactor);
    CalcSecondDerivativeZZ(zTerm, slices, centralSliceIndex, scaleFactor);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Combining %1")
            .arg(GetPrettyFullName())
        );

    cv::add(xyTerm, zTerm, mat);
}

int MLFeatureLoG::GetDependencyDepth()
{
    if (_is3D)
        return 3;
    else
        return 2;
}

QString MLFeatureLoG::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(std::pow(2.0f, v));
}

QString MLFeatureLoG::GetArg2SetupString(int)
{
    return "";
}
