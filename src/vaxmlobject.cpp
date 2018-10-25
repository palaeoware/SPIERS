#include "vaxmlobject.h"

/**
 * @brief VAXMLObject::VAXMLObject
 */
VAXMLObject::VAXMLObject()
{
    name = "[Unnamed object]";
    key = 0;
    visible = true;
    ingroup = "";
    file = "";
    url = "";

    red = 255;
    green = 255;
    blue = 255;

    matrix[0] = 1.0;
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = 1.0;
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = 0.0;
    matrix[9] = 0.0;
    matrix[10] = 1.0;
    matrix[11] = 0.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = 0.0;
    matrix[15] = 1.0;

    transparency = 0;
    position = -1;
}
