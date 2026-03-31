/**
 * @file
 * Source: NewProjectDialog
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include "newprojectdialog.h"

NewProjectDialogImpl::NewProjectDialogImpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), isNewProject(false)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(OK_Click()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(Cancel_Click()));

    // Set default units to mm (index 2)
    ComboVoxelUnit->setCurrentIndex(2);
    ComboXYUnit->setCurrentIndex(2);
    ComboZUnit->setCurrentIndex(2);

    // Hide new-project-only controls
    GroupFiles->setVisible(false);
    LabelDownsample->setVisible(false);
    spinBox->setVisible(false);
    spinBoxZ->setVisible(false);
    CheckMirrored->setVisible(false);
    CheckMirrored->setChecked(true);
    GroupSpacing->setVisible(false);

    Cancelled = true;
}

void NewProjectDialogImpl::HideCopy()
{
    isNewProject = true;
    setWindowTitle("New Project");
    GroupFiles->setVisible(true);
    LabelDownsample->setVisible(true);
    spinBox->setVisible(true);
    spinBoxZ->setVisible(true);
    CheckMirrored->setVisible(true);
    GroupSpacing->setVisible(true);

    // Disable OK until filename is filled and ≥2 images are listed
    QAbstractItemModel *model = listFiles->model();
    QObject::connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateOkButton()));
    QObject::connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),  this, SLOT(updateOkButton()));
    QObject::connect(model, SIGNAL(modelReset()),                       this, SLOT(updateOkButton()));
    QObject::connect(lineEdit, SIGNAL(textChanged(QString)),            this, SLOT(updateOkButton()));
    updateOkButton();

    adjustSize();
}

void NewProjectDialogImpl::updateOkButton()
{
    bool ok = !lineEdit->text().trimmed().isEmpty() && listFiles->count() >= 2;
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

void NewProjectDialogImpl::OK_Click()
{
    fname = lineEdit->text();
    notes = textEdit->toPlainText();
    Cancelled = false;
    close();
}

void NewProjectDialogImpl::Cancel_Click()
{
    Cancelled = true;
    close();
}

void NewProjectDialogImpl::on_ButtonAddFiles_clicked()
{
    QStringList newFiles = QFileDialog::getOpenFileNames(
        this,
        "Select source images",
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)");

    for (const QString &f : newFiles)
    {
        // Avoid duplicates
        QList<QListWidgetItem *> existing = listFiles->findItems(f, Qt::MatchExactly);
        if (existing.isEmpty())
            listFiles->addItem(f);
    }
    listFiles->sortItems();
}

void NewProjectDialogImpl::on_ButtonAddDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "Select directory containing source images",
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));

    if (dir.isEmpty())
        return;

    QDir d(dir);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.tif" << "*.tiff";
    QStringList entries = d.entryList(filters, QDir::Files, QDir::Name);

    for (const QString &entry : entries)
    {
        QString fullPath = d.filePath(entry);
        QList<QListWidgetItem *> existing = listFiles->findItems(fullPath, Qt::MatchExactly);
        if (existing.isEmpty())
            listFiles->addItem(fullPath);
    }
    listFiles->sortItems();
}

void NewProjectDialogImpl::on_ButtonRemove_clicked()
{
    qDeleteAll(listFiles->selectedItems());
}

void NewProjectDialogImpl::on_ButtonClear_clicked()
{
    listFiles->clear();
}

void NewProjectDialogImpl::on_RadioIsotropic_toggled(bool checked)
{
    StackedSpacing->setCurrentIndex(checked ? 0 : 1);
}

QStringList NewProjectDialogImpl::getFiles() const
{
    QStringList result;
    for (int i = 0; i < listFiles->count(); ++i)
        result.append(listFiles->item(i)->text());
    return result;
}

// Returns the conversion factor from the given unit index to millimetres.
// Indices: 0=m, 1=cm, 2=mm, 3=um, 4=nm
double NewProjectDialogImpl::unitToMM(int unitIndex)
{
    static const double factors[] = {1000.0, 10.0, 1.0, 0.001, 0.000001};
    if (unitIndex < 0 || unitIndex > 4) return 1.0;
    return factors[unitIndex];
}

double NewProjectDialogImpl::pixPerMM() const
{
    if (RadioIsotropic->isChecked())
    {
        double sizeInMM = SpinVoxelSize->value() * unitToMM(ComboVoxelUnit->currentIndex());
        return (sizeInMM > 0.0) ? (1.0 / sizeInMM) : 1.0;
    }
    else
    {
        double xyInMM = SpinXYSize->value() * unitToMM(ComboXYUnit->currentIndex());
        return (xyInMM > 0.0) ? (1.0 / xyInMM) : 1.0;
    }
}

double NewProjectDialogImpl::slicePerMM() const
{
    if (RadioIsotropic->isChecked())
    {
        double sizeInMM = SpinVoxelSize->value() * unitToMM(ComboVoxelUnit->currentIndex());
        return (sizeInMM > 0.0) ? (1.0 / sizeInMM) : 1.0;
    }
    else
    {
        double zInMM = SpinZSize->value() * unitToMM(ComboZUnit->currentIndex());
        return (zInMM > 0.0) ? (1.0 / zInMM) : 1.0;
    }
}
