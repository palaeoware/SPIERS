#include "distributedialogimpl.h"
#include "globals.h"

DistributeDialogImpl::DistributeDialogImpl( QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

}


void DistributeDialogImpl::on_SpinBoxFrom_valueChanged(int v)
{
	SpinBoxTo->setMinimum(v+1);
}

void DistributeDialogImpl::on_SpinBoxTo_valueChanged(int v)
{
	SpinBoxFrom->setMaximum(v-1);
}

void DistributeDialogImpl::on_buttonBox_accepted()
{
	double range = (double) (SpinBoxTo->value()-SpinBoxFrom->value());
	
	int count=0;	
	for (int i=0; i<SegmentCount; i++)
	{	
		if (Segments[i]->Activated && Segments[i]->widgetitem!=0)
		if (Segments[i]->widgetitem->isSelected()) count++;	
	}

	range/=(double) count;
	double pos=(double) SpinBoxFrom->value();
	QList <bool> usedflags; for (int i=0; i<SegmentCount; i++) usedflags.append(false);
	for (int kloop=0; kloop<SegmentCount; kloop++) 
	{
	//find lowest
		int lowestval=999999;
		int lowestindex=-1;		
		for (int j=0; j<SegmentCount; j++)
		{
			if (Segments[j]->ListOrder<lowestval && usedflags[j]==false && Segments[j]->Activated && Segments[j]->widgetitem->isSelected()) 
				{lowestval=Segments[j]->ListOrder; lowestindex=j;}
		}
		if (lowestindex==-1) //didn't find any- get out
			{close();return;}
		int i=lowestindex;
		usedflags[i]=true;
		
		//i is now next in list order
		Segments[i]->RangeBase=(int)(pos+.5);
		Segments[i]->RangeTop=(int)(pos+range+.5);
		pos+=range;
	}
	close();
}

void DistributeDialogImpl::on_buttonBox_rejected()
{
	close();
}

