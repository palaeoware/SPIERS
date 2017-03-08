#ifndef __MYRANGESCENE_H__
#define __MYRANGESCENE_H__

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QTime>

class myrangescene : public QGraphicsScene
{
public:
	myrangescene();
	void Refresh();
protected:
     void mousePressEvent(QGraphicsSceneMouseEvent *event);
     void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
	int button; //current button state - 0 = no, 1 = left down, 2 = right down
	int selectedtop;
	int selectedbottom;
    void MouseUp();
    void DoMouse(int x,int y,int PressedButton);
    int closestbase(int x, int *dist);
    int closesttop(int x, int *dist);
    int LimitTop(int x);
	int LimitBase(int x);
};

extern myrangescene *rangescene;


#endif // __MYRANGESCENE_H__
