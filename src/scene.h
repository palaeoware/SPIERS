#ifndef __SCENE_H__
#define __SCENE_H__
#include <QGraphicsScene>
#include "globals.h"

class CustomScene : public QGraphicsScene
{
	public:
	 CustomScene();
	protected:
	 void mousePressEvent(QGraphicsSceneMouseEvent *event);
	 void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	 void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	private:
	 int selection;
};

#endif // __SCENE_H__
