#include "positionclickhandler.h"
#include "globals.h"
#include "mainwindow.h"
#include "scalegridoverlay.h"

PositionClickHandler::PositionClickHandler(ScaleGridOverlay *overlay, QObject *parent)
    : QObject(parent)
{
    this->overlay = overlay;
    clickCount = 0;
    connect(mainWindow->gl3widget, &GlWidget::worldPositionClicked,
            this, &PositionClickHandler::ClickHandler);
}


void PositionClickHandler::ClickHandler(QVector3D pos, bool hit, int objectIndex)
{

    if (!hit) return;
    overlay->setMarker(clickCount % 2, pos, objectIndex);
    clickCount++;
    if (clickCount > 2) clickCount = 1; // second click onwards: replace oldest
}

float PositionClickHandler::worldDistanceMm(const QVector3D &a, const QVector3D &b)
{
    // Distance in post-globalMatrix scene units
    float sceneUnits = (b - a).length();

    // Convert to mm: scene units are scaled by globalRescale relative to
    // raw model units, and raw model units convert to mm via mmPerUnit
    return sceneUnits * mmPerUnit / globalRescale;
}