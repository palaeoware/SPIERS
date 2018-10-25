#include "quickhelpdialog.h"
#include "ui_quickhelpdialog.h"

quickhelpdialog::quickhelpdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::quickhelpdialog)
{
    ui->setupUi(this);
}

quickhelpdialog::~quickhelpdialog()
{
    delete ui;
}
