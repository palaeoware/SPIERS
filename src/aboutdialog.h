#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class aboutdialog;
}

class aboutdialog : public QDialog
{
    Q_OBJECT
public:
    aboutdialog(QWidget *parent = nullptr);
    ~aboutdialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::aboutdialog *m_ui;

private slots:
    void on_closebutton_clicked();
    QString returnLicense();
};

#endif // ABOUTDIALOG_H
