#include "spv.h"
#include "SPIERSviewglobals.h"
#include <QDebug>

SPV::SPV(int Index, int version, int x, int y, int z)
{
     index=Index;
     FileVersion=version;
     iDim=x;
     jDim=y;
     kDim=z;
     size=iDim*jDim;
     GridSize=(iDim/GRIDSIZE+1) * (jDim/GRIDSIZE+1);
     //qDebug()<<"Grid size is "<<GridSize;
     fullarray=0;
     stretches=0;
}

SPV::~SPV()
{
    if (stretches) free(stretches);
}


QList <SPV *> SPVs;
