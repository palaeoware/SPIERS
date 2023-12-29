/**
 * @file
 * Source: DeleteMaskDialog
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "globals.h"
#include "fileio.h"
#include "deletemaskdialogimpl.h"

DeleteMaskDialogImpl::DeleteMaskDialogImpl(QList <int> MasksToDelete, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    //Populate masks list
    MaskToDeleteIndex = MasksToDelete[0];
    MasksToDeleteList = MasksToDelete;
    if (MasksToDelete.count() == 1) this->setWindowTitle("Delete Mask '" + MasksSettings[MaskToDeleteIndex]->Name + "'");
    else
        this->setWindowTitle("Delete Multiple Masks");
    //first find lowest listorder...
    bool flag = false;
    QList <bool> usedflags;
    for (int i = 0; i <= MaxUsedMask; i++) usedflags.append(false);
    for (int kloop = 0; kloop <= MaxUsedMask; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j <= MaxUsedMask; j++)
        {
            if (MasksSettings[j]->ListOrder < lowestval && usedflags[j] == false)
            {
                lowestval = MasksSettings[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;
        usedflags[i] = true;

        if (MasksToDelete.indexOf(i) < 0) //can't merge with self
        {
            QListWidgetItem *item = new QListWidgetItem;
            item->setText(MasksSettings[i]->Name);
            item->setData(Qt::UserRole, i);
            ListTargetMasks->addItem(item);
            if (flag == false) item->setSelected(true); //do the first one
            flag = true;
        }

    }
    tflag = false;
    if (flag == false) //big fat timer bodge to close window if needbe
    {
        Message("At least one mask must be left undeleted!");
        tflag = true;
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(Timer()));
        timer->start(50);
    }
}

void DeleteMaskDialogImpl::Timer()
{
    if (tflag) close();
}
//

void DeleteMaskDialogImpl::on_buttonBox_accepted()
{
    //This may get hairy!
    int TargetMask = (ListTargetMasks->currentItem()->data(Qt::UserRole)).toInt();


    //First - work out a mask conversion table.
    int ToMasks[256];
    int ToMaskPos = 0;

    //get conversion table for normal masks (not being deleted)
    for (int i = 0; i <= MaxUsedMask; i++)
    {
        if (MasksToDeleteList.indexOf(i) < 0) //this is NOT one of the deleting ones
            ToMasks[i] = ToMaskPos++;
    }

    //repoint ones being deleted to correct target mask AFTER conversion
    for (int i = 0; i < MasksToDeleteList.count(); i++) ToMasks[MasksToDeleteList[i]] = ToMasks[TargetMask];


    //OK, should now have correct conversion list - first handle selections

    SelectedMask = ToMasks[SelectedMask];

    SelectedRMask = ToMasks[SelectedRMask];

    //now do main loop - apply to ALL mask bytes in all files

    progressBar->setMaximum(Files.count() + 1);

    SaveMasks(CurrentFile);
    for (int i = 0; i < Files.count(); i++)
    {
        LoadMasks(i);
        for (int j = 0; j < Masks.size(); j++) Masks[j] = ToMasks[(quint8)Masks[j]]; // new mask from table
        SaveMasks(i);
        progressBar->setValue(i + 1);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    LoadMasks(CurrentFile);

    //Now hack apart the actual masksdetails list - move pointers as above

    //delete all the dead ones
    for (int i = 0; i < MasksToDeleteList.count(); i++) delete MasksSettings[MasksToDeleteList[i]];

    ToMaskPos = 0;
    for (int i = 0; i <= MaxUsedMask; i++)
    {
        if (MasksToDeleteList.indexOf(i) < 0) //this is NOT one of the deleting ones
            MasksSettings[ToMaskPos++] = MasksSettings[i];
    }
    //Finally can now remove the last  (n) entries
    MaxUsedMask -= MasksToDeleteList.count();
    for (int i = 0; i < MasksToDeleteList.count(); i++) MasksSettings.removeLast();

    Cancelled = false;

    for (int i = 0; i < OutputObjectsCount; i++)
        for (int j = 0; j < OutputObjects[i]->ComponentMasks.count(); j++)
            OutputObjects[i]->ComponentMasks[j] = ToMasks[OutputObjects[i]->ComponentMasks[j]];

    close();
}

void DeleteMaskDialogImpl::on_buttonBox_rejected()
{
    Cancelled = true;
    close();
}


void DeleteMaskDialogImpl::on_ListTargetMasks_itemSelectionChanged()
{
    //By definition there must now be a selected item
    //no longer used

}
