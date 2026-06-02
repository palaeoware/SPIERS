/**
 * @file
 * Source: Mlinterface
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */
#include "mlinterface.h"
#include <QDebug>
#include <QImage>

#include "globals.h"
#include "display.h"
#include "src/fileio.h"
#include "display.h"
#include <opencv2/imgproc.hpp>
#include "labelledpoint.h"
#include "mainwindow.h"
#include "mlupdateblockingdialog.h"
#include "mlfeatureintensity.h"
#include "mlfeaturegaussian.h"
#include "mlfeaturecontrast.h"
#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeatureuimanager.h"
#include "mladdfeature.h"
#include <QMessageBox>
#include "mlfileio.h"
#include <QFileDialog>
#include <QThread>
#include "mlparallelforest.h"
#include "mlfeaturepresets.h"
#include <QMessageBox>

bool MLInterface::enabled;



MLInterface::MLInterface()
{
    data = nullptr;
    uiManager = nullptr;
    addFeatureDialog = nullptr;
    rf = std::make_unique<MLParallelForest>();
}

//This should be called after new dataset is loaded or created
//Or after operations that break everything, e.g. change to resampling
void MLInterface::Initialise(MainWindow *mw, QLabel *statusLabel)
{
    lblStatus = statusLabel;
    mainWin = mw;

    if (data != nullptr)
        delete data;

    if (uiManager != nullptr)
        delete uiManager;

    //remake things - nullptr triggers this
    data = nullptr;
    uiManager = nullptr;
    CreateSingletonsIfNeeded();
    ResetRFAndSample();
    UpdateStatusLabel();
    data->ResizeCache();
    InvalidateProbabilityCache();
}

void MLInterface::RemoveAllCacheFiles(bool override)
{
    QDir dir(MLFileIO::GetWorkingPath());

    qDebug() << MLFileIO::GetWorkingPath();
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
            MLUpdateBlockingDialog::showDialog(mainwin, "", "", "Deleting files");

            for (const QString &file : files)
            {
                dir.remove(file);
            }
            MLUpdateBlockingDialog::hideDialog();
        }
    }
}

QByteArray MLInterface::DumpFeaturesToByteArray()
{
    QByteArray outArray;
    QDataStream out(&outArray, QIODevice::WriteOnly);

    out << data->GetFeatureCount();
    for (int i = 0; i < data->GetFeatureCount(); i++)
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

void MLInterface::RetrieveFeaturesFromByteArray(QByteArray &byteArray)
{
    data->ClearFeatures();
    QDataStream in(&byteArray, QIODevice::ReadOnly);
    QList<MLFeature *> newFeatures;
    int itemCount;
    in >> itemCount;
    for (int i = 0; i < itemCount; i++)
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

        if (feature != nullptr)
        {
            feature->SetSelected(isSelected);
            newFeatures.append(feature);
        }
    }

    data->SetFeatures(newFeatures);
    uiManager->Rebuild();
    ResetRFAndSample();
}

void MLInterface::SaveFeaturesToFile()
{
    QString filename = QFileDialog::getSaveFileName(
                           mainWin,
                           "Save feature-set",
                           MLFileIO::GetWorkingPath(),
                           "FEAT files (*.feat)");

    if (filename.isEmpty())
        return;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream out(&file);
    out << DumpFeaturesToByteArray();
    file.close();
}

void MLInterface::LoadFeaturesFromFile()
{
    QString filename = QFileDialog::getOpenFileName(
                           mainWin,
                           "Load feature-set",
                           MLFileIO::GetWorkingPath(),
                           "FEAT files (*.feat)");

    if (filename.isEmpty())
        return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return;

    QDataStream in(&file);
    QByteArray dummy;
    in >> dummy;
    file.close();

    RetrieveFeaturesFromByteArray(dummy);

}

void MLInterface::ResetRFAndSample()
{
    CreateSingletonsIfNeeded();
    //reset rf
    rf->Clear();
    labels.clear();
    InvalidateProbabilityCache();
    UpdateStatusLabel();
}

void MLInterface::ResetCachedData()
{
    data->Reset();
    InvalidateProbabilityCache();
}


//static test method - run by main on startup.
bool MLInterface::TestML()
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

void MLInterface::Generate(QListWidget *SliceSelectorList)
{
    CreateSingletonsIfNeeded();

    if (!rf->IsValid())
    {
        Message("ML model is not trained");
        return;
    }

    WriteAllData(CurrentFile);

    MLUpdateBlockingDialog::showDialog(mainwin, "", "", "Creating segments using ML data");

    for (int i = 0; i < Files.count(); i++)
    {

        if ((SliceSelectorList->item(i))->isSelected())
        {
            MLUpdateBlockingDialog::updateHighLevelText(QString("Slice %1").arg(i));
            MLUpdateBlockingDialog::updateDetailText(QString("Fetching slice data"));
            LoadLocks(i);
            LoadMasks(i);
            for (int s = 0; s < SegmentCount; s++)
            {
                LoadGreyData(i, s);
            }

            ComputeSliceProbabilitiesFromVotes(i);

            MLUpdateBlockingDialog::updateDetailText(QString("Storing slice data"));
            for (int s = 0; s < SegmentCount; s++)
                SaveGreyData(i, s);
        }

        if (MLUpdateBlockingDialog::isCancelled())
            break;

    }

    LoadAllData(CurrentFile);
    MLUpdateBlockingDialog::hideDialog();
}

void MLInterface::UIActivateSelectedFeatures(bool activate)
{
    CreateSingletonsIfNeeded();
    uiManager->ActivateSelectedFeatures(activate);
    ResetRFAndSample();
}

void MLInterface::UIDeleteSelectedFeatures()
{
    CreateSingletonsIfNeeded();
    if (uiManager->DeleteSelectedFeatures() > 0)
        ResetRFAndSample();
}

void MLInterface::UIAddFeature()
{
    CreateSingletonsIfNeeded();
    if (addFeatureDialog == nullptr)
        addFeatureDialog = new MLAddFeature(mainwin);

    addFeatureDialog->Show();
    MLFeature *feature = addFeatureDialog->GetResult();

    if (feature != nullptr)
    {
        data->SetFeatureInUse(data->AddFeature(feature), true);
        ResetRFAndSample();
        uiManager->Rebuild();
    }
}

void MLInterface::SetSamplePercent(int v)
{
    samplePercent = v;
}

void MLInterface::SetMinSampleCount(int v)
{
    minSampleCount = v;
}

void MLInterface::SetTreeCount(int v)
{
    treeCount = v;
}

void MLInterface::SetTreeDepth(int v)
{
    treeDepth = v;
}

int MLInterface::GetSamplePercent()
{
    return samplePercent;
}

int MLInterface::GetMinSampleCount()
{
    return minSampleCount;
}

int MLInterface::GetTreeCount()
{
    return treeCount;
}

int MLInterface::GetTreeDepth()
{
    return treeDepth;
}

void MLInterface::ComputeSliceProbabilitiesFromVotes(int sliceID)
{
    if (!rf || !rf->IsValid())
        return;

    MLUpdateBlockingDialog::updateDetailText(QString("Running ML model"));

    if (!EnsureSliceProbabilityCache(sliceID))
        return;

    MLUpdateBlockingDialog::updateDetailText(QString("Calculating segments from model outputs"));

    QByteArray newLocks = DoMaskLocking();

    for (int y = 0; y < fheight; ++y)
    {
        QVector<uchar *> outRows(SegmentCount);
        for (int c = 0; c < SegmentCount; ++c)
            outRows[c] = GA[c]->scanLine(y);

        for (int x = 0; x < fwidth; ++x)
        {
            const int sampleRow = y * fwidth + x;
            const float *probRow = cachedSliceProbabilities.ptr<float>(sampleRow);

            if (!newLocks[fwidth * y + x])
            {
                int high = -1;
                int highSeg = -1;
                for (int c = 0; c < SegmentCount; ++c)
                {
                    int v = static_cast<int>(probRow[c] * 255.0f + 0.5f);

                    if (v < 0) v = 0;
                    if (v > 255) v = 255;

                    if (v > high)
                    {
                        high = v;
                        highSeg = c;
                    }
                    outRows[c][x] = static_cast<uchar>(v);
                }
                if (high < 128) //ensure no black!
                {
                    outRows[highSeg][x] = static_cast<uchar>(128);
                }
            }
        }
    }
}

QString MLInterface::DescribeSample()
{
    if (labels.count() == 0)
    {
        return "[Not Defined]";
    }
    else
    {
        int minSlice = 9999999;
        int maxSlice = -1;
        QVector<int> counts(SegmentCount, 0);

        for (int i = 0; i < labels.count(); i++)
        {
            if (labels[i].z < minSlice)
                minSlice = labels[i].z;
            if (labels[i].z > maxSlice)
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
                   .arg(minSlice + 1)
                   .arg(maxSlice + 1);
        else
            return QString("%1 samples (%2) slice %3")
                   .arg(labels.count())
                   .arg(details)
                   .arg(minSlice + 1);

    }
}

void MLInterface::UpdateStatusLabel()
{
    if (rf->IsValid())
        lblStatus->setText(QString("Trained on ") + DescribeSample());
    else
        lblStatus->setText("Not trained, no sample");
}


void MLInterface::CreateSingletonsIfNeeded()
{
    if (data == nullptr)
    {
        bool isGrey = false;
        auto format = ColArray.format();
        if (ColArray.format() == QImage::Format_Indexed8)
        {
            if (ColArray.allGray()) isGrey = true;
        }
        else
        {
            if (ColArray.isGrayscale()) isGrey = true;
        }

        data = new MLCachedAccess(FileCount, !isGrey, fwidth, fheight, ColMonoScale, ZDownsample);

        if (uiManager != nullptr)
        {
            delete uiManager;
            uiManager = nullptr;
        }
    }

    if (uiManager == nullptr)
    {
        uiManager = new MLFeatureUIManager(data, mainWin->tblMLFeatureList);
        uiManager->Rebuild();
    }
}


void MLInterface::CalculateFeatureData()
{
    CreateSingletonsIfNeeded();

    MLUpdateBlockingDialog::showDialog(mainWin, "Initialising", "", "Calculating Feature Data");
    int featureCount = data->GetFeatureCount();
    for (int k = 0; k < Files.count(); k++)
    {
        if (mainWin->SliceSelectorList->item(k)->isSelected())
        {
            for (int i = 0; i < featureCount; i++)
            {
                MLUpdateBlockingDialog::updateHighLevelText(QString("Slice %1 Feature: %2").arg(k).arg(data->GetFeature(i)->GetPrettyFullName()));

                float dummy = data->GetFeatureValueAt(0, 0, k, i);
                Q_UNUSED(dummy);
            }
        }
        if (MLUpdateBlockingDialog::isCancelled())
            break;
    }
    MLUpdateBlockingDialog::hideDialog();
}

void MLInterface::AutoSampleTrainAndGenerate()
{
    if (mainWin->SliceSelectorList->selectedItems().count() != 1)
        return;

    if (!mainWin->SliceSelectorList->item(CurrentFile)->isSelected())
        return;


    if (!Sample(true, false))
        return;

    if (!Train(false))
        return;

    ComputeSliceProbabilitiesFromVotes(CurrentFile);

    for (int i = 0; i < SegmentCount; i++)
    {
        SaveGreyData(CurrentFile, i);
    }

    ShowImage(mainWin->graphicsView);
    UpdateStatusLabel();
    MLUpdateBlockingDialog::hideDialog();
}

void MLInterface::ResizeCache()
{
    if (data != nullptr)
        data->ResizeCache();
}



bool MLInterface::Sample(bool incremental, bool noMessages)
{
    if (SegmentCount < 2)
    {
        if (!noMessages) Message(QString("You need at least two segments to perform training"));
        return false;
    }

    qDebug() << "Feature count " << data->GetFeaturesInUse().count();
    if (data->GetFeaturesInUse().count() == 0)
    {
        if (!noMessages) Message("At least one active feature is required to perform training");
        return false;
    }

    MLUpdateBlockingDialog::showDialog(mainWin, "Collecting training data", "", "Train");

    if (incremental)
    {
        //remove any labels in selected files
        for (int k = 0; k < Files.count(); k++)
        {
            if (mainWin->SliceSelectorList->item(k)->isSelected())
            {
                labels.removeIf([k](const LabelledPoint & item)
                {
                    return item.z == k;
                });

            }
        }
    }
    else
    {
        labels.clear();
    }

    auto newLabels = GenerateLabels(mainWin, samplePercent);
    labels.append(newLabels);
    return true;

}


bool MLInterface::Train(bool noMessages)
{
    QList<int> counts;
    for (int i = 0; i < SegmentCount; i++)
        counts.append(0);

    for (int i = 0; i < labels.count(); i++)
        counts[labels[i].segment]++;

    int minValue = *std::min_element(counts.begin(), counts.end());
    if (minValue < 2)
    {
        if (!noMessages) MLUpdateBlockingDialog::hideDialog();
        if (!noMessages) Message("You need at least two samples in each segment to perform training");
        return false;
    }

    QList<int> featureIDs = data->GetFeaturesInUse();
    int featureCount = featureIDs.count();

    cv::Mat trainingDataMat(labels.count(), featureCount, CV_32F);
    cv::Mat labelsMat(labels.count(), 1, CV_32S);

    for (int i = 0; i < labels.count(); i++)
    {
        if (i % 100 == 0)
        {
            MLUpdateBlockingDialog::updateHighLevelText(
                QString("Fetching features for training %1%").arg((i * 100) / labels.count()));
            MLUpdateBlockingDialog::updateDetailText(QString(""));
        }

        LabelledPoint point = labels[i];
        labelsMat.at<int>(i, 0) = point.segment;

        for (int j = 0; j < featureCount; j++)
            trainingDataMat.at<float>(i, j) = data->GetFeatureValueAt(point.x, point.y, point.z, featureIDs[j]);
    }

    MLUpdateBlockingDialog::updateHighLevelText(QString("Training..."));
    MLUpdateBlockingDialog::updateDetailText(QString(""));

    InvalidateProbabilityCache();

    rf.reset(new MLParallelForest());
    rf->SetShardCount(qMax(1, QThread::idealThreadCount() / 2));
    rf->SetTreeCount(treeCount);
    rf->SetMinSampleCount(minSampleCount);
    rf->SetMaxDepth(treeDepth);

    if (!rf->Train(trainingDataMat, labelsMat, SegmentCount))
    {
        if (!noMessages) MLUpdateBlockingDialog::hideDialog();
        if (!noMessages) Message("Training failed");
        return false;
    }

    InvalidateProbabilityCache();
    return true;
}

void MLInterface::DoImportances()
{
    for (int i = 0; i < data->GetFeatureCount(); i++)
        data->GetFeature(i)->SetImportance(-1);

    if (!rf || !rf->IsValid())
    {
        uiManager->RefreshImportance();
        return;
    }

    cv::Mat importances = rf->GetVarImportance();
    if (importances.empty())
    {
        uiManager->RefreshImportance();
        return;
    }

    double total = cv::sum(importances)[0];
    if (total <= 0.0)
    {
        uiManager->RefreshImportance();
        return;
    }

    auto featureIDs = data->GetFeaturesInUse();

    for (int i = 0; i < importances.rows; ++i)
    {
        float score = importances.at<float>(i, 0);
        float pct = 100.0f * score / static_cast<float>(total);
        data->GetFeature(featureIDs[i])->SetImportance((int)pct);
    }

    uiManager->RefreshImportance();
}

void MLInterface::SampleAndTrain(bool autoGen)
{
    if (autoGen)
    {
        AutoSampleTrainAndGenerate();
    }
    else
    {
        CreateSingletonsIfNeeded();

        if (!Sample(mainWin->actionIncremental_sampling->isChecked(), false))
            return;

        if (!Train(false))
            return;

        UpdateStatusLabel();

        //zero all old importances
        DoImportances();
        MLUpdateBlockingDialog::hideDialog();
    }
}

void MLInterface::DoPreset(int presetCode)
{
    MLFeaturePresets::Preset preset = (MLFeaturePresets::Preset)presetCode;

    if (data == nullptr) return;

    if (data->GetFeatureCount() != 0)
    {
        if (QMessageBox::question
                (mainWin, "Confirm", "This will remove all features and replace them with a predefined set. Are you sure?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
            return;
    }

    data->ClearFeatures();

    //Use a loop and add features to include them all along with their prereqs
    QList<MLFeature *> newList = MLFeaturePresets::GetPresetFeatureList(preset);

    for (int i = 0; i < newList.count(); i++)
    {
        data->AddFeature(newList[i]);
    }


    for (int i = 0; i < newList.count(); i++)
    {
        if (data->GetIndexForFeature(newList[i]) == -1)
        {
            qDebug() << "-1: " << i << data->GetIndexForFeature(newList[i]);
        }
        data->SetFeatureInUse(data->GetIndexForFeature(newList[i]), true);
    }
    uiManager->Rebuild();

    ResetRFAndSample();
}



//Recalc brush stuff


void MLInterface::InvalidateProbabilityCache()
{
    cachedSliceProbabilities.release();
    cachedProbabilitySliceID = -1;
    cachedProbabilitySliceValid = false;
}

bool MLInterface::BuildSliceSampleMatrix(int sliceID, cv::Mat &samples)
{
    if (data == nullptr)
        return false;

    QList<int> featureIndices = data->GetFeaturesInUse();
    const int numFeatures = featureIndices.count();
    const int numPixels = fwidth * fheight;

    if (numFeatures < 1 || numPixels < 1)
        return false;

    QVector<cv::Mat> featureSlices;
    featureSlices.reserve(numFeatures);

    for (int f = 0; f < numFeatures; ++f)
        featureSlices.append(data->GetWholeSliceFeature(sliceID, featureIndices[f]));

    samples.create(numPixels, numFeatures, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        for (int x = 0; x < fwidth; ++x)
        {
            const int row = y * fwidth + x;
            float *sampleRow = samples.ptr<float>(row);

            for (int f = 0; f < numFeatures; ++f)
            {
                const float *srcRow = featureSlices[f].ptr<float>(y);
                sampleRow[f] = srcRow[x];
            }
        }
    }

    return true;
}

bool MLInterface::EnsureSliceProbabilityCache(int sliceID)
{
    if (data == nullptr)
        return false;

    if (!rf || !rf->IsValid())
        return false;

    if (cachedProbabilitySliceValid && cachedProbabilitySliceID == sliceID)
        return true;

    cv::Mat samples;
    if (!BuildSliceSampleMatrix(sliceID, samples))
        return false;

    cv::Mat probabilities;
    if (!rf->PredictProbabilities(samples, probabilities))
        return false;

    cachedSliceProbabilities = probabilities;
    cachedProbabilitySliceID = sliceID;
    cachedProbabilitySliceValid = true;

    return true;
}

void MLInterface::GetProbabilitiesAllSegments(int x, int y, int z, int *segBuffer)
{
    if (segBuffer == nullptr)
        return;

    for (int i = 0; i < SegmentCount; i++)
        segBuffer[i] = 0;

    if (data == nullptr || !rf || !rf->IsValid())
        return;

    if (x < 0 || x >= fwidth || y < 0 || y >= fheight)
        return;

    if (!EnsureSliceProbabilityCache(z))
        return;

    const int row = y * fwidth + x;
    const float *probRow = cachedSliceProbabilities.ptr<float>(row);

    int high = -1;
    int highseg = -1;
    for (int i = 0; i < SegmentCount; i++)
    {
        int v = static_cast<int>(probRow[i] * 255.0f + 0.5f);

        if (v < 0) v = 0;
        if (v > 255) v = 255;

        if (v > high)
        {
            high = v;
            highseg = i;
        }
        segBuffer[i] = v;
    }
    if (high < 128)
    {
        segBuffer[highseg] = 128; //ensure no black
    }
}

