/**
 * @file
 * Header: Mlfileio
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
#ifndef MLFILEIO_H
#define MLFILEIO_H

#include <QByteArray>
#include <QString>
#include <opencv2/opencv.hpp>

class MLROISlice;

class MLFileIO
{
public:
    MLFileIO();

    static cv::Mat LoadMatBinary(const QString &featureName, int x, int y, int fileIndex, bool &ok);
    static bool LoadMatTiles(
        const QString &featureName,
        int x,
        int y,
        int fileIndex,
        int tileSize,
        cv::Mat &mat,
        QByteArray &validTiles);
    static void RemoveMatTiles(
        const QString &featureName,
        int fileIndex);
    static void SaveMatBinary(const QString &featurename, const cv::Mat &mat, int fileIndex);
    static void SaveMatTiles(
        const QString &featureName,
        const cv::Mat &mat,
        int fileIndex,
        const MLROISlice &roi);
    static void SaveSignedVisualisation(const QString &featureName, const cv::Mat &mat, int fileIndex, float maximumAbsoluteValue);
    static cv::Mat LoadMatFromImageFile(int sliceIndex, bool expectColour);
    static QString GetWorkingPath();
private:
    static QString GetFileName(const QString &fname, int index);
    static QString GetTiledFileName(
        const QString &featureName,
        int fileIndex);

};

#endif // MLFILEIO_H
