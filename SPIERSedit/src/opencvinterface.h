#ifndef OPENCVINTERFACE_H
#define OPENCVINTERFACE_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include "mlcachedaccess.h"
#include "mainwindowimpl.h"

class MLFeatureUIManager;
class MLAddFeature;

class OpenCVInterface
{
public:
    OpenCVInterface();
    static bool TestOpenCV();
    static bool enabled;
    cv::Mat QImageToCvMatGrayFloat(QImage &img);
    void Train(int slice, MainWindowImpl *mw);
    uchar GetProbability(int x, int y, int z, int segment);
    void CalculateFeatureData(MainWindowImpl *mw);
    void GetProbabilitiesAllSegments(int x, int y, int z, int *segBuffer);
    void Generate(QListWidget *SliceSelectorList);
    void UIActivateSelectedFeatures(bool activate);
    void UIDeleteSelectedFeatures();
    void UIAddFeature();
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
};

#endif // OPENCVINTERFACE_H
