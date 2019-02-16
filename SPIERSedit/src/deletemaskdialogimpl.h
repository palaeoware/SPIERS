/**
 * @file
 * Header: Delete Mask Dialog Impl.
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef DELETEMASKDIALOGIMPL_H
#define DELETEMASKDIALOGIMPL_H

#include <QDialog>
#include <QTimer>

#include "ui_deletemask.h"

/**
 * @brief The DeleteMaskDialogImpl class
 */
class DeleteMaskDialogImpl : public QDialog, public Ui::DeleteMaskDialog
{
    Q_OBJECT
public:
    DeleteMaskDialogImpl(QList <int> MasksToDelete, QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    bool Cancelled;
    int MaskToDeleteIndex;
    QList <int> MasksToDeleteList;
    bool tflag;
    QTimer *timer;

private slots:
    void on_ListTargetMasks_itemSelectionChanged();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void Timer();
};
#endif





