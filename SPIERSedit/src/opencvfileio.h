#ifndef OPENCVFILEIO_H
#define OPENCVFILEIO_H

#include <QString>
#include <opencv2/opencv.hpp>
class openCVFileIO
{
public:
    openCVFileIO();

    static cv::Mat LoadMatBinary(const QString &featureName, int x, int y, int fileIndex, bool &ok);
    static void SaveMatBinary(const QString &featurename, const cv::Mat &mat, int fileIndex);
    static cv::Mat LoadMatFromImageFile(int sliceIndex, bool expectColour);
private:
    static QString GetFileName(const QString &fname, int index);
};

#endif // OPENCVFILEIO_H
