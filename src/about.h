#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include "ui_about.h"

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

private:
    Ui::About *ui;
    QString returnLicense();
};

#endif // ABOUT_H
