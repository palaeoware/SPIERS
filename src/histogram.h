#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include <QGraphicsLineItem>
#include <QGraphicsView>

class histgv : public QGraphicsView
{
public:
	histgv();
	void Refresh();
protected:
     void resizeEvent ( QResizeEvent * event )  ;
private:
	QGraphicsScene *histscene;

	//the line items for the scene
	QGraphicsLineItem Hist_Grey[256];
	QGraphicsLineItem Hist_Red[256];
	QGraphicsLineItem Hist_Green[256];
	QGraphicsLineItem Hist_Blue[256];
	QGraphicsLineItem ThreshLine;
	bool inresize;
};

extern histgv *GVHist;


#endif // __HISTOGRAM_H__
