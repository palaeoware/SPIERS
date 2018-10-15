#include <QDebug>

#include "spv.h"
#include "globals.h"

/**
 * @brief SPVs
 */
QList <SPV *> SPVs;

/**
 * @brief SPV::SPV
 * @param Index
 * @param version
 * @param x
 * @param y
 * @param z
 */
SPV::SPV(int Index, int version, int x, int y, int z)
{
    index = Index;
    FileVersion = version;
    iDim = x;
    jDim = y;
    kDim = z;
    size = iDim * jDim;
    GridSize = (iDim / GRIDSIZE + 1) * (jDim / GRIDSIZE + 1);
    //qDebug()<<"Grid size is "<<GridSize;
    fullarray = nullptr;
    stretches = nullptr;
}

/**
 * @brief SPV::~SPV
 */
SPV::~SPV()
{
    if (stretches) free(stretches);
}
