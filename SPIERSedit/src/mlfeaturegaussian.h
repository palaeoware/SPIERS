#ifndef MLFEATUREGAUSSIAN_H
#define MLFEATUREGAUSSIAN_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureGaussian : public MLFeature
{
public:
    MLFeatureGaussian(Channel channel, bool is3D, int arg1);

    // MLFeature interface
public:
    void CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data) override;
    QList<MLFeature *> GetDependencies() override;
    QString GetPrettyName() override;
    QString GetPrettyArgs() override;
    QString GetPretty3D() override;


    int GetDependencyDepth() override;
protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;
private:
    void CalcFeatureGaussian2D(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    void CalcFeatureGaussian3D(cv::Mat &mat, int sliceID, MLCachedAccess *data);

    // MLFeature interface
public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif // MLFEATUREGAUSSIAN_H
