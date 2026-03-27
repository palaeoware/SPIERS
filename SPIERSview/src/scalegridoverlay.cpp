#include "scalegridoverlay.h"
#include "gl3widget.h"
#include "globals.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
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
// Format an absolute mm value into the most readable unit string
// ---------------------------------------------------------------------------
static QString formatLabel(float valueMm, float majorSpacing)
{
    float absVal = std::abs(valueMm);

    if (majorSpacing >= 10000.0f)
    {
        float v = absVal / 1000.0f;
        if (v == static_cast<int>(v))
            return QString("%1m").arg(static_cast<int>(v));
        return QString("%1m").arg(v, 0, 'g', 4);
    }
    else if (majorSpacing >= 10.0f)
    {
        float v = absVal / 10.0f;
        if (v == static_cast<int>(v))
            return QString("%1cm").arg(static_cast<int>(v));
        return QString("%1cm").arg(v, 0, 'g', 4);
    }
    else
    {
        float v = absVal;
        if (v == static_cast<int>(v))
            return QString("%1mm").arg(static_cast<int>(v));
        return QString("%1mm").arg(v, 0, 'g', 4);
    }
}

// ---------------------------------------------------------------------------
// Draw a label with a background rectangle for legibility
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

void ScaleGridOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    // ------------------------------------------------------------------
    // Nothing to show
    // ------------------------------------------------------------------
    if (!showScaleGrid) return;

    // ------------------------------------------------------------------
    // Not in orthographic mode — show warning and bail
    // ------------------------------------------------------------------
    if (!mainWindow->ui->actionOrthographic_View->isChecked())
    {
        QFont font;
        font.setPointSize(9);
        font.setBold(true);

        QPainter painter(this);
        painter.setFont(font);

        QColor bgColour(colorBackgroundRed, colorBackgroundGreen, colorBackgroundBlue);

        drawLabelWithBackground(painter,
                                "Scale grid requires orthographic mode",
                                10, 10 + QFontMetrics(font).ascent(),
                                Qt::red,
                                bgColour,
                                true);
        return;
    }

    // ------------------------------------------------------------------
    // Orthographic mode — draw grid
    // ------------------------------------------------------------------
    if (!glWidget) return;

    float widthMm  = glWidget->realOrthoWidthMm;
    float heightMm = glWidget->realOrthoHeightMm;

    if (widthMm <= 0.0f || heightMm <= 0.0f) return;

    // Major spacing
    float majorSpacing = 1.0f;
    float candidate = 1.0e9f;
    while (candidate > 1.0e-9f)
    {
        if (candidate <= widthMm / 1.5f)
        {
            majorSpacing = candidate;
            break;
        }
        candidate /= 10.0f;
    }
    float minorSpacing = majorSpacing / 10.0f;

    // Pixel scale
    float pxPerMm = static_cast<float>(width())  / widthMm;
    float pyPerMm = static_cast<float>(height()) / heightMm;

    float minorPxX = minorSpacing * pxPerMm;
    int   minorStep = (minorPxX < 20.0f) ? 2 : 1;

    float cx = width()  / 2.0f;
    float cy = height() / 2.0f;

    QColor majorColour(colorGridRed,      colorGridGreen,      colorGridBlue);
    QColor minorColour(colorGridMinorRed, colorGridMinorGreen, colorGridMinorBlue);
    QColor bgColour(colorBackgroundRed,   colorBackgroundGreen, colorBackgroundBlue);

    QFont font;
    font.setPointSize(9);
    font.setBold(false);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setFont(font);
    QFontMetrics fm(font);

    QFont boldFont = font;
    boldFont.setBold(true);
    QFontMetrics fmBold(boldFont);

    const int marginTop  = 2;
    const int marginLeft = 4;
    int textH = fm.ascent();

    // ------------------------------------------------------------------
    // Vertical lines (varying x)
    // ------------------------------------------------------------------
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
            int textW = isMajor ? fmBold.horizontalAdvance(label)
                                : fm.horizontalAdvance(label);

            float lx = xPx - textW / 2.0f;
            lx = qBound(0.0f, lx, static_cast<float>(width() - textW));

            bool collides = (lx < lastLabelRightX + 2.0f);
            if (collides) labelRow = 1 - labelRow;
            else          labelRow = 0;

            float ly = marginTop + textH + labelRow * (textH + 4);

            drawLabelWithBackground(painter, label, lx, ly,
                                    isMajor ? majorColour : minorColour,
                                    bgColour, isMajor);

            lastLabelRightX = lx + textW;
        }
    }

    // ------------------------------------------------------------------
    // Horizontal lines (varying y)
    // ------------------------------------------------------------------
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
            int textW = isMajor ? fmBold.horizontalAdvance(label)
                                : fm.horizontalAdvance(label);

            float lx = marginLeft;
            float ly = yPx + textH / 2.0f;

            ly = qBound(static_cast<float>(textH + 2),
                        ly,
                        static_cast<float>(height() - 2));

            drawLabelWithBackground(painter, label, lx, ly,
                                    isMajor ? majorColour : minorColour,
                                    bgColour, isMajor);
        }
    }
}
