#ifndef BEAMHARDENINGCENTERICON_H
#define BEAMHARDENINGCENTERICON_H

#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>

class BeamHardeningCenterIcon
{
public:
    BeamHardeningCenterIcon();

    void ShowCenter(bool show);
    void PlaceCenter(int x, int y, int radius);
private:
    QGraphicsLineItem *center1, *center2;
    QGraphicsEllipseItem *radiusItem;
    bool centerVisible;
    int lastX, lastY, lastRadius;
};

#endif // BEAMHARDENINGCENTERICON_H
