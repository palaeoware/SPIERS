#ifndef MLFEATURESQUAREINTENSITY_H
#define MLFEATURESQUAREINTENSITY_H

#include "mlfeature.h"

class MLFeatureSquareIntensity : public MLFeature
{
public:
    MLFeatureSquareIntensity(Channel channel);

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

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;

    // MLFeature interface
public:
    int GetMinMaxForArgs(int arg, bool max) override;
};

#endif // MLFEATURESQUAREINTENSITY_H
