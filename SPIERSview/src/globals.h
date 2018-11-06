#ifndef GLOBALS_H
#define GLOBALS_H

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QProgressBar>

class MainWindow;

//Legal Stuff
#define COPYRIGHT "Copyright © 2018 Mark D. Sutton, Russell J. Garwood, Alan R.T. Spencer"
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
#define SPVFILEVERSION 10
#define GL_MAJOR 2
#define GL_MINOR 1
#define STEREO_SEPARATION_MODIFIER 15.0
#define SHININESS 0.0

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
extern bool showMinorGridValues;

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

extern QList<QScreen *> availableScreens;
extern QMatrix4x4 globalMatrix;
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
