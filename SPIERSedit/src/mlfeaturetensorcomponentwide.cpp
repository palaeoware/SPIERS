#include "mlfeaturetensorcomponentwide.h".h"

#include "globals.h"
#include "mlfeaturegradientcomponent.h"
#include "mlupdateblockingdialog.h"

#include <cmath>

MLFeatureTensorComponentWide::MLFeatureTensorComponentWide(Channel channel, int arg1, TensorComponent component)
    : MLFeature(FeatureType::Tensor_component_wide, channel, false, arg1, static_cast<int>(component))
{
}

MLFeatureTensorComponentWide::MLFeatureTensorComponentWide(Channel channel, int arg1, int arg2)
    : MLFeature(FeatureType::Tensor_component_wide, channel, false, arg1, arg2)
{
}

MLFeatureTensorComponentWide::TensorComponent MLFeatureTensorComponentWide::GetComponent() const
{
    return static_cast<TensorComponent>(_arg2);
}

QString MLFeatureTensorComponentWide::GetComponentString() const
{
    switch (GetComponent())
    {
    case TensorComponent::XX:
        return "xx";
    case TensorComponent::YY:
        return "yy";
    case TensorComponent::ZZ:
        return "zz";
    case TensorComponent::XY:
        return "xy";
    case TensorComponent::XZ:
        return "xz";
    case TensorComponent::YZ:
        return "yz";
    default:
        return "?";
    }
}

int MLFeatureTensorComponentWide::GetIntegrationRadiusLog2() const
{
    return _arg1+1;   // wide flavour: rho = 2*sigma
}

void MLFeatureTensorComponentWide::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating %1")
            .arg(GetPrettyFullName())
        );

    using GD = MLFeatureGradientComponent::GradientDirection;

    switch (GetComponent())
    {
    case TensorComponent::XX:
    {
        int gxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::X));

        cv::Mat prod(fheight, fwidth, CV_32F);
        CalcFeatureProductOfFeatures(prod, sliceID, data, gxIndex, gxIndex);
        CalcLocalMean2D(mat, prod, GetIntegrationRadiusLog2());
        break;
    }

    case TensorComponent::YY:
    {
        int gyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::Y));

        cv::Mat prod(fheight, fwidth, CV_32F);
        CalcFeatureProductOfFeatures(prod, sliceID, data, gyIndex, gyIndex);
        CalcLocalMean2D(mat, prod, GetIntegrationRadiusLog2());
        break;
    }

    case TensorComponent::ZZ:
    {
        int gzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::Z));

        cv::Mat prod(fheight, fwidth, CV_32F);
        CalcFeatureProductOfFeatures(prod, sliceID, data, gzIndex, gzIndex);
        CalcLocalMean2D(mat, prod, GetIntegrationRadiusLog2());
        break;
    }

    case TensorComponent::XY:
    {
        int gxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::X));
        int gyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::Y));

        cv::Mat prod(fheight, fwidth, CV_32F);
        CalcFeatureProductOfFeatures(prod, sliceID, data, gxIndex, gyIndex);
        CalcLocalMean2D(mat, prod, GetIntegrationRadiusLog2());
        break;
    }

    case TensorComponent::XZ:
    {
        int gxIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::X));
        int gzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::Z));

        cv::Mat prod(fheight, fwidth, CV_32F);
        CalcFeatureProductOfFeatures(prod, sliceID, data, gxIndex, gzIndex);
        CalcLocalMean2D(mat, prod, GetIntegrationRadiusLog2());
        break;
    }

    case TensorComponent::YZ:
    {
        int gyIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::Y));
        int gzIndex = data->GetIndexForFeature(MLFeature::FeatureType::Gradient_component, _channel, false, _arg1,
                                               static_cast<int>(GD::Z));

        cv::Mat prod(fheight, fwidth, CV_32F);
        CalcFeatureProductOfFeatures(prod, sliceID, data, gyIndex, gzIndex);
        CalcLocalMean2D(mat, prod, GetIntegrationRadiusLog2());
        break;
    }

    default:
        Q_ASSERT(false);
        break;
    }
}

QList<MLFeature *> MLFeatureTensorComponentWide::GetDependencies()
{
    QList<MLFeature *> deps;

    using GD = MLFeatureGradientComponent::GradientDirection;

    switch (GetComponent())
    {
    case TensorComponent::XX:
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::X));
        break;

    case TensorComponent::YY:
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::Y));
        break;

    case TensorComponent::ZZ:
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::Z));
        break;

    case TensorComponent::XY:
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::X));
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::Y));
        break;

    case TensorComponent::XZ:
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::X));
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::Z));
        break;

    case TensorComponent::YZ:
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::Y));
        deps.append(new MLFeatureGradientComponent(_channel, _arg1, GD::Z));
        break;

    default:
        Q_ASSERT(false);
        break;
    }

    return deps;
}

QString MLFeatureTensorComponentWide::GetPrettyName()
{
    return "tensor wide";
}

QString MLFeatureTensorComponentWide::GetPrettyArgs()
{
    return QString("sigma=%1, %2")
        .arg(std::pow(2.0f, static_cast<float>(_arg1)))
        .arg(GetComponentString());
}

QString MLFeatureTensorComponentWide::GetPretty3D()
{
    return "";
}

QString MLFeatureTensorComponentWide::GetTypeCodeForFile()
{
    return "tcw";
}

QString MLFeatureTensorComponentWide::GetArgsForFile()
{
    return QString("@%1@%2").arg(_arg1).arg(_arg2);
}

QString MLFeatureTensorComponentWide::Get3DForFile()
{
    return "";
}

int MLFeatureTensorComponentWide::GetDependencyDepth()
{
    return 4;
}

int MLFeatureTensorComponentWide::GetMinMaxForArgs(int arg, bool max)
{
    if (arg == 1)
        return max ? 6 : 0;

    if (arg == 2)
        return max ? 5 : 0;

    return 0;
}

QString MLFeatureTensorComponentWide::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(std::pow(2.0f, static_cast<float>(v)));
}

QString MLFeatureTensorComponentWide::GetArg2SetupString(int v)
{
    TensorComponent component = static_cast<TensorComponent>(v);

    switch (component)
    {
    case TensorComponent::XX:
        return "xx";
    case TensorComponent::YY:
        return "yy";
    case TensorComponent::ZZ:
        return "zz";
    case TensorComponent::XY:
        return "xy";
    case TensorComponent::XZ:
        return "xz";
    case TensorComponent::YZ:
        return "yz";
    default:
        return "";
    }
}
