#ifndef POSITIONCLICKHANDLER_H
#define POSITIONCLICKHANDLER_H
#include <QObject>
#include <QVector3D>

#include "qtmetamacros.h"

class ScaleGridOverlay;

class PositionClickHandler: public QObject
{
    Q_OBJECT
public:
    PositionClickHandler(ScaleGridOverlay *overlay, QObject *parent);
private slots:
    void ClickHandler(QVector3D pos, bool hit, int objectIndex);
private:
    float worldDistanceMm(const QVector3D &a, const QVector3D &b);
    ScaleGridOverlay *overlay;
    QVector3D clickPoints[2];
    int       clickCount = 0;
};

#endif // POSITIONCLICKHANDLER_H
