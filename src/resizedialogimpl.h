#ifndef RESIZEDIALOGIMPL_H
#define RESIZEDIALOGIMPL_H

#include <QDialog>
#include <QIcon>
#include "ui_curveresize.h"

/**
 * @brief The ResizeDialogImpl class
 */
class ResizeDialogImpl : public QDialog, public Ui::ResizeDialog
{
    Q_OBJECT
public:
    ResizeDialogImpl( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    bool Cancelled;
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};
#endif
