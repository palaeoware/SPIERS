#ifndef MLFEATUREMEAN_H
#define MLFEATUREMEAN_H

#include "mlfeature.h"

class MLFeatureMean : public MLFeature
{
public:
    MLFeatureMean(Channel channel, bool is3D, int arg1);

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
    void CalcFeatureMean3D(cv::Mat &, int, MLCachedAccess *);
    void CalcFeatureMean2D(cv::Mat &, int, MLCachedAccess *);
};

#endif // MLFEATUREMEAN_H
