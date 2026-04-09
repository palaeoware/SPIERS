/**
 * @file
 * Source: Main Window
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

#include "mainwindow.h"
#include "globals.h"
#include "display.h"
#include "brush.h"
#include "histogram.h"
#include "generatetestdata.h"
#include "globals.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QShortcut>
#include <QTimer>
#include <QTime>
#include <QInputDialog>
#include <QPicture>
#include <QPainter>
#include <QDesktopServices>
#include <QHeaderView>

/**
 * @brief MainWindow::SetUpDocks
 * Initializes and configures all dock widgets and the main toolbar.
 * Sets up dock positions, visibility states, and the brush size toolbar control.
 */
void MainWindow::SetUpDocks()
{
    addDockWidget (Qt::LeftDockWidgetArea, dockWidget_Main);
    addDockWidget (Qt::LeftDockWidgetArea, SliceSelector);
    addDockWidget (Qt::RightDockWidgetArea, DockPreview3D);
    addDockWidget (Qt::RightDockWidgetArea, DockInfo);
    addDockWidget (Qt::RightDockWidgetArea, dockWidget_Generate);
    addDockWidget (Qt::RightDockWidgetArea, DockSegmentsSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockMasksSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockCurvesSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockOutputSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockHist);
    addDockWidget (Qt::RightDockWidgetArea, DockGenerateTestData);

    tabifyDockWidget(dockWidget_Main, SliceSelector);

    dockWidget_Main->setVisible(true);
    SliceSelector->setVisible(false);
    DockMasksSettings->setVisible(true);
    DockCurvesSettings->setVisible(false);
    DockSegmentsSettings->setVisible(false);
    DockOutputSettings->setVisible(false);
    DockHist->setVisible(false);
    DockInfo->setVisible(false);
    dockWidget_Generate->setVisible(true);
    DockGenerateTestData->setVisible(false);

    SliceSelector->setFloating(false);
    DockMasksSettings->setFloating(false);
    DockCurvesSettings->setFloating(false);
    DockOutputSettings->setFloating(false);
    DockSegmentsSettings->setFloating(false);
    DockInfo->setFloating(false);
    DockHist->setFloating(false);
    DockPreview3D->setVisible(true);
    DockPreview3D->setFloating(false);
    dockWidget_Generate->setFloating(false);
    DockGenerateTestData->setFloating(false);

    GVHist = new histgv;
    DockHist->setWidget(GVHist);

    GenerateTestData *generateTestDataWidget = new GenerateTestData;
    DockGenerateTestData->setWidget(generateTestDataWidget);

    //sort out toolbar too
    toolBar->addSeparator();
    QLabel *l1 = new QLabel(" Brush Size:");
    l1->setMargin(5);
    toolBar->addWidget(l1);

    BrushSize = new KeysafeSpinBox;
    BrushSize->setKeyboardTracking(true);
    BrushSize->setMinimum(1);
    BrushSize->setMaximum(2000);
    BrushSize->setValue(1);
    toolBar->addWidget(BrushSize);   
}

/**
 * @brief MainWindow::on_actionRefresh_triggered
 * Slot called when the refresh action is triggered. Redraws the current image view.
 */
void MainWindow::on_actionRefresh_triggered()
{
    ShowImage(graphicsView);
}

/**
 * @brief MainWindow::on_action3D_Brush_toggled
 * Slot called when the 3D brush mode toggle is changed.
 * Updates brush mode and redraws the brush preview.
 * @param mode  True to enable 3D brush mode, false for 2D mode
 */
void MainWindow::on_action3D_Brush_toggled(bool mode)
{
    ThreeDmode = mode;
    SetUpBrushEnabling();
    if (ThreeDmode)
    {
        if (SquareBrush) Brush.resize(Brush_Size, 2, 0);
        else Brush.resize(Brush_Size, 3, 0);
        if (LastMouseX > 0) Brush.draw(LastMouseX, LastMouseY);
    }
    else
    {
        if (SquareBrush) Brush.resize(Brush_Size, 0, 0);
        else Brush.resize(Brush_Size, 1, 0);
        if (LastMouseX > 0) Brush.draw(LastMouseX, LastMouseY);
    }
}

/**
 * @brief MainWindow::SetUpBrushEnabling
 * Configures the enabled/disabled state of brush-related controls based on the current mode.
 * Currently placeholder for future brush mode-specific UI state management.
 */
void MainWindow::SetUpBrushEnabling()
{
    if (ThreeDmode)
    {
        //BrushSizeY->setEnabled(true);
        //BrushSizeZ->setEnabled(true);
        //Yaw->setEnabled(true);
        //etPitch->setEnabled(true);
        //Roll->setEnabled(true);
    }
    else
    {
//      BrushSizeY->setEnabled(false);
//      BrushSizeZ->setEnabled(false);
//      Yaw->setEnabled(false);
//      Pitch->setEnabled(false);
//      Roll->setEnabled(false);
    }
}

/**
 * @brief MainWindow::on_actionManual_triggered
 * Slot called when the manual/help action is triggered.
 * Opens the Read the Docs documentation page in the default web browser.
 */
void MainWindow::on_actionManual_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(READTHEDOCS)));
}

/**
 * @brief MainWindow::wheelEvent
 * Handles mouse wheel events for zooming the image view.
 * Each wheel tick increments/decrements the zoom slider.
 * @param event  The wheel event
 */
void MainWindow::wheelEvent(QWheelEvent *event)
{
    ZoomSlider->setValue(ZoomSlider->value() + event->angleDelta().y() / 12);
    event->ignore();
}

void MainWindow::on_actionExport_Curves_as_CSV_triggered()
{
    QString filen = QFileDialog::getSaveFileName(
                        this,
                        "CSV file for curves output",
                        "",
                        "CSV files (*.csv)");

    if (filen.isNull()) return; //if nothing there, cancel

    QFile file(filen);
    if (!file.open(QIODevice::WriteOnly))
    {
        Message("Warning - can't open");
        return;
    }
    QTextStream out(&file);

    //First header - standard text, v number, then SettingsFileName (might not be same as current filename - so store!)
    //do export of curves

    for (int i = 0; i < Curves.count(); i++)
    {
        for (int j = 0; j < Curves[i]->SplinePoints.count(); j++)
        {
            out << Curves[i]->SplinePoints[j]->Count;
            for (int k = 0; k < Curves[i]->SplinePoints[j]->X.count(); k++)
            {
                out << "," << Curves[i]->SplinePoints[j]->X[k];
                out << "," << Curves[i]->SplinePoints[j]->Y[k];
            }
            out << Qt::endl;
        }
    }
    file.close();
}

void MainWindow::on_actionImport_Curves_as_CSV_triggered()
{
    Message("This is an experimental bodge. Only use if the curves dataset you are importing has the same number of curves and the same number of tomograms");
    QString filen = QFileDialog::getOpenFileName(
                        this,
                        "CSV curves file",
                        "",
                        "CSV files (*.csv)");

    if (filen.isNull()) return; //if nothing there, cancel

    QFile file(filen);
    if (!file.open(QIODevice::ReadOnly))
    {
        Message("Warning - can't open");
        return;
    }

    //First header - standard text, v number, then SettingsFileName (might not be same as current filename - so store!)
    //do export of curves

    for (int i = 0; i < Curves.count(); i++)
    {
        for (int j = 0; j < Curves[i]->SplinePoints.count(); j++)
        {
            QString temp;
            temp = file.readLine();
            QStringList items = temp.split(",");
            Curves[i]->SplinePoints[j]->Count = items[0].toInt();
            Curves[i]->SplinePoints[j]->X.clear();
            Curves[i]->SplinePoints[j]->Y.clear();
            int pos = 1;

            for (int k = 0; k < Curves[i]->SplinePoints[j]->Count; k++)
            {
                Curves[i]->SplinePoints[j]->X.append((items[pos++]).toDouble());
                Curves[i]->SplinePoints[j]->Y.append((items[pos++]).toDouble());
            }
        }
    }
    file.close();
}

void MainWindow::on_actionOutput_visible_image_set_triggered()
{
    int SaveCurrentFile = CurrentFile = SliderPos->value();
    if (ExportingImages == true)
    {
        ExportingImages = false;
        return;
    }

    if (FullFiles.length() == 0) return;
    //Get current dir
    int lastsep = qMax(FullFiles.at(0).lastIndexOf("\\"), FullFiles.at(0).lastIndexOf("/")); //this is last separator in path
    QString workingdir = FullFiles.at(0).left(lastsep);

    QString targetdir = QFileDialog::getExistingDirectory(nullptr, "Select output directory", workingdir);

    //loop through all files

    ExportingImages = true;
    for (int i = 0; i < FileCount; i++)
    {
        Moveimage(i + 1);
        int percentcomplete = (i * 100) / FileCount;

        statusBar()->showMessage(QString("Exporting images... %1%. Select menu item again to abort.").arg(percentcomplete));
        qApp->processEvents();

        QString outstring;
        QTextStream out(&outstring);

        QString formattedi = QString::asprintf("%05i", i);
        out << targetdir << "/" << formattedi << ".png";
        SaveMainImage(outstring);

        if (ExportingImages == false) break;

    }
    SliderPos->setValue(SaveCurrentFile);
    statusBar()->showMessage("Done.");
}

/**
 * @brief MainWindow::on_actionBugIssueFeatureRequest_triggered
 * Slot called when the bug/issue/feature request action is triggered.
 * Opens the GitHub issues page for the SPIERS repository in the default web browser.
 */
void MainWindow::on_actionBugIssueFeatureRequest_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY) + QString(GITISSUE)));
}

/**
 * @brief MainWindow::on_actionCode_on_GitHub_triggered
 * Slot called when the GitHub code action is triggered.
 * Opens the SPIERS repository main page in the default web browser.
 */
void MainWindow::on_actionCode_on_GitHub_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY)));
}

/**
 * @brief MainWindow::DoGradientsUpdate
 * Called when gradient settings change in the UI to trigger a preview update.
 * Refreshes the image view to reflect the new gradient values.
 */
void MainWindow::DoGradientsUpdate()
{
    /// Redo preview with updated gradient values
    ShowImage(graphicsView);
}


