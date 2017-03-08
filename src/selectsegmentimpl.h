#ifndef SELECTSEGMENTIMPL_H
#define SELECTSEGMENTIMPL_H
//
#include <QDialog>
#include "ui_selectsegment.h"
//
class SelectSegmentImpl : public QDialog, public Ui::SelectSegment
{
Q_OBJECT
public:
    SelectSegmentImpl(int seg,  QWidget * parent = 0, Qt::WindowFlags f = 0 );
	bool Cancelled;
	int RetValue;

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
};
#endif





