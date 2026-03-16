#ifndef MLFEATURELOG_H
#define MLFEATURELOG_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureLoG : public MLFeature
{
public:
    MLFeatureLoG(Channel channel, bool is3D, int arg1);

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
    void CalcFeatureLoG2D(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    void CalcFeatureLoG3D(cv::Mat &mat, int sliceID, MLCachedAccess *data);

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif // MLFEATURELOG_H
