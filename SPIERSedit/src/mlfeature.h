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
        Local_mean,
        Contrast,
        Gradient,
        Laplacian_of_gaussian,
        Local_variance,
        Structure_tensor,
        Hessian
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
protected:
    FeatureType _type;
    Channel _channel;
    bool _isSelected = false;
    bool _is3D;
    int _arg1, _arg2;

    virtual QString GetTypeCodeForFile()=0;
    QString GetChannelCodeForFile();
    virtual QString GetArgsForFile()=0;
    virtual QString Get3DForFile()=0;
    void CalcFeatureDifferenceOfFeatures(cv::Mat &mat, int sliceID,
                                         MLCachedAccess *data, int featureIndex1, int featureIndex2);
};
