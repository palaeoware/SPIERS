#include "movetogroup.h"
#include "svobject.h"
#include <QTreeWidgetItem>

bool MoveToGroup::IsGroupOrParentsSelected(int i)
{
    if (svobjects[i]->widgetitem->isSelected()) return true;
    if (svobjects[i]->InGroup == -1) return false;
    return IsGroupOrParentsSelected(svobjects[i]->Parent());
}

MoveToGroup::MoveToGroup(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
        setupUi(this);
        valid=false;
        comboBox->addItem("[No Group]");
        items.append(-1);
        for (int i=0; i<svobjects.count(); i++)
            if (svobjects[i]->IsGroup)
            {
                if (!(IsGroupOrParentsSelected(i)))
                //first - is this group selected?
                {
                    //second  - are any of it's parents selected?
                    comboBox->addItem(svobjects[i]->Name);
                    items.append(i);
                    valid=true;
                }
             }

        Group=-2;
        //populate combo
}

void MoveToGroup::on_buttonBox_accepted()
{

    if (comboBox->currentIndex()==-1) Group=-2;
    else Group=items[comboBox->currentIndex()];
    close();
}

void MoveToGroup::on_buttonBox_rejected()
{
    close();
}
