#ifndef __MYPIXMAPITEM_H__
#define __MYPIXMAPITEM_H__
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QString>


class mypixmapitem : public QGraphicsPixmapItem
{
public:
	mypixmapitem(QString fname);
	void testmethod();
protected:
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);
	
};

extern mypixmapitem *MainImage;

#endif // __MYPIXMAPITEM_H__
