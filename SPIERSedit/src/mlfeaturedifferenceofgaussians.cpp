/**
 * @file
 * Source: Mlfeaturedifferenceofgaussians
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

bool MLFeatureDifferenceOfGaussians::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &roi)
{
    const int feature1 = data->GetIndexForFeature(
        MLFeature::FeatureType::Gaussian,
        _channel,
        _is3D,
        _arg1,
        0);
    const int feature2 = data->GetIndexForFeature(
        MLFeature::FeatureType::Gaussian,
        _channel,
        _is3D,
        _arg2,
        0);
    CalcFeatureDifferenceOfFeaturesROI(
        mat,
        sliceID,
        data,
        feature1,
        feature2,
        roi);
    return false;
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
