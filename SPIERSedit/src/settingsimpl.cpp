/**
 * @file
 * Source: SettingsImpl
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

#include "settingsimpl.h"
#include "fileio.h"
#include "globals.h"
#include "copyingimpl.h"

SettingsImpl::SettingsImpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    SpinBoxCache->setValue(CacheMem);
    SpinBoxUndo->setValue(UndoMem);
    SpinBoxUndoTimer->setValue(UndoTimerSetting);
    BoxBackCache->setChecked(BackgroundCacheFilling);
    BoxRenderCache->setChecked(RenderCache);
    AutoSave->setValue(AutoSaveFrequency);
    SliderCacheCompression->setValue(CacheCompressionLevel);
    SliderFileCompression->setValue(FileCompressionLevel);

    BoxBackCache->setVisible(false);
}

void SettingsImpl::on_pushButton_clicked()
{
    //clear RUF list
    RecentFileList.clear();
    WriteSuperGlobals();
    Message("List Cleared");
    pushButton->setEnabled(false);
}

void SettingsImpl::on_buttonBox_accepted()
{
    CacheMem = SpinBoxCache->value();
    UndoMem = SpinBoxUndo->value();
    UndoTimerSetting = SpinBoxUndoTimer->value();
    AutoSaveFrequency = AutoSave->value();
    BackgroundCacheFilling = BoxBackCache->isChecked();

    bool rc = RenderCache;
    RenderCache = BoxRenderCache->isChecked();
    if (rc != RenderCache) ClearCache();

    if (SliderFileCompression->value() == 0) FileCompressionLevel = 0;
    if (SliderFileCompression->value() == 1) FileCompressionLevel = 1;
    if (SliderFileCompression->value() == 2) FileCompressionLevel = 9;

    int ccl = CacheCompressionLevel;
    if (SliderCacheCompression->value() == 0) CacheCompressionLevel = 0;
    if (SliderCacheCompression->value() == 1) CacheCompressionLevel = 1;
    if (SliderCacheCompression->value() == 2) CacheCompressionLevel = 9;
    if (ccl != CacheCompressionLevel) ClearCache();
    close();
}

void SettingsImpl::on_buttonBox_rejected()
{
    close();
}

void SettingsImpl::on_SliderFileCompression_valueChanged(int value)
{
    if (value == 0) LabelFileCompression->setText("Off");
    if (value == 1) LabelFileCompression->setText("Low");
    if (value == 2) LabelFileCompression->setText("High");
}

void SettingsImpl::on_SliderCacheCompression_valueChanged(int value)
{
    if (value == 0) LabelCacheCompression->setText("Off");;
    if (value == 1) LabelCacheCompression->setText("Low");
    if (value == 2) LabelCacheCompression->setText("High");
}

void SettingsImpl::on_RecompressFiles_pressed()
{
    if (!Active) return;
    QString mess;
    if (SliderFileCompression->value() == 0)
        mess = "This will decompress ALL working files, increasing the disk space used. Are you sure?";
    if (SliderFileCompression->value() == 1)
        mess = "This will compress/decompress ALL working files to 'low' level. Are you sure?";
    if (SliderFileCompression->value() == 2)
        mess = "This will compress ALL working files to maximum level. Are you sure?";
    if (QMessageBox::question(this, "Recompress Files", mess, QMessageBox::Ok | QMessageBox::Cancel)
            == QMessageBox::Ok)
    {
        CopyingImpl cop;
        cop.CompressAllWorkingFiles(SliderFileCompression->value());
    }//Do it!
}

void SettingsImpl::on_RecompressSourceFiles_pressed()
{
    if (!Active) return;
    QString mess;
    if (SliderFileCompression->value() == 0)
        mess = "This will convert ALL source files to uncompressed 'bmp' format. Are you sure?";
    if (SliderFileCompression->value() == 1)
        mess = "This will convert ALL source files to compressed 'png' format. Are you sure?";
    if (SliderFileCompression->value() == 2)
        mess = "This will compress ALL source files to 'jpg' format. Although this is done at maximum fidelity, some slight image degradation is possible. Are you sure?";
    if (QMessageBox::question(this, "Recompress Files", mess, QMessageBox::Ok | QMessageBox::Cancel)
            == QMessageBox::Ok)
    {
        CopyingImpl cop;
        cop.CompressAllSourceFiles(SliderFileCompression->value());
    }
}
