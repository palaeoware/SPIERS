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
bool showMinorGridValues = false;

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
