/**
 * @file
 * Source: Globals
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
#include "globals.h"

MainWindow *mainWindow;

bool isSP2;
bool validprogbar;
bool isFileDirty = false;
bool sp2Lock = false;
bool containsPresurfaced = false;
bool containsNonPresurfaced = false;
bool isFirstObject;
bool isVaxmlMode;
bool showScaleGrid = false;
bool showMinorGridValues = false;
bool showMinorGridLines = true;
bool isFileLoaded = false;

double applicationScaleX;
double applicationScaleY;
double currentFOV = 0.0;

float globalRescale;
float rotationX;
float rotationY;
float rotationZ;
float transformX;
float transformY;
float transformZ;
float scaleBallScale;
float mmPerUnit;
float scaleMatrix[16];
float defaultClipAngle;
float minX;
float maxX;
float minY;
float maxY;
float minZ;
float maxZ;

int colorBackgroundRed;
int colorBackgroundGreen;
int colorBackgroundBlue;
int colorGridRed;
int colorGridGreen;
int colorGridBlue;
int colorGridMinorRed;
int colorGridMinorGreen;
int colorGridMinorBlue;
int modelKTr;
int nextActualDlist = 1;
int totalTriangles = 0;
int scaleBallColour[3]; //info stuff

QList<QScreen *> availableScreens;
QSurfaceFormat surfaceFormat;
QOpenGLBuffer cubeBuffer;
QScreen *currentScreen;
QString fname; //filename passed in argv
QString currentfile; //SPV file being processed at moment (used for status)
QString stlHash;
QStringList infoComments;
QStringList infoReference;
QStringList infoAuthor;
QStringList infoSpecimen;
QStringList infoProvenance;
QStringList infoClassificationName;
QStringList infoClassificationRank;
QStringList infoTitle;
QMatrix4x4 globalMatrix;

int mainLightXYAngle = 120;
int mainLightZPos = -20;
int mainLightPower = 5;
QColor mainLightColour = Qt::white;
bool secondaryLightActive=true, headlightActive=false;
int secondaryLightXYAngle= 260;;
int secondaryLightZPos = -10;
int secondaryLightPower = 4;
QColor secondaryLightColour = Qt::white;
int headlightPower = 3;
QColor headlightColour = Qt::white;
ShadowMode mainLightShadows = ShadowMode::Soft, secondaryLightShadows = ShadowMode::None,
    headlightShadows= ShadowMode::None; //always