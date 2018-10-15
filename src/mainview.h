#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>

class mainview : public QGraphicsView
{
    Q_OBJECT
public:
    explicit mainview(QObject *parent);
    
protected:
void wheelEvent(QWheelEvent *event);
bool eventFilter(QObject *obj, QEvent *event);

signals:
    
public slots:
    
};

#endif // MAINVIEW_H
