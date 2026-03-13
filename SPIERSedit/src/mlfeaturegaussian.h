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


protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;
private:
    void CalcFeatureGaussian2D(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    void CalcFeatureGaussian3D(cv::Mat &mat, int sliceID, MLCachedAccess *data);
};

#endif // MLFEATUREGAUSSIAN_H
