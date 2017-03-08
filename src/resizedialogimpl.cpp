#include "resizedialogimpl.h"
//
ResizeDialogImpl::ResizeDialogImpl( QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

}
//

void ResizeDialogImpl::on_buttonBox_accepted()
{
	Cancelled=false;
	close();
}

void ResizeDialogImpl::on_buttonBox_rejected()
{
	Cancelled=true;
	close();
}

