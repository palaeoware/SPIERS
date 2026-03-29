#include "scalegridoverlay.h"
#include "gl3widget.h"
#include "globals.h"
#include "svobject.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
#include <QVector4D>
#include <cmath>

ScaleGridOverlay::ScaleGridOverlay(GlWidget *glWidget, QWidget *parent)
    : QWidget(parent)
    , glWidget(glWidget)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

// ---------------------------------------------------------------------------
// Public marker interface
// ---------------------------------------------------------------------------
void ScaleGridOverlay::setMarker(int index, QVector3D worldPos, int objectIndex)
{
    if (index < 0 || index > 1) return;
    markers[index].worldPos    = worldPos;
    markers[index].objectIndex = objectIndex;
    markers[index].valid       = true;

    // Store position in object local space so marker tracks rotation/translation
    if (objectIndex >= 0 && objectIndex < SVObjects.count())
    {
        QMatrix4x4 objMatrix;
        objMatrix.translate(0, 0, -1);
        objMatrix *= QMatrix4x4(SVObjects[objectIndex]->matrix);
        objMatrix *= globalMatrix;
        bool invertible = false;
        QMatrix4x4 inv = objMatrix.inverted(&invertible);
        markers[index].localPos = invertible ? (inv * worldPos) : worldPos;
    }
    else
    {
        markers[index].localPos = worldPos;
    }

    update();
}

void ScaleGridOverlay::clearMarkers()
{
    markers[0].valid = false;
    markers[1].valid = false;
    update();
}

// ---------------------------------------------------------------------------
// Project a raw world-space position to overlay pixel coordinates
// ---------------------------------------------------------------------------
QPointF ScaleGridOverlay::worldToScreen(const QVector3D &worldPos) const
{
    QMatrix4x4 vMatrix;
    vMatrix.setToIdentity();
    vMatrix.lookAt(QVector3D(glWidget->cameraX, glWidget->cameraY, glWidget->cameraZ),
                   QVector3D(glWidget->centerX, glWidget->centerY, glWidget->centerZ),
                   QVector3D(0, 1, 0));

    QMatrix4x4 mvp = glWidget->pMatrix * vMatrix;
    QVector4D clip = mvp * QVector4D(worldPos, 1.0f);

    if (qAbs(clip.w()) < 1e-6f) return QPointF(-1, -1);
    QVector3D ndc = clip.toVector3D() / clip.w();

    float fbX = (ndc.x() + 1.0f) * 0.5f * glWidget->xdim;
    float fbY = (1.0f - ndc.y()) * 0.5f * glWidget->ydim;
    return QPointF(fbX / applicationScaleX, fbY / applicationScaleY);
}

// ---------------------------------------------------------------------------
// Project a marker to screen — reconstructs world pos from local space
// using the object's CURRENT matrix, so it tracks rotation/translation
// ---------------------------------------------------------------------------
QPointF ScaleGridOverlay::markerToScreen(const Marker &marker) const
{
    QVector3D pos;
    if (marker.objectIndex >= 0 && marker.objectIndex < SVObjects.count())
    {
        QMatrix4x4 objMatrix;
        objMatrix.translate(0, 0, -1);
        objMatrix *= QMatrix4x4(SVObjects[marker.objectIndex]->matrix);
        objMatrix *= globalMatrix;
        pos = objMatrix.map(marker.localPos);
    }
    else
    {
        pos = marker.worldPos;
    }
    return worldToScreen(pos);
}

// ---------------------------------------------------------------------------
// Draw a crosshair with a circle underlay for visibility
// ---------------------------------------------------------------------------
void ScaleGridOverlay::drawMarker(QPainter &painter, QPointF pos,
                                  const QColor &colour,
                                  const QColor &bgColour) const
{
    const int crossRadius  = 10;
    const int circleRadius = 6;

    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColour);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(pos, circleRadius, circleRadius);

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(colour, 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(QPointF(pos.x() - crossRadius, pos.y()),
                     QPointF(pos.x() + crossRadius, pos.y()));
    painter.drawLine(QPointF(pos.x(), pos.y() - crossRadius),
                     QPointF(pos.x(), pos.y() + crossRadius));

    painter.setPen(Qt::NoPen);
    painter.setBrush(colour);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(pos, 2, 2);
}

// ---------------------------------------------------------------------------
// Format grid label
// ---------------------------------------------------------------------------
static QString formatLabel(float valueMm, float majorSpacing)
{
    float absVal = std::abs(valueMm);
    if (majorSpacing >= 10000.0f)
    {
        float v = absVal / 1000.0f;
        if (v == static_cast<int>(v)) return QString("%1m").arg(static_cast<int>(v));
        return QString("%1m").arg(v, 0, 'g', 4);
    }
    else if (majorSpacing >= 10.0f)
    {
        float v = absVal / 10.0f;
        if (v == static_cast<int>(v)) return QString("%1cm").arg(static_cast<int>(v));
        return QString("%1cm").arg(v, 0, 'g', 4);
    }
    else
    {
        float v = absVal;
        if (v == static_cast<int>(v)) return QString("%1mm").arg(static_cast<int>(v));
        return QString("%1mm").arg(v, 0, 'g', 4);
    }
}

// ---------------------------------------------------------------------------
// Draw label with background rect
// ---------------------------------------------------------------------------
static void drawLabelWithBackground(QPainter &painter,
                                    const QString &label,
                                    float lx, float ly,
                                    const QColor &textColour,
                                    const QColor &bgColour,
                                    bool bold)
{
    QFont f = painter.font();
    f.setBold(bold);
    painter.setFont(f);
    QFontMetrics fmLocal(f);

    QRect bounds = fmLocal.boundingRect(label);
    bounds.translate(static_cast<int>(lx), static_cast<int>(ly));
    bounds.adjust(-2, -1, 2, 1);

    painter.fillRect(bounds, bgColour);
    painter.setPen(QPen(textColour));
    painter.drawText(QPointF(lx, ly), label);

    f.setBold(false);
    painter.setFont(f);
}

// ---------------------------------------------------------------------------
// paintEvent
// ---------------------------------------------------------------------------
void ScaleGridOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (!glWidget) return;

    QColor bgColour(colorBackgroundRed, colorBackgroundGreen, colorBackgroundBlue);

    // ------------------------------------------------------------------
    // Markers — always drawn regardless of grid state or mode
    // ------------------------------------------------------------------
    {
        QPainter painter(this);

        const QColor markerColour0(255, 220, 0);
        const QColor markerColour1(0, 220, 255);
        const QColor lineColour(255, 255, 255);

        QPointF screenPos[2];
        for (int m = 0; m < 2; m++)
        {
            if (!markers[m].valid) continue;
            screenPos[m] = markerToScreen(markers[m]);
            drawMarker(painter, screenPos[m],
                       m == 0 ? markerColour0 : markerColour1,
                       bgColour);
        }

        if (markers[0].valid && markers[1].valid)
        {
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(QPen(lineColour, 1.0, Qt::DashLine));
            painter.drawLine(screenPos[0], screenPos[1]);

            // Distance — use original worldPos values (measurement fixed at click time)
            float sceneUnits = (markers[1].worldPos - markers[0].worldPos).length();
            float distMm = sceneUnits * static_cast<float>(mmPerUnit)
                           / static_cast<float>(globalRescale);

            QString distLabel;
            if (distMm >= 10000.0f)
                distLabel = QString("%1 m").arg(static_cast<double>(distMm / 1000.0f), 0, 'f', 2);
            else if (distMm >= 10.0f)
                distLabel = QString("%1 cm").arg(static_cast<double>(distMm / 10.0f), 0, 'f', 2);
            else
                distLabel = QString("%1 mm").arg(static_cast<double>(distMm), 0, 'f', 2);

            QPointF mid((screenPos[0].x() + screenPos[1].x()) / 2.0,
                        (screenPos[0].y() + screenPos[1].y()) / 2.0);

            QFont labelFont;
            labelFont.setPointSize(10);
            labelFont.setBold(true);
            painter.setFont(labelFont);
            QFontMetrics labelFm(labelFont);

            int textW = labelFm.horizontalAdvance(distLabel);
            int textH = labelFm.ascent();

            float lx = static_cast<float>(mid.x()) - textW / 2.0f;
            float ly = static_cast<float>(mid.y()) + textH / 2.0f;
            lx = qBound(0.0f, lx, static_cast<float>(width()  - textW));
            ly = qBound(static_cast<float>(textH + 2), ly, static_cast<float>(height() - 2));

            QRect bounds = labelFm.boundingRect(distLabel);
            bounds.translate(static_cast<int>(lx), static_cast<int>(ly));
            bounds.adjust(-3, -2, 3, 2);
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.fillRect(bounds, bgColour);
            painter.setPen(QPen(lineColour));
            painter.drawText(QPointF(lx, ly), distLabel);

            QFont baseFont;
            baseFont.setPointSize(9);
            painter.setFont(baseFont);
        }
    }

    // ------------------------------------------------------------------
    // Grid
    // ------------------------------------------------------------------
    if (!showScaleGrid) return;

    if (!mainWindow->ui->actionOrthographic_View->isChecked())
    {
        QFont font; font.setPointSize(9); font.setBold(true);
        QPainter painter(this);
        painter.setFont(font);
        drawLabelWithBackground(painter, "Scale grid requires orthographic mode",
                                10, 10 + QFontMetrics(font).ascent(),
                                Qt::red, bgColour, true);
        return;
    }

    float widthMm  = glWidget->realOrthoWidthMm;
    float heightMm = glWidget->realOrthoHeightMm;
    if (widthMm <= 0.0f || heightMm <= 0.0f) return;

    float majorSpacing = 1.0f;
    float candidate = 1.0e9f;
    while (candidate > 1.0e-9f)
    {
        if (candidate <= widthMm / 1.5f) { majorSpacing = candidate; break; }
        candidate /= 10.0f;
    }
    float minorSpacing = majorSpacing / 10.0f;

    float pxPerMm  = static_cast<float>(width())  / widthMm;
    float pyPerMm  = static_cast<float>(height()) / heightMm;
    float minorPxX = minorSpacing * pxPerMm;
    int   minorStep = (minorPxX < 20.0f) ? 2 : 1;

    float cx = width()  / 2.0f;
    float cy = height() / 2.0f;

    QColor majorColour(colorGridRed,      colorGridGreen,      colorGridBlue);
    QColor minorColour(colorGridMinorRed, colorGridMinorGreen, colorGridMinorBlue);

    QFont font; font.setPointSize(9); font.setBold(false);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setFont(font);
    QFontMetrics fm(font);
    QFont boldFont = font; boldFont.setBold(true);
    QFontMetrics fmBold(boldFont);

    const int marginTop  = 2;
    const int marginLeft = 4;
    int textH = fm.ascent();

    // Vertical lines
    int stepsX = static_cast<int>(std::ceil((widthMm / 2.0f) / minorSpacing)) + 1;
    float lastLabelRightX = -9999.0f;
    int   labelRow = 0;

    for (int i = -stepsX; i <= stepsX; i++)
    {
        if (i % minorStep != 0) continue;
        float xMm = i * minorSpacing;
        float xPx = cx + xMm * pxPerMm;
        if (xPx < 0 || xPx > width()) continue;
        bool isMajor = (i % 10 == 0);

        if (isMajor || showMinorGridLines)
        {
            painter.setPen(QPen(isMajor ? majorColour : minorColour, isMajor ? 2 : 1));
            painter.drawLine(QPointF(xPx, 0), QPointF(xPx, height()));
        }

        bool drawLabel = isMajor || (showMinorGridValues && showMinorGridLines);
        if (drawLabel)
        {
            QString label = (i == 0) ? QString("0") : formatLabel(xMm, majorSpacing);
            int textW = isMajor ? fmBold.horizontalAdvance(label) : fm.horizontalAdvance(label);
            float lx = xPx - textW / 2.0f;
            lx = qBound(0.0f, lx, static_cast<float>(width() - textW));
            bool collides = (lx < lastLabelRightX + 2.0f);
            if (collides) labelRow = 1 - labelRow;
            else          labelRow = 0;
            float ly = marginTop + textH + labelRow * (textH + 4);
            drawLabelWithBackground(painter, label, lx, ly,
                                    isMajor ? majorColour : minorColour, bgColour, isMajor);
            lastLabelRightX = lx + textW;
        }
    }

    // Horizontal lines
    int stepsY = static_cast<int>(std::ceil((heightMm / 2.0f) / minorSpacing)) + 1;

    for (int i = -stepsY; i <= stepsY; i++)
    {
        if (i % minorStep != 0) continue;
        float yMm = i * minorSpacing;
        float yPx = cy + yMm * pyPerMm;
        if (yPx < 0 || yPx > height()) continue;
        bool isMajor = (i % 10 == 0);

        if (isMajor || showMinorGridLines)
        {
            painter.setPen(QPen(isMajor ? majorColour : minorColour, isMajor ? 2 : 1));
            painter.drawLine(QPointF(0, yPx), QPointF(width(), yPx));
        }

        bool drawLabel = isMajor || (showMinorGridValues && showMinorGridLines);
        if (drawLabel)
        {
            QString label = (i == 0) ? QString("0") : formatLabel(yMm, majorSpacing);
            int textW = isMajor ? fmBold.horizontalAdvance(label) : fm.horizontalAdvance(label);
            float lx = marginLeft;
            float ly = yPx + textH / 2.0f;
            ly = qBound(static_cast<float>(textH + 2), ly, static_cast<float>(height() - 2));
            drawLabelWithBackground(painter, label, lx, ly,
                                    isMajor ? majorColour : minorColour, bgColour, isMajor);
        }
    }
}
