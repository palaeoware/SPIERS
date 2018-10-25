#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QStringList>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLabel>
#include <QMatrix4x4>
#include <QOpenGLBuffer>

class MainWindow;

//Legal Stuff
#define COPYRIGHT "Copyright © 2018 Mark D. Sutton, Russell J. Garwood, Alan R.T.Spencer"
#define LICENCE "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the GPL v3  license"

//Programme Name
#define PRODUCTNAME "SPIERSview"

//Email
#define EMAIL "palaeoware@gmail.com"

//Github
#define GITURL "https://github.com/"
#define GITREPOSITORY "palaeoware/SPIERSview"
#define GITISSUE "/issues"

//SPIERSview Defines
#define GRIDSIZE 32
#define SCALE 2
#define MAXDLISTSIZE 100000
#define SENSITIVITY 5
#define SPINTIMERINTERVAL 10
#define SPINRATE .02
#define VERSION 10
#define GL_MAJOR 2
#define GL_MINOR 1

// SPIERSview extrern
extern QString fname;
extern QString currentfile;
extern bool IsSP2;
extern MainWindow *MainWin;
extern bool validprogbar, FileDirty;
extern int NextActualDlist;
extern int FirstObject;
extern int TotalTriangles;
extern bool SP2_lock;
extern bool ContainsPresurfaced;
extern bool ContainsNonPresurfaced;
extern int back_red, back_green, back_blue;
extern int grid_red, grid_green, grid_blue;
extern int grid_minor_red, grid_minor_green, grid_minor_blue;
extern float Xrot, Yrot, Zrot;
extern float Xtrans, Ytrans, Ztrans;
extern QStringList i_comments, i_reference, i_author, i_specimen, i_provenance, i_classification_name, i_classification_rank, i_title; //info stuff
extern bool vaxml_mode;
extern int ScaleBallColour[3]; //info stuff
extern float ScaleBallScale; //resize applied to ball - for scale calc
extern float globalrescale;
extern float mm_per_unit;
extern float ScaleMatrix[16];
//bounding box on import VAXML stuff
extern float minx, maxx, miny, maxy, minz, maxz;
extern bool firstobject;
extern int model_ktr;
extern QString STLHash;
extern QMatrix4x4 globalmatrix;
extern double applicationScaleX;
extern double applicationScaleY;
extern QScreen *currentScreen;
extern QList<QScreen *> availableScreens;

#endif // GLOBALS_H
