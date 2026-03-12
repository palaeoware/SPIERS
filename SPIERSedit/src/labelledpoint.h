#ifndef LABELLEDPOINT_H
#define LABELLEDPOINT_H

#include <QObject>

class LabelledPoint
{
public:
    LabelledPoint(int xpos, int ypos, int slice, int seg);
    int x;
    int y;
    int z;
    int segment;
};

#endif // LABELLEDPOINT_H
