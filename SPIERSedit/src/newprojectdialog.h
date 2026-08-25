/**
 * @file
 * Header: Import Dialog Impl
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include "ui_newprojectdialog.h"

/**
 * @brief The NewProjectDialogImpl class
 */
class NewProjectDialogImpl : public QDialog, public Ui::NewProjectDialog
{
    Q_OBJECT
public:
    NewProjectDialogImpl(QWidget *parent = nullptr, Qt::WindowFlags f = {} );
    QString fname;
    QString notes;
    bool Cancelled;
    void HideCopy();
    QStringList getFiles() const;
    double pixPerMM() const;
    double slicePerMM() const;

private:
    bool isNewProject;
    static double unitToMM(int unitIndex);

private slots:
    void OK_Click();
    void Cancel_Click();
    void on_ButtonAddFiles_clicked();
    void on_ButtonAddDir_clicked();
    void on_ButtonRemove_clicked();
    void on_ButtonClear_clicked();
    void on_RadioIsotropic_toggled(bool checked);
    void updateOkButton();
};

#endif
