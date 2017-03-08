#include "histogram.h"
#include "globals.h"

#include <QDebug>
histgv *GVHist;
#include <QResizeEvent>

histgv::histgv()
{
	histscene=new QGraphicsScene();
	
	for (int i=0; i<256; i++)
	{
		histscene->addItem(&(Hist_Grey[i]));
	}
	ThreshLine.setLine(128,0,128,255);
	ThreshLine.setPen(QPen(Qt::magenta));
	histscene->addItem(&ThreshLine);
	setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff);
	setScene(histscene);
	Refresh();
}

void histgv::resizeEvent ( QResizeEvent * event )
{
  	fitInView(QRectF(0,0,256,256), Qt::IgnoreAspectRatio);
}
 
void histgv::Refresh()
{

	if (!Active) return;
	int bins[256];
	
	for (int i=0; i<256; i++) bins[i]=0;
	uchar *data;
	data = GA[CurrentSegment]->bits();
	int max=fwidth*fheight;

	if (MenuHistSelectedOnly)
	for (int i=0; i<max; i++) 
		{
			if (Locks[i*2]) bins[data[i]]++;
		}	
	else
	for (int ix=0; ix<fwidth; ix++) 
	for (int iy=0; iy<fheight; iy++) 
		{
			bins[data[fwidth4*iy+ix]]++;
		}
		

	int bmax=0;
	for (int i=0; i<256; i++) if (bins[i]>bmax) bmax=bins[i];
	
	bmax/=240;
	if (bmax==0) bmax=1;
	for (int i=0; i<256; i++) {	Hist_Grey[i].setLine(i,256,i,256-(bins[i]/bmax));}
}
