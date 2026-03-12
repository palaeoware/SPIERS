#ifndef MLFEATURECALCULATOR_H
#define MLFEATURECALCULATOR_H

#include <QObject>
#include <QString>
#include "opencv2/core.hpp"
#include "mlcachedaccess.h"

class MLFeatureCalculator
{
public:
    MLFeatureCalculator();
    static void CalculateFeature(cv::Mat &mat, int sliceID, QString featureName, MLCachedAccess *data);
    static QList<QString> GetDependencies(QString featureName);
private:
    static void CalcFeatureIntensity(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    static void CalcFeatureColor(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString col);
    static void CalcFeatureMean3D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString radius);
    static void CalcFeatureMean2D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString radius);
    static QList<QString> GetFeatureDependecies(QString featurename);
    static void CalcFeatureGaussian2D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString sigmaStr);
    static void CalcFeatureGaussian3D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString sigmaStr);
    static void CalcFeatureDifferenceOfFeatures(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString feature1, QString feature2);
};

#endif // MLFEATURECALCULATOR_H
