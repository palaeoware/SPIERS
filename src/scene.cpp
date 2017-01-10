#include "scene.h"
#include <math.h>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include "mainwindowimpl.h"
#include <QLabel>
#include <QMessageBox>


CustomScene::CustomScene() : QGraphicsScene()
{
	selection=-1;
	return;
}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(markersUp==0||markersLocked==1||setupFlag==1)return;
	QPointF position=event->scenePos();

	int x,y;

	x=(int)position.x();
	y=(int)position.y();

	if (event->button()==Qt::LeftButton)
	{ 
        float shortestDistance=1000.;
        int shortestMarker=-1;
	for(int i=0;i<markers.count();i++)
		{
		float distance=100., xT=0., yT=0.;
		
		if(markers[i]->shape==0)
			{
			xT=((float)x-markers[i]->markerRect->x());
			xT=xT*xT;
		
			yT=((float)y-(float)markers[i]->markerRect->y());
			yT=yT*yT;
			
			
			distance=sqrt(xT+yT);

                        }
			
		if(markers[i]->shape==1)
		{
			float xT1,xT2,xT3,yT1,yT2,yT3,distances[3];
			
			xT1=((float)x-((markers[i]->markerRect->x()+ markers[i]->linePoint.x())/2.));
			yT1=((float)y-((markers[i]->markerRect->bottom() + markers[i]->linePoint.y())/2.));
			distances[0]=sqrt((xT1*xT1)+(yT1*yT1));
			
			xT2=((float)x-markers[i]->markerRect->x());
			yT2=((float)y-markers[i]->markerRect->bottom());
			distances[1]=sqrt((xT2*xT2)+(yT2*yT2));
			
			xT3=((float)x-markers[i]->linePoint.x());
			yT3=((float)y-markers[i]->linePoint.y());
			distances[2]=sqrt((xT3*xT3)+(yT3*yT3));
			
			distance=distances[0];
			
			for(int i=0;i<3;i++)if(distance>distances[i])distance=distances[i];
		}
			
		if (distance<50.)
							{
								if(distance<shortestDistance)	{
                                                                                                shortestMarker=i;
                                                                                                shortestDistance=distance;
                                                                                                }
                                                        }
		}

        if (shortestMarker!=-1)
                {
                selectedMarker=shortestMarker;
                markerList->setCurrentRow(selectedMarker);//calls selected marker change slot which calls redrawimage()
                }
    }


	if(event->button()==Qt::RightButton)
	{
		markers[selectedMarker]->markerRect->moveTo((double)x,(double)y);
		TheMainWindow->RedrawImage();
	}


}


void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{


//qDebug()<<flagupselection;
QPointF position=event->scenePos();
int x=(int)position.x();
int y=(int)position.y();

//Update info box
if(infoChecked==1)
{
showInfo(x,y);
}

if(cropUp==1)
		{
                QApplication::restoreOverrideCursor();
                if(abs(y-CropArea->bottom())<50 && abs(x-CropArea->left())<50)QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
                else if(abs(y-CropArea->bottom())<50 && abs(x-CropArea->right())<50)QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
                else if(abs(y-CropArea->top())<50 && abs(x-CropArea->left())<50)QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
                else if(abs(y-CropArea->top())<50 && abs(x-CropArea->right())<50)QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
		else{
                                if(abs(x-CropArea->right())<50 && y>CropArea->top()&& y<CropArea->bottom())QApplication::setOverrideCursor(Qt::SizeHorCursor);
                                else if(abs(x-CropArea->left())<50 && y>CropArea->top()&& y<CropArea->bottom())QApplication::setOverrideCursor(Qt::SizeHorCursor);
                                else if(abs(y-CropArea->top())<50 && x<CropArea->right() && x>CropArea->left())QApplication::setOverrideCursor(Qt::SizeVerCursor);
                                else if(abs(y-CropArea->bottom())<50 && x<CropArea->right() && x>CropArea->left())QApplication::setOverrideCursor(Qt::SizeVerCursor);
                                else if(y>CropArea->top()&& y<CropArea->bottom()&& x<CropArea->right() && x>CropArea->left())QApplication::setOverrideCursor(Qt::SizeAllCursor);
                        }
		
	}


if(setupFlag==1)
    {
    if( flagupselection==-1)
                {
                int mappedX, mappedY, mappedX2, mappedY2;

                 //First attempt to map rectangle to mouse deleted. Far better way = invert the matrix and then map X and Y back to what they would have been on original thing....
                 QTransform setupMi, setupMi2;
                 if(setupM.isInvertible()&&setupM2.isInvertible()){setupMi=setupM.inverted();setupMi2=setupM2.inverted();}
                 else   {
                          QMessageBox::warning(0,"Error","Setup matrix is not invertible. You should never see this, please email", QMessageBox::Ok);
                          return;
                           }

                 setupMi.map(x,y,&mappedX,&mappedY);
                 setupMi2.map(x,y,&mappedX2,&mappedY2);

                 int qW=autoEdgeOne->width()/4;
                 int qH=autoEdgeOne->height()/4;

                 int qW2=autoEdgeTwo->width()/4;
                 int qH2=autoEdgeTwo->height()/4;

                 QApplication::restoreOverrideCursor();
                 if(abs(mappedY-autoEdgeOne->bottom())<qH && abs(mappedX-autoEdgeOne->left())<qW)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                 else if(abs(mappedY-autoEdgeOne->bottom())<qH && abs(mappedX-autoEdgeOne->right())<qW)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                 else if(abs(mappedY-autoEdgeOne->top())<qH && abs(mappedX-autoEdgeOne->left())<qW)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                 else if(abs(mappedY-autoEdgeOne->top())<qH && abs(mappedX-autoEdgeOne->right())<qW)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                 else   {
                         if(abs(mappedX-autoEdgeOne->right())<qW && mappedY>autoEdgeOne->top()&& mappedY<autoEdgeOne->bottom())QApplication::setOverrideCursor(Qt::SizeHorCursor);
                         else if(abs(mappedX-autoEdgeOne->left())<qW && mappedY>autoEdgeOne->top()&& mappedY<autoEdgeOne->bottom())QApplication::setOverrideCursor(Qt::SizeHorCursor);
                         else if(abs(mappedY-autoEdgeOne->top())<qH && mappedX<autoEdgeOne->right() && mappedX>autoEdgeOne->left())QApplication::setOverrideCursor(Qt::SizeVerCursor);
                         else if(abs(mappedY-autoEdgeOne->bottom())<qH && mappedX<autoEdgeOne->right() && mappedX>autoEdgeOne->left())QApplication::setOverrideCursor(Qt::SizeVerCursor);
                         else if(mappedY>autoEdgeOne->top()&& mappedY<autoEdgeOne->bottom() && mappedX<autoEdgeOne->right() && mappedX>autoEdgeOne->left())QApplication::setOverrideCursor(Qt::SizeAllCursor);
                         else if(abs(mappedY2-autoEdgeTwo->bottom())<qH2 && abs(mappedX2-autoEdgeTwo->left())<qW2)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                         else if(abs(mappedY2-autoEdgeTwo->bottom())<qH2 && abs(mappedX2-autoEdgeTwo->right())<qW2)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                         else if(abs(mappedY2-autoEdgeTwo->top())<qH2 && abs(mappedX2-autoEdgeTwo->left())<qW2)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                         else if(abs(mappedY2-autoEdgeTwo->top())<qH2 && abs(mappedX2-autoEdgeTwo->right())<qW2)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                         else   {
                                 if(abs(mappedX2-autoEdgeTwo->right())<qW2 && mappedY2>autoEdgeTwo->top()&& mappedY2<autoEdgeTwo->bottom())QApplication::setOverrideCursor(Qt::SizeHorCursor);
                                 else if(abs(mappedX2-autoEdgeTwo->left())<qW2 && mappedY2>autoEdgeTwo->top()&& mappedY2<autoEdgeTwo->bottom())QApplication::setOverrideCursor(Qt::SizeHorCursor);
                                 else if(abs(mappedY2-autoEdgeTwo->top())<qH2 && mappedX2<autoEdgeTwo->right() && mappedX2>autoEdgeTwo->left())QApplication::setOverrideCursor(Qt::SizeVerCursor);
                                 else if(abs(mappedY2-autoEdgeTwo->bottom())<qH2 && mappedX2<autoEdgeTwo->right() && mappedX2>autoEdgeTwo->left())QApplication::setOverrideCursor(Qt::SizeVerCursor);
                                 else if(mappedY2>autoEdgeTwo->top()&& mappedY2<autoEdgeTwo->bottom() && mappedX2<autoEdgeTwo->right() && mappedX2>autoEdgeOne->left())QApplication::setOverrideCursor(Qt::SizeAllCursor);
                                }




                        }
                }
    }

if(autoMarkersUp==1)
    {
     if(TheMainWindow->actionLock_File->isChecked())return;

     if( flagupselection==-1)
                {
                    int mappedX, mappedY;

                     //First attempt to map rectangle to mouse deleted. Far better way = invert the matrix and then map X and Y back to what they would have been on original thing....
                     QTransform aMi;

                     if(aM.isInvertible())aMi=aM.inverted();
                       else   {
                               QMessageBox::warning(0,"Error","Matrix is not invertible. You should never see this, please email", QMessageBox::Ok);
                               return;
                                }

                     aMi.map(x,y,&mappedX,&mappedY);

                     QApplication::restoreOverrideCursor();
                     if(abs(mappedY-GridOutline->bottom())<100 && abs(mappedX-GridOutline->left())<50)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                     else if(abs(mappedY-GridOutline->bottom())<100 && abs(mappedX-GridOutline->right())<50)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                     else if(abs(mappedY-GridOutline->top())<100 && abs(mappedX-GridOutline->left())<50)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                     else if(abs(mappedY-GridOutline->top())<100 && abs(mappedX-GridOutline->right())<50)QApplication::setOverrideCursor(Qt::OpenHandCursor);
                     else if(mappedY>GridOutline->top()&& mappedY<GridOutline->bottom() && mappedX<GridOutline->right() && mappedX>GridOutline->left())QApplication::setOverrideCursor(Qt::SizeAllCursor);

                }
    }

if(event->buttons() == Qt::LeftButton && cropUp==1)
        {
		if (selection==-1)
		{
                                if(abs(y-CropArea->bottom())<50 && abs(x-CropArea->left())<50)selection=1;
                                else if(abs(y-CropArea->bottom())<50 && abs(x-CropArea->right())<50)selection=2;
                                else if(abs(y-CropArea->top())<50 && abs(x-CropArea->left())<50)selection=3;
                                else if(abs(y-CropArea->top())<50 && abs(x-CropArea->right())<50)selection=4;
				else
				{
                                        if(abs(x-CropArea->right())<50)selection=5;
                                        else if(abs(x-CropArea->left())<50)selection=6;
                                        else if(abs(y-CropArea->top())<50)selection=7;
                                        else if(abs(y-CropArea->bottom())<50)selection=8;
					else selection=9;
				}
		}

//**************************************************************
//Here is where I need to set hard limits for position of crop box

		QRect OldRect=*CropArea;
        int w=CropArea->width();
        int h=CropArea->height();

        if(selection==1)CropArea->setBottomLeft(position.toPoint());
		else if(selection==2)CropArea->setBottomRight(position.toPoint());
		else if(selection==3)CropArea->setTopLeft(position.toPoint());
		else if(selection==4)CropArea->setTopRight(position.toPoint());
		else
		{
            if(selection==5)CropArea->setRight(x);
            else if(selection==6)CropArea->setLeft(x);
            else if(selection==7)CropArea->setTop(y);
            else if(selection==8)CropArea->setBottom(y);
			else if(selection==9)
				{
                QPoint topLeftCorner((x-(w/2)),(y-(h/2)));
				CropArea->moveTopLeft(topLeftCorner);
				}
		}
		

		if (CropArea->width()<20) *CropArea=OldRect;
		if (CropArea->height()<20) *CropArea=OldRect;

if (selection==9)
        {
            if (CropArea->right()>TheMainWindow->width){QPoint topLeftCorner((TheMainWindow->width-w),(y-(h/2)));CropArea->moveTopLeft(topLeftCorner);}
            if (CropArea->left()<0){QPoint topLeftCorner(0,(y-(h/2)));CropArea->moveTopLeft(topLeftCorner);}
            if (CropArea->top()<0){QPoint topLeftCorner(x-(w/2),0);CropArea->moveTopLeft(topLeftCorner);}
            if (CropArea->bottom()>TheMainWindow->height){QPoint topLeftCorner(x-(w/2),TheMainWindow->height-h);CropArea->moveTopLeft(topLeftCorner);}

            if (CropArea->left()<1 && CropArea->top()<1) *CropArea=OldRect;
            if (CropArea->right()>TheMainWindow->width && CropArea->top()<1) *CropArea=OldRect;
            if (CropArea->right()>TheMainWindow->width-2 && CropArea->bottom()>TheMainWindow->height-2) *CropArea=OldRect;
            if (CropArea->left()<1 && CropArea->bottom()>TheMainWindow->height-2) *CropArea=OldRect;
        }
        else
            {
            if (CropArea->right()>TheMainWindow->width)CropArea->setRight(TheMainWindow->width-1);
            if (CropArea->left()<0)CropArea->setLeft(0);
            if (CropArea->top()<0)CropArea->setTop(0);
            if (CropArea->bottom()>TheMainWindow->height)CropArea->setBottom(TheMainWindow->height-1);
            }

        TheMainWindow->RedrawJustCropBox();
	}

if(event->buttons() == Qt::LeftButton && autoMarkersUp==1)
    {

     if(TheMainWindow->actionLock_File->isChecked())return;
     flagup++;

     int flag=-1;
     int mappedX, mappedY;

     int B=GridOutline->bottom();
     int T=GridOutline->top();
     int L=GridOutline->left();
     int R=GridOutline->right();

     float w2, h2;

     w2=TheMainWindow->width/2.;
     h2=TheMainWindow->height/2;

     QTransform aMi;

     if(aM.isInvertible())aMi=aM.inverted();
     else   {
           QMessageBox::warning(0,"Error","Matrix is not invertible. You should never see this, please email", QMessageBox::Ok);
           return;
            }

     aMi.map(x,y,&mappedX,&mappedY);

     if(flagup==1)
     {
        QApplication::restoreOverrideCursor();

                if(abs(mappedY-B)<50 && abs(mappedX-L)<100){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=0;}
                else if(abs(mappedY-B)<50 && abs(mappedX-R)<100){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=1;}
                else if(abs(mappedY-T)<50 && abs(mappedX-L)<100){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=2;}
                else if(abs(mappedY-T)<50 && abs(mappedX-R)<100){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=3;}
                else if(mappedY>T && mappedY<B && mappedX<R && mappedX>L)
                                        {
                                            QApplication::setOverrideCursor(Qt::SizeAllCursor);
                                            flag=4;
                                        }
                else QApplication::restoreOverrideCursor();
                flagupselection=flag;
    }


    //Bottom Left
        if (flagupselection==0)
                    {
                    while(mappedX>L && mappedY>B){aM.translate(w2,h2).rotate(-1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX<L && mappedY<B){aM.translate(w2,h2).rotate(1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                     }
        //Bottom right
        if (flagupselection==1)
                    {
                    while(mappedX<R && mappedY>B){aM.translate(w2,h2).rotate(1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX>R && mappedY<B){aM.translate(w2,h2).rotate(-1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }
        //Top Left
        if (flagupselection==2)
                    {
                    while(mappedX>L && mappedY<T){aM.translate(w2,h2).rotate(1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX<L && mappedY>T){aM.translate(w2,h2).rotate(-1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }
        //Top Right
        if (flagupselection==3)
                    {
                    while(mappedX<R && mappedY<T){aM.translate(w2,h2).rotate(-1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX>R && mappedY>T){aM.translate(w2,h2).rotate(1.).translate(-1.*w2,-1.*h2);aMi=aM.inverted();aMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }

        //Translate
        if (flagupselection==4)
                    {

                    //Both below also work to get angle
                    //double angle=sin(acos(aM.m11()));
                    //double angle=sqrt(1-(aM.m11()*aM.m11()));

                    while(mappedX<(L+R)/2){
                                           double angle=atan2(aM.m12(),aM.m11());
                                           //If the angle is over 1 radian you have to correct the movement by just the figure after the decimal place.
                                           if(angle>1.)
                                           {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                           }
                                           else if (angle<-1.)
                                           {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                           }

                                           aM.translate(-1.,angle);
                                           aMi=aM.inverted();
                                           aMi.map(x,y,&mappedX,&mappedY);
                                            }
                    while(mappedX>(L+R)/2){
                                            double angle=atan2(aM.m12(),aM.m11());

                                           if(angle>1.)
                                           {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                           }
                                           else if (angle<-1.)
                                           {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                           }

                                            aM.translate(1.,-angle);
                                            aMi=aM.inverted();
                                            aMi.map(x,y,&mappedX,&mappedY);
                                            }
                    while(mappedY<(T+B)/2){
                                            double angle=atan2(aM.m12(),aM.m11());

                                         if(angle>1.)
                                           {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                           }
                                           else if (angle<-1.)
                                           {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                           }

                                            aM.translate(angle,-1);
                                            aMi=aM.inverted();
                                            aMi.map(x,y,&mappedX,&mappedY);
                                             }
                    while(mappedY>(T+B)/2){
                                            double angle=atan2(aM.m12(),aM.m11());

                                            if(angle>1.)
                                            {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                             }
                                             else if (angle<-1.)
                                             {
                                               int angleint=(int)angle;
                                               angle=angle- (float)angleint;
                                             }

                                            aM.translate(angle,1);
                                            aMi=aM.inverted();
                                            aMi.map(x,y,&mappedX,&mappedY);
                                             }

                    TheMainWindow->RedrawJustAM();
                    }

       }


if(event->buttons() == Qt::LeftButton && setupFlag==1)
{
    flagup++;
    int flag=-1;
    int mappedX, mappedY, mappedX2, mappedY2;

    int B=autoEdgeOne->bottom();
    int T=autoEdgeOne->top();
    int L=autoEdgeOne->left();
    int R=autoEdgeOne->right();

    int B2=autoEdgeTwo->bottom();
    int T2=autoEdgeTwo->top();
    int L2=autoEdgeTwo->left();
    int R2=autoEdgeTwo->right();


    QTransform setupMi, setupMi2;
    if(setupM.isInvertible()&&setupM2.isInvertible()){setupMi=setupM.inverted();setupMi2=setupM2.inverted();}
      else   {
              QMessageBox::warning(0,"Error","Setup matrix is not invertible. You should never see this, please email", QMessageBox::Ok);
              return;
               }

    setupMi.map(x,y,&mappedX,&mappedY);
    setupMi2.map(x,y,&mappedX2,&mappedY2);

    if(flagup==1)
    {
       QApplication::restoreOverrideCursor();

       int qW=autoEdgeOne->width()/4;
       int qH=autoEdgeOne->height()/4;
       int qW2=autoEdgeTwo->width()/4;
       int qH2=autoEdgeTwo->height()/4;

       if(abs(mappedY-B)<qH && abs(mappedX-L)<qW){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=0;}
        else if(abs(mappedY-B)<qH && abs(mappedX-R)<qW){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=1;}
        else if(abs(mappedY-T)<qH && abs(mappedX-L)<qW){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=2;}
        else if(abs(mappedY-T)<qH && abs(mappedX-R)<qW){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=3;}
        else   {
            if(abs(mappedX-R)<qW && mappedY>T&& mappedY<B){QApplication::setOverrideCursor(Qt::SizeHorCursor);flag=4;}
            else if(abs(mappedX-L)<qW && mappedY>T&& mappedY<B){QApplication::setOverrideCursor(Qt::SizeHorCursor);flag=5;}
            else if(abs(mappedY-T)<qH && mappedX<R && mappedX>L){QApplication::setOverrideCursor(Qt::SizeVerCursor);flag=6;}
            else if(abs(mappedY-B)<qH && mappedX<R && mappedX>L){QApplication::setOverrideCursor(Qt::SizeVerCursor);flag=7;}
            else if(mappedY>T && mappedY<B && mappedX<R && mappedX>L){QApplication::setOverrideCursor(Qt::SizeAllCursor);flag=8;}
            else if(abs(mappedY2-B2)<qH2 && abs(mappedX2-L2)<qW2){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=9;}
            else if(abs(mappedY2-B2)<qH2 && abs(mappedX2-R2)<qW2){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=10;}
            else if(abs(mappedY2-T2)<qH2 && abs(mappedX2-L2)<qW2){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=11;}
            else if(abs(mappedY2-T2)<qH2 && abs(mappedX2-R2)<qW2){QApplication::setOverrideCursor(Qt::ClosedHandCursor);flag=12;}
            else   {
                    if(abs(mappedX2-R2)<qW2 && mappedY2>T2&& mappedY2<B2){QApplication::setOverrideCursor(Qt::SizeHorCursor);flag=13;}
                    else if(abs(mappedX2-L2)<qW2 && mappedY2>T2 && mappedY2<B2){QApplication::setOverrideCursor(Qt::SizeHorCursor);flag=14;}
                    else if(abs(mappedY2-T2)<qH2 && R2 && mappedX2>L2){QApplication::setOverrideCursor(Qt::SizeVerCursor);flag=15;}
                    else if(abs(mappedY2-B2)<qH2 && mappedX2<R2 && mappedX2>L2){QApplication::setOverrideCursor(Qt::SizeVerCursor);flag=16;}
                    else if(mappedY2>T2&& mappedY2<B2 && mappedX2<R2 && mappedX2>L2){QApplication::setOverrideCursor(Qt::SizeAllCursor);flag=17;}
                   }
            }

       flagupselection=flag;
    }

    float hW, hH, left, top;
    float midW, midH;
    float hW2, hH2, left2, top2;
    float midW2, midH2;

    hW=autoEdgeOne->width()/2.;
    hH=autoEdgeOne->height()/2.;
    left=autoEdgeOne->right();
    top=autoEdgeOne->top();
    midW=left-hW;
    midH=top+hH;

    hW2=autoEdgeTwo->width()/2.;
    hH2=autoEdgeTwo->height()/2.;
    left2=autoEdgeTwo->right();
    top2=autoEdgeTwo->top();
    midW2=left2-hW2;
    midH2=top2+hH2;


    //Bottom Left
        if (flagupselection==0)
                    {
                    while(mappedX>L && mappedY>B){setupM.translate(midW,midH).rotate(-1.).translate(-1.*midW,-1.*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX<L && mappedY<B){setupM.translate(midW,midH).rotate(1.).translate(-1.*midW,-1*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                     }
    //Bottom right
        if (flagupselection==1)
                    {
                    while(mappedX<R && mappedY>B){setupM.translate(midW,midH).rotate(1.).translate(-1.*midW,-1.*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX>R && mappedY<B){setupM.translate(midW,midH).rotate(-1.).translate(-1.*midW,-1.*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }
    //Top Left
        if (flagupselection==2)
                    {
                    while(mappedX>L && mappedY<T){setupM.translate(midW,midH).rotate(1.).translate(-1.*midW,-1.*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX<L && mappedY>T){setupM.translate(midW,midH).rotate(-1.).translate(-1.*midW,-1.*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }
    //Top Right
        if (flagupselection==3)
                    {
                    while(mappedX<R && mappedY<T){setupM.translate(midW,midH).rotate(-1.).translate(-1.*midW,-1.*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    while(mappedX>R && mappedY>T){setupM.translate(midW,midH).rotate(1.).translate(-1.*midW,-1.*midH);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }
    //Resize Right
        if (flagupselection==4)
                    {
                    if(mappedX!=R){autoEdgeOne->setRight(mappedX);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }
    //Resize Left
        if (flagupselection==5)
                    {
                    if(mappedX!=L){autoEdgeOne->setLeft(mappedX);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                    TheMainWindow->RedrawJustAM();
                    }
    //Resize Top
            if (flagupselection==6)
                        {
                       if(mappedY!=T){autoEdgeOne->setTop(mappedY);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                       TheMainWindow->RedrawJustAM();
                        }

    //Resize Bottom
        if (flagupselection==7)
                    {
                   if(mappedY!=B){autoEdgeOne->setBottom(mappedY);setupMi=setupM.inverted();setupMi.map(x,y,&mappedX,&mappedY);}
                   TheMainWindow->RedrawJustAM();
                    }


   //Translate
        if (flagupselection==8)
                   {

                    //Both below also work to get angle
                    //double angle=sin(acos(aM.m11()));
                    //double angle=sqrt(1-(aM.m11()*aM.m11()));

                    while(mappedX<(L+R)/2)
                                    {
                                   double angle=atan2(setupM.m12(),setupM.m11());
                                   //If the angle is over 1 radian you have to correct the movement by just the figure after the decimal place.
                                   if(angle>1.)
                                   {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                   }
                                   else if (angle<-1.)
                                   {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                   }

                                   setupM.translate(-1.,angle);
                                   setupMi=setupM.inverted();
                                   setupMi.map(x,y,&mappedX,&mappedY);
                                    }
                    while(mappedX>(L+R)/2){
                                    double angle=atan2(setupM.m12(),setupM.m11());

                                   if(angle>1.)
                                   {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                   }
                                   else if (angle<-1.)
                                   {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                   }

                                    setupM.translate(1.,-angle);
                                    setupMi=setupM.inverted();
                                    setupMi.map(x,y,&mappedX,&mappedY);
                                    }
                    while(mappedY<(T+B)/2){
                                    double angle=atan2(setupM.m12(),setupM.m11());

                                 if(angle>1.)
                                   {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                   }
                                   else if (angle<-1.)
                                   {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                   }

                                    setupM.translate(angle,-1);
                                    setupMi=setupM.inverted();
                                    setupMi.map(x,y,&mappedX,&mappedY);
                                     }
                    while(mappedY>(T+B)/2){
                                    double angle=atan2(setupM.m12(),setupM.m11());

                                    if(angle>1.)
                                    {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                     }
                                     else if (angle<-1.)
                                     {
                                       int angleint=(int)angle;
                                       angle=angle- (float)angleint;
                                     }

                                    setupM.translate(angle,1);
                                    setupMi=setupM.inverted();
                                    setupMi.map(x,y,&mappedX,&mappedY);
                                     }


                    TheMainWindow->RedrawJustAM();
                    }

        //Bottom Left 2
               if (flagupselection==9)
                           {
                           while(mappedX2>L2 && mappedY2>B2){setupM2.translate(midW2,midH2).rotate(-1.).translate(-1.*midW2,-1.*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           while(mappedX2<L2 && mappedY2<B2){setupM2.translate(midW2,midH2).rotate(1.).translate(-1.*midW2,-1*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           TheMainWindow->RedrawJustAM();
                            }
           //Bottom right 2
               if (flagupselection==10)
                           {
                           while(mappedX2<R2 && mappedY2>B2){setupM2.translate(midW2,midH2).rotate(1.).translate(-1.*midW2,-1.*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           while(mappedX2>R2 && mappedY2<B2){setupM2.translate(midW2,midH2).rotate(-1.).translate(-1.*midW2,-1.*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           TheMainWindow->RedrawJustAM();
                           }
           //Top Left 2
               if (flagupselection==11)
                           {
                           while(mappedX2>L2 && mappedY2<T2){setupM2.translate(midW2,midH2).rotate(1.).translate(-1.*midW2,-1.*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           while(mappedX2<L2 && mappedY2>T2){setupM2.translate(midW2,midH2).rotate(-1.).translate(-1.*midW2,-1.*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           TheMainWindow->RedrawJustAM();
                           }
           //Top Right 2
               if (flagupselection==12)
                           {
                           while(mappedX2<R2 && mappedY2<T2){setupM2.translate(midW2,midH2).rotate(-1.).translate(-1.*midW2,-1.*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           while(mappedX2>R2 && mappedY2>T2){setupM2.translate(midW2,midH2).rotate(1.).translate(-1.*midW2,-1.*midH2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           TheMainWindow->RedrawJustAM();
                           }
           //Resize Right 2
               if (flagupselection==13)
                           {
                           if(mappedX2!=R2){autoEdgeTwo->setRight(mappedX2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           TheMainWindow->RedrawJustAM();
                           }
           //Resize Left 2
               if (flagupselection==14)
                           {
                           if(mappedX2!=L2){autoEdgeTwo->setLeft(mappedX2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                           TheMainWindow->RedrawJustAM();
                           }
           //Resize Top 2
                   if (flagupselection==15)
                               {
                              if(mappedY2!=T2){autoEdgeTwo->setTop(mappedY2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                              TheMainWindow->RedrawJustAM();
                               }

           //Resize Bottom 2
               if (flagupselection==16)
                           {
                          if(mappedY2!=B2){autoEdgeTwo->setBottom(mappedY2);setupMi2=setupM2.inverted();setupMi2.map(x,y,&mappedX2,&mappedY2);}
                          TheMainWindow->RedrawJustAM();
                           }


          //Translate 2
               if (flagupselection==17)
                          {

                           //Both below also work to get angle
                           //double angle=sin(acos(aM.m11()));
                           //double angle=sqrt(1-(aM.m11()*aM.m11()));

                           while(mappedX2<(L2+R2)/2)
                                           {
                                          double angle=atan2(setupM2.m12(),setupM2.m11());
                                          //If the angle is over 1 radian you have to correct the movement by just the figure after the decimal place.
                                          if(angle>1.)
                                          {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                          }
                                          else if (angle<-1.)
                                          {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                          }

                                          setupM2.translate(-1.,angle);
                                          setupMi2=setupM2.inverted();
                                          setupMi2.map(x,y,&mappedX2,&mappedY2);
                                           }
                           while(mappedX2>(L2+R2)/2){
                                           double angle=atan2(setupM2.m12(),setupM2.m11());

                                          if(angle>1.)
                                          {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                          }
                                          else if (angle<-1.)
                                          {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                          }

                                           setupM2.translate(1.,-angle);
                                           setupMi2=setupM2.inverted();
                                           setupMi2.map(x,y,&mappedX2,&mappedY2);
                                           }
                           while(mappedY2<(T2+B2)/2){
                                           double angle=atan2(setupM2.m12(),setupM2.m11());

                                        if(angle>1.)
                                          {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                          }
                                          else if (angle<-1.)
                                          {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                          }

                                           setupM2.translate(angle,-1);
                                           setupMi2=setupM2.inverted();
                                           setupMi2.map(x,y,&mappedX2,&mappedY2);
                                            }
                           while(mappedY2>(T2+B2)/2){
                                           double angle=atan2(setupM2.m12(),setupM2.m11());

                                           if(angle>1.)
                                           {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                            }
                                            else if (angle<-1.)
                                            {
                                              int angleint=(int)angle;
                                              angle=angle- (float)angleint;
                                            }

                                           setupM2.translate(angle,1);
                                           setupMi2=setupM2.inverted();
                                           setupMi2.map(x,y,&mappedX2,&mappedY2);
                                            }


                           TheMainWindow->RedrawJustAM();
                           }

 }

if (event->buttons() == Qt::LeftButton && markersUp==1)
                {
             if(markersLocked==1||setupFlag==1)return;
             if(markers[selectedMarker]->shape==0)
			{
			if (selection==-1)
                            {
                            scenedx=x-(int)markers[selectedMarker]->markerRect->left();
                            scenedy=y-(int)markers[selectedMarker]->markerRect->top();
                            selection=1;
                            }
			
			if(selection==1 && scenedx<300 && scenedy<300)markers[selectedMarker]->markerRect->moveTo(((double)x-(double)scenedx),((double)y-(double)scenedy));
			}

		
		if(markers[selectedMarker]->shape==1)
			{ 
			
			if (selection==-1)
							{
                                                                        if(abs(y-markers[selectedMarker]->linePoint.y())<30 && abs(x-markers[selectedMarker]->linePoint.x())<30)selection=1;
                                                                        else if(abs(y-markers[selectedMarker]->markerRect->bottom())<30 && abs(x-markers[selectedMarker]->markerRect->x())<30)selection=2;
									else
											{
											selection=3; 
											markers[selectedMarker]->dx = markers[selectedMarker]->linePoint.x() - markers[selectedMarker]->markerRect->x();
											markers[selectedMarker]->dy = markers[selectedMarker]->linePoint.y() - markers[selectedMarker]->markerRect->bottom();
											
											scenedx=x-((int)markers[selectedMarker]->linePoint.x()-(int)(0.5*markers[selectedMarker]->dx));
											scenedy=y-((int)markers[selectedMarker]->linePoint.y()-(int)(0.5*markers[selectedMarker]->dy));
											}
							}
			else
					{
					if(selection==1)
									{
									markers[selectedMarker]->linePoint.setX((double)x);
									markers[selectedMarker]->linePoint.setY((double)y);
									}
					if(selection==2)
					 				markers[selectedMarker]->markerRect->moveTo((double)x,(double)y);
					if(selection==3 && scenedx<300 && scenedy<300)
									{
									 markers[selectedMarker]->markerRect->moveTo
									(
									 
									 	(
									 		(double)x-(markers[selectedMarker]->dx/2.+(double)scenedx)
									 	),
									 	(
									 		(double)y-(markers[selectedMarker]->dy/2.+(double)scenedy)
									 	)
									 );
									 markers[selectedMarker]->linePoint.setX((markers[selectedMarker]->markerRect->x()+(markers[selectedMarker]->dx)));
									 markers[selectedMarker]->linePoint.setY((markers[selectedMarker]->markerRect->bottom()+(markers[selectedMarker]->dy)));
									}
					
					}
		
			}
		TheMainWindow->RedrawJustDecorations();
		}



}


void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	selection=-1;
        flagup=0;
        flagupselection=-1;
	if(markersUp==1)
		{
			markers[selectedMarker]->dx=0.0;
			markers[selectedMarker]->dy=0.0;
		}	
        QApplication::restoreOverrideCursor();
}


