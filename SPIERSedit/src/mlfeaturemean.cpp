#include "mlfeaturemean.h"
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include "mlfeatureintensity.h"

MLFeatureMean::MLFeatureMean(Channel channel, bool is3D, int arg1)
    : MLFeature(FeatureType::Local_mean, channel, is3D, arg1, 0)
{

}

void MLFeatureMean::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    if (_is3D)
        CalcFeatureMean3D(mat, sliceID, data);
    else
        CalcFeatureMean2D(mat, sliceID, data);
}

QList<MLFeature *> MLFeatureMean::GetDependencies()
{
    QList<MLFeature *> deps;
    if (_is3D)
        deps.append(new MLFeatureMean(_channel, false, _arg1));
    else
        deps.append(new MLFeatureIntensity(_channel));

    return deps;
}

QString MLFeatureMean::GetPrettyName()
{
    return "local mean";
}

QString MLFeatureMean::GetPrettyArgs()
{
    return QString("size=%1").arg(_arg1);
}

QString MLFeatureMean::GetPretty3D()
{
    if (_is3D)
        return "3D";
    else
        return "2D";
}

QString MLFeatureMean::GetTypeCodeForFile()
{
    return "mea";
}

QString MLFeatureMean::GetArgsForFile()
{
    return QString("@%1").arg(_arg1);
}

QString MLFeatureMean::Get3DForFile()
{
    if (_is3D)
        return "3";
    else
        return "2";
}


void MLFeatureMean::CalcFeatureMean3D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    int r = _arg1;
    int mea2index = data->GetIndexForFeature(MLFeature::FeatureType::Local_mean, _channel, false, _arg1, 0);
    qDebug()<<"mea2index"<<mea2index;
    //Get all the slices I need
    int z0 = std::max(0, sliceID - r);
    int z1 = std::min(FileCount - 1, sliceID + r);

    QVector<cv::Mat> slices;
    for (int i=z0; i<=z1; i++)
    {
        slices.append(data->GetWholeSliceFeature(i, mea2index));
    }


    for (int y = 0; y < fheight; ++y)
    {

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1 for slice %2: %3%")
                    .arg(GetPrettyFullName())
                    .arg(sliceID)
                    .arg((y*100)/fheight)
                );
        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double sum = 0.0;
            int count = 0;

            for (int z = z0; z <= z1; ++z)
            {
                sum += slices[z-z0].at<float>(y,x);
                ++count;
            }

            outRow[x] = (count > 0) ? static_cast<float>(sum / count) : 0.0f;
        }
    }
}


void MLFeatureMean::CalcFeatureMean2D(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{

    int r = _arg1;
    int idx = data->GetIndexForFeature(MLFeature::FeatureType::Intensity, _channel, false, 0, 0);
    cv::Mat slice = data->GetWholeSliceFeature(sliceID, idx);

    for (int y = 0; y < fheight; ++y)
    {

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating %1 for slice %2: %3%")
                    .arg(GetPrettyFullName())
                    .arg(sliceID)
                    .arg((y*100)/fheight)
                );
        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double sum = 0.0;
            int count = 0;

            int y0 = std::max(0, y - r);
            int y1 = std::min(fheight - 1, y + r);

            int x0 = std::max(0, x - r);
            int x1 = std::min(fwidth - 1, x + r);


            for (int yy = y0; yy <= y1; ++yy)
            {
                float *row = slice.ptr<float>(yy);
                for (int xx = x0; xx <= x1; ++xx)
                {
                    sum += row[xx];
                    ++count;
                }
            }


            outRow[x] = (count > 0) ? static_cast<float>(sum / count) : 0.0f;
        }
    }
}

