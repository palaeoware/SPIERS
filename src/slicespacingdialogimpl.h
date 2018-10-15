#ifndef SLICESPACINGDIALOGIMPL_H
#define SLICESPACINGDIALOGIMPL_H
//
#include <QDialog>
#include "ui_slicespacing.h"
//
class slicespacingdialogImpl : public QDialog, public Ui::slicespacingdialog
{
Q_OBJECT
public:
    slicespacingdialogImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	double relative;
private slots:
	void on_Relative_valueChanged(double );
	void on_Absolute_valueChanged(double );
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
};
#endif





