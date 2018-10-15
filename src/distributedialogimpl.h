#ifndef DISTRIBUTEDIALOGIMPL_H
#define DISTRIBUTEDIALOGIMPL_H
//
#include <QDialog>
#include "ui_distribute.h"
//
class DistributeDialogImpl : public QDialog, public Ui::DistributeDialog
{
Q_OBJECT
public:
    DistributeDialogImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
private slots:
	void on_SpinBoxFrom_valueChanged(int );
	void on_SpinBoxTo_valueChanged(int );
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
};
#endif





