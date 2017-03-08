#ifndef __UNDO_H__
#define __UNDO_H__
#include <QImage>
#include <QByteArray>
#include <QList>
#include "mainwindowimpl.h"
#include "globals.h"
#include "curves.h"
//max amount of memory (roughly) for buffer

extern int TotalUndoSize;

class UndoDataObject
{
	public:
	UndoDataObject(int type, const int curve_index);
	int Type; //-3=masks, -2=locks, -1=curves, 0+ = segments (seg number)
	QByteArray StoredData;	
	QImage StoredImage;
	PointList CurvePoints;
	int CurveNumber;
	int FileNumber;
    int undosize;

};

class UndoEvent
{
	public:
    UndoEvent(int Dfrom, int Dto);
	~UndoEvent();
	void Undo(MainWindowImpl *m);
	void Redo(MainWindowImpl *m);
    QString Type;
	int FileNumber; //if this ISN'T -1 then it's index of file moved to - a move event in fact
	int FileTo; //move To this (for redo)
	QList <struct UndoDataObject *> DataObjects;
};

extern QList <UndoEvent *> UndoEvents;
extern QList <UndoEvent *> RedoEvents;
#endif // __UNDO_H__
