#ifndef QUICKHELPBOX_H
#define QUICKHELPBOX_H

#include <QDialog>

namespace Ui {
    class QuickHelpBox;
}

class QuickHelpBox : public QDialog
{
    Q_OBJECT

public:
    explicit QuickHelpBox(QWidget *parent = 0);
    ~QuickHelpBox();

private:
    Ui::QuickHelpBox *ui;
};

#endif // QUICKHELPBOX_H
