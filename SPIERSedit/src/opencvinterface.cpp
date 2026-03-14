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
#include "ui/mlAddFeature.h"
#include <QMessageBox>
#include "opencvfileio.h"
#include <QFileDialog>

bool OpenCVInterface::enabled;



OpenCVInterface::OpenCVInterface()
{
    qDebug()<<"Here";
    data = nullptr;
    uiManager = nullptr;
    addFeatureDialog = nullptr;
}

//This should be called after new dataset is loaded or created
//Or after operations that break everything, e.g. change to resampling
void OpenCVInterface::Initialise(MainWindowImpl *mw, QLabel *statusLabel)
{
    lblStatus = statusLabel;
    mainWin = mw;
    qDebug()<<"H1";
    if (data!=nullptr)
        delete data;
    qDebug()<<"H2";
    if (uiManager!=nullptr)
        delete uiManager;


    qDebug()<<"H3";
    //remake things - nullptr triggers this
    data=nullptr;
    uiManager=nullptr;
    CreateSingletonsIfNeeded();
    ResetRFAndSample();
    UpdateStatusLabel();
    qDebug()<<"H4";
}

void OpenCVInterface::RemoveAllCacheFiles(bool override)
{
    QDir dir(openCVFileIO::GetWorkingPath());

    qDebug()<<openCVFileIO::GetWorkingPath();
    QStringList files = dir.entryList({"ml_*"}, QDir::Files);

    if (override)
    {

        for (const QString &file : files)
        {
            dir.remove(file);
        }

    }
    else
    {
        if (QMessageBox::question(mainWin,
                                  "Confirm",
                                  QString("This will remove %1 feature cache files - proceed?")
                                .arg(files.count()),
                            QMessageBox::Yes | QMessageBox::No,
                                                  QMessageBox::No)
            == QMessageBox::Yes)
        {
            MLUpdateBlockingDialog::showDialog(mainwin, "", "","Deleting files");

            for (const QString &file : files)
            {
                dir.remove(file);
            }
            MLUpdateBlockingDialog::hideDialog();
        }
    }
}

QByteArray OpenCVInterface::DumpFeaturesToByteArray()
{
    QByteArray outArray;
    QDataStream out(&outArray, QIODevice::WriteOnly);

    out << data->GetFeatureCount();
    for (int i=0; i<data->GetFeatureCount(); i++)
    {
        MLFeature *feature = data->GetFeature(i);
        out << (uchar)feature->GetType();
        out << (uchar)feature->GetChannel();
        out << feature->is3D();
        out << feature->GetArg1();
        out << feature->GetArg2();
        out << feature->IsSelected();
    }
    return outArray;
}

void OpenCVInterface::RetrieveFeaturesFromByteArray(QByteArray &byteArray)
{
    data->ClearFeatures();
    QDataStream in(&byteArray, QIODevice::ReadOnly);
    QList<MLFeature *> newFeatures;
    int itemCount;
    in >> itemCount;
    for (int i=0; i<itemCount; i++)
    {
        uchar dummy;

        bool is3D, isSelected;
        int arg1, arg2;

        in >> dummy;
        MLFeature::FeatureType type = (MLFeature::FeatureType)dummy;

        in >> dummy;
        MLFeature::Channel channel = (MLFeature::Channel)dummy;

        in >> is3D;
        in >> arg1;
        in >> arg2;
        in >> isSelected;

        MLFeature *feature = MLFeature::CreateFromData
            (type, channel, is3D, arg1, arg2);

        feature->SetSelected(isSelected);
        newFeatures.append(feature);
    }

    data->SetFeatures(newFeatures);
    uiManager->Rebuild();
    ResetRFAndSample();
}

void OpenCVInterface::SaveFeaturesToFile()
{
    QString filename = QFileDialog::getSaveFileName(
        mainWin,
        "Save feature-set",
        openCVFileIO::GetWorkingPath(),
        "FEAT files (*.feat)");

    if (filename.isEmpty())
        return;

    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    QDataStream out(&file);
    out << DumpFeaturesToByteArray();
    file.close();
}

void OpenCVInterface::LoadFeaturesFromFile()
{
    QString filename = QFileDialog::getOpenFileName(
        mainWin,
        "Load feature-set",
        openCVFileIO::GetWorkingPath(),
        "FEAT files (*.feat)");

    if (filename.isEmpty())
        return;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    QDataStream in(&file);
    QByteArray dummy;
    in >> dummy;
    file.close();

    RetrieveFeaturesFromByteArray(dummy);

}

void OpenCVInterface::ResetRFAndSample()
{
    CreateSingletonsIfNeeded();
    //reset rf
    rf.release();
    rf = cv::ml::RTrees::create();

    labels.clear();
    UpdateStatusLabel();
}

void OpenCVInterface::ResetCachedData()
{
    data->Reset();
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
    CreateSingletonsIfNeeded();

    if (!rf->isTrained())
    {
        Message("ML model is not trained");
        return;
    }

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
    CreateSingletonsIfNeeded();
    uiManager->ActivateSelectedFeatures(activate);
}

void OpenCVInterface::UIDeleteSelectedFeatures()
{
    CreateSingletonsIfNeeded();
    if (uiManager->DeleteSelectedFeatures()>0)
        ResetRFAndSample();
}

void OpenCVInterface::UIAddFeature()
{
    CreateSingletonsIfNeeded();
    if (addFeatureDialog==nullptr)
        addFeatureDialog = new MLAddFeature(mainwin);

    addFeatureDialog->Show();
    MLFeature *feature = addFeatureDialog->GetResult();

    if (feature!=nullptr)
    {
        data->SetFeatureInUse(data->AddFeature(feature),true);
        ResetRFAndSample();
        uiManager->Rebuild();
    }
}

void OpenCVInterface::SetSamplePercent(int v)
{
    samplePercent = v;
}

void OpenCVInterface::SetMinSampleCount(int v)
{
    minSampleCount = v;
}

void OpenCVInterface::SetTreeCount(int v)
{
    treeCount = v;
}

void OpenCVInterface::SetTreeDepth(int v)
{
    treeDepth = v;
}

int OpenCVInterface::GetSamplePercent()
{
    return samplePercent;
}

int OpenCVInterface::GetMinSampleCount()
{
    return minSampleCount;
}

int OpenCVInterface::GetTreeCount()
{
    return treeCount;
}

int OpenCVInterface::GetTreeDepth()
{
    return treeDepth;
}

void OpenCVInterface::SetCacheMemSizeGb(int v)
{
    if (!IsDatasetLoaded()) return;
    CreateSingletonsIfNeeded();
    data->SetMaxMemoryUsage(1024ul*1024ul*1014ul*(ulong)v);
}


int OpenCVInterface::GetCacheMemSizeGb()
{
    CreateSingletonsIfNeeded();
    return data->GetMaxMemoryUsageGb();
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

QString OpenCVInterface::DescribeSample()
{
    if (labels.count()==0)
    {
        return "[Not Defined]";
    }
    else
    {
        int minSlice=9999999;
        int maxSlice=-1;
        QVector<int> counts(SegmentCount, 0);

        for (int i=0; i<labels.count(); i++)
        {
            if (labels[i].z<minSlice)
                minSlice = labels[i].z;
            if (labels[i].z>maxSlice)
                maxSlice = labels[i].z;
            counts[labels[i].segment]++;
        }

        QStringList parts;
        for (int v : counts)
            parts << QString::number(v);

        QString details = parts.join(",");

        if (minSlice < maxSlice)
            return QString("%1 samples (%2) slices %3-%4")
                .arg(labels.count())
                .arg(details)
                .arg(minSlice+1)
                .arg(maxSlice+1);
        else
            return QString("%1 samples (%2) slice %3")
                .arg(labels.count())
                .arg(details)
                .arg(minSlice+1);

    }
}

void OpenCVInterface::UpdateStatusLabel()
{
    if (rf->isTrained())
        lblStatus->setText(QString("Trained on ")+DescribeSample());
    else
        lblStatus->setText("Not trained, no sample");
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

        if (uiManager!=nullptr)
        {
            delete uiManager;
            uiManager=nullptr;
        }
    }

    if (uiManager==nullptr)
    {
        uiManager = new MLFeatureUIManager(data, mainWin->tblMLFeatureList);
        uiManager->Rebuild();
    }
}

void OpenCVInterface::TestSetUpFeatures()
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

    uiManager->Rebuild();
}

void OpenCVInterface::MakeML(int fnum)
{

}

void OpenCVInterface::CalculateFeatureData()
{
    CreateSingletonsIfNeeded();

    MLUpdateBlockingDialog::showDialog(mainWin, "Initialising", "", "Calculating Feature Data");
    int featureCount = data->GetFeatureCount();
    for (int k = 0; k < Files.count(); k++)
    {
        if (mainWin->SliceSelectorList->item(k)->isSelected())
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

void OpenCVInterface::SampleAndTrain()
{
    CreateSingletonsIfNeeded();

    if (SegmentCount<2)
    {
        Message(QString("You need at least two segments to perform training"));
        return;
    }

    qDebug()<<"Feature count "<<data->GetFeaturesInUse().count();
    if (data->GetFeaturesInUse().count()==0)
    {
        Message("At least one active feature is required to perform training");
        return;
    }

    MLUpdateBlockingDialog::showDialog(mainWin, "Collecting training data", "", "Train");

    if (!mainWin->actionIncremental_sampling->isChecked())
        labels.clear();

    auto newLabels = GenerateLabels(mainWin, samplePercent);
    labels.append(newLabels);
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
        Message("You need at least two samples in each segment to perform training");
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
    rf->setCalculateVarImportance(true);
    rf->setMaxDepth(treeDepth);
    rf->setMinSampleCount(minSampleCount);
    rf->setRegressionAccuracy(0.0f);
    rf->setUseSurrogates(false);
    rf->setMaxCategories(2);
    rf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, treeCount, 0));

    auto td = cv::ml::TrainData::create(
        trainingDataMat,
        cv::ml::ROW_SAMPLE,
        labelsMat
        );

    rf->train(td);

    UpdateStatusLabel();

    //calc importances
    auto importances = rf->getVarImportance();

    double total = cv::sum(importances)[0];
    for (int i = 0; i < importances.rows; ++i)
    {
        float score = importances.at<float>(i,0);
        float pct = 100.0f * score / total;
        data->GetFeature(featureIDs[i])->SetImportance((int)pct);
    }

    uiManager->RefreshImportance();
    MLUpdateBlockingDialog::hideDialog();

}
