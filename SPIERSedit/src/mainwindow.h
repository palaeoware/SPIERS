/**
 * @file
 * Header: Main Window
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QEvent>
#include <QTime>
#include <QList>
#include <QTreeWidgetItem>
#include <QSpinBox>
#include <QGraphicsView>

#include "../ui/ui_mainwindow.h"
#include "beamhardeningcentericon.h"
#include "beamhardening.h"
#include "qelapsedtimer.h"

/**
 * @brief The MainWindow class
 *
 * Central controller for the SPIERSedit application. Inherits QMainWindow and
 * the Qt Designer-generated Ui::MainWindow to provide the full editing
 * environment for tomographic image stacks.
 *
 * Responsibilities:
 *  - Loading, saving, and managing image-stack datasets.
 *  - Painting and managing segment masks on individual slices.
 *  - Creating and editing output objects (OOs) and curve annotations.
 *  - Driving the generation toolbox (threshold, linear, gradient, ML modes).
 *  - Hosting dock widgets for the slice selector, masks, segments, curves,
 *    and output-object panels.
 *  - Exporting to SPIERSview (.spv), DXF, and image sets.
 *  - Undo/redo management and autosave.
 *  - Beam-hardening correction controls.
 *  - Machine-learning feature extraction and random-forest classification.
 */
class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    /**
     *
     * Constructs the main window, wires up all dock widgets, toolbars, timers,
     * and keyboard shortcuts, and installs the initial application state.
     *
     **/
    MainWindow( QWidget *parent = nullptr, Qt::WindowFlags f = {} );

    /**
     *
     * Destructor — saves window geometry and tears down owned resources.
     *
     **/
    ~MainWindow();

    QTimer *timer2; /// Secondary timer used for autosave and deferred UI updates

    /**
     *
     * Rebuilds the tree widget row for segment @p i, updating its colour
     * swatch, activation/lock icons, and selection marker.
     *
     **/
    void RefreshOneSegmentItem(QTreeWidgetItem *item, int i);

    /**
     *
     * Repopulates the left/right segment combo boxes from the current segment list.
     *
     **/
    void RefreshSegmentsBoxes();

    /**
     *
     * Configures the generation toolbox UI for the given generation mode @p s.
     *
     **/
    void SetUpGenerationToolbox(int s);

    /**
     *
     * Triggers the currently active generation mode to produce mask output.
     *
     **/
    void GenButton();

    /**
     *
     * Populates all GUI controls from the currently loaded dataset settings.
     *
     **/
    void SetUpGUIFromSettings();

    /**
     *
     * Performs full application startup after a dataset has been opened:
     * rebuilds all tree widgets, refreshes the display, and enables editing
     * commands.
     *
     **/
    void Start();

    /**
     *
     * Applies a zoom delta triggered by the mouse wheel.
     *
     **/
    void MouseZoom(int delta);

    /** @name Radial (beam-hardening) centre and radius controls
     *  These setters/getters relay values between the beam-hardening spin
     *  boxes and the BeamHardening processing object.
     * @{ */
    void SetRadialCentreX(int v);   /// Sets the X coordinate of the radial correction centre.
    void SetRadialCentreY(int v);   /// Sets the Y coordinate of the radial correction centre.
    void SetRadialRadius(int v);    /// Sets the outer radius for the radial correction zone.
    void SetRadialAdjust(int v);    /// Sets the adjustment strength for the radial correction.
    int  GetRadialCentreX();        /// Returns the current X coordinate of the radial correction centre.
    int  GetRadialCentreY();        /// Returns the current Y coordinate of the radial correction centre.
    int  GetRadialRadius();         /// Returns the current outer radius for the radial correction zone.
    int  GetRadialAdjust();         /// Returns the current radial correction adjustment strength.
    /** @} */

    /** @name Gradient generation controls
     *  Relay values between the gradient-mode spin boxes and the generation
     *  subsystem.
     * @{ */
    void SetGradientDensity(int v);        /// Sets the sample point density for gradient generation.
    void SetGradientMinDist(int v);        /// Sets the minimum distance threshold for gradient generation.
    void SetGradientMaxDist(int v);        /// Sets the maximum distance threshold for gradient generation.
    void SetGradientMinDistValue(int v);   /// Sets the value mapped to the minimum gradient distance.
    void SetGradientMaxDistValue(int v);   /// Sets the value mapped to the maximum gradient distance.
    /** @} */

public slots:
    /**
     *
     * Responds to network connectivity changes, enabling or disabling the
     * update-check action accordingly.
     *
     **/
    void onConnectivityChanged(bool online);

private slots:

    // ── Slice navigation ──────────────────────────────────────────────────
    void Moveimage(int);                /// Loads and displays the slice at the given index.

    // ── 3D preview ────────────────────────────────────────────────────────
    /**
     * @brief updatePreview3DStepCombos
     * Refreshes both 3D-preview step combos (XY and Z): disables step
     * combinations whose texture would exceed available GPU VRAM given the
     * other combo's current selection. ⅛/⅛ is always kept enabled as the
     * minimum fallback.
     */
    void updatePreview3DStepCombos();

    // ── File menu actions ─────────────────────────────────────────────────
    void Menu_File_Import();                              /// Opens the import dialog for adding image files to the dataset.
    void Menu_File_New();                                 /// Starts a new empty dataset, prompting to save if needed.
    void FileOpen();                                      /// Opens the file-chooser dialog and loads a dataset.
    void SaveSettings();                                  /// Saves the current dataset settings to disk.
    void SaveAs();                                        /// Saves the current dataset to a new location.
    void BuildRecentFiles();                              /// Rebuilds the Recent Files submenu from the stored list.
    void openRecentFile();                                /// Opens the dataset identified by the triggering recent-file action.
    void openMore();                                      /// Opens additional image files and appends them to the current dataset.
    void on_actionExport_SPIERSview_and_Launch_triggered(); /// Exports the dataset to an SPV file and launches SPIERSview.
    void on_actionOutput_SPIERSview_triggered();          /// Exports the dataset to an SPV file without launching the viewer.
    void on_actionExport_DXF_triggered();                 /// Exports the current output objects as a DXF file.
    void on_actionOutput_visible_image_set_triggered();   /// Exports the currently visible slice set as an image sequence.
    void on_actionView_in_SPIERSview_triggered();         /// Launches SPIERSview with the most recently exported SPV file.
    void on_actionExport_Curves_as_CSV_triggered();       /// Exports all curve annotations to a CSV file.
    void on_actionImport_Curves_as_CSV_triggered();       /// Imports curve annotations from a CSV file.

    // ── Edit / undo ───────────────────────────────────────────────────────
    void Undo();                                          /// Steps back one undo state.
    void Redo();                                          /// Steps forward one undo state.
    void UndoTimer();                                     /// Timer callback that commits the current slice state as an undo step.
    void autosave();                                      /// Autosave timer callback; writes the dataset if it has been modified.

    // ── View / window menu actions ────────────────────────────────────────
    void Menu_Window_MainToolbox();                       /// Toggles visibility of the main toolbox dock.
    void Menu_Window_Generate();                          /// Toggles visibility of the generation toolbox dock.
    void Menu_Window_SliceSelector();                     /// Toggles visibility of the slice-selector dock.
    void Menu_Window_Masks();                             /// Toggles visibility of the masks dock.
    void Menu_Window_Curves();                            /// Toggles visibility of the curves dock.
    void Menu_Window_Segments();                          /// Toggles visibility of the segments dock.
    void Menu_Window_Output();                            /// Toggles visibility of the output objects dock.
    void Menu_Window_GenerateTestData();                  /// Toggles visibility of the generate test data dock.
    void ScreenUpdate();                                  /// Forces a full repaint of the current slice view.
    void InitStates();                                    /// Resets all tool-state flags to their default values.

    // ── Zoom and display controls ─────────────────────────────────────────
    void Zoom_Slider_Changed(int);                        /// Responds to the zoom slider changing value.
    void Trans_Changed(int);                              /// Responds to the transparency slider changing value.
    void Min_Changed(int);                                /// Responds to the brightness-minimum slider changing value.
    void Max_Changed(int);                                /// Responds to the brightness-maximum slider changing value.
    void on_ZoomSpinBox_valueChanged(int);                /// Responds to direct zoom spin box edits.
    void on_PosSpinBox_valueChanged(int);                 /// Responds to the slice-position spin box changing value.
    void on_actionChange_downsampling_triggered();        /// Opens the downsampling change dialog.
    void on_actionRefresh_triggered();                    /// Reloads and redraws the current slice from disk.

    // ── Brightness / contrast presets ────────────────────────────────────
    void Preset1();   /// Applies brightness/contrast preset 1.
    void Preset2();   /// Applies brightness/contrast preset 2.
    void Preset3();   /// Applies brightness/contrast preset 3.
    void Preset4();   /// Applies brightness/contrast preset 4.
    void Preset5();   /// Applies brightness/contrast preset 5.
    void Preset6();   /// Applies brightness/contrast preset 6.
    void Preset7();   /// Applies brightness/contrast preset 7.
    void Preset8();   /// Applies brightness/contrast preset 8.
    void Preset9();   /// Applies brightness/contrast preset 9.
    void Preset0();   /// Applies brightness/contrast preset 0 (reset to defaults).

    // ── Drawing mode and brush controls ──────────────────────────────────
    void Mode_Changed(QAction *);                         /// Responds to a drawing-mode action being toggled.
    void BrushChanged(int);                               /// Responds to the brush-shape selector changing.
    void BrightDownChanged(int);                          /// Responds to the brightness lower-bound spin box changing.
    void BrightUpChanged(int);                            /// Responds to the brightness upper-bound spin box changing.
    void SoftChanged(int);                                /// Responds to the soft-brush radius spin box changing.
    void BoostSpinBoxChanged(int);                        /// Responds to the boost-strength spin box changing.
    void BoostRadiusSpinBoxChanged(int);                  /// Responds to the boost-radius spin box changing.
    void BoostAdjustSpinBoxChanged(int);                  /// Responds to the boost-adjust spin box changing.
    void TransToggled();                                  /// Toggles transparency display on/off.
    void SquareToggled();                                 /// Toggles square-brush mode on/off.
    void ThresholdFlag();                                 /// Toggles threshold-assist flag for mask painting.
    void SetMasksFlag();                                  /// Toggles the "set masks" paint mode flag.
    void SetSegsFlag();                                   /// Toggles the "set segments" paint mode flag.
    void LockShape();                                     /// Locks the current curve shape against further edits.
    void cmac();                                          /// Applies the current mask to all slices in the selected range.
    void on_action3D_Brush_toggled(bool);                 /// Toggles 3D-brush (interpolating) paint mode.
    void on_actionThreshold_triggered();                  /// Opens the threshold-generation dialog.
    void on_action1_x_1_triggered();                      /// Resets the brush size to 1×1.
    void on_actionIncrease_Size_triggered();              /// Increases the current brush size by one step.
    void on_actionDecrease_Size_triggered();              /// Decreases the current brush size by one step.
    void on_actionCycle_Bright_Segment_Mask_triggered();  /// Cycles through the brightness-segment-mask display modes.

    // ── Keyboard shortcut slots ───────────────────────────────────────────
    void q_pressed();       /// Keyboard shortcut: step one slice forward.
    void a_pressed();       /// Keyboard shortcut: step one slice backward.
    void right_pressed();   /// Keyboard shortcut: nudge selection right.
    void left_pressed();    /// Keyboard shortcut: nudge selection left.

    // ── Slice range controls ──────────────────────────────────────────────
    void on_FirstFile_valueChanged(int);                          /// Updates the first-slice index of the active range.
    void on_LastFile_valueChanged(int);                           /// Updates the last-slice index of the active range.
    void on_DownsampleXY_valueChanged(int);                       /// Updates the XY downsampling factor.
    void on_DownsampleZ_valueChanged(int);                        /// Updates the Z (slice) downsampling factor.
    void on_PixelSensitivity_valueChanged(int);                   /// Updates the pixel-sensitivity threshold value.
    void on_Edge_Down_MM_valueChanged(double);                    /// Updates the bottom crop edge in millimetres.
    void on_Edge_Left_MM_valueChanged(double);                    /// Updates the left crop edge in millimetres.
    void on_CheckMirror_toggled(bool checked);                    /// Toggles horizontal mirroring of the slice display.
    void on_Pixels_Per_MM_valueChanged(double);                   /// Updates the pixel pitch (pixels per millimetre).
    void on_Slices_Per_MM_valueChanged(double);                   /// Updates the slice pitch (slices per millimetre).
    void on_CheckBoxRangeSelectedOnly_toggled(bool checked);      /// Toggles generation to operate on selected slices only.
    void on_CheckBoxRangeHardFill_toggled(bool checked);          /// Toggles hard-fill mode for range-based generation.
    void on_actionDistribute_over_range_triggered();              /// Distributes the current mask state across the selected range.
    void on_SpinBoxRangeBase_valueChanged(int v);                 /// Updates the lower bound of the generation range.
    void on_SpinBoxRangeTop_valueChanged(int v);                  /// Updates the upper bound of the generation range.
    void on_actionInterpolate_over_selected_slices_triggered();   /// Interpolates mask data across the selected slices.
    void on_actionChange_slice_spacing_triggered();               /// Opens the slice-spacing adjustment dialog.
    void on_actionSet_slice_position_triggered();                 /// Opens the slice absolute-position dialog.

    // ── Dataset / slice selector ──────────────────────────────────────────
    void on_SliceSelectorList_itemSelectionChanged();   /// Responds to the slice-selector list selection changing.
    void on_actionShow_position_slice_selector_toggled(bool); /// Toggles display of position numbers in the slice selector.

    // ── Mask management ───────────────────────────────────────────────────
    void on_actionNew_mask_triggered();                              /// Creates a new mask with default settings.
    void on_actionDelete_selected_mask_s_triggered();               /// Deletes all currently selected masks.
    void on_MaskAdd_pressed();                                       /// Adds a new mask (toolbar button equivalent of New Mask).
    void on_MaskDelete_pressed();                                    /// Deletes the selected mask (toolbar button).
    void on_MaskMoveUp_pressed();                                    /// Moves the selected mask up in the list order.
    void on_MaskMoveDown_pressed();                                  /// Moves the selected mask down in the list order.
    void on_actionLock_Selected_Masks_triggered();                   /// Locks all selected masks against painting.
    void on_actionUnlock_Selected_Masks_triggered();                 /// Unlocks all selected masks.
    void on_actionShow_Selected_Masks_triggered();                   /// Makes all selected masks visible in the display.
    void on_actionUnShow_Selected_Masks_triggered();                 /// Hides all selected masks from the display.
    void on_actionWrite_Selected_Masks_triggered();                  /// Enables writing (painting) on all selected masks.
    void on_actionUnWrite_Selected_Masks_triggered();                /// Disables writing on all selected masks.
    void on_actionHidden_masks_locked_for_generation_triggered();    /// Toggles whether hidden masks are excluded from generation.
    void on_actionMaskCopy_selected_from_Previous_triggered();       /// Copies selected mask data from the previous slice.
    void on_actionMaskCopy_all_from_previous_triggered();            /// Copies all mask data from the previous slice.
    void on_actionMaskCopy_selected_from_next_triggered();           /// Copies selected mask data from the next slice.
    void on_actionMaskCopy_all_from_next_triggered();                /// Copies all mask data from the next slice.
    void on_actionMask_Advance_slice_after_copy_operation_toggled(bool); /// Toggles automatic slice advance after a mask copy.
    void on_actionMaskGo_back_one_slice_after_copy_toggled(bool);   /// Toggles automatic slice step-back after a mask copy.
    void on_MasksTreeWidget_itemPressed(QTreeWidgetItem *item, int column);   /// Handles a press on a mask tree item to select or toggle state.
    void on_MasksTreeWidget_itemChanged(QTreeWidgetItem *item, int column);   /// Responds to an inline edit in the mask tree (e.g. name change).
    void on_MasksTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column); /// Opens the mask properties dialog on double-click.
    void on_MasksTreeWidget_itemSelectionChanged();                  /// Updates the active mask when the tree selection changes.
    void LeftMaskChanged(int index);                                 /// Responds to the left-mask combo selection changing.
    void RightMaskChanged(int index);                                /// Responds to the right-mask combo selection changing.

    // ── Segment management ────────────────────────────────────────────────
    void on_actionCreate_new_segment_triggered();                    /// Creates a new segment with default settings.
    void on_actionDelete_selected_segments_triggered();              /// Deletes all currently selected segments.
    void on_SegmentAdd_pressed();                                    /// Adds a new segment (toolbar button equivalent of Create Segment).
    void on_SegmentDelete_pressed();                                 /// Deletes the selected segment (toolbar button).
    void on_SegmentMoveUp_pressed();                                 /// Moves the selected segment up in the list order.
    void on_SegmentMoveDown_pressed();                               /// Moves the selected segment down in the list order.
    void on_actionLock_selected_segments_triggered();                /// Locks all selected segments against activation.
    void on_actionUnlock_selected_segments_triggered();              /// Unlocks all selected segments.
    void on_actionActivate_selected_segments_triggered();            /// Activates (shows in display) all selected segments.
    void on_actionDeactivate_selected_segments_triggered();          /// Deactivates all selected segments from the display.
    void on_actionSegment_brush_applies_masks_triggered();           /// Toggles whether the segment brush also paints masks.
    void on_actionSegment_brush_applies_locks_triggered();           /// Toggles whether the segment brush also applies locks.
    void on_SegmentsTreeWidget_itemPressed(QTreeWidgetItem *item, int column);     /// Handles a press on a segment tree item.
    void on_SegmentsTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column); /// Opens segment properties on double-click.
    void on_SegmentsTreeWidget_pressed(QModelIndex index);           /// Low-level press handler for the segments tree.
    void on_SegmentsTreeWidget_doubleClicked(QModelIndex index);     /// Low-level double-click handler for the segments tree.
    void on_SegmentsTreeWidget_itemSelectionChanged();               /// Updates the active segment when the tree selection changes.
    void LeftSegChanged(int index);                                  /// Responds to the left-segment combo selection changing.
    void RightSegChanged(int index);                                 /// Responds to the right-segment combo selection changing.

    // ── Output object management ──────────────────────────────────────────
    void on_actionNew_Output_Object_triggered();                              /// Creates a new output object.
    void on_actionNew_Output_Object_Group_triggered();                        /// Creates a new output object group.
    void on_actionDelete_Output_Object_triggered();                           /// Deletes the selected output object(s).
    void on_actionMove_item_to_Group_triggered();                             /// Moves the selected output object into a group.
    void on_actionUngroup_triggered();                                        /// Removes the selected output object from its group.
    void on_OONew_clicked();                                                  /// Toolbar button: creates a new output object.
    void on_OOGroup_clicked();                                                /// Toolbar button: creates a new output object group.
    void on_OODelete_clicked();                                               /// Toolbar button: deletes the selected output object.
    void on_OOUp_clicked();                                                   /// Toolbar button: moves the selected output object up.
    void on_OODown_clicked();                                                 /// Toolbar button: moves the selected output object down.
    void on_OOTreeWidget_itemPressed(QTreeWidgetItem *item, int column);      /// Handles a press on an output object tree item.
    void on_OOTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column); /// Opens output object properties on double-click.
    void on_OOTreeWidget_expanded(QModelIndex index);                         /// Responds to a group node being expanded in the OO tree.
    void on_OOTreeWidget_itemSelectionChanged();                              /// Updates the active output object when the tree selection changes.

    // ── Curve management ──────────────────────────────────────────────────
    void on_Curve_Add_pressed();                                               /// Adds a new curve annotation to the current slice.
    void on_CurveDelete_pressed();                                             /// Deletes the selected curve.
    void on_CurveMoveUp_pressed();                                             /// Moves the selected curve up in the list order.
    void on_CurveMoveDown_pressed();                                           /// Moves the selected curve down in the list order.
    void on_actionCreate_mask_from_curve_triggered();                          /// Fills a mask region from the boundary of the selected curve.
    void on_actionResize_keeping_curve_shape_triggered();                      /// Resizes curve nodes while preserving the overall shape.
    void on_actionLock_curve_shape_triggered(bool checked);                    /// Locks the selected curve against node edits.
    void on_actionAdd_new_point_triggered();                                   /// Adds a new node to the selected curve at the cursor position.
    void on_actionRemove_node_under_cursor_triggered();                        /// Removes the curve node nearest to the cursor.
    void on_actionCurve_markers_as_crosses_triggered();                        /// Toggles curve node display between circles and crosses.
    void on_actionRemove_selected_curves_from_selected_slices_triggered();     /// Removes selected curves from all selected slices.
    void on_actionCopy_from_current_slice_to_selected_triggered();             /// Copies curves from the current slice to all selected slices.
    void on_actionCopyCurvesFromPrevious_triggered();                          /// Copies curves from the previous slice to the current slice.
    void on_actionCopyCurvesFromNext_triggered();                              /// Copies curves from the next slice to the current slice.
    void on_actionCopyCurvesToCurrent_triggered();                             /// Copies the clipboard curves to the current slice.
    void on_actionGrey_out_curves_not_no_current_slice_triggered();            /// Toggles greying-out of curves that have no data on the current slice.
    void on_CurvesTreeWidget_itemPressed(QTreeWidgetItem *item, int column);   /// Handles a press on a curve tree item.
    void on_CurvesTreeWidget_itemSelectionChanged();                           /// Updates the active curve when the tree selection changes.
    void on_CurvesTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous); /// Responds to the current curve item changing.
    void on_CurvesTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column); /// Opens curve properties on double-click.

    // ── Generation controls ───────────────────────────────────────────────
    void on_tabWidget_currentChanged(int index);             /// Responds to the generation-mode tab changing.
    void on_LinearGreenSpinBox_valueChanged(int);            /// Updates the green-channel linear generation threshold.
    void on_LinearRedSpinBox_valueChanged(int);              /// Updates the red-channel linear generation threshold.
    void on_LinearGreenSpinBox_editingFinished();            /// Commits a direct edit to the green-channel spin box.
    void on_LinearBlueSpinBox_valueChanged(int);             /// Updates the blue-channel linear generation threshold.
    void on_LinearGlobalSpinBox_valueChanged(int);           /// Updates the global (greyscale) linear generation threshold.
    void on_GenInvert_toggled(bool checked);                 /// Toggles inversion of the generation result.
    void on_GenerateAuto_toggled(bool checked);              /// Toggles automatic generation on every slice advance.
    void on_GenerateButton_clicked();                        /// Manually triggers generation for the current slice.

    // ── Gradient generation ───────────────────────────────────────────────
    void on_spinGradientsDistMinEffect_valueChanged(int);    /// Updates the minimum-effect distance for gradient generation.
    void on_spinGradientsDistMaxEffect_valueChanged(int);    /// Updates the maximum-effect distance for gradient generation.
    void on_spinGradientsDistMin_valueChanged(int);          /// Updates the inner distance bound for gradient generation.
    void on_spinGradientsDistMax_valueChanged(int);          /// Updates the outer distance bound for gradient generation.
    void on_chkGradientsPreview_toggled(bool checked);       /// Toggles live gradient preview overlay on/off.
    void on_spinGradientsPointDensity_valueChanged(int);     /// Updates the point density for gradient sample generation.

    // ── Beam-hardening correction ─────────────────────────────────────────
    void on_Measure_clicked();                               /// Samples the image to measure beam-hardening parameters.
    void on_ShowCenter_stateChanged(int);                    /// Toggles display of the beam-hardening centre overlay.
    void on_CentreX_valueChanged(int);                       /// Updates the beam-hardening correction centre X coordinate.
    void on_CentreY_valueChanged(int);                       /// Updates the beam-hardening correction centre Y coordinate.
    void on_HardeningRadiusSpinBox_valueChanged(int);        /// Updates the beam-hardening correction radius.
    void on_AdjustRadialSpinBox_valueChanged(int);           /// Updates the beam-hardening radial adjustment value.
    void on_SetCentre_clicked();                             /// Commits the current centre coordinates as the correction origin.

    // ── Histogram ─────────────────────────────────────────────────────────
    void on_actionHistogram_shows_selected_triggered(bool checked); /// Toggles the histogram to show only selected-mask pixels.
    void on_actionHistorgram_triggered(bool checked);               /// Toggles visibility of the histogram dock.

    // ── Machine learning ──────────────────────────────────────────────────
    void on_trainML_clicked();                               /// Trains the random-forest classifier from the current samples.
    void on_btnMLActivateFeature_clicked();                  /// Moves the selected feature from inactive to active.
    void on_btnMLDeactivateFeature_clicked();                /// Moves the selected feature from active to inactive.
    void on_btnMLAddFeature_clicked();                       /// Opens the add-feature dialog.
    void on_btnMLRemoveFeature_clicked();                    /// Removes the selected feature from the feature set.
    void on_spinBoxMinSampleCount_valueChanged(int);         /// Updates the minimum per-class sample count for training.
    void on_spinBoxMLTrees_valueChanged(int);                /// Updates the number of trees in the random forest.
    void on_spinBoxMLDepth_valueChanged(int);                /// Updates the maximum depth of each decision tree.
    void on_spinBoxMLSampling_valueChanged(int);             /// Updates the feature-sampling fraction used during training.
    void on_actionCalculate_Features_triggered();            /// Runs feature extraction across the entire image stack.
    void on_actionSave_feature_set_triggered();              /// Saves the current feature set configuration to disk.
    void on_actionLoad_feature_set_triggered();              /// Loads a feature set configuration from disk.
    void on_actionRemove_feature_files_triggered();          /// Deletes cached feature files from the working directory.
    void on_actionClear_sample_triggered();                  /// Clears all collected training samples.
    void on_actionCT_simpl_triggered();                      /// Loads the simple CT feature preset.
    void on_actionCT_complex_triggered();                    /// Loads the complex CT feature preset.
    void on_actionColour_simple_triggered();                 /// Loads the simple colour feature preset.
    void on_actionColour_complex_triggered();                /// Loads the complex colour feature preset.
    void on_actionMLAuto_Update_triggered();                 /// Toggles automatic ML classification update on brush strokes.

    // ── Settings and help ─────────────────────────────────────────────────
    void on_actionSettings_triggered();                  /// Opens the global settings dialog.
    void on_actionAb_out_triggered();                    /// Opens the About dialog.
    void on_actionCheck_for_Updates_triggered();         /// Initiates an online update check.
    void on_actionManual_triggered();                    /// Opens the online documentation in the system browser.
    void on_actionCode_on_GitHub_triggered();            /// Opens the SPIERS GitHub repository in the system browser.
    void on_actionBugIssueFeatureRequest_triggered();    /// Opens the GitHub issue tracker in the system browser.
    void on_actionInfo_triggered(bool checked);          /// Toggles the information overlay on the slice display.

    // ── Extra Tools ─────────────────────────────────────────────────
    void on_actionMeasure_Volumes_triggered();          /// Toggles the volume measuring.

    // ── Developer/test ────────────────────────────────────────────────────
    void on_actionTEST_triggered(); /// Developer test action — not exposed in release builds.

private:
    /**
     *
     * Records the current slice state as a named undo step of the given @p type.
     *
     **/
    void MakeUndo(QString type);

    /**
     *
     * Fully rebuilds the masks tree widget from the current MasksSettings array.
     *
     **/
    void RefreshMasks();

    /**
     *
     * Fully rebuilds the segments tree widget from the current Segments array.
     *
     **/
    void RefreshSegments();

    /**
     *
     * Fully rebuilds the output objects tree widget from the current OutputObjects array.
     *
     **/
    void RefreshOO();

    /**
     *
     * Fully rebuilds the curves tree widget from the current Curves array.
     *
     **/
    void RefreshCurves();

    /**
     *
     * Rebuilds the tree widget row for mask @p i, updating its colour swatches,
     * show/lock icons, and selection marker.
     *
     **/
    void RefreshOneMaskItem(QTreeWidgetItem *item, int i);

    /**
     *
     * Rebuilds the tree widget row for curve @p i, updating its colour swatch
     * and greyed-out state.
     *
     **/
    void RefreshOneCurveItem(QTreeWidgetItem *item, int i);

    /**
     *
     * Rebuilds the tree widget row for output object @p i, updating its colour
     * swatch, key label, and merge icon.
     *
     **/
    void RefreshOneOOItem(QTreeWidgetItem *item, int i);

    /**
     *
     * Repopulates the left/right mask combo boxes from the current mask list.
     *
     **/
    void RefreshMasksBoxes();

    /**
     *
     * Copies curve data from slice @p fromfile to the current slice for all
     * selected curves.
     *
     **/
    void CurveCopy(int fromfile);

    /**
     *
     * Clears the undo and redo stacks and resets the undo-state counter.
     *
     **/
    void ResetUndo();

    /**
     *
     * Recursively draws child output objects under @p parent into the OO tree
     * according to @p selflags.
     *
     **/
    void OODrawChildren(QList<bool> selflags, int parent);

    /**
     *
     * Recursively underlines the names of all child items under @p par in the
     * OO tree to indicate merged state.
     *
     **/
    void OOunderlineChildren(QTreeWidgetItem *par);

    /**
     *
     * Returns true if output object @p i contributes to any merged group.
     *
     **/
    bool AmIMerged(int i);

    /**
     *
     * Removes output objects that no longer have any associated segment data.
     *
     **/
    void CleanseOO();

    /**
     *
     * Low-level Qt event handler; currently a passthrough to QWidget::event().
     *
     **/
    bool event(QEvent *event) override;

    /**
     *
     * Catches all wheel events in the MainWindow.
     *
     **/
    void wheelEvent(QWheelEvent *event) override;

    /**
     *
     * Catches QEvent::StyleChange to rebuild tree widget icon pixmaps when the
     * application theme is switched live. Skipped when no dataset is open.
     *
     **/
    void changeEvent(QEvent *event) override;

    /**
     *
     * Repairs dock-widget stretch factors after a layout change to prevent
     * docks from collapsing to zero height.
     *
     **/
    void FixUpStretches();

    /**
     *
     * Enables or disables the window menu items based on whether a project is active.
     * When @p enabled is false, all items except actionGenerateTestData are disabled.
     * When @p enabled is true, all items are enabled.
     *
     **/
    void SetWindowMenuState(bool enabled);

    /**
     *
     * Returns the display text for slice @p i in the slice-selector list,
     * including position and optional annotation data.
     *
     **/
    QString TextForSliceSelectorBox(int i);

    /**
     *
     * Creates and installs all dock widgets, connects their visibility actions,
     * and restores saved geometry.
     *
     **/
    void SetUpDocks();

    /**
     *
     * Enables or disables individual brush tool actions based on the currently
     * active generation mode and dataset state.
     *
     **/
    void SetUpBrushEnabling();

    /**
     *
     * Triggers a recalculation and redisplay of the gradient generation overlay.
     *
     **/
    void DoGradientsUpdate();

    QAction *shortcutright;   /// Keyboard shortcut action for moving one slice forward.
    QAction *shortcutleft;    /// Keyboard shortcut action for moving one slice backward.
    QAction *shortcutright2;  /// Secondary keyboard shortcut action for moving one slice forward.
    QAction *shortcutleft2;   /// Secondary keyboard shortcut action for moving one slice backward.
    QAction *shortcutspace;   /// Keyboard shortcut action for the spacebar paint toggle.

    QSpinBox *ZoomBox;           /// Spin box embedded in the toolbar for direct zoom entry.
    QSpinBox *BrushSize;         /// Spin box embedded in the toolbar for direct brush-size entry.
    QList<double> OldStretches;  /// Saved dock stretch factors used by FixUpStretches().
    QGraphicsView *RangeGraphicsView; /// Graphics view used to display the slice-range histogram.

    bool DontRedoZoom;           /// Guard flag that prevents recursive zoom-change handling.
    bool ExportingImages;        /// True while an image-set export is in progress.

    QElapsedTimer DoubleClickTimer; /// Measures inter-click time to distinguish double-clicks in the slice view.

    BeamHardeningCenterIcon *centerIcon; /// Overlay icon marking the beam-hardening correction centre on the slice view.
    BeamHardening *bh;                   /// Beam-hardening correction processing object.
};

extern MainWindow *AppMainWindow; /// Global pointer to the single MainWindow instance.

#endif
