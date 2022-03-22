/**
 * @file
 * Header: Main Window
 *
 * All SPIERSalign code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSalign code is Copyright 2008-2019 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QDir>
#include <QDial>
#include <QGridLayout>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QSpinBox>

#include "scene.h"
#include "ui_mainwindow.h"
#include "about.h"

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindowImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    ~MainWindowImpl();

    void LogText(QString text);
    void redrawImage();
    void redrawJustDecorations();
    void redrawJustCropBox();
    void redrawJustAM();
    void on_actionLock_File_triggered(bool checked);
    void buildRecentFiles();
    int width;
    int height;

private slots:
    void on_actionLoad_Settings_File_triggered();
    void on_actionCompress_Dataset_triggered();
    void on_actionSwap_Image_With_Next_triggered();
    void on_actionRotate_Clockwise_Less_2_triggered();
    void on_actionReset_Scene_triggered();
    void on_actionReset_Image_triggered();
    void on_actionHide_Image_triggered();
    void on_actionShow_Image_triggered();
    void on_actionShow_All_triggered();
    void on_actionAdvance_To_Hidden_triggered();
    void on_actionRetreat_To_Hidden_triggered();
    void on_actionSave_Backup_triggered();
    void savetriggered(QString filename = "");
    void on_actionLock_Forward_triggered(bool checked);
    void on_actionLock_Back_triggered(bool checked);
    void on_actionMove_Forward_Back_triggered();
    void on_actionApply_Propogation_triggered();
    void on_actionPropogate_Mode_triggered(bool checked);
    void on_actionCrop_triggered();
    void on_actionMove_Marker_Left_triggered();
    void on_actionMove_Marker_Right_triggered();
    void on_actionMove_Marker_Up_triggered();
    void on_actionMove_Marker_Down_triggered();
    void on_actionSelect_Marker_triggered();
    void on_actionShift_Left_More_triggered();
    void on_actionShift_Left_less_triggered();
    void on_actionShift_Right_More_triggered();
    void on_actionShift_Right_Less_triggered();
    void on_actionShift_Up_More_triggered();
    void on_actionShift_Up_triggered();
    void on_actionShift_Up_Less_triggered();
    void on_actionShift_Down_More_triggered();
    void on_actionShift_Down_triggered();
    void on_actionShift_Down_Less_triggered();
    void on_actionCreate_Crop_Area_triggered(bool checked);
    void on_actionAbout_triggered();
    void on_actionEnlarge_More_triggered();
    void on_actionShrink_More_triggered();
    void on_actionEnlarge_Less_triggered();
    void on_actionShrink_Less_triggered();
    void on_actionEnlarge_triggered();
    void on_actionShrink_triggered();
    void on_actionShift_Left_triggered();
    void on_actionShift_Right_triggered();
    void on_actionRotate_Clockwise_More_triggered();
    void on_actionRotate_Anticlockwise_More_triggered();
    void on_actionRotate_Anticlockwise_Less_triggered();
    void on_actionRotate_Clockwise_triggered();
    void on_actionRotate_Anticlockwise_triggered();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionZoom_100_triggered();
    void on_actionFit_Window_triggered();
    void on_actionNext_Image_triggered();
    void on_actionPrevious_Image_triggered();
    void on_actionOpen_triggered();
    void on_actionInfo_triggered(bool checked);
    void on_actionManual_triggered();
    void on_actionAdd_Markers_triggered(bool checked);
    void on_actionAuto_Align_triggered (bool checked);
    void on_horizontalSlider_valueChanged(int value);
    void on_actionCode_on_GitHub_triggered();
    void on_actionBugIssueFeatureRequest_triggered();

    void selectMarker();
    void changeRed(int value);
    void changeGreen(int value);
    void changeBlue(int value);
    void changeShape();
    void addMarkerSlot();
    void removeMarkerSlot();
    void aMTopLeftXChanged(int value);
    void aMTopLeftYChanged(int value);
    void aMWidthChanged(int value);
    void aMHeightChanged(int value);
    void aMThicknessChanged(int value);
    void aMHorizChanged(int value);
    void aMVertChanged(int value);
    void executeAutoAlignTriggered();
    void openRecentFile();
    void changeMarkerSize(int size);
    void markersLockToggled();
    void autoMarkersToggled();
    void autoMarkersAlign();
    void autoMarkersGrid ();
    void setupAutoAlignTriggered();
    void okClicked();
    void resizeCropW(int value);
    void resizeCropH(int value);
    void clearList();
    void pickMarkerColourSlot();

private:
    void rotate (qreal rotateAngle);
    void resize(qreal sizeChange);
    void lateralShift(qreal shiftSize);
    void redrawShift();
    void verticalShift(qreal shiftSize);
    void redrawDecorations();

    CustomScene *scene;
    QGraphicsPixmapItem *pixMapPointer;
    QDir filesDirectory;
    QStringList drectoryFileList;
    QDockWidget *markersDialogue;
    QDockWidget *info, *cropDock, *aMOptions, *autoAlign;
    QSpinBox *red, *red2;
    QSpinBox *green, *green2;
    QSpinBox *blue, *blue2;
    QGridLayout *aMGridLayout;
    QVBoxLayout *markerLayout, *infoLayout, *cropLayout, *aMVertLayout, *autoLayout;
    QHBoxLayout *horizontalLayout1, *horizontalLayout2, *horizontalLayout3, *horizontalLayout4, *horizontalLayout5, *horizontalLayout6, *horizontalLayout7, *horizontalLayout8, *horizontalLayout9,
                *horizontalLayout10, *horizontalLayout11;
    QSpinBox *mThickness, *mSize, *cropWidth, *cropHeight;
    QSpinBox *aMTopLeftX, *aMTopLeftY, *aMWidth, *aMHeight, *aMThickness, *aMHoriz, *aMVert;
    QWidget *layoutWidgetOne, *layoutWidgetTwo, *layoutWidgetThree, *layoutWidgetFour, *layoutWidgetFive;
    QPlainTextEdit *notes;
    QRectF sceneRectangle;
    QPushButton *lockMarkers, *autoMarkers, *grid, *setupAlign, *pickMarkerColourButton;
    QString filesDirectoryString;
    int propogateImage, propogateStep, lockImage;
    int redValue;
    int greenValue;
    int blueValue;
    int verbose;

};

extern MainWindowImpl *theMainWindow;
#endif




