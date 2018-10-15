#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <QByteArray>
#include <QLabel>

extern void PopulateOutputArray (int *FullOutArrayCount, QByteArray *FullOutArray, int *Count, int FirstOutputFile, int LastOutputFile, int Object, int awidth, int aheight,  QLabel *pb);
extern bool freached(int f, int to, int from);
extern uchar ResampleForLoRes(int X, int Y, QByteArray *a);
#endif // __OUTPUT_H__
