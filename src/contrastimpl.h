#ifndef CONTRASTIMPL_H
#define CONTRASTIMPL_H
//
#include <QDialog>
#include "ui_contrast.h"
//
class ContrastImpl : public QDialog, public Ui::ContrastDialog
{
Q_OBJECT
public:
    ContrastImpl( int MaskNo, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	bool Cancelled;
	int MaskNumber;
private slots:
	void on_ContrastSlider_valueChanged(int value);
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void RedrawColour();
};
#endif





