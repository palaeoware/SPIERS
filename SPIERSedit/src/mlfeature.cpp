#include "mlfeature.h"
#include <QDebug>
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <QString>
#include "opencv2/imgproc.hpp"
#include "mlfeaturecontrast.h"
#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeatureintensity.h"
#include "mlfeaturegaussian.h"
#include "mlfeaturegradient.h"
#include "mlfeaturesquareintensity.h"
#include "mlfeaturevariance.h"
#include "mlfeaturelog.h"
#include "mlfeaturehessian.h"

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

    case MLFeature::FeatureType::Contrast:
        return new MLFeatureContrast(channel, is3D, arg1);

    case MLFeature::FeatureType::Gradient:
        return new MLFeatureGradient(channel, is3D, arg1);

    case MLFeature::FeatureType::Local_variance:
        return new MLFeatureVariance(channel, is3D, arg1);

    case MLFeature::FeatureType::Square:
        return new MLFeatureSquareIntensity(channel);

    case MLFeature::FeatureType::Laplacian_of_gaussian:
        return new MLFeatureLoG(channel, is3D, arg1);

    case MLFeature::FeatureType::Hessian:
        return new MLFeatureHessian(channel, is3D, arg1, arg2);

    case MLFeature::FeatureType::Structure_tensor:

    default:
        qDebug()<<"ERROR - not implemented in CreateNewFeature";
        return nullptr;
    }
}

//Default - works for one sigma-like argument
int MLFeature::GetMinMaxForArgs(int arg, bool max)
{
    if (arg==1 && !max) return 0;
    if (arg==1 && max) return 6;
    if (arg==2) return 0;
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

void MLFeature::CalcLocalMean2D(cv::Mat &out, const cv::Mat &in, int radiusLog2)
{
    int r = pow(2.0f,radiusLog2);

    for (int y = 0; y < fheight; ++y)
    {

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating 2D mean: %1%")
                    .arg((y*100)/fheight)
                );
        float* outRow = out.ptr<float>(y);

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
                const float *row = in.ptr<float>(yy);
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

//assumed slicesIn contains slice data in z order, but not necessarily the full volume
//centralSliceIndex is index of slicesIn for the central slice
void MLFeature::CalcZMean(cv::Mat &out,
                          const QVector<cv::Mat> &slicesIn,
                          int centralSliceIndex,
                          int radiusLog2)
{
    int r = pow(2.0f, radiusLog2);

    int z0 = centralSliceIndex - r;
    int z1 = centralSliceIndex + r;

    Q_ASSERT(z0 >= 0);
    Q_ASSERT(z1 < slicesIn.count());

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating mean Z %1%")
                    .arg((y * 100) / fheight)
                );

        float* outRow = out.ptr<float>(y);

        // cache row pointers for this y across slices
        QVector<const float*> rows;
        rows.reserve(z1 - z0 + 1);

        for (int z = z0; z <= z1; ++z)
            rows.append(slicesIn[z].ptr<float>(y));

        for (int x = 0; x < fwidth; ++x)
        {
            double sum = 0.0;
            int count = 0;

            for (int i = 0; i < rows.size(); ++i)
            {
                sum += rows[i][x];
                ++count;
            }

            outRow[x] = (count > 0) ? static_cast<float>(sum / count) : 0.0f;
        }
    }
}


void MLFeature::CalcLaplacian2D(cv::Mat &out, const cv::Mat &in, float scaleFactor)
{
    Q_ASSERT(in.type() == CV_32F);
    Q_ASSERT(in.cols == fwidth);
    Q_ASSERT(in.rows == fheight);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating Laplacian XY")
        );

    cv::Laplacian(in, out, CV_32F, 1, scaleFactor, 0.0, cv::BORDER_REPLICATE);
}


void MLFeature::CalcSecondDerivativeXX(cv::Mat &out, const cv::Mat &in, float scaleFactor)
{
    Q_ASSERT(in.type() == CV_32F);
    Q_ASSERT(in.cols == fwidth);
    Q_ASSERT(in.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating second derivative XX %1%")
                    .arg((y * 100) / fheight)
                );

        const float *inRow = in.ptr<float>(y);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            int xm1 = std::max(0, x - 1);
            int xp1 = std::min(fwidth - 1, x + 1);

            outRow[x] = scaleFactor * (inRow[xp1] - 2.0f * inRow[x] + inRow[xm1]);
        }
    }
}


void MLFeature::CalcSecondDerivativeYY(cv::Mat &out, const cv::Mat &in, float scaleFactor)
{
    Q_ASSERT(in.type() == CV_32F);
    Q_ASSERT(in.cols == fwidth);
    Q_ASSERT(in.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating second derivative YY %1%")
                    .arg((y * 100) / fheight)
                );

        int ym1 = std::max(0, y - 1);
        int yp1 = std::min(fheight - 1, y + 1);

        const float *rowM1 = in.ptr<float>(ym1);
        const float *row0  = in.ptr<float>(y);
        const float *rowP1 = in.ptr<float>(yp1);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
            outRow[x] = scaleFactor * (rowP1[x] - 2.0f * row0[x] + rowM1[x]);
    }
}


void MLFeature::CalcSecondDerivativeXY(cv::Mat &out, const cv::Mat &in, float scaleFactor)
{
    Q_ASSERT(in.type() == CV_32F);
    Q_ASSERT(in.cols == fwidth);
    Q_ASSERT(in.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating second derivative XY %1%")
                    .arg((y * 100) / fheight)
                );

        int ym1 = std::max(0, y - 1);
        int yp1 = std::min(fheight - 1, y + 1);

        const float *rowM1 = in.ptr<float>(ym1);
        const float *rowP1 = in.ptr<float>(yp1);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            int xm1 = std::max(0, x - 1);
            int xp1 = std::min(fwidth - 1, x + 1);

            float v =
                rowP1[xp1]
                - rowM1[xp1]
                - rowP1[xm1]
                + rowM1[xm1];

            outRow[x] = scaleFactor * 0.25f * v;
        }
    }
}


// assumed slicesIn contains slice data in z order
// centralSliceIndex is the index of the central slice within slicesIn
void MLFeature::CalcSecondDerivativeZZ(cv::Mat &out,
                                       const QVector<cv::Mat> &slicesIn,
                                       int centralSliceIndex,
                                       float scaleFactor)
{
    Q_ASSERT(!slicesIn.isEmpty());
    Q_ASSERT(centralSliceIndex >= 0);
    Q_ASSERT(centralSliceIndex < slicesIn.count());

    int prevIndex = std::max(0, centralSliceIndex - 1);
    int nextIndex = std::min((int)slicesIn.count() - 1, centralSliceIndex + 1);

    const cv::Mat &prev = slicesIn[prevIndex];
    const cv::Mat &cur  = slicesIn[centralSliceIndex];
    const cv::Mat &next = slicesIn[nextIndex];

    Q_ASSERT(prev.type() == CV_32F);
    Q_ASSERT(cur.type() == CV_32F);
    Q_ASSERT(next.type() == CV_32F);

    Q_ASSERT(prev.cols == fwidth && prev.rows == fheight);
    Q_ASSERT(cur.cols == fwidth && cur.rows == fheight);
    Q_ASSERT(next.cols == fwidth && next.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating second derivative ZZ %1%")
                    .arg((y * 100) / fheight)
                );

        const float *prevRow = prev.ptr<float>(y);
        const float *curRow  = cur.ptr<float>(y);
        const float *nextRow = next.ptr<float>(y);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
            outRow[x] = scaleFactor * (nextRow[x] - 2.0f * curRow[x] + prevRow[x]);
    }
}


void MLFeature::CalcSecondDerivativeXZ(cv::Mat &out,
                                       const QVector<cv::Mat> &slicesIn,
                                       int centralSliceIndex,
                                       float scaleFactor)
{
    Q_ASSERT(!slicesIn.isEmpty());
    Q_ASSERT(centralSliceIndex >= 0);
    Q_ASSERT(centralSliceIndex < slicesIn.count());

    int prevIndex = std::max(0, centralSliceIndex - 1);
    int nextIndex = std::min((int)slicesIn.count() - 1, (int)centralSliceIndex + 1);

    const cv::Mat &prev = slicesIn[prevIndex];
    const cv::Mat &next = slicesIn[nextIndex];

    Q_ASSERT(prev.type() == CV_32F);
    Q_ASSERT(next.type() == CV_32F);
    Q_ASSERT(prev.cols == fwidth && prev.rows == fheight);
    Q_ASSERT(next.cols == fwidth && next.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating second derivative XZ %1%")
                    .arg((y * 100) / fheight)
                );

        const float *prevRow = prev.ptr<float>(y);
        const float *nextRow = next.ptr<float>(y);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            int xm1 = std::max(0, x - 1);
            int xp1 = std::min(fwidth - 1, x + 1);

            float v =
                nextRow[xp1]
                - prevRow[xp1]
                - nextRow[xm1]
                + prevRow[xm1];

            outRow[x] = scaleFactor * 0.25f * v;
        }
    }
}


void MLFeature::CalcSecondDerivativeYZ(cv::Mat &out,
                                       const QVector<cv::Mat> &slicesIn,
                                       int centralSliceIndex,
                                       float scaleFactor)
{
    Q_ASSERT(!slicesIn.isEmpty());
    Q_ASSERT(centralSliceIndex >= 0);
    Q_ASSERT(centralSliceIndex < slicesIn.count());

    int prevIndex = std::max(0, centralSliceIndex - 1);
    int nextIndex = std::min((int)slicesIn.count() - 1, centralSliceIndex + 1);

    const cv::Mat &prev = slicesIn[prevIndex];
    const cv::Mat &next = slicesIn[nextIndex];

    Q_ASSERT(prev.type() == CV_32F);
    Q_ASSERT(next.type() == CV_32F);
    Q_ASSERT(prev.cols == fwidth && prev.rows == fheight);
    Q_ASSERT(next.cols == fwidth && next.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating second derivative YZ %1%")
                    .arg((y * 100) / fheight)
                );

        int ym1 = std::max(0, y - 1);
        int yp1 = std::min(fheight - 1, y + 1);

        const float *prevRowM1 = prev.ptr<float>(ym1);
        const float *prevRowP1 = prev.ptr<float>(yp1);
        const float *nextRowM1 = next.ptr<float>(ym1);
        const float *nextRowP1 = next.ptr<float>(yp1);

        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            float v =
                nextRowP1[x]
                - prevRowP1[x]
                - nextRowM1[x]
                + prevRowM1[x];

            outRow[x] = scaleFactor * 0.25f * v;
        }
    }
}
