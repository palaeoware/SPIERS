#include "quickhelpbox.h"
#include "ui_quickhelpbox.h"

/**
 * @brief QuickHelpBox::QuickHelpBox
 * @param parent
 */
QuickHelpBox::QuickHelpBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuickHelpBox)
{
    ui->setupUi(this);
}

/**
 * @brief QuickHelpBox::~QuickHelpBox
 */
QuickHelpBox::~QuickHelpBox()
{
    delete ui;
}
