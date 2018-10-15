#ifndef __MYGRAPHICSVIEW_H__
#define __MYGRAPHICSVIEW_H__

#include <QGraphicsView>

class mygraphicsview : public QGraphicsView
{
Q_OBJECT
	public:
	mygraphicsview();
	
	protected:
	void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif 
