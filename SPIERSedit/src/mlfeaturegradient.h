#ifndef MLFEATUREGRADIENT_H
#define MLFEATUREGRADIENT_H

#include "mlfeature.h"

class MLFeatureGradient : public MLFeature
{
public:
    MLFeatureGradient(Channel channel, bool is3D, int arg1);

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
    void CalcFeatureGradient2D(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    void CalcFeatureGradient3D(cv::Mat &mat, int sliceID, MLCachedAccess *data);

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif // MLFEATUREGRADIENT_H
