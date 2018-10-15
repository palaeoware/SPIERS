#ifndef __CURVES_H__
#define __CURVES_H__

#include <QImage>
#include <QGraphicsScene>
#include "mainwindowimpl.h"

//maybe oneday make this modifyable?
#define TRIANGLE_DENSITY 100

extern void AddNode();
extern void KillNode(MainWindowImpl *th);
extern QList <QGraphicsItem *> MarkerList;
extern int FindClosestNode(double X, double Y);
extern void DrawCurve(int c, int mycol, int file, QImage *Thresh);
extern void DrawCurveOutput(int c, int file, uchar *fullarray, QList <bool> *UseMasks, bool remove);
extern void DrawCurveMarkers(QGraphicsScene *scene);
extern void PopulateTriangleList(int OutObject, int firstfile, int lastfile, QList<double>*stretches , int resamps, QVector<double> *TrigArray, int *TrigCount);

#endif // __CURVES_H__
