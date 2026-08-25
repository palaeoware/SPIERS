/**
 * @file
 * Source: Movetogroup
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */
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
