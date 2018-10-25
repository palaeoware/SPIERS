#include "slicespacingdialogimpl.h"
#include "globals.h"
//

bool stoploopflag;

slicespacingdialogImpl::slicespacingdialogImpl( QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

	Absolute->setValue(1.0/SlicePerMM);
	stoploopflag=false;
}
//
void slicespacingdialogImpl::on_Relative_valueChanged(double )
{
	if (stoploopflag==false) 
	{	
		stoploopflag=true;
		Absolute->setValue((Relative->value()/100)/SlicePerMM);
		stoploopflag=false;
	}
}

void slicespacingdialogImpl::on_Absolute_valueChanged(double )
{
	if (stoploopflag==false) 
	{	
		stoploopflag=true;	
		Relative->setValue(100*(Absolute->value()*SlicePerMM));
		stoploopflag=false;
	}
}

void slicespacingdialogImpl::on_buttonBox_accepted()
{
	relative=Relative->value()/100;
	close();
}

void slicespacingdialogImpl::on_buttonBox_rejected()
{
	relative=-1;
	close();
}

