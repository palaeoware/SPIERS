#ifndef OPENCVINTERFACE_H
#define OPENCVINTERFACE_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include "mlcachedaccess.h"
#include "mainwindowimpl.h"
class OpenCVInterface
{
public:
    OpenCVInterface();
    static bool TestOpenCV();
    static bool enabled;
    cv::Mat QImageToCvMatGrayFloat(QImage &img);
    void Train(int slice, MainWindowImpl *mw);
    uchar GetProbability(int x, int y, int z, int segment);

    void CalculateFeatureData(MainWindowImpl *mw);
private:
    bool dataComputed;
    int currentSlice;
    cv::Ptr<cv::ml::RTrees> rf;
    MLCachedAccess *data;
    void CreateCacheHandlerIfNeeded();
    void SetUpFeatures();
};

#endif // OPENCVINTERFACE_H
