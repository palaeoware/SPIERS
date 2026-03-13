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
#include "mlfeatureintensity.h"
#include "mlfeaturegaussian.h"
#include "mlfeaturecontrast.h"
#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeaturemean.h"
#include "mlfeatureuimanager.h"

bool OpenCVInterface::enabled;



OpenCVInterface::OpenCVInterface()
{
    data = nullptr;
    uiManager = nullptr;

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


void OpenCVInterface::GetProbabilitiesAllSegments(int x, int y, int z, int *segBuffer)
{
    //segbuffer is votes 0-255 for each segment. Zero it.
    for (int i=0; i<SegmentCount; i++)
        segBuffer[i]=0;

    if (data == nullptr || !rf->isTrained()) return;

    QList<int> featureIDs = data->GetFeaturesInUse();
    int featureCount = featureIDs.count();
    cv::Mat sample(1, featureCount, CV_32F);

    for (int i=0; i<featureCount; i++)
    {
        float v = data->GetFeatureValueAt(x,y,z,featureIDs[i]);
        sample.at<float>(0,i)= v;
    }

    cv::Mat votes;
    rf->getVotes(sample, votes, 0);

    //extract votes into buffer
    int tot=0;
    for (int i=0; i<SegmentCount; i++)
    {
        int thisSeg = votes.at<int>(0,i);
        int theseVotes = votes.at<int>(1,i);
        segBuffer[thisSeg] = theseVotes;
        tot += theseVotes;
    }

    //normalise
    for (int i=0; i<SegmentCount; i++)
    {
        segBuffer[i] = (segBuffer[i] * 255)/tot;
    }
}

//Replaces the CopyingImpl progress bar system

void OpenCVInterface::Generate(QListWidget *SliceSelectorList)
{
    WriteAllData(CurrentFile);

    MLUpdateBlockingDialog::showDialog(mainwin, "", "","Creating segments using ML data");

    for (int i = 0; i < Files.count(); i++)
    {
        if ((SliceSelectorList->item(i))->isSelected())
        {
            MLUpdateBlockingDialog::updateHighLevelText(QString("Slice %1").arg(i));
            MLUpdateBlockingDialog::updateDetailText(QString("Fetching slice data"));
            LoadLocks(i);
            LoadMasks(i);
            for (int s=0; s<SegmentCount; s++)
            {
                LoadGreyData(i, s);
            }

            ComputeSliceProbabilitiesFromVotes(i);

            MLUpdateBlockingDialog::updateDetailText(QString("Storing slice data"));
            for (int s=0; s<SegmentCount; s++)
                SaveGreyData(i, s);
        }
    }

    LoadAllData(CurrentFile);
    MLUpdateBlockingDialog::hideDialog();
}

void OpenCVInterface::UIActivateSelectedFeatures(bool activate)
{
    uiManager->ActivateSelectedFeatures(activate);
}

void OpenCVInterface::UIDeleteSelectedFeatures()
{

}

void OpenCVInterface::UIAddFeature()
{

}

void OpenCVInterface::ComputeSliceProbabilitiesFromVotes(int sliceID)
{
    QList<int> featureIndices = data->GetFeaturesInUse();
    const int numFeatures = featureIndices.count();
    const int numPixels = fwidth * fheight;

    QVector<cv::Mat> featureSlices;
    featureSlices.reserve(numFeatures);

    for (int f = 0; f < numFeatures; ++f)
        featureSlices.append(data->GetWholeSliceFeature(sliceID, featureIndices[f]));

    // Allocate samples matrix
    cv::Mat samples(numPixels, numFeatures, CV_32F);

    MLUpdateBlockingDialog::updateDetailText(QString("Assembling feature data"));
    // Fill samples matrix using raw pointers
    for (int y = 0; y < fheight; ++y)
    {

        for (int x = 0; x < fwidth; ++x)
        {
            int row = y * fwidth + x;
            float *sampleRow = samples.ptr<float>(row);

            for (int f = 0; f < numFeatures; ++f)
            {
                const float *srcRow = featureSlices[f].ptr<float>(y);
                sampleRow[f] = srcRow[x];
            }
        }
    }

    MLUpdateBlockingDialog::updateDetailText(QString("Running ML model"));
    cv::Mat votes;
    rf->getVotes(samples, votes, 0);

    int numTrees = static_cast<int>(rf->getRoots().size());

    QVector<int> classForColumn(SegmentCount);
    const int *labelRow = votes.ptr<int>(0);

    for (int col = 0; col < SegmentCount; ++col)
        classForColumn[col] = labelRow[col];

    MLUpdateBlockingDialog::updateDetailText(QString("Calculating segments from model outputs"));

    QByteArray newLocks = DoMaskLocking();

    for (int y = 0; y < fheight; ++y)
    {
        QVector<uchar*> outRows(SegmentCount);
        for (int c = 0; c < SegmentCount; ++c)
            outRows[c] = GA[c]->scanLine(y);

        for (int x = 0; x < fwidth; ++x)
        {
            int sampleRow = y * fwidth + x + 1;
            const int *voteRow = votes.ptr<int>(sampleRow);

            for (int col = 0; col < SegmentCount; ++col)
            {
                int label = classForColumn[col];
                int voteCount = voteRow[col];

                float prob = static_cast<float>(voteCount) / numTrees;
                int v = static_cast<int>(prob * 255.0f + 0.5f);

                if (v < 0) v = 0;
                if (v > 255) v = 255;

                if (!newLocks[fwidth * y + x])
                    outRows[label][x] = static_cast<uchar>(v);
            }
        }
    }
}


uchar OpenCVInterface::GetProbability(int x, int y, int z, int segment)
{
    if (data == nullptr || !rf->isTrained())
    {
        return 0;
    }

    QList<int> featureIDs = data->GetFeaturesInUse();
    int featureCount = featureIDs.count();
    cv::Mat sample(1, featureCount, CV_32F);

    for (int i=0; i<featureCount; i++)
    {
        float v = data->GetFeatureValueAt(x,y,z,featureIDs[i]);
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

void OpenCVInterface::CreateSingletonsIfNeeded()
{
    if (data==nullptr)
    {
        data = new MLCachedAccess(FileCount, !ColArray.isGrayscale(), fwidth, fheight, ColMonoScale, ZDownsample);
    }

    if (uiManager==nullptr)
    {
        uiManager = new MLFeatureUIManager(data, mainwin->tblMLFeatureList);
        uiManager->Rebuild();
    }
}

void OpenCVInterface::SetUpFeatures()
{
    data->SetFeatureInUse(data->AddFeature(new MLFeatureIntensity(MLFeature::Channel::Intensity)),true);

    data->SetFeatureInUse(data->AddFeature(new MLFeatureContrast(MLFeature::Channel::Intensity, true, 2)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureDifferenceOfGaussians
                                           (MLFeature::Channel::Intensity, true, 2,1)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureDifferenceOfGaussians
                                           (MLFeature::Channel::Intensity, true, 3,2)),true);

    data->SetFeatureInUse(data->AddFeature(new MLFeatureMean
                                           (MLFeature::Channel::Intensity, false, 2)),true);

    data->SetFeatureInUse(data->AddFeature(new MLFeatureMean
                                           (MLFeature::Channel::Intensity, true, 2)),true);

    data->SetFeatureInUse(data->AddFeature(new MLFeatureIntensity(MLFeature::Channel::Intensity)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureGaussian(MLFeature::Channel::Intensity,true, 2)), true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureIntensity(MLFeature::Channel::Red)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureGaussian(MLFeature::Channel::Red,true, 2)), true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureIntensity(MLFeature::Channel::Green)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureGaussian(MLFeature::Channel::Green,true, 2)), true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureIntensity(MLFeature::Channel::Blue)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureGaussian(MLFeature::Channel::Blue,true, 2)), true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureIntensity(MLFeature::Channel::R_G)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureGaussian(MLFeature::Channel::R_G,true, 2)), true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureIntensity(MLFeature::Channel::G_B)),true);
    data->SetFeatureInUse(data->AddFeature(new MLFeatureGaussian(MLFeature::Channel::G_B,true, 2)), true);

    data->DumpFeatures();
    uiManager->Rebuild();
}

void OpenCVInterface::CalculateFeatureData(MainWindowImpl *mw)
{
    CreateSingletonsIfNeeded();
    SetUpFeatures();

    MLUpdateBlockingDialog::showDialog(mw, "Initialising", "", "Calculating Feature Data");
    int featureCount = data->GetFeatureCount();
    for (int k = 0; k < Files.count(); k++)
    {
        if (mw->SliceSelectorList->item(k)->isSelected())
        {
            for (int i=0; i<featureCount; i++)
            {
                MLUpdateBlockingDialog::updateHighLevelText(QString("Slice %1 Feature: %2").arg(k).arg(data->GetFeature(i)->GetPrettyFullName()));

                float dummy = data->GetFeatureValueAt(0,0,k,i);
                Q_UNUSED(dummy);
            }
        }
    }
    MLUpdateBlockingDialog::hideDialog();
}

void OpenCVInterface::Train(int slice, MainWindowImpl *mw)
{
    CreateSingletonsIfNeeded();
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
    if (minValue<2)
    {
        MLUpdateBlockingDialog::hideDialog();
        Message("You need at least two sample voxels in each segment to train the model");
        return;
    }

    QList<int> featureIDs = data->GetFeaturesInUse();
    int featureCount = featureIDs.count();
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
            trainingDataMat.at<float>(i, j) = data->GetFeatureValueAt(point.x, point.y, point.z, featureIDs[j]);
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
