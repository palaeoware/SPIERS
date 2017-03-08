#ifndef IMPORTDIALOGIMPL_H
#define IMPORTDIALOGIMPL_H
//
#include "ui_import.h"
//
class ImportDialogImpl : public QDialog, public Ui::ImportDialog
{
Q_OBJECT
public:
    ImportDialogImpl(QWidget * parent = 0, Qt::WindowFlags f = 0 );
	QString fname;
	QString notes;
	bool Cancelled;
	void HideCopy();
private slots:
	void OK_Click();
	void Cancel_Click();
};
#endif





