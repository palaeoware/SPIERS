/**
 * @file
 * Header: Globals
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QProgressBar>
#include <QSurfaceFormat>

class MainWindow;

//Legal Stuff
#define COPYRIGHT "Copyright © 2018-2023 Mark D. Sutton, Russell J. Garwood, Alan R.T. Spencer"
#define LICENCE "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the GPL v3 license"

//Programme Name
#define PRODUCTNAME "SPIERSview"

//Email
#define EMAIL "palaeoware@gmail.com"

//Github
#define GITURL "https://github.com/"
#define GITREPOSITORY "palaeoware/SPIERS"
#define GITISSUE "/issues"

#define READTHEDOCS "https://spiersview.readthedocs.io/"

// Test file location
#define TESTDUMPLOCATION "C:/SPIERSview_test/"

//SPIERSview defines
#define GRIDSIZE 32
#define SCALE 2
#define MAXDLISTSIZE 100000
#define SENSITIVITY 5
#define SPINTIMERINTERVAL 10
#define SPINRATE .02
#define SPVFILEVERSION 11
#define GL_MAJOR 2
#define GL_MINOR 1
#define GL_MAJOR_MAC 3
#define GL_MINOR_MAC 3
#define STEREO_SEPARATION_MODIFIER 15.0
#define SHININESS 0.0
#define FONT_SCALE_FACTOR 24.0 // Eqates to roughtly the same font size as the default used by the QT GUI

// SPIERSview extrerns
extern MainWindow *mainWindow;

extern bool isSP2;
extern bool validprogbar;
extern bool isFileDirty;
extern bool sp2Lock;
extern bool containsPresurfaced;
extern bool containsNonPresurfaced;
extern bool isVaxmlMode;
extern bool isFirstObject;
extern bool showScaleGrid;
extern bool showMinorGridValues;
extern bool showMinorGridLines;
extern bool isFileLoaded;

extern double applicationScaleX;
extern double applicationScaleY;
extern double currentFOV;

extern float globalRescale;
extern float mmPerUnit;
extern float minX;
extern float maxX;
extern float minY;
extern float maxY;
extern float minZ;
extern float maxZ;
extern float rotationX;
extern float rotationY;
extern float rotationZ;
extern float scaleBallScale;
extern float scaleMatrix[16];
extern float transformX;
extern float transformY;
extern float transformZ;
extern float defaultClipAngle;

extern int nextActualDlist;
extern int totalTriangles;
extern int colorBackgroundRed;
extern int colorBackgroundGreen;
extern int colorBackgroundBlue;
extern int colorGridRed;
extern int colorGridGreen;
extern int colorGridBlue;
extern int colorGridMinorRed;
extern int colorGridMinorGreen;
extern int colorGridMinorBlue;
extern int scaleBallColour[3];
extern int modelKTr;
extern int fontSizeGrid;

extern QList<QScreen *> availableScreens;
extern QMatrix4x4 globalMatrix;
extern QSurfaceFormat surfaceFormat;
extern QOpenGLBuffer cubeBuffer;
extern QScreen *currentScreen;
extern QString fname;
extern QString currentfile;
extern QStringList infoComments;
extern QStringList infoReference;
extern QStringList infoAuthor;
extern QStringList infoSpecimen;
extern QStringList infoProvenance;
extern QStringList infoClassificationName;
extern QStringList infoClassificationRank;
extern QStringList infoTitle;
extern QString stlHash;

#endif // GLOBALS_H
