#include "opencvinterface.h"
#include <QDebug>
#include <QImage>

#include "globals.h"
#include "src/fileio.h"
#include "display.h"
#include <opencv2/imgproc.hpp>
#include "labelledpoint.h"
#include "mainwindowimpl.h"
#include "mlupdateblockingdialog.h"

bool OpenCVInterface::enabled;



OpenCVInterface::OpenCVInterface()
{
    data = nullptr;

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


uchar OpenCVInterface::GetProbability(int x, int y, int z, int segment)
{
    if (data == nullptr || !rf->isTrained())
    {
        return 0;
    }

    int featureCount = data->GetFeatureCount();
    cv::Mat sample(1, featureCount, CV_32F);

    for (int i=0; i<featureCount; i++)
    {
        float v = data->GetFeatureValueAt(x,y,z,i);
        sample.at<float>(0,i)= v;
    }

    cv::Mat votes;
    rf->getVotes(sample, votes, 0);

    int tot=0;
    int thisCount = -1;
    for (int i=0; i<SegmentCount; i++)
    {
        if (votes.at<int>(0,i)==segment)
            thisCount = votes.at<int>(1,i);
        tot += votes.at<int>(1,i);

    }

    if (thisCount == -1)
    {
        qDebug()<<"Error!";
        return 0;
    }
    else
    {
        return (thisCount * 255)/tot;
    }
}

void OpenCVInterface::CreateCacheHandlerIfNeeded()
{
    if (data==nullptr)
        data = new MLCachedAccess(FileCount, !ColArray.isGrayscale(), fwidth, fheight, ColMonoScale, ZDownsample);

}

void OpenCVInterface::SetUpFeatures()
{
    data->AddFeature("int");
    data->AddFeature("gau3@1");
    data->AddFeature("gau3@2");
    data->AddFeature("gau3@3");
    data->AddFeature("dog3@2@1");  //difference of gaussians
    data->AddFeature("dog3@3@2");  //difference of gaussians
    data->AddFeature("cnt3@2");
    data->AddFeature("cnt3@3");

}

void OpenCVInterface::CalculateFeatureData(MainWindowImpl *mw)
{
    CreateCacheHandlerIfNeeded();
    SetUpFeatures();

    MLUpdateBlockingDialog::showDialog(mw, "Initialising", "", "Calculating Feature Data");
    int featureCount = data->GetFeatureCount();
    for (int k = 0; k < Files.count(); k++)
    {
        if (mw->SliceSelectorList->item(k)->isSelected())
        {
            for (int i=0; i<featureCount; i++)
            {
                MLUpdateBlockingDialog::updateHighLevelText(QString("Slice %1 Feature: %2").arg(k).arg(data->GetFeatureName(i)));
                float dummy = data->GetFeatureValueAt(0,0,k,i);
            }
        }
    }
    MLUpdateBlockingDialog::hideDialog();
}

void OpenCVInterface::Train(int slice, MainWindowImpl *mw)
{
    CreateCacheHandlerIfNeeded();
    SetUpFeatures();

    if (SegmentCount<2)
    {
        Message(QString("You need at least two segments to perform training"));
        return;
    }

    MLUpdateBlockingDialog::showDialog(mw, "Collecting training data", "", "Train");
    QList<LabelledPoint> labels = GenerateLabels(mw);

    QList<int> counts;
    for (int i=0; i<SegmentCount; i++)
    {
        counts.append(0);
    }

    for (int i=0; i<labels.count(); i++)
    {
        counts[labels[i].segment]++;
    }

    int minValue = *std::min_element(counts.begin(), counts.end());
    if (minValue<1)
    {
        MLUpdateBlockingDialog::hideDialog();
        Message("You need at least one sample in each segment to train the model");
        return;
    }

    int featureCount = data->GetFeatureCount();
    auto trainingDataMat = cv::Mat(labels.count(), featureCount, CV_32F);
    auto labelsMat = cv::Mat(labels.count(), 1, CV_32S);


    for (int i=0; i<labels.count(); i++)
    {
        if (i%100==0)
        {    MLUpdateBlockingDialog::updateHighLevelText(QString("Fetching features for training %1%")
                                                        .arg((i*100)/labels.count()));
            MLUpdateBlockingDialog::updateDetailText(QString(""));
        }
        LabelledPoint point = labels[i];
        labelsMat.at<int>(i, 0) = point.segment;

        for (int j=0; j<featureCount; j++)
        {            
            trainingDataMat.at<float>(i, j) = data->GetFeatureValueAt(point.x, point.y, point.z, j);
        }
    }


    MLUpdateBlockingDialog::updateHighLevelText(QString("Training..."));
    MLUpdateBlockingDialog::updateDetailText(QString(""));
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
    MLUpdateBlockingDialog::hideDialog();

}
