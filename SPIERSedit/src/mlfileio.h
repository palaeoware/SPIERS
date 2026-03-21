#ifndef MLFILEIO_H
#define MLFILEIO_H

#include <QString>
#include <opencv2/opencv.hpp>
class MLFileIO
{
public:
    MLFileIO();

    static cv::Mat LoadMatBinary(const QString &featureName, int x, int y, int fileIndex, bool &ok);
    static void SaveMatBinary(const QString &featurename, const cv::Mat &mat, int fileIndex);
    static cv::Mat LoadMatFromImageFile(int sliceIndex, bool expectColour);
    static QString GetWorkingPath();
private:
    static QString GetFileName(const QString &fname, int index);

};

#endif // MLFILEIO_H
