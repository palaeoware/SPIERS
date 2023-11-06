#include "beamhardeningcentericon.h"
#include "myscene.h"
#include "globals.h"

BeamHardeningCenterIcon::BeamHardeningCenterIcon()
{
    centerVisible = false;
    center1 = nullptr;
    center2 = nullptr;
    radiusItem = nullptr;
    lastX = 0;
    lastY = 0;
}

void BeamHardeningCenterIcon::ShowCenter(bool show)
{
    centerVisible = show;
    PlaceCenter(lastX, lastY, lastRadius);
}

void BeamHardeningCenterIcon::PlaceCenter(int x, int y, int radius)
{

    lastX = x;
    lastY = y;
    lastRadius = radius;
    int n;
    QPen MyPen;
    qreal rcms = static_cast<qreal>(ColMonoScale);
    qreal rx1, rx2, ry1, ry2;
    int lx, ly;

    if (center1!=nullptr)
    {
        scene->removeItem(center1);
        delete(center1);
        center1=nullptr;
    }
    if (center2!=nullptr)
    {
        scene->removeItem(center2);
        delete(center2);
        center2=nullptr;
    }

    if (radiusItem!=nullptr)
    {
        scene->removeItem(radiusItem);
        delete(radiusItem);
        radiusItem=nullptr;
    }

    if (Active == false || centerVisible==false) return;

    MyPen.setCosmetic(true);
    MyPen.setColor(QColor(255, 255, 255));
    MyPen.setWidth(3);

    rx1 = static_cast<qreal>(x-3);
    rx2 = static_cast<qreal>(x+3);
    ry1 = static_cast<qreal>(y-3);
    ry2 = static_cast<qreal>(y + 3);
    center1 = scene->addLine( rx1+.5, ry1+.5, rx2+.5, ry2+.5, MyPen);
    center1->setZValue(1);

    ry1 = static_cast<qreal>(y+3);
    ry2 = static_cast<qreal>(y-3);
    center2 = scene->addLine( rx1+.5, ry1+.5, rx2+.5, ry2+.5, MyPen);
    center2->setZValue(1);

    rx1 = static_cast<qreal>(x+.5);
    ry1 = static_cast<qreal>(y+.5);
    radiusItem = scene->addEllipse(rx1 - (qreal)radius/2.0, ry1- (qreal)radius/2.0, radius, radius, MyPen);

}
