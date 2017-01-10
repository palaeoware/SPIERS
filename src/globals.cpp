#include "globals.h"
//ImageData Constructor
ImageData::ImageData(QString fn)
{
	FileName=fn;
}

MarkerData::MarkerData(QRectF *startrect, int shp)
{
	markerRect=startrect;
	shape=shp;
	markerPointer=NULL;
}

QList <QGraphicsLineItem *> linePointers;
QList <MarkerData *> markers;
QList <ImageData *> ImageList;
QList <PropogationData *> Propogation;
//MarkerData *append;
int CurrentImage;
int selectedMarker;
QListWidget *markerList, *fileList;
int markersUp, markersLocked, setupFlag;
int cropUp, infoChecked, autoMarkersUp;
qreal CurrentScale;
QRect *CropArea, *GridOutline, *autoEdgeOne, *autoEdgeTwo;
QStringList RecentFileList;
void WriteSuperGlobals();
void showInfo(int x, int y);
void ReadSuperGlobals();
void RecentFile(QString fname);
QLabel *infoLabel;
int cropping;
QString FullSettingsFileName;
int scenedx,scenedy;
QGraphicsItemGroup *autoMarkersGroup;
QPushButton *add, *swapbutton, *removeM, *executeAlign, *resetImage ;
QTransform aM, setupM, setupM2;
QGraphicsRectItem *amRectPointer, *rectPointer, *suRectPointer, *suRectPointer2;
int flagup=0, flagupselection=-1;

