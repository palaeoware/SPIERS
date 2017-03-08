#ifndef DELETEMASKDIALOGIMPL_H
#define DELETEMASKDIALOGIMPL_H
//
#include <QDialog>
#include <QTimer>
#include "ui_deletemask.h"
//
class DeleteMaskDialogImpl : public QDialog, public Ui::DeleteMaskDialog
{
Q_OBJECT
public:
    DeleteMaskDialogImpl(QList <int> MasksToDelete, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	bool Cancelled;
	int MaskToDeleteIndex;
	QList <int> MasksToDeleteList;
	bool tflag;
	QTimer *timer;
private slots:
	void on_ListTargetMasks_itemSelectionChanged();
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void Timer();
};
#endif





