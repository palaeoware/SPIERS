#include "mlfeatureintensity.h"
#include "mlupdateblockingdialog.h"
#include "mlcachedaccess.h"

MLFeatureIntensity::MLFeatureIntensity(Channel channel)
    : MLFeature(FeatureType::Intensity, channel, false, 0, 0)
{

}

void MLFeatureIntensity::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_channel==MLFeature::Channel::Intensity)
        CalcFeatureIntensity(mat, sliceID, data);
    else
        CalcFeatureColor(mat, sliceID, data);
}

void MLFeatureIntensity::CalcFeatureIntensity(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    int xSize = data->GetXSize();
    int ySize = data->GetYSize();

    for (int y=0; y<ySize; y++)
    {
        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating intensity feature for slice %1: %2%").arg(sliceID)
                    .arg((y*100)/ySize)
                );
        for (int x=0; x<xSize; x++)
        {
            mat.at<float>(y,x) = data->GetIntensityAsFloat(x,y,sliceID);
        }
    }
}

void MLFeatureIntensity::CalcFeatureColor(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    int xSize = data->GetXSize();
    int ySize = data->GetYSize();

    switch(_channel)
    {

    case MLFeature::Channel::Intensity:
        qDebug()<<"ERROR";
        break;
    case MLFeature::Channel::Red:
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating red intensity feature for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                mat.at<float>(y,x) = data->GetRGBFloat(x,y,sliceID).redF();
            }
        }
        break;
    case MLFeature::Channel::Green:
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating green intensity feature for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                mat.at<float>(y,x) = data->GetRGBFloat(x,y,sliceID).greenF();
            }
        }
        break;
    case MLFeature::Channel::Blue:
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating blue intensity feature for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                mat.at<float>(y,x) = data->GetRGBFloat(x,y,sliceID).blueF();
            }
        }
        break;
    case MLFeature::Channel::R_G:
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating red-green for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                QColor c = data->GetRGBFloat(x,y,sliceID);
                mat.at<float>(y,x) = c.redF() - c.greenF();
            }
        }
        break;
    case MLFeature::Channel::G_B:
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating green-blue for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                QColor c = data->GetRGBFloat(x,y,sliceID);
                mat.at<float>(y,x) = c.greenF() - c.blueF();
            }
        }
        break;
    }


}


QString MLFeatureIntensity::GetPrettyName()
{
    return "Intensity";
}

QString MLFeatureIntensity::GetPrettyArgs()
{
    return "";
}

QString MLFeatureIntensity::GetPretty3D()
{
    return "";
}


QString MLFeatureIntensity::Get3DForFile()
{
    return "";
}

QString MLFeatureIntensity::GetTypeCodeForFile()
{
    return "int";
}

QString MLFeatureIntensity::GetArgsForFile()
{
    return "";
}

QList<MLFeature *> MLFeatureIntensity::GetDependencies()
{
    return QList<MLFeature *>();  //no dependecies
}
