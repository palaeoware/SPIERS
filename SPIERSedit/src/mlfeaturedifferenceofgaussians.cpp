#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeaturegaussian.h"
#include "globals.h"

MLFeatureDifferenceOfGaussians::MLFeatureDifferenceOfGaussians(Channel channel, bool is3D, int arg1, int arg2)
    : MLFeature(FeatureType::Difference_of_gaussians, channel, is3D, arg1, arg2)
{

}

void MLFeatureDifferenceOfGaussians::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    int feature1 = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian,
                                            _channel, _is3D, _arg1, 0);
    int feature2 = data->GetIndexForFeature(MLFeature::FeatureType::Gaussian,
                                            _channel, _is3D, _arg2, 0);
    CalcFeatureDifferenceOfFeatures(mat, sliceID, data, feature1, feature2);
}

QList<MLFeature *> MLFeatureDifferenceOfGaussians::GetDependencies()
{
    QList<MLFeature *> deps;

    deps.append(new MLFeatureGaussian(_channel, _is3D, _arg1));
    deps.append(new MLFeatureGaussian(_channel, _is3D, _arg2));
    return deps;
}

QString MLFeatureDifferenceOfGaussians::GetPrettyName()
{
    return "diff gaussian";
}

QString MLFeatureDifferenceOfGaussians::GetPrettyArgs()
{
    return QString("(%1-%2)")
        .arg(pow(2.0f,_arg1))
        .arg(pow(2.0f,_arg2));

}

QString MLFeatureDifferenceOfGaussians::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureDifferenceOfGaussians::GetTypeCodeForFile()
{
    return "dog";
}

QString MLFeatureDifferenceOfGaussians::GetArgsForFile()
{
    return QString("@%1@%2").arg(_arg1).arg(_arg2);
}

QString MLFeatureDifferenceOfGaussians::Get3DForFile()
{    if (_is3D)
        return "3";
    else
        return "2";
}


int MLFeatureDifferenceOfGaussians::GetDependencyDepth()
{
    if (_is3D)
        return 3;  //=>gau3=>gau2>=int
    else
        return 2;
}


QString MLFeatureDifferenceOfGaussians::GetArg1SetupString(int v)
{
    return QString("sigma=%1")
        .arg(pow(2.0f,v));
}

QString MLFeatureDifferenceOfGaussians::GetArg2SetupString(int v)
{
    return QString("sigma=%1")
        .arg(pow(2.0f,v));
}


int MLFeatureDifferenceOfGaussians::GetMinMaxForArgs(int arg, bool max)
{
    if (arg==1 && !max) return 0;
    if (arg==1 && max) return 6;
    if (arg==2 && !max) return 0;
    if (arg==2 && max) return 6;
    return 0;
}
