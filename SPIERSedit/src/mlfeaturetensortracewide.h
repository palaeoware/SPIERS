#ifndef MLFEATURETENSORTRACEWIDE_H
#define MLFEATURETENSORTRACEWIDE_H

#include "mlfeature.h"

class MLFeatureTensorTraceWide : public MLFeature
{
public:
    MLFeatureTensorTraceWide(Channel channel, bool is3D, int arg1);

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

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif
