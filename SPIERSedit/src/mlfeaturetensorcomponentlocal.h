#ifndef MLFEATURETENSORCOMPONENTLOCAL_H
#define MLFEATURETENSORCOMPONENTLOCAL_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureTensorComponentLocal : public MLFeature
{
public:
    enum class TensorComponent
    {
        XX = 0,
        YY,
        ZZ,
        XY,
        XZ,
        YZ
    };

    MLFeatureTensorComponentLocal(Channel channel, int arg1, TensorComponent component);
    MLFeatureTensorComponentLocal(Channel channel, int arg1, int arg2);

public:
    void CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data) override;
    QList<MLFeature *> GetDependencies() override;
    QString GetPrettyName() override;
    QString GetPrettyArgs() override;
    QString GetPretty3D() override;

    int GetDependencyDepth() override;
    int GetMinMaxForArgs(int arg, bool max) override;

protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;

private:
    TensorComponent GetComponent() const;
    QString GetComponentString() const;
    int GetIntegrationRadiusLog2() const;

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif // MLFEATURETENSORCOMPONENTLOCAL_H
