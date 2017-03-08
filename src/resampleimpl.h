#ifndef RESAMPLEIMPL_H
#define RESAMPLEIMPL_H
//
#include <QDialog>
#include "ui_resample.h"
//
class resampleImpl : public QDialog, public Ui::resample
{
Q_OBJECT
public:
    resampleImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
private:
	int ClosestTo(QString NewFileString);
private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
};
#endif





