#include "movetogroup.h"
#include "svobject.h"
#include <QTreeWidgetItem>

/**
 * @brief MoveToGroup::IsGroupOrParentsSelected
 * @param i
 * @return
 */
bool MoveToGroup::IsGroupOrParentsSelected(int i)
{
    if (SVObjects[i]->widgetitem->isSelected()) return true;
    if (SVObjects[i]->InGroup == -1) return false;
    return IsGroupOrParentsSelected(SVObjects[i]->Parent());
}

/**
 * @brief MoveToGroup::MoveToGroup
 * @param parent
 * @param f
 */
MoveToGroup::MoveToGroup(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);
    valid = false;
    comboBox->addItem("[No Group]");
    items.append(-1);
    for (int i = 0; i < SVObjects.count(); i++)
        if (SVObjects[i]->IsGroup)
        {
            if (!(IsGroupOrParentsSelected(i)))
                //first - is this group selected?
            {
                //second  - are any of it's parents selected?
                comboBox->addItem(SVObjects[i]->Name);
                items.append(i);
                valid = true;
            }
        }

    Group = -2;
    //populate combo
}

/**
 * @brief MoveToGroup::on_buttonBox_accepted
 */
void MoveToGroup::on_buttonBox_accepted()
{

    if (comboBox->currentIndex() == -1) Group = -2;
    else Group = items[comboBox->currentIndex()];
    close();
}

/**
 * @brief MoveToGroup::on_buttonBox_rejected
 */
void MoveToGroup::on_buttonBox_rejected()
{
    close();
}
