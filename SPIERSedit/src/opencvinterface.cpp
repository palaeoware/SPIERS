#include "opencvinterface.h"
#include <QDebug>
#include <QImage>

#include "globals.h"
#include "src/fileio.h"
#include "display.h"
#include <opencv2/imgproc.hpp>
#include "labelledpoint.h"

bool OpenCVInterface::enabled;

OpenCVInterface::OpenCVInterface()
{


}

//static test method - run by main on startup.
bool OpenCVInterface::TestOpenCV()
{
    cv::Mat m = cv::Mat::eye(3, 3, CV_32F);
    auto rf = cv::ml::RTrees::create();

    //rf is reference counted and will auto delete when it goes out of scope

    if (m.rows == 3 && rf != nullptr)
    {
        enabled = true;
        return true;
    }
    else
    {
        enabled = false;
        return false;
    }
}

//Convert a QImage to the OpenCV image format
cv::Mat OpenCVInterface::QImageToCvMatGrayFloat(QImage &img)
{
    QImage useImage;
    if (img.format() == QImage::Format_Grayscale8)
        useImage = img;
    else
        useImage = img.convertToFormat(QImage::Format_Grayscale8);

    //Convert QImage to openCV format
    auto mat =  cv::Mat(
        useImage.height(),
        useImage.width(),
        CV_8UC1,
        const_cast<uchar*>(useImage.bits()),
        useImage.bytesPerLine()
        );

    //And convert to 32 bit float
    cv::Mat mat32;
    mat.convertTo(mat32, CV_32F, 1.0 / 255.0);

    return mat32;
}

cv::Mat ComputeLocalMean(const cv::Mat& img, int radius)
{
    cv::Mat mean;

    int k = 2 * radius + 1;

    cv::boxFilter(
        img,        // input
        mean,       // output
        CV_32F,     // output type
        cv::Size(k, k),
        cv::Point(-1, -1),
        true,       // normalize (divide by area)
        cv::BORDER_REFLECT
        );

    return mean;
}

uchar OpenCVInterface::GetProbability(int x, int y, int segment)
{
    cv::Mat sample(1, 2, CV_32F);
    for (int j=0; j<featureData.count(); j++)
    {
        sample.at<float>(0,j) = featureData[j].at<float>(y,x);
    }

    cv::Mat votes;
    //()<<"Running rf";
    rf->getVotes(sample, votes, 0);
    //qDebug()<<"Run votes";
    int tot=0;
    int thisCount = -1;
    for (int i=0; i<SegmentCount; i++)
    {
        if (votes.at<int>(0,i)==segment)
            thisCount = votes.at<int>(1,i);
        tot += votes.at<int>(1,i);

        //if (x==50)
        //{
        //    qDebug()<<"y: "<<y<<" votes: "<<votes.at<int>(0,0)<<votes.at<int>(0,1)<<votes.at<int>(0,2)<<"\n"
        //             << votes.at<int>(1,0)<<votes.at<int>(1,1)<<votes.at<int>(1,2);
        //}
    }

    if (thisCount == -1)
    {
        qDebug()<<"Error!";
        return 0;
    }
    else
    {
        //if (x==50)
        //    qDebug()<<"y: "<<y<<" tot: "<<tot<<  " this: "<<thisCount<<"   ret: "<<(thisCount * 255)/tot;
        return (thisCount * 255)/tot;
    }
}


void OpenCVInterface::TrainOnOneSlice(int slice)
{
    dataComputed = false;
    currentSlice = slice;
    if (SegmentCount<2)
    {
        qDebug()<<"Proper error here!";
        return;
    }

    featureData.clear();
    //Grab the colour image for this
    qDebug()<<"Converting slice";
    LoadColourData(slice);
    auto openCVImage = QImageToCvMatGrayFloat(ColArray);
    qDebug()<<"Calculating local mean";
    featureData.append(openCVImage);

    auto means = ComputeLocalMean(openCVImage, 5);
    featureData.append(openCVImage);
    qDebug()<<"Done";

    QList<LabelledPoint> labels = GenerateLabels();

    //For now, ignore downsampling

    auto trainingDataMat = cv::Mat(labels.count(), 2, CV_32F);
    auto labelsMat = cv::Mat(labels.count(), 1, CV_32S);

    for (int i=0; i<labels.count(); i++)
    {
        LabelledPoint point = labels[i];
        labelsMat.at<int>(i, 0) = point.segment;

        for (int j=0; j<featureData.count(); j++)
        {
            trainingDataMat.at<float>(i, j) = featureData[j].at<float>(point.y, point.x);
        }
    }
    qDebug()<<"Created training data with "<<labels.count()<<" samples";

    rf = cv::ml::RTrees::create();
    rf->setMaxDepth(10);
    rf->setMinSampleCount(2);
    rf->setRegressionAccuracy(0.0f);
    rf->setUseSurrogates(false);
    rf->setMaxCategories(2);
    rf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 50, 0));

    auto td = cv::ml::TrainData::create(
        trainingDataMat,
        cv::ml::ROW_SAMPLE,
        labelsMat
        );

    bool ok = rf->train(td);

    qDebug() << "trained! ok =" << ok;

}
