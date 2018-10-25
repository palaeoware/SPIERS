/*********************************************

SPIERSedit 2: memory.cpp

Functions relating to allocating / freeing GA objects

**********************************************/

#include "globals.h"
#include <stdio.h>
#include <QMessageBox>
#include <QString>


void FreeSegment(int n)
{
	//delete(GA.at(n));
	GA.removeAt(n);
}

void NewSegment(int n) //allocs to new seg at position n, returns index of it
{
	GA.insert(n,new QImage);	
}
