#ifndef GRIDFONTSIZEDIALOG_H
#define GRIDFONTSIZEDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class GridFontSizeDialog;
}

class GridFontSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridFontSizeDialog(QWidget *parent = nullptr);
    ~GridFontSizeDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::GridFontSizeDialog *ui;
};

#endif // GRIDFONTSIZEDIALOG_H
