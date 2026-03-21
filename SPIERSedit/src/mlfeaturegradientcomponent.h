#ifndef MLFEATUREGRADIENTCOMPONENT_H
#define MLFEATUREGRADIENTCOMPONENT_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureGradientComponent : public MLFeature
{
public:
    enum class GradientDirection
    {
        X = 0,
        Y,
        Z
    };

    MLFeatureGradientComponent(Channel channel, int arg1, GradientDirection dir);
    MLFeatureGradientComponent(Channel channel, int arg1, int arg2);

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
    GradientDirection GetDirection() const;
    QString GetDirectionString() const;

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;

    // MLFeature interface
public:
    int GetMinMaxForArgs(int arg, bool max) override;
};

#endif // MLFEATUREGRADIENTCOMPONENT_H
