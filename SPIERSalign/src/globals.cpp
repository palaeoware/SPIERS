/**
 * @file
 * Globals
 *
 * All SPIERSalign code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSalign code is Copyright 2008-2023 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "globals.h"

/**
 * @brief ImageData::ImageData
 * @param fn
 */
ImageData::ImageData(QString fn)
{
    fileName = fn;
}

/**
 * @brief MarkerData::MarkerData
 * @param startRectangle
 * @param shp
 */
MarkerData::MarkerData(QRectF *startRectangle, int shp)
{
    markerRect = startRectangle;
    shape = shp;
    markerPointer = nullptr;
}

void writeSuperGlobals();
void showInfo(int x, int y);
void readSuperGlobals();
void recentFile(QString fname);

int currentImage;
int selectedMarker;
int markersUp, markersLocked, setupFlag;
int cropUp, infoChecked, autoMarkersUp;
int cropping;
int sceneDX, sceneDY;
int flagUp = 0, flagUpSelection = -1;

qreal currentScale;

QGraphicsItemGroup *autoMarkersGroup;
QGraphicsRectItem *amRectPointer, *rectPointer, *suRectPointer, *suRectPointer2;
QLabel *infoLabel;
QList <QGraphicsLineItem *> linePointers;
QList <MarkerData *> markers;
QList <ImageData *> imageList;
QList <PropogationData *> propogation;
QListWidget *markerList, *fileList;
QPushButton *addMarker, *swapButton, *removeMarker, *executeAlign, *resetImage ;
QRect *cropArea, *gridOutline, *autoEdgeOne, *autoEdgeTwo;
QStringList recentFileList;
QString fullSettingsFileName;
QTransform aM, setupM, setupM2;

