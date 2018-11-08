#include "gridfontsizedialog.h"
#include "ui_gridfontsizedialog.h"

/**
 * @brief GridFontSizeDialog::GridFontSizeDialog
 * @param parent
 */
GridFontSizeDialog::GridFontSizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridFontSizeDialog)
{
    ui->setupUi(this);

    ui->comboBox->setCurrentIndex(fontSizeGrid);
}

/**
 * @brief GridFontSizeDialog::~GridFontSizeDialog
 */
GridFontSizeDialog::~GridFontSizeDialog()
{
    delete ui;
}

/**
 * @brief GridFontSizeDialog::on_buttonBox_accepted
 */
void GridFontSizeDialog::on_buttonBox_accepted()
{
    fontSizeGrid = ui->comboBox->currentIndex();

    mainWindow->UpdateGL();
}

/**
 * @brief GridFontSizeDialog::on_buttonBox_rejected
 */
void GridFontSizeDialog::on_buttonBox_rejected()
{
    return;
}
