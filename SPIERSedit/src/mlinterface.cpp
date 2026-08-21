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

#include "display.h"
#include "globals.h"
#include "labelledpoint.h"
#include "mainwindow.h"
#include "mladdfeature.h"
#include "mlfeaturecontrast.h"
#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeaturegaussian.h"
#include "mlfeatureintensity.h"
#include "mlfeaturepresets.h"
#include "mlfeatureuimanager.h"
#include "mlfileio.h"
#include "mlparallelforest.h"
#include "mlroislice.h"
#include "mlupdateblockingdialog.h"
#include "src/fileio.h"

#include <QDebug>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMessageBox>
#include <QMutexLocker>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScopeGuard>
#include <QSpinBox>
#include <QTemporaryDir>
#include <QThread>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <opencv2/imgproc.hpp>

#include <cstring>
#include <vector>

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
    WriteAllData(CurrentFile);

    if (!rf->IsValid())
    {
        Message("ML model is not trained");
        return;
    }

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

    MLROISlice roi(fwidth, fheight, DoMaskLocking());
    if (!roi.isValid())
    {
        qWarning() << "Could not create ML ROI for slice" << sliceID;
        return;
    }
    const QByteArray &newLocks = roi.excludedPixels();

    if (!EnsureSliceProbabilityCache(sliceID, &roi))
        return;

    MLUpdateBlockingDialog::updateDetailText(QString("Calculating segments from model outputs"));

    int sampleRow = 0;
    for (int y = 0; y < fheight; ++y)
    {
        QVector<uchar *> outRows(SegmentCount);
        for (int c = 0; c < SegmentCount; ++c)
            outRows[c] = GA[c]->scanLine(y);

        for (int x = 0; x < fwidth; ++x)
        {
            if (!newLocks[fwidth * y + x])
            {
                const float *probRow = cachedSliceProbabilities.ptr<float>(sampleRow);

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
                sampleRow++;
            }
        }
    }
    Q_ASSERT(sampleRow == cachedSliceProbabilities.rows);
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
    WriteAllData(CurrentFile);

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

    QVector<QVector<int>> sampleRowsBySlice(FileCount);
    for (int i = 0; i < labels.count(); i++)
    {
        const LabelledPoint &point = labels.at(i);
        Q_ASSERT(point.x >= 0 && point.x < fwidth);
        Q_ASSERT(point.y >= 0 && point.y < fheight);
        Q_ASSERT(point.z >= 0 && point.z < FileCount);
        labelsMat.at<int>(i, 0) = point.segment;
        sampleRowsBySlice[point.z].append(i);
    }

    qint64 trainingTargetTiles = 0;
    qint64 trainingPossibleTiles = 0;
    qint64 uniqueTrainingPixels = 0;
    int sampledSliceCount = 0;
    int processedSampleCount = 0;
    int trainingTileSize = 0;

    {
        const bool oldPartialTileLogging =
            data->IsPartialFeatureTileLoggingEnabled();
        data->SetPartialFeatureTileLoggingEnabled(false);
        const auto restorePartialTileLogging = qScopeGuard(
            [&]()
            {
                data->SetPartialFeatureTileLoggingEnabled(
                    oldPartialTileLogging);
            });

        for (int sliceID = 0;
             sliceID < sampleRowsBySlice.size();
             sliceID++)
        {
            const QVector<int> &sampleRows =
                sampleRowsBySlice.at(sliceID);
            if (sampleRows.isEmpty())
                continue;

            QByteArray excludedPixels(
                static_cast<qsizetype>(fwidth) * fheight,
                static_cast<char>(1));
            for (int sampleRow : sampleRows)
            {
                const LabelledPoint &point =
                    labels.at(sampleRow);
                excludedPixels[
                    static_cast<qsizetype>(point.y) * fwidth
                    + point.x] = 0;
            }

            const MLROISlice roi(
                fwidth,
                fheight,
                excludedPixels);
            if (!roi.isValid())
            {
                MLUpdateBlockingDialog::hideDialog();
                if (!noMessages)
                {
                    Message(
                        QString(
                            "Could not create training ROI for "
                            "slice %1")
                            .arg(sliceID + 1));
                }
                return false;
            }

            sampledSliceCount++;
            uniqueTrainingPixels += roi.activePixelCount();
            trainingTargetTiles += roi.targetTileCount();
            trainingPossibleTiles += roi.totalTileCount();
            trainingTileSize = roi.tileSize();

            MLUpdateBlockingDialog::updateHighLevelText(
                QString("Fetching features for training %1%")
                    .arg(
                        (processedSampleCount * 100)
                        / labels.count()));
            MLUpdateBlockingDialog::updateDetailText(
                QString(
                    "Slice %1: %2 samples in %3 feature tiles")
                    .arg(sliceID + 1)
                    .arg(sampleRows.count())
                    .arg(roi.targetTileCount()));

            QVector<cv::Mat> featureSlices;
            featureSlices.reserve(featureCount);
            for (int featureID : featureIDs)
            {
                featureSlices.append(
                    data->GetROISliceFeature(
                        sliceID,
                        featureID,
                        roi));
            }

            for (int sampleRow : sampleRows)
            {
                const LabelledPoint &point =
                    labels.at(sampleRow);
                for (int feature = 0;
                     feature < featureCount;
                     feature++)
                {
                    trainingDataMat.at<float>(
                        sampleRow,
                        feature) =
                        featureSlices.at(feature)
                            .at<float>(point.y, point.x);
                }
            }

            processedSampleCount += sampleRows.count();
        }
    }

    qDebug() << "ML training feature ROIs:"
             << labels.count() << "samples at"
             << uniqueTrainingPixels << "unique pixels"
             << "- target tiles:" << trainingTargetTiles
             << "of" << trainingPossibleTiles
             << "across" << sampledSliceCount << "slices"
             << "- tile size:" << trainingTileSize;

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
    CreateSingletonsIfNeeded();
    WriteAllData(CurrentFile);

    if (autoGen)
    {
        AutoSampleTrainAndGenerate();
    }
    else
    {
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
    cachedProbabilityExcludedPixels.clear();
    cachedProbabilitySliceID = -1;
    cachedProbabilitySliceValid = false;
    cachedProbabilityRestricted = false;
}

bool MLInterface::BuildSliceSampleMatrix(
    int sliceID,
    cv::Mat &samples,
    const MLROISlice *roi)
{
    if (data == nullptr)
        return false;

    QList<int> featureIndices = data->GetFeaturesInUse();
    const int numFeatures = featureIndices.count();
    const int numPixels = fwidth * fheight;

    if (numFeatures < 1 || numPixels < 1)
        return false;

    if (roi != nullptr
        && (!roi->isValid()
            || roi->width() != fwidth
            || roi->height() != fheight))
    {
        return false;
    }

    const int numSamples =
        roi != nullptr ? roi->activePixelCount() : numPixels;
    const QByteArray *excludedPixels =
        roi != nullptr ? &roi->excludedPixels() : nullptr;

    QVector<cv::Mat> featureSlices;
    featureSlices.reserve(numFeatures);

    for (int f = 0; f < numFeatures; ++f)
    {
        if (roi != nullptr)
        {
            featureSlices.append(
                data->GetROISliceFeature(
                    sliceID,
                    featureIndices[f],
                    *roi));
        }
        else
        {
            featureSlices.append(
                data->GetWholeSliceFeature(sliceID, featureIndices[f]));
        }
    }

    samples.create(numSamples, numFeatures, CV_32F);

    int sampleRow = 0;
    for (int y = 0; y < fheight; ++y)
    {
        for (int x = 0; x < fwidth; ++x)
        {
            const int pixel = y * fwidth + x;
            if (excludedPixels != nullptr && excludedPixels->at(pixel) != 0)
                continue;

            float *sample = samples.ptr<float>(sampleRow);

            for (int f = 0; f < numFeatures; ++f)
            {
                const float *srcRow = featureSlices[f].ptr<float>(y);
                sample[f] = srcRow[x];
            }
            sampleRow++;
        }
    }
    Q_ASSERT(sampleRow == samples.rows);

    return true;
}

bool MLInterface::EnsureSliceProbabilityCache(
    int sliceID,
    const MLROISlice *roi)
{
    if (data == nullptr)
        return false;

    if (!rf || !rf->IsValid())
        return false;

    const bool restricted = roi != nullptr;
    if (cachedProbabilitySliceValid
        && cachedProbabilitySliceID == sliceID
        && cachedProbabilityRestricted == restricted)
    {
        if (!restricted
            || cachedProbabilityExcludedPixels == roi->excludedPixels())
        {
            return true;
        }
    }

    cv::Mat samples;
    if (!BuildSliceSampleMatrix(sliceID, samples, roi))
        return false;

    if (restricted)
    {
        MLROISlice featureROI = *roi;
        const int xyHalo = data->GetRequiredXYHalo();
        featureROI.addHaloPixels(xyHalo);

        qDebug() << "ML prediction pixels:" << samples.rows
                 << "of" << roi->totalPixelCount()
                 << "- target tiles:" << roi->targetTileCount()
                 << "of" << roi->totalTileCount()
                 << "- tile size:" << roi->tileSize()
                 << "- feature halo:" << featureROI.haloTileCount()
                 << "tiles for"
                 << (xyHalo < 0 ? QStringLiteral("global support")
                               : QStringLiteral("%1 px").arg(xyHalo));
    }

    cv::Mat probabilities;
    if (samples.rows > 0)
    {
        MLUpdateBlockingDialog::updateDetailText(
            QStringLiteral("Running ML model on %1 of %2 pixels")
                .arg(samples.rows)
                .arg(fwidth * fheight));

        if (!rf->PredictProbabilities(samples, probabilities))
            return false;
    }
    else
    {
        probabilities.create(0, SegmentCount, CV_32F);
    }

    cachedSliceProbabilities = probabilities;
    cachedProbabilityExcludedPixels =
        restricted ? roi->excludedPixels() : QByteArray();
    cachedProbabilitySliceID = sliceID;
    cachedProbabilitySliceValid = true;
    cachedProbabilityRestricted = restricted;

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

bool MLInterface::FloodFillMask(
    int x,
    int y,
    int maskId,
    int seedRadius,
    int segmentationInfluencePercent,
    int grabCutIterations,
    bool fillHoles,
    QString *errorMessage)
{
    if (errorMessage != nullptr)
    {
        errorMessage->clear();
    }

    if (x < 0 || x >= fwidth || y < 0 || y >= fheight)
    {
        return false;
    }
    if (maskId < 0 || maskId > MaxUsedMask || maskId >= MasksSettings.count())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("The destination mask is invalid");
        }
        return false;
    }
    if (Masks.size() != fwidth * fheight
        || GA.count() != SegmentCount
        || ColArray.isNull())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("The current slice data is unavailable");
        }
        return false;
    }

    cv::Mat sourceProbability = cv::Mat::zeros(fheight, fwidth, CV_8UC1);
    QImage ctImage;
    int sourceSegment = -1;
    {
        QMutexLocker<QRecursiveMutex> locker(&mutex);
        const QVector<int> segmentAssignments = GetSegmentMap();
        sourceSegment = segmentAssignments.at(y * fwidth + x);
        if (sourceSegment < 0 || sourceSegment >= SegmentCount)
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = QStringLiteral("The clicked pixel is not assigned to an active segment");
            }
            return false;
        }

        for (int rowIndex = 0; rowIndex < fheight; rowIndex++)
        {
            const uchar *sourceRow = GA[sourceSegment]->constScanLine(rowIndex);
            memcpy(
                sourceProbability.ptr<uchar>(rowIndex),
                sourceRow,
                static_cast<size_t>(fwidth));
        }

        ctImage = ColArray.convertToFormat(QImage::Format_Grayscale8).scaled(
            fwidth,
            fheight,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation);
    }

    if (ctImage.isNull())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("The current CT image could not be prepared");
        }
        return false;
    }

    cv::Mat ctIntensity = cv::Mat::zeros(fheight, fwidth, CV_8UC1);
    for (int rowIndex = 0; rowIndex < fheight; rowIndex++)
    {
        memcpy(
            ctIntensity.ptr<uchar>(rowIndex),
            ctImage.constScanLine(rowIndex),
            static_cast<size_t>(fwidth));
    }

    const int influence = qBound(0, segmentationInfluencePercent, 100);
    cv::Mat weightedProbability = cv::Mat::zeros(fheight, fwidth, CV_8UC1);
    for (int rowIndex = 0; rowIndex < fheight; rowIndex++)
    {
        const uchar *sourceRow = sourceProbability.ptr<uchar>(rowIndex);
        uchar *weightedRow = weightedProbability.ptr<uchar>(rowIndex);
        for (int columnIndex = 0; columnIndex < fwidth; columnIndex++)
        {
            const int centredValue = static_cast<int>(sourceRow[columnIndex]) - 128;
            weightedRow[columnIndex] = static_cast<uchar>(
                qBound(0, 128 + qRound(centredValue * influence / 100.0), 255));
        }
    }

    cv::Mat smoothedProbability;
    cv::GaussianBlur(
        weightedProbability,
        smoothedProbability,
        cv::Size(),
        1.2,
        1.2,
        cv::BORDER_REPLICATE);

    std::vector<cv::Mat> grabCutChannels {
        ctIntensity,
        weightedProbability,
        smoothedProbability
    };
    cv::Mat grabCutImage;
    cv::merge(grabCutChannels, grabCutImage);

    cv::Mat grabCutMask(
        fheight,
        fwidth,
        CV_8UC1,
        cv::Scalar(cv::GC_PR_BGD));
    cv::circle(
        grabCutMask,
        cv::Point(x, y),
        qMax(2, seedRadius * 3),
        cv::Scalar(cv::GC_PR_FGD),
        cv::FILLED);
    for (int rowIndex = 0; rowIndex < fheight; rowIndex++)
    {
        const uchar *sourceRow = sourceProbability.ptr<uchar>(rowIndex);
        uchar *maskRow = grabCutMask.ptr<uchar>(rowIndex);
        for (int columnIndex = 0; columnIndex < fwidth; columnIndex++)
        {
            if (sourceRow[columnIndex] <= 25)
            {
                maskRow[columnIndex] = cv::GC_BGD;
            }
        }
    }

    grabCutMask.row(0).setTo(cv::GC_BGD);
    grabCutMask.row(fheight - 1).setTo(cv::GC_BGD);
    grabCutMask.col(0).setTo(cv::GC_BGD);
    grabCutMask.col(fwidth - 1).setTo(cv::GC_BGD);
    cv::circle(
        grabCutMask,
        cv::Point(x, y),
        qMax(1, seedRadius),
        cv::Scalar(cv::GC_FGD),
        cv::FILLED);

    cv::Mat backgroundModel;
    cv::Mat foregroundModel;
    try
    {
        cv::grabCut(
            grabCutImage,
            grabCutMask,
            cv::Rect(),
            backgroundModel,
            foregroundModel,
            qMax(1, grabCutIterations),
            cv::GC_INIT_WITH_MASK);
    }
    catch (const cv::Exception &exception)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("GrabCut failed: %1")
                                .arg(QString::fromUtf8(exception.what()));
        }
        return false;
    }

    cv::Mat foregroundPixels;
    cv::compare(grabCutMask, cv::GC_FGD, foregroundPixels, cv::CMP_EQ);
    cv::Mat probableForeground;
    cv::compare(grabCutMask, cv::GC_PR_FGD, probableForeground, cv::CMP_EQ);
    cv::bitwise_or(foregroundPixels, probableForeground, foregroundPixels);

    cv::Mat componentLabels;
    cv::connectedComponents(foregroundPixels, componentLabels, 8, CV_32S);
    const int componentLabel = componentLabels.at<int>(y, x);
    if (componentLabel == 0)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("GrabCut did not retain the clicked position");
        }
        return false;
    }

    cv::Mat selectedComponent;
    cv::compare(componentLabels, componentLabel, selectedComponent, cv::CMP_EQ);
    if (fillHoles)
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(
            selectedComponent.clone(),
            contours,
            cv::RETR_EXTERNAL,
            cv::CHAIN_APPROX_SIMPLE);
        selectedComponent.setTo(0);
        cv::drawContours(
            selectedComponent,
            contours,
            -1,
            cv::Scalar(255),
            cv::FILLED);
    }

    bool changed = false;
    {
        QMutexLocker<QRecursiveMutex> locker(&mutex);
        uchar *maskData = reinterpret_cast<uchar *>(Masks.data());
        for (int rowIndex = 0; rowIndex < fheight; rowIndex++)
        {
            const uchar *componentRow = selectedComponent.ptr<uchar>(rowIndex);
            const int maskRow = fheight - rowIndex - 1;
            for (int columnIndex = 0; columnIndex < fwidth; columnIndex++)
            {
                if (componentRow[columnIndex] == 0)
                {
                    continue;
                }

                const int maskPosition = maskRow * fwidth + columnIndex;
                const int oldMaskId = maskData[maskPosition];
                if (oldMaskId <= MaxUsedMask
                    && !MasksSettings[oldMaskId]->Lock
                    && oldMaskId != maskId)
                {
                    maskData[maskPosition] = static_cast<uchar>(maskId);
                    changed = true;
                }
            }
        }

        if (changed)
        {
            MasksDirty = true;
            MasksUndoDirty = true;
        }
    }

    return changed;
}
