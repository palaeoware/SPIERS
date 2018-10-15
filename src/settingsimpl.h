#ifndef SETTINGSIMPL_H
#define SETTINGSIMPL_H
//
#include <QDialog>
#include "ui_settings.h"
//
class SettingsImpl : public QDialog, public Ui::Settings
{
Q_OBJECT
public:
    SettingsImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
private slots:
	void on_RecompressSourceFiles_pressed();
	void on_RecompressFiles_pressed();
	void on_SliderFileCompression_valueChanged(int value);
	void on_SliderCacheCompression_valueChanged(int value);
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void on_pushButton_clicked();
};
#endif









