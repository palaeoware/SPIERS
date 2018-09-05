#include "globals.h"

//ImageData Constructor
ImageData::ImageData(QString fn)
{
    fileName = fn;
}

MarkerData::MarkerData(QRectF *startRectangle, int shp)
{
    markerRect = startRectangle;
    shape = shp;
    markerPointer = NULL;
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
int flagup = 0, flagupselection = -1;

qreal currentScale;

QGraphicsItemGroup *autoMarkersGroup;
QGraphicsRectItem *amRectPointer, *rectPointer, *suRectPointer, *suRectPointer2;
QLabel *infoLabel;
QList <QGraphicsLineItem *> linePointers;
QList <MarkerData *> markers;
QList <ImageData *> imageList;
QList <PropogationData *> propogation;
QListWidget *markerList, *fileList;
QPushButton *add, *swapButton, *removeMarker, *executeAlign, *resetImage ;
QRect *cropArea, *gridOutline, *autoEdgeOne, *autoEdgeTwo;
QStringList recentFileList;
QString fullSettingsFileName;
QTransform aM, setupM, setupM2;

