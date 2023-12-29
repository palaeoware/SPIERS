/**
 * @file
 * Source: SelectSegementImpl
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

#include "selectsegmentimpl.h"
#include "globals.h"

SelectSegmentImpl::SelectSegmentImpl( int seg, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    comboBox->addItem("[None]", 0);

    //add segments - but in correct order

    QList <bool> usedflags;
    for (int i = 0; i < SegmentCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < SegmentCount; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < SegmentCount; j++)
        {
            if (Segments[j]->ListOrder < lowestval && usedflags[j] == false)
            {
                lowestval = Segments[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;
        usedflags[i] = true;

        comboBox->addItem(Segments[i]->Name, i + 1);
    }

    comboBox->setCurrentIndex (comboBox->findData(seg));

}
//



void SelectSegmentImpl::on_buttonBox_accepted()
{
    Cancelled = false;
    RetValue = comboBox->itemData(comboBox->currentIndex()).toInt();
    close();
}

void SelectSegmentImpl::on_buttonBox_rejected()
{
    Cancelled = true;
    close();
}

