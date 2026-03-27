#include "scalegridoverlay.h"
#include "gl3widget.h"

ScaleGridOverlay::ScaleGridOverlay(GlWidget *glWidget, QWidget *parent)
    : QWidget(parent)
    , glWidget(glWidget)
{
    // Transparent background — lets the OpenGL widget show through
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

void ScaleGridOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    qDebug()<<"Here";
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Simple test: one horizontal line across the centre of the widget
    int cx = width()  / 2;
    int cy = height() / 2;

    painter.setPen(QPen(Qt::white, 1.0));
    painter.drawLine(0, cy, width(), cy);
}
