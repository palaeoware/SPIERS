/**
 * @file
 * Source: Mlfileio
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
#include "mlfileio.h"
#include <opencv2/opencv.hpp>

#include <stdexcept>

#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QRect>
#include <QString>
#include <QIODevice>
#include <QVector>
#include <QtGlobal>
#include "globals.h"
#include "mlroislice.h"
#include "mlupdateblockingdialog.h"

MLFileIO::MLFileIO()
{}

static constexpr quint32 kMagic = 0x4D415431; // "MAT1"
static constexpr quint32 kVersion = 1;
static constexpr quint32 kTileMagic = 0x4D544C31; // "MTL1"
static constexpr quint32 kTileVersion = 1;
static constexpr qint64 kTileHeaderSize = 9 * sizeof(quint32);
static constexpr qint64 kTileDirectoryEntrySize =
    2 * sizeof(quint64);

struct MatFileHeader
{
    quint32 magic = kMagic;
    quint32 version = kVersion;
    qint32 rows = 0;
    qint32 cols = 0;
    qint32 type = 0;
    quint64 dataSize = 0;
};

struct MatTileFileHeader
{
    quint32 magic = kTileMagic;
    quint32 version = kTileVersion;
    qint32 rows = 0;
    qint32 cols = 0;
    qint32 type = 0;
    qint32 tileSize = 0;
    qint32 tileColumns = 0;
    qint32 tileRows = 0;
    qint32 tileCount = 0;
};

struct MatTileDirectoryEntry
{
    quint64 dataOffset = 0;
    quint64 dataSize = 0;
};

namespace
{

void ConfigureDataStream(QDataStream &stream)
{
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_6_0);
}

QRect TileRect(
    int tile,
    int columns,
    int tileSize,
    int width,
    int height)
{
    const int tileX = tile % columns;
    const int tileY = tile / columns;
    const int left = tileX * tileSize;
    const int top = tileY * tileSize;
    return QRect(
        left,
        top,
        qMin(tileSize, width - left),
        qMin(tileSize, height - top));
}

quint64 TileDataSize(const QRect &rect, int type)
{
    return static_cast<quint64>(rect.width())
           * rect.height()
           * CV_ELEM_SIZE(type);
}

bool ReadTileHeader(
    QFile &file,
    MatTileFileHeader &header)
{
    if (!file.seek(0))
        return false;

    QDataStream input(&file);
    ConfigureDataStream(input);
    input >> header.magic
          >> header.version
          >> header.rows
          >> header.cols
          >> header.type
          >> header.tileSize
          >> header.tileColumns
          >> header.tileRows
          >> header.tileCount;
    return input.status() == QDataStream::Ok;
}

bool TileHeaderMatches(
    const MatTileFileHeader &header,
    int width,
    int height,
    int type,
    int tileSize)
{
    if (header.magic != kTileMagic
        || header.version != kTileVersion
        || header.rows != height
        || header.cols != width
        || header.type != type
        || header.tileSize != tileSize
        || header.tileColumns <= 0
        || header.tileRows <= 0)
    {
        return false;
    }

    const int expectedColumns =
        ((width - 1) / tileSize) + 1;
    const int expectedRows =
        ((height - 1) / tileSize) + 1;
    return header.tileColumns == expectedColumns
           && header.tileRows == expectedRows
           && header.tileCount
                  == expectedColumns * expectedRows;
}

QVector<MatTileDirectoryEntry> ReadTileDirectory(
    QFile &file,
    const MatTileFileHeader &header)
{
    QVector<MatTileDirectoryEntry> entries(
        header.tileCount);
    if (!file.seek(kTileHeaderSize))
        return {};

    QDataStream input(&file);
    ConfigureDataStream(input);
    for (int tile = 0; tile < header.tileCount; tile++)
    {
        input >> entries[tile].dataOffset
              >> entries[tile].dataSize;
    }
    if (input.status() != QDataStream::Ok)
        return {};

    const quint64 directoryEnd =
        static_cast<quint64>(kTileHeaderSize)
        + static_cast<quint64>(header.tileCount)
              * kTileDirectoryEntrySize;
    const quint64 fileSize =
        static_cast<quint64>(file.size());
    for (int tile = 0; tile < entries.size(); tile++)
    {
        MatTileDirectoryEntry &entry = entries[tile];
        if (entry.dataOffset == 0)
        {
            entry.dataSize = 0;
            continue;
        }

        const QRect rect = TileRect(
            tile,
            header.tileColumns,
            header.tileSize,
            header.cols,
            header.rows);
        const quint64 expectedSize =
            TileDataSize(rect, header.type);
        if (entry.dataOffset < directoryEnd
            || entry.dataSize != expectedSize
            || entry.dataOffset > fileSize
            || entry.dataSize > fileSize - entry.dataOffset)
        {
            entry = {};
        }
    }
    return entries;
}

bool CreateTileFile(
    QFile &file,
    const MatTileFileHeader &header)
{
    file.close();
    if (!file.open(
            QIODevice::ReadWrite
            | QIODevice::Truncate))
    {
        return false;
    }

    QDataStream output(&file);
    ConfigureDataStream(output);
    output << header.magic
           << header.version
           << header.rows
           << header.cols
           << header.type
           << header.tileSize
           << header.tileColumns
           << header.tileRows
           << header.tileCount;
    for (int tile = 0; tile < header.tileCount; tile++)
        output << quint64(0) << quint64(0);

    return output.status() == QDataStream::Ok
           && file.flush();
}

}

void MLFileIO::SaveMatBinary(const QString& featurename, const cv::Mat& mat, int fileIndex)
{
    MLUpdateBlockingDialog::updateDetailText(
        QString("Writing cache file for feature %1 for slice %2").arg(featurename)
            .arg(fileIndex));

    QString filename = GetFileName(featurename, fileIndex);

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        throw std::runtime_error(
            QString("Failed to open file for writing: %1").arg(filename).toStdString());
    }

    MatFileHeader header;
    header.rows = mat.rows;
    header.cols = mat.cols;
    header.type = mat.type();
    header.dataSize = mat.empty() ? 0 : static_cast<quint64>(mat.total() * mat.elemSize());

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_6_0);

    out << header.magic
        << header.version
        << header.rows
        << header.cols
        << header.type
        << header.dataSize;

    if (out.status() != QDataStream::Ok)
    {
        Error("Failed to write header: " + filename);
    }

    if (mat.empty()) {
        return;
    }

    if (mat.isContinuous()) {
        const qint64 written = file.write(
            reinterpret_cast<const char*>(mat.data),
            static_cast<qint64>(header.dataSize));

        if (written != static_cast<qint64>(header.dataSize)) {
            Error("Failed to write matrix data: " + filename);
        }
    } else {
        const qint64 rowSize = static_cast<qint64>(mat.cols) * mat.elemSize();
        for (int r = 0; r < mat.rows; ++r) {
            const qint64 written = file.write(
                reinterpret_cast<const char*>(mat.ptr(r)),
                rowSize);

            if (written != rowSize) {
                Error(
                    QString("Failed to write matrix row %1: %2")
                        .arg(r)
                        .arg(filename)
                    );
            }
        }
    }

}

bool MLFileIO::LoadMatTiles(
    const QString &featureName,
    int x,
    int y,
    int fileIndex,
    int tileSize,
    cv::Mat &mat,
    QByteArray &validTiles)
{
    const QString filename =
        GetTiledFileName(featureName, fileIndex);
    QFile file(filename);
    if (!file.exists())
        return false;
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not open tiled feature cache:"
                   << filename;
        return false;
    }

    MatTileFileHeader header;
    if (!ReadTileHeader(file, header)
        || !TileHeaderMatches(
            header,
            x,
            y,
            CV_32F,
            tileSize))
    {
        qWarning() << "Ignoring incompatible tiled feature cache:"
                   << filename;
        return false;
    }

    const QVector<MatTileDirectoryEntry> entries =
        ReadTileDirectory(file, header);
    if (entries.size() != header.tileCount)
    {
        qWarning() << "Ignoring invalid tiled feature directory:"
                   << filename;
        return false;
    }

    mat = cv::Mat::zeros(y, x, header.type);
    validTiles.fill(0, header.tileCount);
    for (int tile = 0; tile < entries.size(); tile++)
    {
        const MatTileDirectoryEntry &entry =
            entries.at(tile);
        if (entry.dataOffset == 0)
            continue;

        const QRect rect = TileRect(
            tile,
            header.tileColumns,
            header.tileSize,
            header.cols,
            header.rows);
        if (!file.seek(
                static_cast<qint64>(entry.dataOffset)))
        {
            continue;
        }

        const qint64 rowSize =
            static_cast<qint64>(rect.width())
            * mat.elemSize();
        bool tileLoaded = true;
        for (int row = rect.top();
             row <= rect.bottom();
             row++)
        {
            char *destination =
                reinterpret_cast<char *>(mat.ptr(row))
                + static_cast<qint64>(rect.left())
                      * mat.elemSize();
            if (file.read(destination, rowSize) != rowSize)
            {
                tileLoaded = false;
                break;
            }
        }
        if (tileLoaded)
            validTiles[tile] = 1;
    }

    return true;
}

void MLFileIO::RemoveMatTiles(
    const QString &featureName,
    int fileIndex)
{
    QFile::remove(
        GetTiledFileName(featureName, fileIndex));
}

void MLFileIO::SaveMatTiles(
    const QString &featureName,
    const cv::Mat &mat,
    int fileIndex,
    const MLROISlice &roi)
{
    Q_ASSERT(roi.isValid());
    Q_ASSERT(mat.rows == roi.height());
    Q_ASSERT(mat.cols == roi.width());
    Q_ASSERT(mat.type() == CV_32F);

    const QString filename =
        GetTiledFileName(featureName, fileIndex);
    QFile file(filename);
    MatTileFileHeader header;
    header.rows = mat.rows;
    header.cols = mat.cols;
    header.type = mat.type();
    header.tileSize = roi.tileSize();
    header.tileColumns = roi.tileColumns();
    header.tileRows = roi.tileRows();
    header.tileCount = roi.totalTileCount();

    bool recreateFile = !file.exists();
    if (!recreateFile
        && file.open(QIODevice::ReadWrite))
    {
        MatTileFileHeader existingHeader;
        recreateFile =
            !ReadTileHeader(file, existingHeader)
            || !TileHeaderMatches(
                existingHeader,
                mat.cols,
                mat.rows,
                mat.type(),
                roi.tileSize());
    }
    else if (!recreateFile)
    {
        throw std::runtime_error(
            QString(
                "Failed to open tiled feature cache: %1")
                .arg(filename)
                .toStdString());
    }

    if (recreateFile)
    {
        if (!CreateTileFile(file, header))
        {
            throw std::runtime_error(
                QString(
                    "Failed to create tiled feature cache: %1")
                    .arg(filename)
                    .toStdString());
        }
    }

    QVector<MatTileDirectoryEntry> entries =
        ReadTileDirectory(file, header);
    if (entries.size() != header.tileCount)
    {
        if (!CreateTileFile(file, header))
        {
            throw std::runtime_error(
                QString(
                    "Failed to recreate tiled feature cache: %1")
                    .arg(filename)
                    .toStdString());
        }
        entries = ReadTileDirectory(file, header);
    }

    struct PendingTileEntry
    {
        int tile = -1;
        MatTileDirectoryEntry entry;
    };
    QVector<PendingTileEntry> pendingEntries;

    for (int tileY = 0;
         tileY < roi.tileRows();
         tileY++)
    {
        for (int tileX = 0;
             tileX < roi.tileColumns();
             tileX++)
        {
            if (roi.tileState(tileX, tileY)
                == MLROISlice::TileState::Inactive)
            {
                continue;
            }

            const int tile =
                tileY * roi.tileColumns() + tileX;
            if (entries.at(tile).dataOffset != 0)
                continue;

            const QRect rect =
                roi.tileRect(tileX, tileY);
            if (!file.seek(file.size()))
            {
                throw std::runtime_error(
                    QString(
                        "Failed to seek tiled feature cache: %1")
                        .arg(filename)
                        .toStdString());
            }

            PendingTileEntry pending;
            pending.tile = tile;
            pending.entry.dataOffset =
                static_cast<quint64>(file.pos());
            pending.entry.dataSize =
                TileDataSize(rect, mat.type());

            const qint64 rowSize =
                static_cast<qint64>(rect.width())
                * mat.elemSize();
            for (int row = rect.top();
                 row <= rect.bottom();
                 row++)
            {
                const char *source =
                    reinterpret_cast<const char *>(
                        mat.ptr(row))
                    + static_cast<qint64>(rect.left())
                          * mat.elemSize();
                if (file.write(source, rowSize) != rowSize)
                {
                    throw std::runtime_error(
                        QString(
                            "Failed to write tiled feature "
                            "data: %1")
                            .arg(filename)
                            .toStdString());
                }
            }
            pendingEntries.append(pending);
        }
    }

    if (!file.flush())
    {
        throw std::runtime_error(
            QString(
                "Failed to flush tiled feature data: %1")
                .arg(filename)
                .toStdString());
    }

    for (const PendingTileEntry &pending :
         pendingEntries)
    {
        const qint64 directoryPosition =
            kTileHeaderSize
            + static_cast<qint64>(pending.tile)
                  * kTileDirectoryEntrySize;
        if (!file.seek(directoryPosition))
        {
            throw std::runtime_error(
                QString(
                    "Failed to seek tiled feature directory: "
                    "%1")
                    .arg(filename)
                    .toStdString());
        }

        QDataStream output(&file);
        ConfigureDataStream(output);
        output << pending.entry.dataOffset
               << pending.entry.dataSize;
        if (output.status() != QDataStream::Ok)
        {
            throw std::runtime_error(
                QString(
                    "Failed to update tiled feature "
                    "directory: %1")
                    .arg(filename)
                    .toStdString());
        }
    }

    if (!file.flush())
    {
        throw std::runtime_error(
            QString(
                "Failed to flush tiled feature directory: %1")
                .arg(filename)
                .toStdString());
    }
}

void MLFileIO::SaveSignedVisualisation(
    const QString &featureName,
    const cv::Mat &mat,
    int fileIndex,
    float maximumAbsoluteValue)
{
    Q_ASSERT(mat.type() == CV_32F);

    cv::Mat visualisation;
    if (maximumAbsoluteValue > 0.0f)
    {
        mat.convertTo(
            visualisation,
            CV_8U,
            127.5 / static_cast<double>(maximumAbsoluteValue),
            127.5);
    }
    else
    {
        visualisation = cv::Mat(mat.size(), CV_8U, cv::Scalar(128));
    }

    QString filename = GetFileName(featureName, fileIndex);
    filename.chop(4);
    filename.append(QStringLiteral(".png"));
    if (!cv::imwrite(filename.toStdString(), visualisation))
    {
        throw std::runtime_error(
            QStringLiteral("Failed to write feature visualisation: %1")
                .arg(filename)
                .toStdString());
    }
}

QString MLFileIO::GetFileName(const QString &fname, int index)
{
    QString Fname = Files.at(index);
    int lastsep = lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    QString sfname = Fname.left(lastsep);
    QString temp = "/" + SettingsFileName + "/" + QString("ml_%1").arg(index,6,10,QChar('0')) + QString("_")
                   + fname+".bin";
    return sfname +temp;

}

QString MLFileIO::GetTiledFileName(
    const QString &featureName,
    int fileIndex)
{
    QString filename =
        GetFileName(featureName, fileIndex);
    filename.chop(4);
    filename.append(QStringLiteral(".tiles"));
    return filename;
}

QString MLFileIO::GetWorkingPath()
{
    QFileInfo fi(Files.at(0));
    return fi.path() + "/" + SettingsFileName;
}


cv::Mat MLFileIO::LoadMatBinary(const QString& featureName, int x, int y, int fileIndex,  bool &ok)
{
    MLUpdateBlockingDialog::updateDetailText(
        QString("Loading cache file for feature %1 for slice %2").arg(featureName)
            .arg(fileIndex));

    QString filename = GetFileName(featureName, fileIndex);

    QFile file(filename);
    ok = false;

    if (!file.exists())
    {
        return cv::Mat(0,0,0);
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        Error(QString("Failed to open file for reading: %1").arg(filename));
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_6_0);

    MatFileHeader header;
    in >> header.magic
        >> header.version
        >> header.rows
        >> header.cols
        >> header.type
        >> header.dataSize;

    if (in.status() != QDataStream::Ok) {
        Error(
            QString("Failed to read header: %1").arg(filename));
    }

    if (header.magic != kMagic) {
        Error(
            QString("Invalid matrix file magic: %1").arg(filename));
    }

    if (header.version != kVersion) {
        Error(
            QString("Unsupported matrix file version %1: %2")
                .arg(header.version)
                .arg(filename)
                );
    }

    if (header.rows < 0 || header.cols < 0) {
        Error(
            QString("Invalid matrix dimensions in file: %1").arg(filename));
    }

    if (header.rows!= y || header.cols!=x)
    {
        qDebug()<<"Wrong file size - ignoring";
        return cv::Mat(0,0,0);
    }

    cv::Mat mat(header.rows, header.cols, header.type);

    if (mat.empty()) {
        return mat;
    }

    const quint64 expectedSize =
        static_cast<quint64>(mat.total() * mat.elemSize());

    if (header.dataSize != expectedSize) {
        throw std::runtime_error(
            QString("Matrix data size mismatch in file: %1").arg(filename).toStdString());
    }

    const qint64 bytesRead = file.read(
        reinterpret_cast<char*>(mat.data),
        static_cast<qint64>(header.dataSize));

    if (bytesRead != static_cast<qint64>(header.dataSize)) {
        throw std::runtime_error(
            QString("Failed to read matrix data: %1").arg(filename).toStdString());
    }

    ok = true;
    return mat;
}

cv::Mat MLFileIO::LoadMatFromImageFile(int sliceIndex, bool expectColour)
{
    MLUpdateBlockingDialog::updateDetailText(
        QString("Converting source file for slice %1").arg(sliceIndex)
            );
    QString filename = Files[sliceIndex];
    cv::Mat img = cv::imread(filename.toStdString(), cv::IMREAD_UNCHANGED);

    if (img.type() == CV_8UC1 && !expectColour)
    {
        cv::Mat mat32;
        mat32.create(img.size(), CV_32F);
        img.convertTo(mat32, CV_32F, 1.0 / 255.0);
        return mat32;
    }

    //Next one can happen if it's a grayscale image accidentally saved as colour
    if (img.type() == CV_8UC3 && !expectColour)
    {
        //qDebug()<<"Images are saved in colour mode but really grayscale";
        cv::Mat mat32;

        cv::cvtColor(img, mat32, cv::COLOR_BGR2GRAY);
        mat32.convertTo(mat32, CV_32F, 1.0 / 255.0);
        return mat32;
    }

    if (img.type() == CV_8UC3 && expectColour)
    {
        cv::Mat mat32;
        mat32.create(img.size(), CV_32FC3);
        img.convertTo(mat32, CV_32FC3, 1.0 / 255.0);
        return mat32;
    }

    Error(QString("Oh dear, error reading source image"));
    return cv::Mat();
}
