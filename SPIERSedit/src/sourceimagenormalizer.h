/**
 * @file
 * Palette-aware normalization for source images.
 */

#ifndef SOURCEIMAGENORMALIZER_H
#define SOURCEIMAGENORMALIZER_H

#include <QImage>
#include <QVector>

/**
 * Replace indexed-palette bytes with their actual grayscale values.
 *
 * SPIERSedit's grayscale rendering paths operate directly on the one-byte
 * image buffer. For an indexed image those bytes are palette indices, so a
 * non-identity palette must be resolved once when the source image is loaded.
 * The image remains one byte per pixel and receives an identity gray palette.
 */
inline void normalizeIndexedSourceImage(QImage &image)
{
    if (image.format() != QImage::Format_Indexed8) return;

    uchar lookup[256];
    bool identityPalette = true;
    const int colorCount = image.colorCount();
    for (int i = 0; i < 256; ++i)
    {
        const int gray = i < colorCount ? qGray(image.color(i)) : i;
        lookup[i] = static_cast<uchar>(gray);
        if (gray != i) identityPalette = false;
    }

    if (identityPalette) return;

    image.detach();
    for (int y = 0; y < image.height(); ++y)
    {
        uchar *row = image.scanLine(y);
        for (int x = 0; x < image.width(); ++x) row[x] = lookup[row[x]];
    }

    QVector<QRgb> identityGrayTable(256);
    for (int i = 0; i < identityGrayTable.count(); ++i)
        identityGrayTable[i] = qRgb(i, i, i);
    image.setColorTable(identityGrayTable);
}

#endif // SOURCEIMAGENORMALIZER_H
