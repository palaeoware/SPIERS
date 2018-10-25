/**
 * @file
 * Header: Globals
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <QCursor>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QMenu>
#include <QPointF>
#include <QPushButton>
#include <QString>
#include <QTransform>

//Legal Stuff
#define COPYRIGHT "Copyright © 2018 Mark D. Sutton, Russell J. Garwood, Alan R.T.Spencer"
#define LICENCE "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the GPL v3  license"

//Programme Name
#define PRODUCTNAME "SPIERSalign"

//Email
#define EMAIL "palaeoware@gmail.com"

//Github
#define GITURL "https://github.com/"
#define GITREPOSITORY "palaeoware/SPIERS"
#define GITISSUE "/issues"

//Readthedocs
#define READTHEDOCS "https://spiersalign.readthedocs.io/"

/**
 * @brief The ImageData class
 */
class ImageData
{
public:
    ImageData(QString fn);
    QString fileName;
    QTransform m;
    bool hidden;
    int format;
};

/**
 * @brief The PropogationData class
 */
class PropogationData
{
public:
    int transformation;
    qreal value;
};

/**
 * @brief The MarkerData class
 */
class MarkerData
{
public:
    MarkerData(QRectF *startRectangle, int shp);
    QRectF *markerRect;
    int shape;
    QGraphicsItem *markerPointer;
    QPointF linePoint;
    qreal dX, dY;
};

extern void writeSuperGlobals();
extern void readSuperGlobals();
extern void showInfo(int, int);
extern void recentFile(QString);

extern int selectedMarker;
extern int currentImage;
extern int markersUp, markersLocked, setupFlag;
extern int cropUp, infoChecked, autoMarkersUp;
extern int cropping;
extern int sceneDX, sceneDY;
extern int flagUp, flagUpSelection;

extern QGraphicsItemGroup *autoMarkersGroup;
extern QGraphicsRectItem *rectPointer, *suRectPointer, *suRectPointer2;
extern QGraphicsRectItem *amRectPointer;
extern QLabel *infoLabel;
extern QList <QGraphicsLineItem *> linePointers;
extern QList <MarkerData *> markers;
extern QList <ImageData *> imageList;
extern QList <PropogationData *> propogation;
extern QListWidget *markerList, *fileList;
extern QPushButton *addMarker, *swapButton, *removeMarker, *executeAlign, *resetImage ;
extern qreal currentScale;
extern QRect *cropArea, *gridOutline, *autoEdgeOne, *autoEdgeTwo;
extern QString fullSettingsFileName;
extern QStringList recentFileList;
extern QTransform aM, setupM, setupM2;

#endif // __GLOBALS_H__
