#ifndef __MYSCENE_H__
#define __MYSCENE_H__

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QTime>

class myscene : public QGraphicsScene
{
public:
	myscene();
protected:
     void mousePressEvent(QGraphicsSceneMouseEvent *event);
     void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
	int button; //current button state - 0 = no, 1 = left down, 2 = right down
	void DoAction(int x, int y);
    void MouseUp();
    void DoMouse(int x,int y,int PressedButton);
    QTime mousetimer;
    int CurrentClosestNode;
private slots:
     void ScreenUpdate();
};

extern myscene *scene;


#endif // __MYSCENE_H__
