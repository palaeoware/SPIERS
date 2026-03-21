#pragma once

#include <QObject>
#include "opencv2/core.hpp"

class MLCachedAccess;
class MLFeature
{
public:
    virtual ~MLFeature() = default;
    virtual void CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data) = 0; //makes class abstract
    enum class FeatureType
    {
        Gaussian,
        Difference_of_gaussians,
        Intensity,
        Contrast,
        Gradient_magnitude,
        Laplacian_of_gaussian,
        Local_variance,
        Tensor_component_local,
        Tensor_component_wide,
        Tensor_determinant_local,
        Tensor_determinant_wide,
        Tensor_coherence_local,
        Tensor_coherence_wide,
        Tensor_trace_local,
        Tensor_trace_wide,

        Hessian,
        Square,
        Gradient_component
    };

    enum class Channel
    {
        Intensity,
        Red,
        Green,
        Blue,
        R_G,
        G_B
    };

    MLFeature(FeatureType type, Channel channel, bool is3D, int arg1=0, int arg2=0);
    virtual QList<MLFeature *>GetDependencies() =0;
    virtual QString GetPrettyName()=0;
    virtual QString GetPrettyArgs()=0;
    QString GetPrettyChannel();
    static QString GetPrettyChannel(Channel i);
    virtual QString GetPretty3D()=0;
    virtual int GetDependencyDepth()=0;
    virtual QString GetArg1SetupString(int v)=0;
    virtual QString GetArg2SetupString(int v)=0;
    bool IsSelected();
    void SetSelected(bool selected);
    FeatureType GetType();
    Channel GetChannel();
    int GetArg1();
    int GetArg2();
    bool is3D();
    QString GetEncodedNameForFile();

    QString GetPrettyFullName();
    bool Compare(FeatureType type, Channel channel, bool is3D, int arg1, int arg2);
    bool Compare(MLFeature *other);
    QString Dump();
    void SetImportance(int percent);
    int GetImportance();
    static MLFeature *CreateFromData(FeatureType type, Channel channel, bool is3D, int arg1, int arg2);
    virtual int GetMinMaxForArgs(int arg, bool max);
protected:
    FeatureType _type;
    Channel _channel;
    bool _isSelected = false;
    bool _is3D;
    int _arg1, _arg2;
    int _importancePercent;

    virtual QString GetTypeCodeForFile()=0;
    QString GetChannelCodeForFile();
    virtual QString GetArgsForFile()=0;
    virtual QString Get3DForFile()=0;

    void CalcFeatureDifferenceOfFeatures(cv::Mat &mat, int sliceID,
                                         MLCachedAccess *data, int featureIndex1, int featureIndex2);
    void CalcLocalMean2D(cv::Mat &out, const cv::Mat &in, int radiusLog2);
    void CalcZMean(cv::Mat &out, const QVector<cv::Mat> &slicesIn, int centralSliceIndex, int radiusLog2);

    void CalcLaplacian2D(cv::Mat &out, const cv::Mat &in, float scaleFactor = 1.0f);

    void CalcSecondDerivativeXX(cv::Mat &out, const cv::Mat &in, float scaleFactor = 1.0f);
    void CalcSecondDerivativeYY(cv::Mat &out, const cv::Mat &in, float scaleFactor = 1.0f);
    void CalcSecondDerivativeXY(cv::Mat &out, const cv::Mat &in, float scaleFactor = 1.0f);

    void CalcSecondDerivativeZZ(cv::Mat &out,
                                const QVector<cv::Mat> &slicesIn,
                                int centralSliceIndex,
                                float scaleFactor = 1.0f);

    void CalcSecondDerivativeXZ(cv::Mat &out,
                                const QVector<cv::Mat> &slicesIn,
                                int centralSliceIndex,
                                float scaleFactor = 1.0f);

    void CalcSecondDerivativeYZ(cv::Mat &out,
                                const QVector<cv::Mat> &slicesIn,
                                int centralSliceIndex,
                                float scaleFactor = 1.0f);

    void CalcGaussian1DKernel(QVector<float> &kernel, float sigma);
    void CalcGaussian2D(cv::Mat &out, const cv::Mat &in, float sigma);
    void CalcFirstDerivativeX(cv::Mat &out, const cv::Mat &in, float scaleFactor = 1.0f);
    void CalcFirstDerivativeY(cv::Mat &out, const cv::Mat &in, float scaleFactor = 1.0f);
    void CalcFirstDerivativeZ(cv::Mat &out, const QVector<cv::Mat> &slicesIn, int centralSliceIndex, float scaleFactor = 1.0f);
    void CalcFeatureProductOfFeatures(cv::Mat &mat, int sliceID,
                                      MLCachedAccess *data, int featureIndex1, int featureIndex2);

    void CalcMatrixProduct(cv::Mat &out, const cv::Mat &in1, const cv::Mat &in2);

};
