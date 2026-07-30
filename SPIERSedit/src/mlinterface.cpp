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
#include "mlenvelopemaskgenerator.h"
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
#include <QSpinBox>
#include <QTemporaryDir>
#include <QThread>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <opencv2/imgproc.hpp>

#include <cstring>
#include <vector>

namespace
{

class MLEnvelopePreviewView : public QGraphicsView
{
public:
    explicit MLEnvelopePreviewView(QWidget *parent = nullptr)
        : QGraphicsView(parent)
        , previewScene(this)
    {
        setScene(&previewScene);
        previewItem = previewScene.addPixmap(QPixmap());
        setAlignment(Qt::AlignCenter);
        setDragMode(QGraphicsView::ScrollHandDrag);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setResizeAnchor(QGraphicsView::AnchorViewCenter);
        setMinimumSize(480, 360);
    }

    void setImage(const QImage &image)
    {
        previewItem->setPixmap(QPixmap::fromImage(image));
        previewScene.setSceneRect(previewItem->boundingRect());
        fitImage();
    }

    void fitImage()
    {
        if (previewItem->pixmap().isNull())
        {
            return;
        }

        fitMode = true;
        fitInView(previewItem, Qt::KeepAspectRatio);
    }

    void zoomBy(double factor)
    {
        if (previewItem->pixmap().isNull())
        {
            return;
        }

        fitMode = false;
        const double proposedScale = transform().m11() * factor;
        if (proposedScale >= 0.02 && proposedScale <= 100.0)
        {
            scale(factor, factor);
        }
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QGraphicsView::resizeEvent(event);
        if (fitMode)
        {
            fitImage();
        }
    }

    void wheelEvent(QWheelEvent *event) override
    {
        if (event->angleDelta().y() == 0)
        {
            QGraphicsView::wheelEvent(event);
            return;
        }

        zoomBy(event->angleDelta().y() > 0 ? 1.25 : 0.8);
        event->accept();
    }

private:
    QGraphicsScene previewScene;
    QGraphicsPixmapItem *previewItem = nullptr;
    bool fitMode = true;
};

}

bool MLInterface::enabled;



MLInterface::MLInterface()
{
    data = nullptr;
    uiManager = nullptr;
    addFeatureDialog = nullptr;
    m_envelopeClosingRadius = 5;
    m_envelopeExpansionRadius = 1;
    m_envelopeSensitivity = 80;
    m_envelopeSmoothingRadius = 2;
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

void MLInterface::RemoveDistanceToMaskCacheFiles()
{
    QDir directory(MLFileIO::GetWorkingPath());
    const QStringList files = directory.entryList(
        {
            QStringLiteral("ml_*_md1*.bin"),
            QStringLiteral("ml_*_md1*.png"),
            QStringLiteral("ml_*_md2*.bin"),
            QStringLiteral("ml_*_md2*.png")
        },
        QDir::Files);
    for (const QString &file : files)
    {
        directory.remove(file);
    }
}

void MLInterface::MaskDataChanged()
{
    if (data == nullptr)
    {
        return;
    }

    bool hasDistanceFeature = false;
    for (int featureIndex = 0; featureIndex < data->GetFeatureCount(); featureIndex++)
    {
        if (data->GetFeature(featureIndex)->GetType() == MLFeature::FeatureType::Distance_to_mask)
        {
            hasDistanceFeature = true;
            break;
        }
    }
    if (!hasDistanceFeature)
    {
        return;
    }

    RemoveDistanceToMaskCacheFiles();
    data->Reset();
    ResetRFAndSample();
}

int MLInterface::RetargetMaskFeatures(const QVector<int> &maskMap, const QList<int> &deletedMaskIds)
{
    if (data == nullptr)
    {
        return 0;
    }

    int retargetedCount = 0;
    bool hasDistanceFeature = false;
    for (int featureIndex = 0; featureIndex < data->GetFeatureCount(); featureIndex++)
    {
        MLFeature *feature = data->GetFeature(featureIndex);
        if (feature->GetType() != MLFeature::FeatureType::Distance_to_mask)
        {
            continue;
        }

        hasDistanceFeature = true;
        for (int deletedMaskId : deletedMaskIds)
        {
            if (feature->ReferencesMask(deletedMaskId))
            {
                retargetedCount++;
                break;
            }
        }
        feature->RemapMasks(maskMap);
    }

    if (hasDistanceFeature)
    {
        RemoveDistanceToMaskCacheFiles();
        data->Reset();
        ResetRFAndSample();
        uiManager->Rebuild();
    }
    return retargetedCount;
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
        featureSlices.append(data->GetWholeSliceFeature(sliceID, featureIndices[f]));

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
        qDebug() << "ML prediction pixels:" << samples.rows
                 << "of" << roi->totalPixelCount()
                 << "- target tiles:" << roi->targetTileCount()
                 << "of" << roi->totalTileCount()
                 << "- tile size:" << roi->tileSize();
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

    if (changed)
    {
        MaskDataChanged();
    }
    return changed;
}

void MLInterface::MaskFromSegment()
{
    QList <QTreeWidgetItem *> selectedMasks = mainWin->MasksTreeWidget->selectedItems();
    QList <QTreeWidgetItem *> selectedSegments = mainWin->SegmentsTreeWidget->selectedItems();

    if (selectedMasks.count()!=1 || selectedSegments.count()!=1)
    {
        QMessageBox::warning(mainWin, "Error", "You need exactly one mask and one segment selected to perform this action");
        return;
    }

    int maskId=-1;
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) == selectedMasks[0])
        {
            maskId = i;
        }
    }
    if (maskId == -1)
    {
        QMessageBox::warning(mainWin, "Error", "Internal error - could not find mask ID");
        return;
    }

    int segId=-1;
    for (int i = 0;  i < SegmentCount; i++)
    {
        if ((Segments[i]->widgetitem) == selectedSegments[0])
        {
            segId = i;
        }
    }
    if (segId == -1)
    {
        QMessageBox::warning(mainWin, "Error", "Internal error - could not find segment ID");
        return;
    }

    QDialog dialog(mainWin);
    dialog.setWindowTitle(QStringLiteral("Generate Envelope Mask from Segment"));

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *messageLabel = new QLabel(
        QStringLiteral("This will generate a filled envelope mask from the selected segment on the selected slices. "
                       "Unselected neighbouring slices may be read to condition the boundary, but will not be changed."),
        &dialog);
    messageLabel->setWordWrap(true);
    layout->addWidget(messageLabel);

    QFormLayout *formLayout = new QFormLayout();

    QSpinBox *smoothingRadiusSpinBox = new QSpinBox(&dialog);
    smoothingRadiusSpinBox->setRange(0, 20);
    smoothingRadiusSpinBox->setSuffix(QStringLiteral(" px"));
    smoothingRadiusSpinBox->setValue(m_envelopeSmoothingRadius);
    formLayout->addRow(QStringLiteral("Smoothing radius"), smoothingRadiusSpinBox);

    QSpinBox *closingRadiusSpinBox = new QSpinBox(&dialog);
    closingRadiusSpinBox->setRange(0, 50);
    closingRadiusSpinBox->setSuffix(QStringLiteral(" px"));
    closingRadiusSpinBox->setValue(m_envelopeClosingRadius);
    formLayout->addRow(QStringLiteral("Gap-closing radius"), closingRadiusSpinBox);

    QSpinBox *sensitivitySpinBox = new QSpinBox(&dialog);
    sensitivitySpinBox->setRange(1, 99);
    sensitivitySpinBox->setSuffix(QStringLiteral("%"));
    sensitivitySpinBox->setValue(m_envelopeSensitivity);
    formLayout->addRow(QStringLiteral("Sensitivity"), sensitivitySpinBox);

    QSpinBox *expansionRadiusSpinBox = new QSpinBox(&dialog);
    expansionRadiusSpinBox->setRange(0, 50);
    expansionRadiusSpinBox->setSuffix(QStringLiteral(" px"));
    expansionRadiusSpinBox->setValue(m_envelopeExpansionRadius);
    formLayout->addRow(QStringLiteral("Final expansion"), expansionRadiusSpinBox);

    layout->addLayout(formLayout);

    QComboBox *previewModeComboBox = new QComboBox(&dialog);
    previewModeComboBox->addItem(QStringLiteral("Segment evidence"));
    previewModeComboBox->addItem(QStringLiteral("Conditioned boundary"));
    previewModeComboBox->addItem(QStringLiteral("Filled envelope"));
    previewModeComboBox->setCurrentIndex(2);
    layout->addWidget(previewModeComboBox);

    MLEnvelopePreviewView *previewView = new MLEnvelopePreviewView(&dialog);
    layout->addWidget(previewView);

    QHBoxLayout *previewToolLayout = new QHBoxLayout();

    QToolButton *zoomOutButton = new QToolButton(&dialog);
    zoomOutButton->setText(QStringLiteral("-"));
    zoomOutButton->setToolTip(QStringLiteral("Zoom out"));
    previewToolLayout->addWidget(zoomOutButton);

    QToolButton *zoomInButton = new QToolButton(&dialog);
    zoomInButton->setText(QStringLiteral("+"));
    zoomInButton->setToolTip(QStringLiteral("Zoom in"));
    previewToolLayout->addWidget(zoomInButton);

    QPushButton *fitPreviewButton = new QPushButton(QStringLiteral("Fit"), &dialog);
    fitPreviewButton->setToolTip(QStringLiteral("Fit preview to window"));
    previewToolLayout->addWidget(fitPreviewButton);
    previewToolLayout->addStretch();

    QPushButton *previewButton = new QPushButton(QStringLiteral("Preview Current Slice"), &dialog);
    previewToolLayout->addWidget(previewButton);
    layout->addLayout(previewToolLayout);

    QVector<QImage> previewImages(3);
    const auto displayPreview = [previewView, previewModeComboBox, &previewImages]()
    {
        const int previewIndex = previewModeComboBox->currentIndex();
        if (previewIndex < 0 || previewIndex >= previewImages.count() || previewImages[previewIndex].isNull())
        {
            return;
        }

        previewView->setImage(previewImages[previewIndex]);
    };

    QObject::connect(zoomOutButton,
                     &QToolButton::clicked,
                     &dialog,
                     [previewView]()
                     {
                         previewView->zoomBy(0.8);
                     });
    QObject::connect(zoomInButton,
                     &QToolButton::clicked,
                     &dialog,
                     [previewView]()
                     {
                         previewView->zoomBy(1.25);
                     });
    QObject::connect(fitPreviewButton,
                     &QPushButton::clicked,
                     &dialog,
                     [previewView]()
                     {
                         previewView->fitImage();
                     });

    QObject::connect(previewModeComboBox,
                     QOverload<int>::of(&QComboBox::currentIndexChanged),
                     &dialog,
                     [displayPreview](int)
                     {
                         displayPreview();
                     });

    QObject::connect(previewButton,
                     &QPushButton::clicked,
                     &dialog,
                     [this,
                      segId,
                      smoothingRadiusSpinBox,
                      closingRadiusSpinBox,
                      sensitivitySpinBox,
                      expansionRadiusSpinBox,
                      &previewImages,
                      displayPreview]()
                     {
                         WriteAllData(CurrentFile);

                         MLEnvelopeMaskParameters parameters;
                         parameters.smoothingRadius = smoothingRadiusSpinBox->value();
                         parameters.closingRadius = closingRadiusSpinBox->value();
                         parameters.sensitivity = sensitivitySpinBox->value();
                         parameters.expansionRadius = expansionRadiusSpinBox->value();

                         MLUpdateBlockingDialog::showDialog(mainWin, QStringLiteral("Generating envelope preview"), QString());
                         QString errorMessage;
                         bool success = false;
                         {
                             MLEnvelopeMaskGenerator generator(data, segId, parameters);
                             success = generator.generatePreview(
                                 CurrentFile,
                                 previewImages[0],
                                 previewImages[1],
                                 previewImages[2]);
                             errorMessage = generator.errorMessage();
                         }
                         MLUpdateBlockingDialog::hideDialog();

                         if (success)
                         {
                             displayPreview();
                         }
                         else if (!errorMessage.isEmpty() && errorMessage != QStringLiteral("Envelope generation was cancelled"))
                         {
                             QMessageBox::warning(mainWin, QStringLiteral("Envelope Preview"), errorMessage);
                         }
                     });

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    m_envelopeSmoothingRadius = smoothingRadiusSpinBox->value();
    m_envelopeClosingRadius = closingRadiusSpinBox->value();
    m_envelopeSensitivity = sensitivitySpinBox->value();
    m_envelopeExpansionRadius = expansionRadiusSpinBox->value();

    QList<int> selectedSliceIds;
    for (int i = 0; i < Files.count(); i++)
    {
        if (mainWin->SliceSelectorList->item(i)->isSelected())
        {
            selectedSliceIds.append(i);
        }
    }

    if (selectedSliceIds.isEmpty())
    {
        QMessageBox::warning(mainWin, QStringLiteral("Envelope Mask"), QStringLiteral("No slices are selected"));
        return;
    }

    MLEnvelopeMaskParameters parameters;
    parameters.smoothingRadius = m_envelopeSmoothingRadius;
    parameters.closingRadius = m_envelopeClosingRadius;
    parameters.sensitivity = m_envelopeSensitivity;
    parameters.expansionRadius = m_envelopeExpansionRadius;

    WriteAllData(CurrentFile);
    QTemporaryDir stagingDirectory;
    if (!stagingDirectory.isValid())
    {
        QMessageBox::warning(mainWin, QStringLiteral("Envelope Mask"), QStringLiteral("Could not create a temporary staging directory"));
        return;
    }

    MLUpdateBlockingDialog::showDialog(mainWin, QStringLiteral("Generating envelope masks"), QString());
    QString generationError;
    bool generationSucceeded = false;
    {
        MLEnvelopeMaskGenerator generator(data, segId, parameters);
        generationSucceeded = generator.stageSlices(selectedSliceIds, stagingDirectory.path());
        generationError = generator.errorMessage();
    }
    MLUpdateBlockingDialog::hideDialog();

    if (!generationSucceeded)
    {
        if (!generationError.isEmpty() && generationError != QStringLiteral("Envelope generation was cancelled"))
        {
            QMessageBox::warning(mainWin, QStringLiteral("Envelope Mask"), generationError);
        }
        LoadAllData(CurrentFile);
        ShowImage(mainWin->graphicsView);
        return;
    }

    MLUpdateBlockingDialog::showDialog(mainWin, QStringLiteral("Writing envelope masks"), QString());
    const uchar maskValue = static_cast<uchar>(maskId);
    for (int index = 0; index < selectedSliceIds.count(); index++)
    {
        const int sliceId = selectedSliceIds[index];
        MLUpdateBlockingDialog::updateDetailText(
            QStringLiteral("Writing selected slice %1 of %2")
                .arg(index + 1)
                .arg(selectedSliceIds.count()));

        QFile stagedFile(MLEnvelopeMaskGenerator::stagedSliceFileName(stagingDirectory.path(), sliceId));
        if (!stagedFile.open(QIODevice::ReadOnly))
        {
            MLUpdateBlockingDialog::hideDialog();
            QMessageBox::warning(mainWin, QStringLiteral("Envelope Mask"), QStringLiteral("Could not read staged envelope mask data"));
            LoadAllData(CurrentFile);
            ShowImage(mainWin->graphicsView);
            return;
        }

        const QByteArray stagedMask = stagedFile.readAll();
        if (stagedMask.size() != fwidth * fheight)
        {
            MLUpdateBlockingDialog::hideDialog();
            QMessageBox::warning(mainWin, QStringLiteral("Envelope Mask"), QStringLiteral("Staged envelope mask data has the wrong dimensions"));
            LoadAllData(CurrentFile);
            ShowImage(mainWin->graphicsView);
            return;
        }

        LoadMasks(sliceId);
        for (int position = 0; position < Masks.size(); position++)
        {
            if (static_cast<uchar>(Masks[position]) == maskValue)
            {
                Masks[position] = 0;
            }
        }

        for (int y = 0; y < fheight; y++)
        {
            const int maskY = fheight - y - 1;
            for (int x = 0; x < fwidth; x++)
            {
                if (static_cast<uchar>(stagedMask[y * fwidth + x]) != 0)
                {
                    Masks[maskY * fwidth + x] = static_cast<char>(maskValue);
                }
            }
        }
        SaveMasks(sliceId);
    }
    MLUpdateBlockingDialog::hideDialog();
    MaskDataChanged();
    LoadAllData(CurrentFile);
    ShowImage(mainWin->graphicsView);

}
