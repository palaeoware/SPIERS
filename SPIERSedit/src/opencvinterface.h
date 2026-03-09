#ifndef OPENCVINTERFACE_H
#define OPENCVINTERFACE_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

class OpenCVInterface
{
public:
    OpenCVInterface();
    static bool TestOpenCV();
    static bool enabled;
    cv::Mat QImageToCvMatGrayFloat(QImage &img);
    void TrainOnOneSlice(int slice);
    uchar GetProbability(int x, int y, int segment);

private:
    bool dataComputed;
    int currentSlice;
    QList<cv::Mat> featureData;
    cv::Ptr<cv::ml::RTrees> rf;
};

#endif // OPENCVINTERFACE_H
