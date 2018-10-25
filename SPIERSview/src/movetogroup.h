#ifndef MOVETOGROUP_H
#define MOVETOGROUP_H

#include <QDialog>
#include <QList>
#include "ui_movetogroup.h"

class MoveToGroup : public QDialog, public Ui::movetogroup
{
    Q_OBJECT
    public:
    MoveToGroup(QWidget * parent = 0, Qt::WindowFlags f = 0);
    int Group;
    bool valid;

    private:
    bool IsGroupOrParentsSelected(int i);
    QList <int> items;

    public slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // MOVETOGROUP_H
