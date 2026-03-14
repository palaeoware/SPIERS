#ifndef OPENCVINTERFACE_H
#define OPENCVINTERFACE_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include "mlcachedaccess.h"
#include "mainwindowimpl.h"

class MLFeatureUIManager;
class MLAddFeature;
class LabelledPoint;

class OpenCVInterface
{
public:
    OpenCVInterface();
    static bool TestOpenCV();
    static bool enabled;
    void SampleAndTrain();
    uchar GetProbability(int x, int y, int z, int segment);
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
    void SetCacheMemSizeGb(int v);

    int GetCacheMemSizeGb();
    void Initialise(MainWindowImpl *mw,QLabel *statusLabel);
    void RemoveAllCacheFiles(bool override);
    QByteArray DumpFeaturesToByteArray();
    void RetrieveFeaturesFromByteArray(QByteArray &byteArray);
    void SaveFeaturesToFile();
    void LoadFeaturesFromFile();
    void ResetRFAndSample();
    void ResetCachedData();
private:
    bool dataComputed;
    int currentSlice;
    cv::Ptr<cv::ml::RTrees> rf;
    MLCachedAccess *data;
    void CreateSingletonsIfNeeded();
    void TestSetUpFeatures();
    void MakeML(int fnum);
    void ComputeSliceProbabilitiesFromVotes(int sliceID);
    MLFeatureUIManager *uiManager;
    MLAddFeature *addFeatureDialog;
    int samplePercent, minSampleCount, treeCount, treeDepth;
    void UpdateStatusLabel();

    QLabel *lblStatus;
    MainWindowImpl *mainWin;
    QString DescribeSample();
    QList<LabelledPoint> labels;
};

#endif // OPENCVINTERFACE_H
