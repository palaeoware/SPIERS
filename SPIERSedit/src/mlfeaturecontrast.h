#ifndef MLFEATURECONTRAST_H
#define MLFEATURECONTRAST_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureContrast : public MLFeature
{
public:
    MLFeatureContrast(Channel channel, bool is3D, int arg1);

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

    // MLFeature interface
public:
    int GetDependencyDepth() override;

    // MLFeature interface
public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif // MLFEATURECONTRAST_H
