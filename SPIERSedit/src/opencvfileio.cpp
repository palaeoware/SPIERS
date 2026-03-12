#include "opencvfileio.h"
#include <opencv2/opencv.hpp>

#include <QFile>
#include <QDataStream>
#include <QString>
#include <QIODevice>
#include <QtGlobal>
#include "globals.h"
#include "mlupdateblockingdialog.h"

openCVFileIO::openCVFileIO()
{}

static constexpr quint32 kMagic = 0x4D415431; // "MAT1"
static constexpr quint32 kVersion = 1;

struct MatFileHeader
{
    quint32 magic = kMagic;
    quint32 version = kVersion;
    qint32 rows = 0;
    qint32 cols = 0;
    qint32 type = 0;
    quint64 dataSize = 0;
};

void openCVFileIO::SaveMatBinary(const QString& featurename, const cv::Mat& mat, int fileIndex)
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


    //TEST - create PNG to inspect
    if (featurename!="src")
    {
        cv::Mat tmp;
        mat.convertTo(tmp, CV_8U, 255.0);
        cv::imwrite((filename+QString(".png")).toStdString(), tmp);
    }
}

QString openCVFileIO::GetFileName(const QString &fname, int index)
{
    QString Fname = Files.at(index);
    int lastsep = lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    QString sfname = Fname.left(lastsep);
    QString temp = "/" + SettingsFileName + "/" + QString("ml_%1").arg(index,6,10,QChar('0')) + QString("_")
                   + fname+".bin";
    return sfname +temp;

}

cv::Mat openCVFileIO::LoadMatBinary(const QString& featureName, int x, int y, int fileIndex,  bool &ok)
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

cv::Mat openCVFileIO::LoadMatFromImageFile(int sliceIndex, bool expectColour)
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
}
