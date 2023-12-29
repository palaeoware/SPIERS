/**
 * @file
 * Header: Move to Group Dialog
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef MOVETOGROUP_H
#define MOVETOGROUP_H

#include <QDialog>
#include <QList>
#include "ui_movetogroup.h"

class MoveToGroup : public QDialog, public Ui::movetogroup
{
    Q_OBJECT
public:
    MoveToGroup(QWidget *parent = 0, Qt::WindowFlags f = 0);
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
