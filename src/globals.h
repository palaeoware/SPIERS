#ifndef __GLOBALS_H__
#define __GLOBALS_H__
#include <QString>
#include <QList>
#include <QTransform>
#include <QListWidget>
#include <QGraphicsScene>
#include <QPointF>
#include <QMenu>
#include <QLabel>
#include <QGraphicsItemGroup>
#include <QPushButton>
#include <QCursor>

class ImageData 
{
	public:
	ImageData(QString fn);
	QString FileName;
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
	MarkerData(QRectF *startrect, int shp);
	QRectF *markerRect;
	int shape;
	QGraphicsItem *markerPointer;
	QPointF linePoint;
	qreal dx, dy;
};
extern QList <QGraphicsLineItem *> linePointers;
extern QList <MarkerData *> markers;
extern int selectedMarker;
extern QList <ImageData *> ImageList;
extern QList <PropogationData *> Propogation;
extern int CurrentImage;
extern qreal CurrentScale;
extern QListWidget *markerList, *fileList;
extern void ForceRedrawImage();
extern int markersUp, markersLocked, setupFlag;
extern int cropUp, infoChecked, autoMarkersUp;
extern int cropping;
extern int scenedx,scenedy;
extern QRect *CropArea, *GridOutline, *autoEdgeOne, *autoEdgeTwo;
extern QStringList RecentFileList;
extern QLabel *infoLabel;
extern QPushButton *add, *swapbutton, *removeM, *executeAlign, *resetImage ;
extern void WriteSuperGlobals();
extern void ReadSuperGlobals();
extern void showInfo(int, int);
extern void RecentFile(QString);
extern QString FullSettingsFileName;
extern QGraphicsItemGroup *autoMarkersGroup;
extern QGraphicsRectItem *rectPointer, *suRectPointer, *suRectPointer2;
extern QGraphicsRectItem *amRectPointer;
extern QTransform aM, setupM, setupM2;
extern int flagup, flagupselection;
//extern QCursor *rotate2c;

#endif // __GLOBALS_H__
