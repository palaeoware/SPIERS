#ifndef EXPORTDXF_H
#define EXPORTDXF_H
//
#include <QDialog>
#include "ui_Copying.h"
#include "mainwindowimpl.h"
//
class ExportDXF : public QDialog, public Ui::Copying
{
Q_OBJECT
public:
    ExportDXF( MainWindowImpl *mwp, QWidget * parent = 0, Qt::WindowFlags f = 0 );
	MainWindowImpl *mw;
private slots:
};
#endif





