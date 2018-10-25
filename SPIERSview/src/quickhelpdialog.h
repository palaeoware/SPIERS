#ifndef QUICKHELPDIALOG_H
#define QUICKHELPDIALOG_H

#include <QDialog>

namespace Ui {
    class quickhelpdialog;
}

class quickhelpdialog : public QDialog
{
    Q_OBJECT

public:
    explicit quickhelpdialog(QWidget *parent = 0);
    ~quickhelpdialog();

private:
    Ui::quickhelpdialog *ui;
};

#endif // QUICKHELPDIALOG_H
