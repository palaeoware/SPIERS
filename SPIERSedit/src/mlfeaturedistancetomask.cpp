/**
 * @file
 * Source: Mlfeaturedistancetomask
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
#include "mlfeaturedistancetomask.h"

#include "fileio.h"
#include "globals.h"
#include "mlfileio.h"
#include "mlupdateblockingdialog.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QVector>

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace
{

constexpr qint64 WORKING_MEMORY_BYTES = 64ll * 1024ll * 1024ll;

void checkCancelled()
{
    if (MLUpdateBlockingDialog::isCancelled())
    {
        throw std::runtime_error("Distance-to-mask calculation cancelled");
    }
}

void readExactly(QFile &file, char *data, qint64 byteCount)
{
    if (file.read(data, byteCount) != byteCount)
    {
        throw std::runtime_error("Could not read temporary distance-transform data");
    }
}

void writeExactly(QFile &file, const char *data, qint64 byteCount)
{
    if (file.write(data, byteCount) != byteCount)
    {
        throw std::runtime_error("Could not write temporary distance-transform data");
    }
}

void distanceTransform1D(const float *source,
                         float *destination,
                         int count,
                         double spacingSquared,
                         QVector<int> &locations,
                         QVector<double> &boundaries)
{
    int envelopeIndex = 0;
    locations[0] = 0;
    boundaries[0] = -std::numeric_limits<double>::infinity();
    boundaries[1] = std::numeric_limits<double>::infinity();

    for (int position = 1; position < count; position++)
    {
        double intersection = 0.0;
        while (true)
        {
            const int previous = locations[envelopeIndex];
            intersection =
                (static_cast<double>(source[position]) + spacingSquared * position * position
                 - static_cast<double>(source[previous]) - spacingSquared * previous * previous)
                / (2.0 * spacingSquared * (position - previous));

            if (envelopeIndex == 0 || intersection > boundaries[envelopeIndex])
            {
                break;
            }
            envelopeIndex--;
        }

        envelopeIndex++;
        locations[envelopeIndex] = position;
        boundaries[envelopeIndex] = intersection;
        boundaries[envelopeIndex + 1] = std::numeric_limits<double>::infinity();
    }

    envelopeIndex = 0;
    for (int position = 0; position < count; position++)
    {
        while (boundaries[envelopeIndex + 1] < position)
        {
            envelopeIndex++;
        }

        const double offset = position - locations[envelopeIndex];
        destination[position] = static_cast<float>(
            spacingSquared * offset * offset + source[locations[envelopeIndex]]);
    }
}

}

MLFeatureDistanceToMask::MLFeatureDistanceToMask(int maskId)
    : MLFeature(FeatureType::Distance_to_mask, Channel::Intensity, true, maskId, 0)
{
}

void MLFeatureDistanceToMask::CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *)
{
    calculateVolume();

    bool loaded = false;
    const cv::Mat calculated = MLFileIO::LoadMatBinary(
        GetEncodedNameForFile(),
        fwidth,
        fheight,
        sliceID,
        loaded);
    if (!loaded)
    {
        throw std::runtime_error("Could not load calculated distance-to-mask feature");
    }
    calculated.copyTo(mat);
}

QList<MLFeature *> MLFeatureDistanceToMask::GetDependencies()
{
    return QList<MLFeature *>();
}

QString MLFeatureDistanceToMask::GetPrettyName()
{
    return QStringLiteral("distance to mask");
}

QString MLFeatureDistanceToMask::GetPrettyArgs()
{
    if (_arg1 >= 0 && _arg1 < MasksSettings.count())
    {
        return QStringLiteral("%1 (%2)").arg(MasksSettings[_arg1]->Name).arg(_arg1);
    }
    return QStringLiteral("missing mask %1").arg(_arg1);
}

QString MLFeatureDistanceToMask::GetPretty3D()
{
    return QStringLiteral("3D");
}

int MLFeatureDistanceToMask::GetDependencyDepth()
{
    return 0;
}

QString MLFeatureDistanceToMask::GetArg1SetupString(int value)
{
    if (value >= 0 && value < MasksSettings.count())
    {
        return QStringLiteral("%1: %2").arg(value).arg(MasksSettings[value]->Name);
    }
    return QString::number(value);
}

QString MLFeatureDistanceToMask::GetArg2SetupString(int)
{
    return QString();
}

int MLFeatureDistanceToMask::GetMinMaxForArgs(int arg, bool max)
{
    if (arg == 1)
    {
        return max ? MaxUsedMask : 0;
    }
    return 0;
}

bool MLFeatureDistanceToMask::ReferencesMask(int maskId) const
{
    return _arg1 == maskId;
}

void MLFeatureDistanceToMask::RemapMasks(const QVector<int> &maskMap)
{
    if (_arg1 >= 0 && _arg1 < maskMap.count())
    {
        _arg1 = maskMap[_arg1];
    }
}

QString MLFeatureDistanceToMask::GetTypeCodeForFile()
{
    return QStringLiteral("md2");
}

QString MLFeatureDistanceToMask::GetArgsForFile()
{
    return QStringLiteral("@%1").arg(_arg1);
}

QString MLFeatureDistanceToMask::Get3DForFile()
{
    return QStringLiteral("3");
}

void MLFeatureDistanceToMask::calculateVolume()
{
    if (_arg1 < 0 || _arg1 > MaxUsedMask)
    {
        throw std::runtime_error("Distance-to-mask feature refers to a missing mask");
    }
    if (FileCount < 1 || fwidth < 1 || fheight < 1)
    {
        throw std::runtime_error("Distance-to-mask feature has invalid volume dimensions");
    }
    if (PixPerMM <= 0.0 || SlicePerMM <= 0.0)
    {
        throw std::runtime_error("Distance-to-mask feature requires valid pixel and slice calibration");
    }

    QTemporaryDir temporaryDirectory(
        QDir(MLFileIO::GetWorkingPath()).filePath(QStringLiteral("mask_distance_XXXXXX")));
    if (!temporaryDirectory.isValid())
    {
        throw std::runtime_error("Could not create temporary distance-transform directory");
    }

    QFile squaredDistances(temporaryDirectory.filePath(QStringLiteral("squared.raw")));
    if (!squaredDistances.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        throw std::runtime_error("Could not create temporary distance-transform data");
    }

    const int pixelCount = fwidth * fheight;
    const float xySpacingMicrometres = static_cast<float>(
        1000.0 * static_cast<double>(ColMonoScale) / PixPerMM);
    const double zSpacingMicrometres =
        1000.0 * static_cast<double>(qMax(1, zsparsity)) / SlicePerMM;
    const double volumeDiagonalSquared =
        std::pow(static_cast<double>(fwidth) * xySpacingMicrometres, 2.0)
        + std::pow(static_cast<double>(fheight) * xySpacingMicrometres, 2.0)
        + std::pow(static_cast<double>(FileCount) * zSpacingMicrometres, 2.0);
    const float unreachableSquared = static_cast<float>(volumeDiagonalSquared);

    // The exact XY transform supplies the per-slice cost term for the later Z transform.
    QVector<float> interleavedDistances(pixelCount * 2);
    for (int slice = 0; slice < FileCount; slice++)
    {
        checkCancelled();
        MLUpdateBlockingDialog::updateDetailText(
            QStringLiteral("Preparing mask distances for slice %1 of %2")
                .arg(slice + 1)
                .arg(FileCount));

        QByteArray maskData;
        if (!SimpleLoadMasksForFile(Files.at(slice), pixelCount, &maskData))
        {
            maskData.fill(0, pixelCount);
        }

        cv::Mat binary(fheight, fwidth, CV_8UC1);
        for (int y = 0; y < fheight; y++)
        {
            uchar *row = binary.ptr<uchar>(y);
            const int maskY = fheight - y - 1;
            for (int x = 0; x < fwidth; x++)
            {
                row[x] = static_cast<uchar>(maskData.at(maskY * fwidth + x)) == _arg1 ? 255 : 0;
            }
        }

        const int insidePixelCount = cv::countNonZero(binary);
        const bool hasInsidePixels = insidePixelCount > 0;
        const bool hasOutsidePixels = insidePixelCount < pixelCount;

        cv::Mat insideDistance;
        if (hasOutsidePixels)
        {
            cv::distanceTransform(binary, insideDistance, cv::DIST_L2, cv::DIST_MASK_PRECISE);
        }

        cv::Mat inverse;
        cv::Mat outsideDistance;
        if (hasInsidePixels)
        {
            cv::bitwise_not(binary, inverse);
            cv::distanceTransform(inverse, outsideDistance, cv::DIST_L2, cv::DIST_MASK_PRECISE);
        }

        for (int y = 0; y < fheight; y++)
        {
            const float *insideRow = hasOutsidePixels ? insideDistance.ptr<float>(y) : nullptr;
            const float *outsideRow = hasInsidePixels ? outsideDistance.ptr<float>(y) : nullptr;
            for (int x = 0; x < fwidth; x++)
            {
                const int pixel = y * fwidth + x;
                if (hasOutsidePixels)
                {
                    const float inside = insideRow[x] * xySpacingMicrometres;
                    interleavedDistances[pixel * 2] = inside * inside;
                }
                else
                {
                    interleavedDistances[pixel * 2] = unreachableSquared;
                }

                if (hasInsidePixels)
                {
                    const float outside = outsideRow[x] * xySpacingMicrometres;
                    interleavedDistances[pixel * 2 + 1] = outside * outside;
                }
                else
                {
                    interleavedDistances[pixel * 2 + 1] = unreachableSquared;
                }
            }
        }

        writeExactly(
            squaredDistances,
            reinterpret_cast<const char *>(interleavedDistances.constData()),
            static_cast<qint64>(interleavedDistances.size()) * sizeof(float));
    }
    squaredDistances.flush();

    QFile signedDistances(temporaryDirectory.filePath(QStringLiteral("signed.raw")));
    if (!signedDistances.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        throw std::runtime_error("Could not create temporary signed-distance data");
    }
    if (!signedDistances.resize(
            static_cast<qint64>(FileCount) * pixelCount * static_cast<qint64>(sizeof(float))))
    {
        throw std::runtime_error("Could not size temporary signed-distance data");
    }

    const qint64 bytesPerBlockedPixel =
        static_cast<qint64>(FileCount) * (sizeof(float) * 3 + sizeof(uchar));
    const int blockPixelLimit = static_cast<int>(
        qMax<qint64>(1, qMin<qint64>(pixelCount, WORKING_MEMORY_BYTES / bytesPerBlockedPixel)));
    QVector<int> locations(FileCount);
    QVector<double> boundaries(FileCount + 1);
    QVector<float> transformOutput(FileCount);
    float maximumAbsoluteDistance = 0.0f;

    // Work in bounded pixel blocks; only the temporary files scale with volume size.
    for (int firstPixel = 0; firstPixel < pixelCount; firstPixel += blockPixelLimit)
    {
        checkCancelled();
        const int blockPixels = qMin(blockPixelLimit, pixelCount - firstPixel);
        MLUpdateBlockingDialog::updateDetailText(
            QStringLiteral("Calculating 3D mask distance %1%")
                .arg((static_cast<qint64>(firstPixel) * 100) / pixelCount));

        QVector<float> insideCosts(blockPixels * FileCount);
        QVector<float> outsideCosts(blockPixels * FileCount);
        QVector<uchar> insideFlags(blockPixels * FileCount);
        QVector<float> sliceInput(blockPixels * 2);

        for (int slice = 0; slice < FileCount; slice++)
        {
            const qint64 offset =
                (static_cast<qint64>(slice) * pixelCount + firstPixel) * 2 * sizeof(float);
            if (!squaredDistances.seek(offset))
            {
                throw std::runtime_error("Could not seek temporary distance-transform data");
            }
            readExactly(
                squaredDistances,
                reinterpret_cast<char *>(sliceInput.data()),
                static_cast<qint64>(sliceInput.size()) * sizeof(float));

            for (int pixel = 0; pixel < blockPixels; pixel++)
            {
                const int columnIndex = pixel * FileCount + slice;
                insideCosts[columnIndex] = sliceInput[pixel * 2];
                outsideCosts[columnIndex] = sliceInput[pixel * 2 + 1];
                insideFlags[columnIndex] = sliceInput[pixel * 2] > 0.0f;
            }
        }

        const double zSpacingSquared = zSpacingMicrometres * zSpacingMicrometres;
        for (int pixel = 0; pixel < blockPixels; pixel++)
        {
            float *insideColumn = insideCosts.data() + pixel * FileCount;
            float *outsideColumn = outsideCosts.data() + pixel * FileCount;

            distanceTransform1D(
                insideColumn,
                transformOutput.data(),
                FileCount,
                zSpacingSquared,
                locations,
                boundaries);
            std::copy(transformOutput.cbegin(), transformOutput.cend(), insideColumn);

            distanceTransform1D(
                outsideColumn,
                transformOutput.data(),
                FileCount,
                zSpacingSquared,
                locations,
                boundaries);

            for (int slice = 0; slice < FileCount; slice++)
            {
                const int columnIndex = pixel * FileCount + slice;
                const float squaredDistance = insideFlags[columnIndex]
                                                  ? insideColumn[slice]
                                                  : transformOutput[slice];
                const float distance = std::sqrt(squaredDistance);
                outsideColumn[slice] = insideFlags[columnIndex] ? distance : -distance;
                maximumAbsoluteDistance = qMax(maximumAbsoluteDistance, distance);
            }
        }

        QVector<float> sliceOutput(blockPixels);
        for (int slice = 0; slice < FileCount; slice++)
        {
            for (int pixel = 0; pixel < blockPixels; pixel++)
            {
                sliceOutput[pixel] = outsideCosts[pixel * FileCount + slice];
            }

            const qint64 outputOffset =
                (static_cast<qint64>(slice) * pixelCount + firstPixel) * sizeof(float);
            if (!signedDistances.seek(outputOffset))
            {
                throw std::runtime_error("Could not seek temporary signed-distance data");
            }
            writeExactly(
                signedDistances,
                reinterpret_cast<const char *>(sliceOutput.constData()),
                static_cast<qint64>(sliceOutput.size()) * sizeof(float));
        }
    }

    signedDistances.flush();
    cv::Mat sliceMat(fheight, fwidth, CV_32F);
    for (int slice = 0; slice < FileCount; slice++)
    {
        checkCancelled();
        MLUpdateBlockingDialog::updateDetailText(
            QStringLiteral("Saving mask distance slice %1 of %2")
                .arg(slice + 1)
                .arg(FileCount));

        const qint64 offset = static_cast<qint64>(slice) * pixelCount * sizeof(float);
        if (!signedDistances.seek(offset))
        {
            throw std::runtime_error("Could not seek calculated signed-distance data");
        }
        readExactly(
            signedDistances,
            reinterpret_cast<char *>(sliceMat.data),
            static_cast<qint64>(pixelCount) * sizeof(float));
        MLFileIO::SaveMatBinary(GetEncodedNameForFile(), sliceMat, slice);
        MLFileIO::SaveSignedVisualisation(
            GetEncodedNameForFile(),
            sliceMat,
            slice,
            maximumAbsoluteDistance);
    }
}
