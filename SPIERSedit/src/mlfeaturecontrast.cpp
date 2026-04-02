#include "mlfeaturecontrast.h"
#include "mlfeatureintensity.h"
#include "mlfeaturegaussian.h"
#include "globals.h"

MLFeatureContrast::MLFeatureContrast(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Contrast, channel, is3D, arg1, 0)
{}

void MLFeatureContrast::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    int intensity = data->GetIndexForFeature(MLFeature::FeatureType::Intensity,
                                            _channel, false, 0, 0);
    int gaussian = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian,
                                            _channel, _is3D, _arg1, 0);
    CalcFeatureDifferenceOfFeatures(mat, sliceID, data, intensity, gaussian);
}

QList<MLFeature *> MLFeatureContrast::GetDependencies()
{
    QList<MLFeature *> deps;
    deps.append(new MLFeatureIntensity(_channel));
    deps.append(new MLFeatureGaussian(_channel, _is3D, _arg1));

    return deps;
}

QString MLFeatureContrast::GetPrettyName()
{
    return "contrast";
}

QString MLFeatureContrast::GetPrettyArgs()
{
    return QString("sigma=%1").arg(pow(2.0f,_arg1));
}

QString MLFeatureContrast::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureContrast::GetTypeCodeForFile()
{
    return "cnt";
}

QString MLFeatureContrast::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureContrast::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}


int MLFeatureContrast::GetDependencyDepth()
{
    if (_is3D)
        return 3;  //=>gau3=>gau2>=int
    else
        return 2;
}


QString MLFeatureContrast::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(pow(2.0f,v));
}

QString MLFeatureContrast::GetArg2SetupString(int)
{
    return "";
}
