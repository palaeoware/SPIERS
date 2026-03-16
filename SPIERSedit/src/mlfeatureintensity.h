#ifndef MLFEATUREINTENSITY_H
#define MLFEATUREINTENSITY_H

#include "mlfeature.h"

class MLFeatureIntensity : public MLFeature
{
public:
    MLFeatureIntensity(Channel channel);
    void CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data) override;
private:
    void CalcFeatureIntensity(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    void CalcFeatureColor(cv::Mat &mat, int sliceID, MLCachedAccess *data);

    // MLFeature interface
public:
    QString GetPrettyName() override;
    QString GetPrettyArgs() override;
    QString GetPretty3D() override;

protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;
    // MLFeature interface
public:
    QList<MLFeature *> GetDependencies() override;

    // MLFeature interface
public:
    int GetDependencyDepth() override;

    // MLFeature interface
public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;

    // MLFeature interface
public:
    int GetMinMaxForArgs(int arg, bool max) override;
};

#endif // MLFEATUREINTENSITY_H
