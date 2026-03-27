#ifndef SCALEGRIDOVERLAY_H
#define SCALEGRIDOVERLAY_H

#include <QWidget>
#include <QPainter>

class GlWidget;

class ScaleGridOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit ScaleGridOverlay(GlWidget *glWidget, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GlWidget *glWidget;
};

#endif // SCALEGRIDOVERLAY_H
