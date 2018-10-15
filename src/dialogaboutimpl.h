#ifndef DIALOGABOUTIMPL_H
#define DIALOGABOUTIMPL_H
//
#include <QDialog>
#include "../ui/ui_about.h"

/*
class DialogAboutImpl : public QDialog, public Ui::DialogAbout
{
Q_OBJECT
public:
    DialogAboutImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
private slots:
    void on_pushButton_clicked();
};
#endif
*/

/*RJG
namespace Ui {
class DialogAboutImpl;
}*/

class DialogAboutImpl : public QDialog, public Ui::About
{
    Q_OBJECT

public:
    DialogAboutImpl( QWidget *parent = 0, Qt::WindowFlags f = 0 );

private:
    QString returnLicense();
};

#endif // ABOUT_H
