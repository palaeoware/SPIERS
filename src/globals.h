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

class ImageData
{
public:
    ImageData(QString fn);
    QString fileName;
    QTransform m;
    bool hidden;
    int format;
};

class PropogationData
{
public:
    int transformation;
    qreal value;
};

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
extern int flagup, flagupselection;

extern QGraphicsItemGroup *autoMarkersGroup;
extern QGraphicsRectItem *rectPointer, *suRectPointer, *suRectPointer2;
extern QGraphicsRectItem *amRectPointer;
extern QLabel *infoLabel;
extern QList <QGraphicsLineItem *> linePointers;
extern QList <MarkerData *> markers;
extern QList <ImageData *> imageList;
extern QList <PropogationData *> propogation;
extern QListWidget *markerList, *fileList;
extern QPushButton *add, *swapButton, *removeMarker, *executeAlign, *resetImage ;
extern qreal currentScale;
extern QRect *cropArea, *gridOutline, *autoEdgeOne, *autoEdgeTwo;
extern QString fullSettingsFileName;
extern QStringList recentFileList;
extern QTransform aM, setupM, setupM2;

#endif // __GLOBALS_H__
