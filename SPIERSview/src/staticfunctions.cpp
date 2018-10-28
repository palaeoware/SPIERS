#include "staticfunctions.h"

#include <QMatrix4x4>

/**
 * @brief StaticFunctions::TransposeMatrix
 * @param m
 */
void StaticFunctions::transposeMatrix(float *matrix)
{
    QMatrix4x4 thematrix(matrix);
    thematrix = thematrix.transposed();
    thematrix.copyDataTo(matrix);
}

/**
 * @brief StaticFunctions::invertEndian
 * @param data
 * @param count
 */
void StaticFunctions::invertEndian(unsigned char *data, int count)
{
#ifdef _BIG_ENDIAN
    unsigned char newdata[8]; //max size is double
    int n;

    for (n = 0; n < count; n++) newdata[count - 1 - n] = data[n];
    for (n = 0; n < count; n++) data[n] = newdata[n];
#else
    Q_UNUSED(data)
    Q_UNUSED(count)
#endif
    return;
}
