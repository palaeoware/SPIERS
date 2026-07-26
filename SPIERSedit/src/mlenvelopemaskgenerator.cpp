/**
 * @file
 * Source: Mlenvelopemaskgenerator
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
#include "mlenvelopemaskgenerator.h"

#include "fileio.h"
#include "globals.h"
#include "mlcachedaccess.h"
#include "mlfeature.h"
#include "mlfileio.h"
#include "mlupdateblockingdialog.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QVector>

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <stdexcept>

namespace
{

std::atomic_bool envelopeCacheSessionActive = false;
std::atomic_int envelopeRunCounter = 0;

class EnvelopeOperationCancelled : public std::runtime_error
{
public:
    EnvelopeOperationCancelled()
        : std::runtime_error("Envelope generation cancelled")
    {
    }
};

void throwIfCancelled()
{
    if (MLUpdateBlockingDialog::isCancelled())
    {
        throw EnvelopeOperationCancelled();
    }
}

int dependencyIndex(MLCachedAccess *data, MLFeature::FeatureType type, int runToken)
{
    const int index = data->GetIndexForFeature(type, MLFeature::Channel::Intensity, false, runToken, 0);
    if (index < 0)
    {
        throw std::runtime_error("Missing internal envelope feature dependency");
    }
    return index;
}

int physicalZRadius(int xyRadius)
{
    if (xyRadius <= 0 || PixPerMM <= 0.0 || SlicePerMM <= 0.0)
    {
        return 0;
    }

    const double xySpacingMillimetres = PixPerMM * static_cast<double>(ColMonoScale);
    return qMax(0, qRound(static_cast<double>(xyRadius) * xySpacingMillimetres * SlicePerMM));
}

cv::Mat ellipticalKernel(int radius)
{
    if (radius <= 0)
    {
        return cv::Mat();
    }

    const int size = radius * 2 + 1;
    return cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(size, size));
}

QImage binaryImage(const cv::Mat &source)
{
    cv::Mat binary;
    cv::compare(source, 0.5f, binary, cv::CMP_GT);
    return QImage(binary.data,
                  binary.cols,
                  binary.rows,
                  static_cast<qsizetype>(binary.step),
                  QImage::Format_Grayscale8).copy();
}

class EnvelopeFeatureBase : public MLFeature
{
public:
    EnvelopeFeatureBase(FeatureType type, int runToken, const QString &cacheCode, const QString &prettyName, int dependencyDepth)
        : MLFeature(type, Channel::Intensity, false, runToken, 0)
        , cacheCode(cacheCode)
        , prettyName(prettyName)
        , dependencyDepth(dependencyDepth)
    {
    }

    QList<MLFeature *> GetDependencies() override
    {
        return QList<MLFeature *>();
    }

    QString GetPrettyName() override
    {
        return prettyName;
    }

    QString GetPrettyArgs() override
    {
        return QString();
    }

    QString GetPretty3D() override
    {
        return QString();
    }

    int GetDependencyDepth() override
    {
        return dependencyDepth;
    }

    QString GetArg1SetupString(int) override
    {
        return QString();
    }

    QString GetArg2SetupString(int) override
    {
        return QString();
    }

protected:
    QString GetTypeCodeForFile() override
    {
        return cacheCode;
    }

    QString GetArgsForFile() override
    {
        return QString();
    }

    QString Get3DForFile() override
    {
        return QString();
    }

private:
    QString cacheCode;
    QString prettyName;
    int dependencyDepth;
};

class EnvelopeSegmentMembershipFeature : public EnvelopeFeatureBase
{
public:
    EnvelopeSegmentMembershipFeature(int runToken, const QString &cachePrefix, int segmentId)
        : EnvelopeFeatureBase(FeatureType::Envelope_segment_membership, runToken, cachePrefix + QStringLiteral("m"), QStringLiteral("envelope membership"), 0)
        , segmentId(segmentId)
    {
        for (int i = 0; i < SegmentCount; i++)
        {
            activeSegments.append(Segments[i]->Activated);
        }
    }

    void CalculateFeature(cv::Mat &mat, int sliceId, MLCachedAccess *) override
    {
        mat.setTo(0.0f);
        cv::Mat bestValue(fheight, fwidth, CV_8UC1, cv::Scalar(128));

        for (int currentSegment = 0; currentSegment < SegmentCount; currentSegment++)
        {
            throwIfCancelled();
            if (!activeSegments[currentSegment])
            {
                continue;
            }

            QImage segmentImage;
            if (!SimpleLoadGreyDataForFile(Files.at(sliceId), currentSegment, &segmentImage))
            {
                continue;
            }

            if (segmentImage.width() != fwidth || segmentImage.height() != fheight)
            {
                throw std::runtime_error(QStringLiteral("Segment image dimensions do not match the working dataset dimensions").toStdString());
            }

            if (segmentImage.format() != QImage::Format_Grayscale8 && segmentImage.format() != QImage::Format_Indexed8)
            {
                segmentImage = segmentImage.convertToFormat(QImage::Format_Grayscale8);
            }

            for (int y = 0; y < fheight; y++)
            {
                const uchar *sourceRow = segmentImage.constScanLine(y);
                uchar *bestRow = bestValue.ptr<uchar>(y);
                float *outputRow = mat.ptr<float>(y);

                for (int x = 0; x < fwidth; x++)
                {
                    const uchar value = sourceRow[x];
                    if (value >= bestRow[x])
                    {
                        bestRow[x] = value;
                        outputRow[x] = currentSegment == segmentId ? 1.0f : 0.0f;
                    }
                }
            }
        }
    }

private:
    QVector<bool> activeSegments;
    int segmentId;
};

class EnvelopeXYSmoothingFeature : public EnvelopeFeatureBase
{
public:
    EnvelopeXYSmoothingFeature(int runToken, const QString &cachePrefix, int smoothingRadius)
        : EnvelopeFeatureBase(FeatureType::Envelope_xy_smoothing, runToken, cachePrefix + QStringLiteral("xys"), QStringLiteral("envelope XY smoothing"), 1)
        , smoothingRadius(smoothingRadius)
    {
    }

    void CalculateFeature(cv::Mat &mat, int sliceId, MLCachedAccess *data) override
    {
        const int sourceIndex = dependencyIndex(data, FeatureType::Envelope_segment_membership, _arg1);
        const cv::Mat source = data->GetWholeSliceFeature(sliceId, sourceIndex);

        if (smoothingRadius <= 0)
        {
            source.copyTo(mat);
            return;
        }

        cv::GaussianBlur(source,
                         mat,
                         cv::Size(),
                         static_cast<double>(smoothingRadius),
                         static_cast<double>(smoothingRadius),
                         cv::BORDER_REPLICATE);
    }

private:
    int smoothingRadius;
};

class EnvelopeZSmoothingFeature : public EnvelopeFeatureBase
{
public:
    EnvelopeZSmoothingFeature(int runToken, const QString &cachePrefix, int smoothingRadius)
        : EnvelopeFeatureBase(FeatureType::Envelope_z_smoothing, runToken, cachePrefix + QStringLiteral("zs"), QStringLiteral("envelope Z smoothing"), 2)
        , smoothingRadius(smoothingRadius)
    {
    }

    void CalculateFeature(cv::Mat &mat, int sliceId, MLCachedAccess *data) override
    {
        const int sourceIndex = dependencyIndex(data, FeatureType::Envelope_xy_smoothing, _arg1);
        const double sigma = static_cast<double>(physicalZRadius(smoothingRadius));
        if (sigma < 0.5)
        {
            data->GetWholeSliceFeature(sliceId, sourceIndex).copyTo(mat);
            return;
        }

        const int radius = static_cast<int>(std::ceil(3.0 * sigma));
        const int firstSlice = qMax(0, sliceId - radius);
        const int lastSlice = qMin(FileCount - 1, sliceId + radius);
        double weightSum = 0.0;
        mat.setTo(0.0f);

        for (int z = firstSlice; z <= lastSlice; z++)
        {
            throwIfCancelled();
            const int offset = z - sliceId;
            const double weight = std::exp(-static_cast<double>(offset * offset) / (2.0 * sigma * sigma));
            const cv::Mat source = data->GetWholeSliceFeature(z, sourceIndex);
            cv::scaleAdd(source, weight, mat, mat);
            weightSum += weight;
        }

        if (weightSum > 0.0)
        {
            mat /= weightSum;
        }
    }

private:
    int smoothingRadius;
};

class EnvelopeXYDilationFeature : public EnvelopeFeatureBase
{
public:
    EnvelopeXYDilationFeature(int runToken, const QString &cachePrefix, int closingRadius, int sensitivity)
        : EnvelopeFeatureBase(FeatureType::Envelope_xy_dilation, runToken, cachePrefix + QStringLiteral("xyd"), QStringLiteral("envelope XY dilation"), 3)
        , closingRadius(closingRadius)
        , sensitivity(sensitivity)
    {
    }

    void CalculateFeature(cv::Mat &mat, int sliceId, MLCachedAccess *data) override
    {
        const int sourceIndex = dependencyIndex(data, FeatureType::Envelope_z_smoothing, _arg1);
        const cv::Mat source = data->GetWholeSliceFeature(sliceId, sourceIndex);
        const float threshold = static_cast<float>(100 - sensitivity) / 100.0f;
        cv::Mat binary;
        cv::compare(source, threshold, binary, cv::CMP_GT);
        binary.convertTo(binary, CV_32F, 1.0 / 255.0);

        if (closingRadius <= 0)
        {
            binary.copyTo(mat);
            return;
        }

        cv::dilate(binary,
                   mat,
                   ellipticalKernel(closingRadius),
                   cv::Point(-1, -1),
                   1,
                   cv::BORDER_CONSTANT,
                   cv::Scalar(0));
    }

private:
    int closingRadius;
    int sensitivity;
};

class EnvelopeZDilationFeature : public EnvelopeFeatureBase
{
public:
    EnvelopeZDilationFeature(int runToken, const QString &cachePrefix, int closingRadius)
        : EnvelopeFeatureBase(FeatureType::Envelope_z_dilation, runToken, cachePrefix + QStringLiteral("zd"), QStringLiteral("envelope Z dilation"), 4)
        , closingRadius(closingRadius)
    {
    }

    void CalculateFeature(cv::Mat &mat, int sliceId, MLCachedAccess *data) override
    {
        const int sourceIndex = dependencyIndex(data, FeatureType::Envelope_xy_dilation, _arg1);
        const int radius = physicalZRadius(closingRadius);
        const int firstSlice = qMax(0, sliceId - radius);
        const int lastSlice = qMin(FileCount - 1, sliceId + radius);
        data->GetWholeSliceFeature(firstSlice, sourceIndex).copyTo(mat);

        for (int z = firstSlice + 1; z <= lastSlice; z++)
        {
            throwIfCancelled();
            cv::max(mat, data->GetWholeSliceFeature(z, sourceIndex), mat);
        }
    }

private:
    int closingRadius;
};

class EnvelopeXYErosionFeature : public EnvelopeFeatureBase
{
public:
    EnvelopeXYErosionFeature(int runToken, const QString &cachePrefix, int closingRadius)
        : EnvelopeFeatureBase(FeatureType::Envelope_xy_erosion, runToken, cachePrefix + QStringLiteral("xye"), QStringLiteral("envelope XY erosion"), 5)
        , closingRadius(closingRadius)
    {
    }

    void CalculateFeature(cv::Mat &mat, int sliceId, MLCachedAccess *data) override
    {
        const int sourceIndex = dependencyIndex(data, FeatureType::Envelope_z_dilation, _arg1);
        const cv::Mat source = data->GetWholeSliceFeature(sliceId, sourceIndex);

        if (closingRadius <= 0)
        {
            source.copyTo(mat);
            return;
        }

        cv::erode(source,
                  mat,
                  ellipticalKernel(closingRadius),
                  cv::Point(-1, -1),
                  1,
                  cv::BORDER_CONSTANT,
                  cv::Scalar(0));
    }

private:
    int closingRadius;
};

class EnvelopeZErosionFeature : public EnvelopeFeatureBase
{
public:
    EnvelopeZErosionFeature(int runToken, const QString &cachePrefix, int closingRadius)
        : EnvelopeFeatureBase(FeatureType::Envelope_z_erosion, runToken, cachePrefix + QStringLiteral("ze"), QStringLiteral("envelope Z erosion"), 6)
        , closingRadius(closingRadius)
    {
    }

    void CalculateFeature(cv::Mat &mat, int sliceId, MLCachedAccess *data) override
    {
        const int sourceIndex = dependencyIndex(data, FeatureType::Envelope_xy_erosion, _arg1);
        const int radius = physicalZRadius(closingRadius);
        const int firstSlice = qMax(0, sliceId - radius);
        const int lastSlice = qMin(FileCount - 1, sliceId + radius);
        data->GetWholeSliceFeature(firstSlice, sourceIndex).copyTo(mat);

        for (int z = firstSlice + 1; z <= lastSlice; z++)
        {
            throwIfCancelled();
            cv::min(mat, data->GetWholeSliceFeature(z, sourceIndex), mat);
        }
    }

private:
    int closingRadius;
};

}

MLEnvelopeMaskGenerator::MLEnvelopeMaskGenerator(MLCachedAccess *normalCache,
                                                 int segmentId,
                                                 const MLEnvelopeMaskParameters &parameters)
    : parameters(parameters)
{
    /**
     *
     * BIG CONCURRENCY WARNING
     *
     * The temporary processing cache and the ordinary RF cache both calculate
     * their capacity from CacheMemMLGb. They do not share a live allocator.
     * This is safe only because envelope generation is modal and first empties
     * the ordinary cache's resident matrices. Do not move this operation to a
     * worker/background thread. Concurrent operation requires a shared global
     * cache-budget manager before it can be considered.
     *
     **/

    bool expected = false;
    if (!envelopeCacheSessionActive.compare_exchange_strong(expected, true))
    {
        setError(QStringLiteral("Another envelope cache session is already active"));
        return;
    }
    ownsExclusiveCacheBudget = true;

    if (normalCache == nullptr)
    {
        setError(QStringLiteral("The ML cache is not available"));
        return;
    }

    normalCache->ReleaseCacheMemoryForExclusiveOperation();

    runToken = static_cast<int>((QDateTime::currentMSecsSinceEpoch() + envelopeRunCounter.fetch_add(1)) & 0x7fffffff);
    cachePrefix = QStringLiteral("env%1").arg(runToken);

    try
    {
        processingCache = std::make_unique<MLCachedAccess>(FileCount, false, fwidth, fheight, ColMonoScale, ZDownsample);

        QList<MLFeature *> features;
        features.append(new EnvelopeSegmentMembershipFeature(runToken, cachePrefix, segmentId));
        features.append(new EnvelopeXYSmoothingFeature(runToken, cachePrefix, parameters.smoothingRadius));
        features.append(new EnvelopeZSmoothingFeature(runToken, cachePrefix, parameters.smoothingRadius));
        features.append(new EnvelopeXYDilationFeature(runToken, cachePrefix, parameters.closingRadius, parameters.sensitivity));
        features.append(new EnvelopeZDilationFeature(runToken, cachePrefix, parameters.closingRadius));
        features.append(new EnvelopeXYErosionFeature(runToken, cachePrefix, parameters.closingRadius));
        features.append(new EnvelopeZErosionFeature(runToken, cachePrefix, parameters.closingRadius));
        processingCache->SetFeatures(features);

        membershipFeatureIndex = dependencyIndex(processingCache.get(), MLFeature::FeatureType::Envelope_segment_membership, runToken);
        closedFeatureIndex = dependencyIndex(processingCache.get(), MLFeature::FeatureType::Envelope_z_erosion, runToken);
    }
    catch (const std::exception &exception)
    {
        processingCache.reset();
        cleanCacheFiles();
        setError(QString::fromLocal8Bit(exception.what()));
    }
}

MLEnvelopeMaskGenerator::~MLEnvelopeMaskGenerator()
{
    processingCache.reset();
    cleanCacheFiles();

    if (ownsExclusiveCacheBudget)
    {
        envelopeCacheSessionActive.store(false);
    }
}

bool MLEnvelopeMaskGenerator::generatePreview(int sliceId, QImage &evidenceImage, QImage &boundaryImage, QImage &filledImage)
{
    if (!processingCache)
    {
        return false;
    }

    try
    {
        const cv::Mat evidence = processingCache->GetWholeSliceFeature(sliceId, membershipFeatureIndex);
        const cv::Mat boundary = processingCache->GetWholeSliceFeature(sliceId, closedFeatureIndex);
        cv::Mat filled;
        if (!createFilledSlice(sliceId, filled))
        {
            return false;
        }

        evidenceImage = binaryImage(evidence);
        boundaryImage = binaryImage(boundary);
        filledImage = QImage(filled.data,
                             filled.cols,
                             filled.rows,
                             static_cast<qsizetype>(filled.step),
                             QImage::Format_Grayscale8).copy();
        return true;
    }
    catch (const EnvelopeOperationCancelled &)
    {
        setError(QStringLiteral("Envelope generation was cancelled"));
    }
    catch (const std::exception &exception)
    {
        setError(QString::fromLocal8Bit(exception.what()));
    }
    return false;
}

bool MLEnvelopeMaskGenerator::stageSlices(const QList<int> &sliceIds, const QString &directoryPath)
{
    if (!processingCache)
    {
        return false;
    }

    try
    {
        for (int index = 0; index < sliceIds.count(); index++)
        {
            throwIfCancelled();
            const int sliceId = sliceIds[index];
            MLUpdateBlockingDialog::updateDetailText(
                QStringLiteral("Generating envelope for selected slice %1 of %2")
                    .arg(index + 1)
                    .arg(sliceIds.count()));

            cv::Mat filled;
            if (!createFilledSlice(sliceId, filled))
            {
                return false;
            }

            QSaveFile output(stagedSliceFileName(directoryPath, sliceId));
            if (!output.open(QIODevice::WriteOnly))
            {
                throw std::runtime_error(QStringLiteral("Could not create temporary envelope mask file").toStdString());
            }

            for (int y = 0; y < filled.rows; y++)
            {
                if (output.write(reinterpret_cast<const char *>(filled.ptr<uchar>(y)), filled.cols) != filled.cols)
                {
                    throw std::runtime_error(QStringLiteral("Could not write temporary envelope mask data").toStdString());
                }
            }

            if (!output.commit())
            {
                throw std::runtime_error(QStringLiteral("Could not commit temporary envelope mask file").toStdString());
            }
        }
        return true;
    }
    catch (const EnvelopeOperationCancelled &)
    {
        setError(QStringLiteral("Envelope generation was cancelled"));
    }
    catch (const std::exception &exception)
    {
        setError(QString::fromLocal8Bit(exception.what()));
    }
    return false;
}

QString MLEnvelopeMaskGenerator::errorMessage() const
{
    return lastError;
}

QString MLEnvelopeMaskGenerator::stagedSliceFileName(const QString &directoryPath, int sliceId)
{
    return QDir(directoryPath).filePath(QStringLiteral("envelope_%1.raw").arg(sliceId, 6, 10, QChar('0')));
}

bool MLEnvelopeMaskGenerator::createFilledSlice(int sliceId, cv::Mat &filled)
{
    throwIfCancelled();
    const cv::Mat closedBoundary = processingCache->GetWholeSliceFeature(sliceId, closedFeatureIndex);
    cv::Mat boundary;
    cv::compare(closedBoundary, 0.5f, boundary, cv::CMP_GT);

    cv::Mat padded(boundary.rows + 2, boundary.cols + 2, CV_8UC1, cv::Scalar(0));
    boundary.copyTo(padded(cv::Rect(1, 1, boundary.cols, boundary.rows)));
    cv::floodFill(padded, cv::Point(0, 0), cv::Scalar(128), nullptr, cv::Scalar(), cv::Scalar(), 4);
    cv::compare(padded(cv::Rect(1, 1, boundary.cols, boundary.rows)), 128, filled, cv::CMP_NE);

    if (parameters.expansionRadius > 0)
    {
        cv::dilate(filled,
                   filled,
                   ellipticalKernel(parameters.expansionRadius),
                   cv::Point(-1, -1),
                   1,
                   cv::BORDER_CONSTANT,
                   cv::Scalar(0));
    }
    return true;
}

void MLEnvelopeMaskGenerator::cleanCacheFiles()
{
    if (cachePrefix.isEmpty() || Files.isEmpty())
    {
        return;
    }

    QDir directory(MLFileIO::GetWorkingPath());
    const QStringList files = directory.entryList(
        QStringList() << QStringLiteral("ml_*_%1*.bin").arg(cachePrefix),
        QDir::Files);
    for (const QString &file : files)
    {
        directory.remove(file);
    }
}

void MLEnvelopeMaskGenerator::setError(const QString &message)
{
    lastError = message;
}
