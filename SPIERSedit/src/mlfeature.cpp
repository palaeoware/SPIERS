/**
 * @file
 * Source: Mlfeature
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
#include "mlfeature.h"
#include <QDebug>
#include "globals.h"
#include "mlupdateblockingdialog.h"
#include <QString>
#include <cmath>
#include "opencv2/imgproc.hpp"
#include "mlfeaturecontrast.h"
#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeaturedistancetomask.h"
#include "mlfeatureintensity.h"
#include "mlfeaturegaussian.h"
#include "mlfeaturegradient.h"
#include "mlfeaturesquareintensity.h"
#include "mlfeaturevariance.h"
#include "mlfeaturelog.h"
#include "mlfeaturehessian.h"
#include "mlfeaturegradientcomponent.h"
#include "mlfeaturetensorcomponentlocal.h"
#include "mlfeaturetensorcomponentwide.h"
#include "mlfeaturetensortracelocal.h"
#include "mlfeaturetensortracewide.h"
#include "mlfeaturetensordeterminantlocal.h"
#include "mlfeaturetensordeterminantwide.h"
#include "mlfeaturetensorcoherencelocal.h"
#include "mlfeaturetensorcoherencewide.h"
#include "mlroislice.h"

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

bool MLFeature::CalculateFeatureROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    const MLROISlice &)
{
    CalculateFeature(mat, sliceID, data);
    return true;
}

int MLFeature::GetXYSupportRadius() const
{
    switch (_type)
    {
    case FeatureType::Gaussian:
        return _is3D
                   ? 0
                   : static_cast<int>(
                         std::ceil(3.0 * std::pow(2.0, _arg1)));

    case FeatureType::Gradient_magnitude:
    case FeatureType::Laplacian_of_gaussian:
    case FeatureType::Hessian:
        return 1;

    case FeatureType::Local_variance:
    case FeatureType::Tensor_component_local:
        return static_cast<int>(std::pow(2.0, _arg1));

    case FeatureType::Tensor_component_wide:
        return static_cast<int>(std::pow(2.0, _arg1 + 1));

    case FeatureType::Gradient_component:
        // X and Y derivatives use adjacent pixels. The Z derivative does not.
        return _arg2 == 2 ? 0 : 1;

    case FeatureType::Distance_to_mask:
        // An exact distance transform has unbounded spatial support.
        return -1;

    default:
        return 0;
    }
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

    case MLFeature::FeatureType::Gradient_magnitude:
        return new MLFeatureGradient(channel, is3D, arg1);

    case MLFeature::FeatureType::Local_variance:
        return new MLFeatureVariance(channel, is3D, arg1);

    case MLFeature::FeatureType::Square:
        return new MLFeatureSquareIntensity(channel);

    case MLFeature::FeatureType::Laplacian_of_gaussian:
        return new MLFeatureLoG(channel, is3D, arg1);

    case MLFeature::FeatureType::Hessian:
        return new MLFeatureHessian(channel, is3D, arg1, arg2);

    case MLFeature::FeatureType::Gradient_component:
        return new MLFeatureGradientComponent(channel, arg1, arg2);

    case MLFeature::FeatureType::Distance_to_mask:
        return new MLFeatureDistanceToMask(arg1);

    case MLFeature::FeatureType::Tensor_component_local:
        return new MLFeatureTensorComponentLocal(channel, arg1, arg2);

    case MLFeature::FeatureType::Tensor_component_wide:
        return new MLFeatureTensorComponentWide(channel, arg1, arg2);

    case MLFeature::FeatureType::Tensor_trace_local:
        return new MLFeatureTensorTraceLocal(channel, is3D, arg1);
    case MLFeature::FeatureType::Tensor_trace_wide:
        return new MLFeatureTensorTraceWide(channel, is3D, arg1);


    case MLFeature::FeatureType::Tensor_coherence_local:
        return new MLFeatureTensorCoherenceLocal(channel, arg1);
    case MLFeature::FeatureType::Tensor_coherence_wide:
        return new MLFeatureTensorCoherenceWide(channel, arg1);

    case MLFeature::FeatureType::Tensor_determinant_local:
        return new MLFeatureTensorDeterminantLocal(channel, is3D, arg1);
    case MLFeature::FeatureType::Tensor_determinant_wide:
        return new MLFeatureTensorDeterminantWide(channel, is3D, arg1);

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
    return 0;
}

bool MLFeature::ReferencesMask(int) const
{
    return false;
}

void MLFeature::RemapMasks(const QVector<int> &)
{
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

void MLFeature::CalcFeatureDifferenceOfFeaturesROI(
    cv::Mat &mat,
    int sliceID,
    MLCachedAccess *data,
    int featureIndex1,
    int featureIndex2,
    const MLROISlice &roi)
{
    const cv::Mat data1 =
        data->GetROISliceFeature(sliceID, featureIndex1, roi);
    const cv::Mat data2 =
        data->GetROISliceFeature(sliceID, featureIndex2, roi);

    for (int tileY = 0; tileY < roi.tileRows(); tileY++)
    {
        for (int tileX = 0; tileX < roi.tileColumns(); tileX++)
        {
            if (roi.tileState(tileX, tileY)
                == MLROISlice::TileState::Inactive)
            {
                continue;
            }

            const QRect tile = roi.tileRect(tileX, tileY);
            for (int y = tile.top(); y <= tile.bottom(); y++)
            {
                const float *row1 = data1.ptr<float>(y);
                const float *row2 = data2.ptr<float>(y);
                float *outputRow = mat.ptr<float>(y);
                for (int x = tile.left(); x <= tile.right(); x++)
                    outputRow[x] = row1[x] - row2[x];
            }
        }
    }
}

void MLFeature::CalcLocalMean2D(cv::Mat &out, const cv::Mat &in, int radiusLog2)
{
    int r = pow(2.0f,radiusLog2);

    for (int y = 0; y < fheight; ++y)
    {

        if (y%150==0)
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


void MLFeature::CalcGaussian1DKernel(QVector<float> &kernel, float sigma)
{
    Q_ASSERT(sigma > 0.0f);

    int r = static_cast<int>(std::ceil(3.0f * sigma));
    int ksize = 2 * r + 1;

    kernel.resize(ksize);

    double sum = 0.0;
    for (int k = -r; k <= r; ++k)
    {
        double v = std::exp(-(k * k) / (2.0 * sigma * sigma));
        kernel[k + r] = static_cast<float>(v);
        sum += v;
    }

    if (sum > 0.0)
    {
        for (int i = 0; i < kernel.size(); ++i)
            kernel[i] = static_cast<float>(kernel[i] / sum);
    }
}

void MLFeature::CalcGaussian2D(cv::Mat &out, const cv::Mat &in, float sigma)
{
    Q_ASSERT(in.type() == CV_32F);
    Q_ASSERT(in.cols == fwidth);
    Q_ASSERT(in.rows == fheight);
    Q_ASSERT(sigma > 0.0f);

    QVector<float> kernel;
    CalcGaussian1DKernel(kernel, sigma);

    int r = (kernel.size() - 1) / 2;

    cv::Mat temp(fheight, fwidth, CV_32F);

    // horizontal pass
    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating Gaussian 2D: %1%")
                    .arg((y * 50) / fheight)
                );

        const float *inRow = in.ptr<float>(y);
        float *tempRow = temp.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int xx = std::max(0, std::min(fwidth - 1, x + k));
                acc += kernel[k + r] * inRow[xx];
            }

            tempRow[x] = static_cast<float>(acc);
        }
    }

    out.create(fheight, fwidth, CV_32F);

    // vertical pass
    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating Gaussian 2D: %1%")
                    .arg(50 + (y * 50) / fheight)
                );

        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int yy = std::max(0, std::min(fheight - 1, y + k));
                const float *tempRow = temp.ptr<float>(yy);
                acc += kernel[k + r] * tempRow[x];
            }

            outRow[x] = static_cast<float>(acc);
        }
    }
}

void MLFeature::CalcFirstDerivativeX(cv::Mat &out, const cv::Mat &in, float scaleFactor)
{
    Q_ASSERT(in.type() == CV_32F);
    Q_ASSERT(in.cols == fwidth);
    Q_ASSERT(in.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating first derivative X %1%")
                    .arg((y * 100) / fheight)
                );

        const float *inRow = in.ptr<float>(y);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            int xm1 = std::max(0, x - 1);
            int xp1 = std::min(fwidth - 1, x + 1);

            outRow[x] = scaleFactor * 0.5f * (inRow[xp1] - inRow[xm1]);
        }
    }
}

void MLFeature::CalcFirstDerivativeY(cv::Mat &out, const cv::Mat &in, float scaleFactor)
{
    Q_ASSERT(in.type() == CV_32F);
    Q_ASSERT(in.cols == fwidth);
    Q_ASSERT(in.rows == fheight);

    out.create(fheight, fwidth, CV_32F);

    for (int y = 0; y < fheight; ++y)
    {
        if (y % 50 == 0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating first derivative Y %1%")
                    .arg((y * 100) / fheight)
                );

        int ym1 = std::max(0, y - 1);
        int yp1 = std::min(fheight - 1, y + 1);

        const float *rowM1 = in.ptr<float>(ym1);
        const float *rowP1 = in.ptr<float>(yp1);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
            outRow[x] = scaleFactor * 0.5f * (rowP1[x] - rowM1[x]);
    }
}

void MLFeature::CalcFirstDerivativeZ(cv::Mat &out,
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
                QString("Calculating first derivative Z %1%")
                    .arg((y * 100) / fheight)
                );

        const float *prevRow = prev.ptr<float>(y);
        const float *nextRow = next.ptr<float>(y);
        float *outRow = out.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
            outRow[x] = scaleFactor * 0.5f * (nextRow[x] - prevRow[x]);
    }
}

void MLFeature::CalcFeatureProductOfFeatures(cv::Mat &mat, int sliceID,
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

    cv::Mat data1 = data->GetWholeSliceFeature(sliceID, featureIndex1);
    cv::Mat data2 = data->GetWholeSliceFeature(sliceID, featureIndex2);

    cv::multiply(data1, data2, mat);
}

void MLFeature::CalcMatrixProduct(cv::Mat &out, const cv::Mat &in1, const cv::Mat &in2)
{
    Q_ASSERT(in1.type() == CV_32F);
    Q_ASSERT(in2.type() == CV_32F);
    Q_ASSERT(in1.cols == fwidth && in1.rows == fheight);
    Q_ASSERT(in2.cols == fwidth && in2.rows == fheight);

    out.create(fheight, fwidth, CV_32F);
    cv::multiply(in1, in2, out);
}
