#include "mlfeature.h"
#include <QDebug>
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <QString>

#include "mlfeaturecontrast.h"
#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeatureintensity.h"
#include "mlfeaturegaussian.h"
#include "mlfeaturemean.h"

MLFeature::MLFeature(FeatureType type, Channel channel, bool is3D, int arg1, int arg2)
{
    _type = type;
    _channel = channel;
    _is3D = is3D;
    _arg1 = arg1;
    _arg2 = arg2;
    _isSelected = false;
    _importancePercent=-1; //not calced
}


bool MLFeature::IsSelected()
{
    return _isSelected;
}

void MLFeature::SetSelected(bool selected)
{
    _isSelected = selected;
}

MLFeature::FeatureType MLFeature::GetType()
{
    return _type;
}

MLFeature::Channel MLFeature::GetChannel()
{
    return _channel;
}

int MLFeature::GetArg1()
{
    return _arg1;
}

int MLFeature::GetArg2()
{
    return _arg2;
}

bool MLFeature::is3D()
{
    return _is3D;
}


QString MLFeature::GetEncodedNameForFile()
{
    return QString("%1%2@%3%4")
        .arg(GetTypeCodeForFile())
        .arg(Get3DForFile())
        .arg(GetChannelCodeForFile())
        .arg(GetArgsForFile());
}

QString MLFeature::GetPrettyFullName()
{
    return QString("%1 %2 (%3) %4")
        .arg(GetPrettyName())
        .arg(GetPretty3D())
        .arg(GetPrettyChannel())
        .arg(GetPrettyArgs());
}

//returns true if they are the same
bool MLFeature::Compare(FeatureType type, Channel channel, bool is3D, int arg1, int arg2)
{
    if (type!=_type) return false;
    if (channel!=_channel) return false;
    if (is3D!=_is3D) return false;
    if (arg1!=_arg1) return false;
    if (arg2!=_arg2) return false;
    return true;
}

bool MLFeature::Compare(MLFeature *other)
{
    return Compare(other->GetType(), other->GetChannel(), other->is3D(), other->GetArg1(), other->GetArg2());
}

QString MLFeature::Dump()
{
    return QString("type:%1 channel:%2  3d:%3  arg1:%4")
        .arg((int)_type)
        .arg((int)_channel)
        .arg(_is3D)
        .arg(_arg1);
}

void MLFeature::SetImportance(int percent)
{
    _importancePercent = percent;
}


int MLFeature::GetImportance()
{
    return _importancePercent;
}

MLFeature *MLFeature::CreateFromData(FeatureType type, Channel channel, bool is3D, int arg1, int arg2)
{
    switch (type)
    {
    case MLFeature::FeatureType::Gaussian:
        return new MLFeatureGaussian(channel, is3D, arg1);

    case MLFeature::FeatureType::Difference_of_gaussians:
        return new MLFeatureDifferenceOfGaussians(channel, is3D, arg1, arg2);

    case MLFeature::FeatureType::Intensity:
        return new MLFeatureIntensity(channel);

    case MLFeature::FeatureType::Local_mean:
        return new MLFeatureMean(channel, is3D, arg1);

    case MLFeature::FeatureType::Contrast:
        return new MLFeatureContrast(channel, is3D, arg1);

    case MLFeature::FeatureType::Gradient:
    case MLFeature::FeatureType::Laplacian_of_gaussian:
    case MLFeature::FeatureType::Local_variance:
    case MLFeature::FeatureType::Structure_tensor:
    case MLFeature::FeatureType::Hessian:
    default:
        qDebug()<<"ERROR - not implemented in CreateNewFeature";
        return nullptr;
    }
}

QString MLFeature::GetChannelCodeForFile()
{
    switch (_channel)
    {
    case Channel::Intensity:
        return "t";
    case Channel::Red:
        return "r";
    case Channel::Green:
        return "g";
    case Channel::Blue:
        return "b";
    case Channel::G_B:
        return "gb";
    case Channel::R_G:
        return "rg";
    default:
        qDebug()<<"Missing case in GetChannelCodeForFile";
        return "";
    }
}

QString MLFeature::GetPrettyChannel()
{
    return GetPrettyChannel(_channel);
}

QString MLFeature::GetPrettyChannel(Channel ch)
{
    switch (ch)
    {
    case Channel::Intensity:
        return "grey";
    case Channel::Red:
        return "r";
    case Channel::Green:
        return "g";
    case Channel::Blue:
        return "b";
    case Channel::G_B:
        return "g-b";
    case Channel::R_G:
        return "r-g";
    default:
        qDebug()<<"Missing case in GetPrettyChannel";
        return "";
    }
}

void MLFeature::CalcFeatureDifferenceOfFeatures(cv::Mat &mat, int sliceID,
                                                          MLCachedAccess *data, int featureIndex1, int featureIndex2)
{

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating %1")
            .arg(GetPrettyFullName())
        );

    cv::Mat data1, data2;

    data1 = data->GetWholeSliceFeature(sliceID, featureIndex1);
    data2 = data->GetWholeSliceFeature(sliceID, featureIndex2);

    cv::subtract(data1, data2, mat);
}

