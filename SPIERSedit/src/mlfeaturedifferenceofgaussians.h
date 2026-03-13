#ifndef MLFEATUREDIFFERENCEOFGAUSSIANS_H
#define MLFEATUREDIFFERENCEOFGAUSSIANS_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureDifferenceOfGaussians : public MLFeature
{
public:
    MLFeatureDifferenceOfGaussians(Channel channel, bool is3D, int arg1, int arg2);

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
};

#endif // MLFEATUREDIFFERENCEOFGAUSSIANS_H
