#ifndef MOREIMPL_H
#define MOREIMPL_H
//
#include "ui_more.h"
//
class moreimpl : public QDialog, public Ui::Dialog
{
Q_OBJECT
public:
    moreimpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	QString fname;
	bool Cancelled;
private slots:
	void OK_Click();
	void Cancel_Click();
};
#endif






