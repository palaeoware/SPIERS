#include "mlfeaturegradientcomponent.h"

#include "globals.h"
#include "mlfeaturegaussian.h"
#include "mlupdateblockingdialog.h"

#include <cmath>

MLFeatureGradientComponent::MLFeatureGradientComponent(Channel channel, int arg1, GradientDirection dir)
    : MLFeature(FeatureType::Gradient_component, channel, false, arg1, static_cast<int>(dir))
{
}

MLFeatureGradientComponent::MLFeatureGradientComponent(Channel channel, int arg1, int arg2)
    : MLFeature(FeatureType::Gradient_component, channel, false, arg1, arg2)
{
}

MLFeatureGradientComponent::GradientDirection MLFeatureGradientComponent::GetDirection() const
{
    return static_cast<GradientDirection>(_arg2);
}

QString MLFeatureGradientComponent::GetDirectionString() const
{
    switch (GetDirection())
    {
    case GradientDirection::X:
        return "x";
    case GradientDirection::Y:
        return "y";
    case GradientDirection::Z:
        return "z";
    default:
        return "?";
    }
}

void MLFeatureGradientComponent::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    float sigma = std::pow(2.0f, static_cast<float>(_arg1));
    float scaleFactor = sigma;

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating %1")
            .arg(GetPrettyFullName())
        );

    switch (GetDirection())
    {
    case GradientDirection::X:
    {
        int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, 0);
        cv::Mat slice = data->GetWholeSliceFeature(sliceID, gauIndex);
        CalcFirstDerivativeX(mat, slice, scaleFactor);
        break;
    }

    case GradientDirection::Y:
    {
        int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, false, _arg1, 0);
        cv::Mat slice = data->GetWholeSliceFeature(sliceID, gauIndex);
        CalcFirstDerivativeY(mat, slice, scaleFactor);
        break;
    }

    case GradientDirection::Z:
    {
        int gauIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian, _channel, true, _arg1, 0);

        int prevSlice = std::max(0, sliceID - 1);
        int nextSlice = std::min(FileCount - 1, sliceID + 1);

        QVector<cv::Mat> slices;
        slices.append(data->GetWholeSliceFeature(prevSlice, gauIndex));
        slices.append(data->GetWholeSliceFeature(sliceID, gauIndex));
        slices.append(data->GetWholeSliceFeature(nextSlice, gauIndex));

        CalcFirstDerivativeZ(mat, slices, 1, scaleFactor);
        break;
    }

    default:
        Q_ASSERT(false);
        break;
    }
}

QList<MLFeature *> MLFeatureGradientComponent::GetDependencies()
{
    QList<MLFeature *> deps;

    switch (GetDirection())
    {
    case GradientDirection::X:
    case GradientDirection::Y:
        deps.append(new MLFeatureGaussian(_channel, false, _arg1));
        break;

    case GradientDirection::Z:
        deps.append(new MLFeatureGaussian(_channel, true, _arg1));
        break;

    default:
        Q_ASSERT(false);
        break;
    }

    return deps;
}

QString MLFeatureGradientComponent::GetPrettyName()
{
    return "grad. comp.";
}

QString MLFeatureGradientComponent::GetPrettyArgs()
{
    return QString("sigma=%1, %2")
        .arg(std::pow(2.0f, static_cast<float>(_arg1)))
        .arg(GetDirectionString());
}

QString MLFeatureGradientComponent::GetPretty3D()
{
    return "";
}

QString MLFeatureGradientComponent::GetTypeCodeForFile()
{
    return "grc";
}

QString MLFeatureGradientComponent::GetArgsForFile()
{
    return QString("@%1@%2").arg(_arg1).arg(_arg2);
}

QString MLFeatureGradientComponent::Get3DForFile()
{
    return "";
}

int MLFeatureGradientComponent::GetDependencyDepth()
{
    switch (GetDirection())
    {
    case GradientDirection::X:
    case GradientDirection::Y:
        return 2; // Gaussian2D -> gradient
    case GradientDirection::Z:
        return 3; // Gaussian3D -> gradient
    default:
        Q_ASSERT(false);
        return 0;
    }
}

QString MLFeatureGradientComponent::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(std::pow(2.0f, static_cast<float>(v)));
}

QString MLFeatureGradientComponent::GetArg2SetupString(int v)
{
    GradientDirection dir = static_cast<GradientDirection>(v);

    switch (dir)
    {
    case GradientDirection::X:
        return "x";
    case GradientDirection::Y:
        return "y";
    case GradientDirection::Z:
        return "z";
    default:
        return "";
    }
}


int MLFeatureGradientComponent::GetMinMaxForArgs(int arg, bool max)
{
    if (arg==1 && !max) return 0;
    if (arg==1 && max) return 6;
    if (arg==2 && !max) return 0;
    if (arg==2 && max) return 2;
    return 0;
}
