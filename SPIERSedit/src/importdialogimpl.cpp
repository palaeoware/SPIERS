#include "importdialogimpl.h"


//
ImportDialogImpl::ImportDialogImpl( QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(OK_Click()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(Cancel_Click()));
	LabelDownsample->setVisible(false);
	spinBox->setVisible(false);
	spinBoxZ->setVisible(false);
        CheckMirrored->setVisible(false);
        CheckMirrored->setChecked(true);
        Cancelled=true;
}

void ImportDialogImpl::OK_Click()
{
	fname=lineEdit->text();
	notes=textEdit->toPlainText();
	Cancelled=false;
	close();
}

void ImportDialogImpl::HideCopy()
{
	//checkBox->setVisible(false);
	LabelDownsample->setVisible(true);
	spinBox->setVisible(true);		
	spinBoxZ->setVisible(true);
        CheckMirrored->setVisible(true);
}

void ImportDialogImpl::Cancel_Click()
{
	Cancelled=true;
	close();
}
	

