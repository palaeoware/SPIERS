#include "scene.h"
#include <math.h>

#include <QDebug>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>

CustomScene::CustomScene() : QGraphicsScene()
{
	return;
}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	//qDebug()<<"Mouse Press";
	QPointF position=event->scenePos();

	int x,y;

	x=(int)position.x();
	y=(int)position.y();

	if (event->button()==Qt::LeftButton)
	{
	float shortestDistance=1000.;
	int shortestMarker=11;
	for(int i=0;i<10;i++)
		{
		float distance;
		float xT=((float)x-markers[i]->x());
		xT=xT*xT;
		
		float yT=((float)y-markers[i]->y());
		yT=yT*yT;
		
		distance=sqrt(xT+yT);
		
		if(distance<shortestDistance)	{
										shortestMarker=i;
										shortestDistance=distance;
										}
		}
	selectedMarker=shortestMarker;
	markerList->setCurrentRow(selectedMarker);	
	}
	
	if(event->button()==Qt::RightButton)
	{
		markers[selectedMarker]->moveTo((double)x,(double)y);
	}
}
//have tried forcing select marker, doesn't work. Can't access redraw or make
//global function that accesses it, don't want to make redraw global because then
//half the globals would be variable. No signal for QList changed or anything to create
//custom slot, and custom scene is used to make scene, can't fina a way to make this refresh
//upon the movement of one of the markers.
//only thing I can think of is to make new list widget, for example, and make the signal of that 
//match the redraw function as its slot and then change the chosen list item at this point


void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
if (event->button() == Qt::LeftButton) qDebug()<<"Mouse Move";
	QPointF position = event->pos();
}// place your code here
