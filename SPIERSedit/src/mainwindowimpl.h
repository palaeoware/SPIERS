/**
 * @file
 * Header: File IO
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
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

#include <QAction>
#include <QEvent>
#include <QTime>
#include <QTreeWidgetItem>
#include <QSpinBox>
#include <QGraphicsView>

#include "../ui/ui_mainwindow.h"

/**
 * @brief The MainWindowImpl class
 */
class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindowImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    ~MainWindowImpl();
    QTimer *timer2;
    void RefreshOneSegmentItem(QTreeWidgetItem *item, int i);
    void RefreshSegmentsBoxes();
    void SetUpGenerationToolbox(int s);
    void GenButton();
    void SetUpGUIFromSettings();
    void Start();
    void MouseZoom(int delta);
private slots:
    void Moveimage(int);
    void on_action3D_Brush_toggled(bool );
    void on_actionInterpolate_over_selected_slices_triggered();
    void on_actionRefresh_triggered();
    void on_actionExport_SPIERSview_and_Launch_triggered();
    void on_actionMeasure_Volumes_triggered();
    void on_actionAb_out_triggered();
    void on_actionChange_downsampling_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_CheckBoxRangeSelectedOnly_toggled(bool checked);
    void on_CheckBoxRangeHardFill_toggled(bool checked);
    void on_actionDistribute_over_range_triggered();
    void on_SpinBoxRangeBase_valueChanged(int v);
    void on_SpinBoxRangeTop_valueChanged(int v);
    void on_actionHidden_masks_locked_for_generation_triggered();
    void on_actionSegment_brush_applies_masks_triggered();
    void on_FindPolynomial_pressed();
    void on_actionShow_position_slice_selector_toggled(bool );
    void on_actionChange_slice_spacing_triggered();
    void on_actionSet_slice_position_triggered();
    void on_actionInfo_triggered(bool checked);
    void on_actionExport_DXF_triggered();
    void on_OOTreeWidget_itemPressed(QTreeWidgetItem *item, int column);
    void on_CurvesTreeWidget_itemPressed(QTreeWidgetItem *item, int column);
    void on_actionCreate_mask_from_curve_triggered();
    void on_SliceSelectorList_itemSelectionChanged();
    void on_actionMaskCopy_selected_from_Previous_triggered();
    void on_actionMaskCopy_all_from_previous_triggered();
    void on_actionMaskCopy_selected_from_next_triggered();
    void on_actionMaskCopy_all_from_next_triggered();
    void on_actionMask_Advance_slice_after_copy_operation_toggled(bool );
    void on_actionMaskGo_back_one_slice_after_copy_toggled(bool );
    void on_actionOutput_SPIERSview_triggered();
    void on_actionDelete_Output_Object_triggered();
    void on_actionNew_Output_Object_triggered();
    void on_actionNew_Output_Object_Group_triggered();
    void on_actionMove_item_to_Group_triggered();
    void on_actionUngroup_triggered();
    void on_OOTreeWidget_itemSelectionChanged();
    void on_SegmentsTreeWidget_itemSelectionChanged();
    void on_MasksTreeWidget_itemSelectionChanged();
    void on_OONew_clicked();
    void on_OOGroup_clicked();
    void on_OODelete_clicked();
    void on_OOUp_clicked();
    void on_OODown_clicked();
    void on_OOTreeWidget_expanded(QModelIndex index);
    void on_OOTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_actionResize_keeping_curve_shape_triggered();
    void on_actionLock_curve_shape_triggered(bool checked);
    void on_actionRemove_selected_curves_from_selected_slices_triggered();
    void on_actionCopy_from_current_slice_to_selected_triggered();
    void on_CurvesTreeWidget_itemSelectionChanged();
    void on_actionCopyCurvesFromPrevious_triggered();
    void on_actionCopyCurvesFromNext_triggered();
    void on_actionCopyCurvesToCurrent_triggered();
    void on_Curve_Add_pressed();
    void on_CurveDelete_pressed();
    void on_CurveMoveUp_pressed();
    void on_CurveMoveDown_pressed();
    void on_actionAdd_new_point_triggered();
    void on_actionRemove_node_under_cursor_triggered();
    void on_actionCurve_markers_as_crosses_triggered();
    void on_CurvesTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_CurvesTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_actionHistogram_shows_selected_triggered(bool checked);
    void on_actionHistorgram_triggered(bool checked);
    void on_actionSettings_triggered();
    void on_SpinBoxSparsity_valueChanged(int );
    void on_SpinBoxOrder_valueChanged(int );
    void on_SpinBoxRetries_valueChanged(int );
    void on_SpinBoxContrast_valueChanged(int );
    void on_LinearGreenSpinBox_valueChanged(int );
    void on_LinearRedSpinBox_valueChanged(int );
    void on_LinearGreenSpinBox_editingFinished();
    void on_LinearBlueSpinBox_valueChanged(int );
    void on_LinearGlobalSpinBox_valueChanged(int );
    void on_GenInvert_toggled(bool checked);
    void on_GenerateAuto_toggled(bool checked);
    void on_GenerateButton_clicked();
    void on_actionNew_mask_triggered();
    void on_actionDelete_selected_mask_s_triggered();
    void on_actionCreate_new_segment_triggered();
    void on_actionDelete_selected_segments_triggered();
    void on_SegmentAdd_pressed();
    void on_SegmentDelete_pressed();
    void on_SegmentMoveUp_pressed();
    void on_SegmentMoveDown_pressed();
    void on_actionLock_selected_segments_triggered();
    void on_actionUnlock_selected_segments_triggered();
    void on_actionActivate_selected_segments_triggered();
    void on_actionDeactivate_selected_segments_triggered();
    void on_SegmentsTreeWidget_itemPressed(QTreeWidgetItem *item, int column);
    void on_SegmentsTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_SegmentsTreeWidget_pressed(QModelIndex index);
    void on_SegmentsTreeWidget_doubleClicked(QModelIndex index);
    void on_MasksTreeWidget_itemPressed(QTreeWidgetItem *item, int column);
    void on_MaskAdd_pressed();
    void on_MaskDelete_pressed();
    void on_MaskMoveUp_pressed();
    void on_MaskMoveDown_pressed();
    void on_actionLock_Selected_Masks_triggered();
    void on_actionUnlock_Selected_Masks_triggered();
    void on_actionShow_Selected_Masks_triggered();
    void on_actionUnShow_Selected_Masks_triggered();
    void on_actionWrite_Selected_Masks_triggered();
    void on_actionUnWrite_Selected_Masks_triggered();
    void on_MasksTreeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void on_MasksTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_SpinBoxConverge_valueChanged(int );
    void on_FirstFile_valueChanged(int );
    void on_LastFile_valueChanged(int );
    void on_DownsampleXY_valueChanged(int );
    void on_DownsampleZ_valueChanged(int );
    void on_PixelSensitivity_valueChanged(int );
    void on_Edge_Down_MM_valueChanged(double );
    void on_Edge_Left_MM_valueChanged(double );
    void on_CheckMirror_toggled(bool checked);
    void on_Pixels_Per_MM_valueChanged(double );
    void on_Slices_Per_MM_valueChanged(double );
    void on_actionView_in_SPIERSview_triggered();
    void on_PosSpinBox_valueChanged(int);
    void on_ZoomSpinBox_valueChanged(int);
    void on_actionManual_triggered();
    void on_actionGrey_out_curves_not_no_current_slice_triggered();
    void on_actionExport_Curves_as_CSV_triggered();
    void on_actionImport_Curves_as_CSV_triggered();
    void on_actionOutput_visible_image_set_triggered();
    void on_actionCode_on_GitHub_triggered();
    void on_actionBugIssueFeatureRequest_triggered();

    void ScreenUpdate();
    void Menu_Window_MainToolbox();
    void Menu_Window_Generate();
    void Menu_File_Import();
    void InitStates();
    void Zoom_Slider_Changed(int);
    void Trans_Changed(int);
    void Min_Changed(int);
    void Max_Changed(int);
    void Preset1();
    void Preset2();
    void Preset3();
    void Preset4();
    void Preset5();
    void Preset6();
    void Preset7();
    void Preset8();
    void Preset9();
    void Preset0();
    void Mode_Changed(QAction *);
    void BrushChanged(int);
    void BrightDownChanged(int);
    void BrightUpChanged(int);
    void SoftChanged(int);
    void TransToggled();
    void SquareToggled();
    void ThresholdFlag();
    void SetMasksFlag();
    void SetSegsFlag();
    void cmac();
    void LockShape();
    void BuildRecentFiles();
    void openRecentFile();
    void openMore();

    void q_pressed();
    void a_pressed();
    void right_pressed();
    void left_pressed();
    void LeftMaskChanged(int index);
    void RightMaskChanged(int index);
    void LeftSegChanged(int index);
    void RightSegChanged(int index);
    void SaveSettings();
    void SaveAs();
    void FileOpen();
    void Menu_File_New();
    void UndoTimer();
    void Undo();
    void Redo();
    void Menu_Window_SliceSelector();
    void Menu_Window_Masks();
    void Menu_Window_Curves();
    void Menu_Window_Segments();
    void Menu_Window_Output();

    void autosave();
    void on_actionTEST_triggered();

private:
    void MakeUndo(QString type);
    void RefreshMasks();
    void RefreshSegments();
    void RefreshOO();
    void RefreshCurves();
    void RefreshOneMaskItem(QTreeWidgetItem *item, int i);
    void RefreshOneCurveItem(QTreeWidgetItem *item, int i);
    void RefreshOneOOItem(QTreeWidgetItem *item, int i);
    void RefreshMasksBoxes();
    void CurveCopy(int fromfile);
    void ResetUndo();
    void OODrawChildren( QList <bool> selflags, int parent);
    void OOunderlineChildren(QTreeWidgetItem *par);
    bool AmIMerged(int i);
    void CleanseOO();
    bool event(QEvent *event);
    void FixUpStretches();
    QString TextForSliceSelectorBox(int i);
    QAction *shortcutright, *shortcutleft, *shortcutright2, *shortcutleft2, *shortcutspace;
    QSpinBox *ZoomBox;
    QSpinBox *BrushSize;
    QList<double>OldStretches;
    QGraphicsView *RangeGraphicsView;
    void SetUpDocks();
    void SetUpBrushEnabling();
    bool DontRedoZoom;

    void wheelEvent(QWheelEvent *event);

    bool ExportingImages;

    QTime DoubleClickTimer;
};

extern MainWindowImpl *AppMainWindow;

#endif
