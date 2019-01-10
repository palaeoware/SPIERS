#ifndef WATERSHED_H
#define WATERSHED_H

#include <QImage>

void watershed(QImage *imagePointer);
QImage watershedByMarker(QImage srcImage);
QImage watershedByGradient(QImage srcImage);

#endif // WATERSHED_H
