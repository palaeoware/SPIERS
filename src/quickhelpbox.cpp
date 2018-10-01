#include "quickhelpbox.h"
#include "ui_quickhelpbox.h"

QuickHelpBox::QuickHelpBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickHelpBox)
{
    ui->setupUi(this);
}

QuickHelpBox::~QuickHelpBox()
{
    delete ui;
}
