#ifndef MLINTERFACE_H
#define MLINTERFACE_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include "mlcachedaccess.h"
#include "mainwindowimpl.h"

class MLFeatureUIManager;
class MLAddFeature;
class LabelledPoint;
class MLParallelForest;

class MLInterface
{
public:
    MLInterface();
    static bool TestML();
    static bool enabled;
    void SampleAndTrain();
    void CalculateFeatureData();
    void GetProbabilitiesAllSegments(int x, int y, int z, int *segBuffer);
    void Generate(QListWidget *SliceSelectorList);
    void UIActivateSelectedFeatures(bool activate);
    void UIDeleteSelectedFeatures();
    void UIAddFeature();
    void SetSamplePercent(int v);
    void SetMinSampleCount(int v);
    void SetTreeCount(int v);
    void SetTreeDepth(int v);
    int GetSamplePercent();
    int GetMinSampleCount();
    int GetTreeCount();
    int GetTreeDepth();

    void Initialise(MainWindowImpl *mw,QLabel *statusLabel);
    void RemoveAllCacheFiles(bool override);
    QByteArray DumpFeaturesToByteArray();
    void RetrieveFeaturesFromByteArray(QByteArray &byteArray);
    void SaveFeaturesToFile();
    void LoadFeaturesFromFile();
    void ResetRFAndSample();
    void ResetCachedData();
    void AutoSampleTrainAndGenerate();
    void ResizeCache();
private:
    bool dataComputed;
    int currentSlice;
    std::unique_ptr<MLParallelForest> rf;
    MLCachedAccess *data;
    void CreateSingletonsIfNeeded();
    void ComputeSliceProbabilitiesFromVotes(int sliceID);
    MLFeatureUIManager *uiManager;
    MLAddFeature *addFeatureDialog;
    int samplePercent, minSampleCount, treeCount, treeDepth;
    void UpdateStatusLabel();

    QLabel *lblStatus;
    MainWindowImpl *mainWin;
    QString DescribeSample();
    QVector<LabelledPoint> labels;
    bool Sample(bool incremental, bool noMessages);
    bool Train(bool noMessages);
    void DoImportances();


    //Probabity cache system for recalc brush
    cv::Mat cachedSliceProbabilities;
    int cachedProbabilitySliceID = -1;
    bool cachedProbabilitySliceValid = false;

    void InvalidateProbabilityCache();
    bool BuildSliceSampleMatrix(int sliceID, cv::Mat &samples);
    bool EnsureSliceProbabilityCache(int sliceID);
};

#endif // MLINTERFACE_H
