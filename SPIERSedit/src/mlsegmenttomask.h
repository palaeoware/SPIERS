#ifndef MLSEGMENTTOMASK_H
#define MLSEGMENTTOMASK_H

#include <QVector>

class mlSegmentToMask
{
public:
    mlSegmentToMask();

    void execute(int segID, int maskID, const QVector<int> &segMap, int directionPairCount, int scoreThreshold);
};

#endif // MLSEGMENTTOMASK_H
