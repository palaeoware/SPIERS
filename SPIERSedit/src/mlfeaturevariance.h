#ifndef MLFEATUREVARIANCE_H
#define MLFEATUREVARIANCE_H

#include "mlfeature.h"

class MLFeatureVariance : public MLFeature
{
public:
    MLFeatureVariance(Channel channel, bool is3D, int arg1);

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
    void CalcFeatureVariance2D(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    void CalcFeatureVariance3D(cv::Mat &mat, int sliceID, MLCachedAccess *data);

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif // MLFEATUREVARIANCE_H
