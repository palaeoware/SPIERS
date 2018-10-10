#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QString>
#include "globals.h"

extern void ShowImage(QGraphicsView *gv);
extern void InitImage(QGraphicsView *gv);
extern void DeleteDisplayObjects();
extern void ClearImages();
extern void MakeLinearGreyScale(int seg, int fnum, bool flag);
extern void MakeBlankGreyScale(int seg, int fnum, bool flag);
extern uchar GreyScalePixel(int w, int h, int r, int g, int b, int glob);
extern void MakePolyGreyScale(int seg, int fnum, bool flag);
extern void MakeRangeGreyScale(int seg, int fnum, bool flag);
//extern uchar GreyScalePixel(int w, int h, int r, int g, int b, int glob);
extern uchar PolyPixel(int w, int h, int seg);
extern uchar RangePixel(int w, int h, int bot, int top, double cen, double gra, int seg);
extern uchar GenPixel(int x, int y, int s);
extern double CalcPoly(unsigned char r, unsigned char g, unsigned char b, Segment *seg);
extern void SaveMainImage(QString fname);
#endif // __DISPLAY_H__
