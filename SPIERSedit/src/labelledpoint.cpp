#include "labelledpoint.h"

LabelledPoint::LabelledPoint(int xpos, int ypos, int slice, int seg)
{
    x = xpos;
    y = ypos;
    z = slice;
    segment = seg;
}
