#ifndef SPV_H
#define SPV_H

#include <QList>
#include <QStringList>

class SVObject;

/**
 * @brief The SPV class
 */
class SPV
{
public:
    SPV(int index, int FileVersion, int iDim, int jDim, int kDim);
    ~SPV();
    QList <SVObject *> ComponentObjects;
    float bright;
    QString filename, filenamenopath;
    int FileVersion;
    double *stretches;
    double PixPerMM, SlicePerMM, SkewDown, SkewLeft;
    int iDim, jDim, kDim;
    int size;
    int GridSize;
    int index;
    bool MirrorFlag;
    unsigned char *fullarray;
    unsigned char *AllData;
};

extern QList <SPV *> SPVs;

#endif // SPV_H
