/****************************************************************************
** Meta object code from reading C++ file 'mainwindowimpl.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SPIERSedit/src/mainwindowimpl.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindowimpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowImplENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMainWindowImplENDCLASS = QtMocHelpers::stringData(
    "MainWindowImpl",
    "Moveimage",
    "",
    "on_action3D_Brush_toggled",
    "on_actionInterpolate_over_selected_slices_triggered",
    "on_actionRefresh_triggered",
    "on_actionExport_SPIERSview_and_Launch_triggered",
    "on_actionMeasure_Volumes_triggered",
    "on_actionAb_out_triggered",
    "on_actionChange_downsampling_triggered",
    "on_tabWidget_currentChanged",
    "index",
    "on_CheckBoxRangeSelectedOnly_toggled",
    "checked",
    "on_CheckBoxRangeHardFill_toggled",
    "on_actionDistribute_over_range_triggered",
    "on_SpinBoxRangeBase_valueChanged",
    "v",
    "on_SpinBoxRangeTop_valueChanged",
    "on_actionHidden_masks_locked_for_generation_triggered",
    "on_actionSegment_brush_applies_masks_triggered",
    "on_FindPolynomial_pressed",
    "on_actionShow_position_slice_selector_toggled",
    "on_actionChange_slice_spacing_triggered",
    "on_actionSet_slice_position_triggered",
    "on_actionInfo_triggered",
    "on_actionExport_DXF_triggered",
    "on_OOTreeWidget_itemPressed",
    "QTreeWidgetItem*",
    "item",
    "column",
    "on_CurvesTreeWidget_itemPressed",
    "on_actionCreate_mask_from_curve_triggered",
    "on_SliceSelectorList_itemSelectionChanged",
    "on_actionMaskCopy_selected_from_Previous_triggered",
    "on_actionMaskCopy_all_from_previous_triggered",
    "on_actionMaskCopy_selected_from_next_triggered",
    "on_actionMaskCopy_all_from_next_triggered",
    "on_actionMask_Advance_slice_after_copy_operation_toggled",
    "on_actionMaskGo_back_one_slice_after_copy_toggled",
    "on_actionOutput_SPIERSview_triggered",
    "on_actionDelete_Output_Object_triggered",
    "on_actionNew_Output_Object_triggered",
    "on_actionNew_Output_Object_Group_triggered",
    "on_actionMove_item_to_Group_triggered",
    "on_actionUngroup_triggered",
    "on_OOTreeWidget_itemSelectionChanged",
    "on_SegmentsTreeWidget_itemSelectionChanged",
    "on_MasksTreeWidget_itemSelectionChanged",
    "on_OONew_clicked",
    "on_OOGroup_clicked",
    "on_OODelete_clicked",
    "on_OOUp_clicked",
    "on_OODown_clicked",
    "on_OOTreeWidget_expanded",
    "QModelIndex",
    "on_OOTreeWidget_itemDoubleClicked",
    "on_actionResize_keeping_curve_shape_triggered",
    "on_actionLock_curve_shape_triggered",
    "on_actionRemove_selected_curves_from_selected_slices_triggered",
    "on_actionCopy_from_current_slice_to_selected_triggered",
    "on_CurvesTreeWidget_itemSelectionChanged",
    "on_actionCopyCurvesFromPrevious_triggered",
    "on_actionCopyCurvesFromNext_triggered",
    "on_actionCopyCurvesToCurrent_triggered",
    "on_Curve_Add_pressed",
    "on_CurveDelete_pressed",
    "on_CurveMoveUp_pressed",
    "on_CurveMoveDown_pressed",
    "on_actionAdd_new_point_triggered",
    "on_actionRemove_node_under_cursor_triggered",
    "on_actionCurve_markers_as_crosses_triggered",
    "on_CurvesTreeWidget_currentItemChanged",
    "current",
    "previous",
    "on_CurvesTreeWidget_itemDoubleClicked",
    "on_actionHistogram_shows_selected_triggered",
    "on_actionHistorgram_triggered",
    "on_actionSettings_triggered",
    "on_SpinBoxSparsity_valueChanged",
    "on_SpinBoxOrder_valueChanged",
    "on_SpinBoxRetries_valueChanged",
    "on_SpinBoxContrast_valueChanged",
    "on_LinearGreenSpinBox_valueChanged",
    "on_LinearRedSpinBox_valueChanged",
    "on_LinearGreenSpinBox_editingFinished",
    "on_LinearBlueSpinBox_valueChanged",
    "on_LinearGlobalSpinBox_valueChanged",
    "on_GenInvert_toggled",
    "on_GenerateAuto_toggled",
    "on_GenerateButton_clicked",
    "on_actionNew_mask_triggered",
    "on_actionDelete_selected_mask_s_triggered",
    "on_actionCreate_new_segment_triggered",
    "on_actionDelete_selected_segments_triggered",
    "on_SegmentAdd_pressed",
    "on_SegmentDelete_pressed",
    "on_SegmentMoveUp_pressed",
    "on_SegmentMoveDown_pressed",
    "on_actionLock_selected_segments_triggered",
    "on_actionUnlock_selected_segments_triggered",
    "on_actionActivate_selected_segments_triggered",
    "on_actionDeactivate_selected_segments_triggered",
    "on_SegmentsTreeWidget_itemPressed",
    "on_SegmentsTreeWidget_itemDoubleClicked",
    "on_SegmentsTreeWidget_pressed",
    "on_SegmentsTreeWidget_doubleClicked",
    "on_MasksTreeWidget_itemPressed",
    "on_MaskAdd_pressed",
    "on_MaskDelete_pressed",
    "on_MaskMoveUp_pressed",
    "on_MaskMoveDown_pressed",
    "on_actionLock_Selected_Masks_triggered",
    "on_actionUnlock_Selected_Masks_triggered",
    "on_actionShow_Selected_Masks_triggered",
    "on_actionUnShow_Selected_Masks_triggered",
    "on_actionWrite_Selected_Masks_triggered",
    "on_actionUnWrite_Selected_Masks_triggered",
    "on_MasksTreeWidget_itemChanged",
    "on_MasksTreeWidget_itemDoubleClicked",
    "on_SpinBoxConverge_valueChanged",
    "on_FirstFile_valueChanged",
    "on_LastFile_valueChanged",
    "on_DownsampleXY_valueChanged",
    "on_DownsampleZ_valueChanged",
    "on_PixelSensitivity_valueChanged",
    "on_Edge_Down_MM_valueChanged",
    "on_Edge_Left_MM_valueChanged",
    "on_CheckMirror_toggled",
    "on_Pixels_Per_MM_valueChanged",
    "on_Slices_Per_MM_valueChanged",
    "on_actionView_in_SPIERSview_triggered",
    "on_PosSpinBox_valueChanged",
    "on_ZoomSpinBox_valueChanged",
    "on_actionManual_triggered",
    "on_actionGrey_out_curves_not_no_current_slice_triggered",
    "on_actionExport_Curves_as_CSV_triggered",
    "on_actionImport_Curves_as_CSV_triggered",
    "on_actionOutput_visible_image_set_triggered",
    "on_actionCode_on_GitHub_triggered",
    "on_actionBugIssueFeatureRequest_triggered",
    "ScreenUpdate",
    "Menu_Window_MainToolbox",
    "Menu_Window_Generate",
    "Menu_File_Import",
    "InitStates",
    "Zoom_Slider_Changed",
    "Trans_Changed",
    "Min_Changed",
    "Max_Changed",
    "Preset1",
    "Preset2",
    "Preset3",
    "Preset4",
    "Preset5",
    "Preset6",
    "Preset7",
    "Preset8",
    "Preset9",
    "Preset0",
    "Mode_Changed",
    "QAction*",
    "BrushChanged",
    "BrightDownChanged",
    "BrightUpChanged",
    "SoftChanged",
    "BoostSpinBoxChanged",
    "BoostRadiusSpinBoxChanged",
    "BoostAdjustSpinBoxChanged",
    "TransToggled",
    "SquareToggled",
    "ThresholdFlag",
    "SetMasksFlag",
    "SetSegsFlag",
    "cmac",
    "LockShape",
    "BuildRecentFiles",
    "openRecentFile",
    "openMore",
    "q_pressed",
    "a_pressed",
    "right_pressed",
    "left_pressed",
    "LeftMaskChanged",
    "RightMaskChanged",
    "LeftSegChanged",
    "RightSegChanged",
    "SaveSettings",
    "SaveAs",
    "FileOpen",
    "Menu_File_New",
    "UndoTimer",
    "Undo",
    "Redo",
    "Menu_Window_SliceSelector",
    "Menu_Window_Masks",
    "Menu_Window_Curves",
    "Menu_Window_Segments",
    "Menu_Window_Output",
    "autosave",
    "on_actionTEST_triggered",
    "on_Measure_clicked",
    "on_ShowCenter_stateChanged",
    "arg1",
    "on_CentreX_valueChanged",
    "on_CentreY_valueChanged",
    "on_HardeningRadiusSpinBox_valueChanged",
    "on_AdjustRadialSpinBox_valueChanged",
    "on_SetCentre_clicked",
    "on_spinGradientsDistMinEffect_valueChanged",
    "on_spinGradientsDistMaxEffect_valueChanged",
    "on_spinGradientsDistMin_valueChanged",
    "on_spinGradientsDistMax_valueChanged",
    "on_chkGradientsPreview_toggled",
    "on_actionThreshold_triggered",
    "on_spinGradientsPointDensity_valueChanged",
    "on_actionCycle_Bright_Segment_Mask_triggered",
    "on_action1_x_1_triggered",
    "on_actionIncrease_Size_triggered",
    "on_actionDecrease_Size_triggered",
    "on_trainML_clicked",
    "on_testML_clicked",
    "on_actionSegment_brush_applies_locks_triggered"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowImplENDCLASS_t {
    uint offsetsAndSizes[446];
    char stringdata0[15];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[26];
    char stringdata4[52];
    char stringdata5[27];
    char stringdata6[48];
    char stringdata7[35];
    char stringdata8[26];
    char stringdata9[39];
    char stringdata10[28];
    char stringdata11[6];
    char stringdata12[37];
    char stringdata13[8];
    char stringdata14[33];
    char stringdata15[41];
    char stringdata16[33];
    char stringdata17[2];
    char stringdata18[32];
    char stringdata19[54];
    char stringdata20[47];
    char stringdata21[26];
    char stringdata22[46];
    char stringdata23[40];
    char stringdata24[38];
    char stringdata25[24];
    char stringdata26[30];
    char stringdata27[28];
    char stringdata28[17];
    char stringdata29[5];
    char stringdata30[7];
    char stringdata31[32];
    char stringdata32[42];
    char stringdata33[42];
    char stringdata34[51];
    char stringdata35[46];
    char stringdata36[47];
    char stringdata37[42];
    char stringdata38[57];
    char stringdata39[50];
    char stringdata40[37];
    char stringdata41[40];
    char stringdata42[37];
    char stringdata43[43];
    char stringdata44[38];
    char stringdata45[27];
    char stringdata46[37];
    char stringdata47[43];
    char stringdata48[40];
    char stringdata49[17];
    char stringdata50[19];
    char stringdata51[20];
    char stringdata52[16];
    char stringdata53[18];
    char stringdata54[25];
    char stringdata55[12];
    char stringdata56[34];
    char stringdata57[46];
    char stringdata58[36];
    char stringdata59[63];
    char stringdata60[55];
    char stringdata61[41];
    char stringdata62[42];
    char stringdata63[38];
    char stringdata64[39];
    char stringdata65[21];
    char stringdata66[23];
    char stringdata67[23];
    char stringdata68[25];
    char stringdata69[33];
    char stringdata70[44];
    char stringdata71[44];
    char stringdata72[39];
    char stringdata73[8];
    char stringdata74[9];
    char stringdata75[38];
    char stringdata76[44];
    char stringdata77[30];
    char stringdata78[28];
    char stringdata79[32];
    char stringdata80[29];
    char stringdata81[31];
    char stringdata82[32];
    char stringdata83[35];
    char stringdata84[33];
    char stringdata85[38];
    char stringdata86[34];
    char stringdata87[36];
    char stringdata88[21];
    char stringdata89[24];
    char stringdata90[26];
    char stringdata91[28];
    char stringdata92[42];
    char stringdata93[38];
    char stringdata94[44];
    char stringdata95[22];
    char stringdata96[25];
    char stringdata97[25];
    char stringdata98[27];
    char stringdata99[42];
    char stringdata100[44];
    char stringdata101[46];
    char stringdata102[48];
    char stringdata103[34];
    char stringdata104[40];
    char stringdata105[30];
    char stringdata106[36];
    char stringdata107[31];
    char stringdata108[19];
    char stringdata109[22];
    char stringdata110[22];
    char stringdata111[24];
    char stringdata112[39];
    char stringdata113[41];
    char stringdata114[39];
    char stringdata115[41];
    char stringdata116[40];
    char stringdata117[42];
    char stringdata118[31];
    char stringdata119[37];
    char stringdata120[32];
    char stringdata121[26];
    char stringdata122[25];
    char stringdata123[29];
    char stringdata124[28];
    char stringdata125[33];
    char stringdata126[29];
    char stringdata127[29];
    char stringdata128[23];
    char stringdata129[30];
    char stringdata130[30];
    char stringdata131[38];
    char stringdata132[27];
    char stringdata133[28];
    char stringdata134[26];
    char stringdata135[56];
    char stringdata136[40];
    char stringdata137[40];
    char stringdata138[44];
    char stringdata139[34];
    char stringdata140[42];
    char stringdata141[13];
    char stringdata142[24];
    char stringdata143[21];
    char stringdata144[17];
    char stringdata145[11];
    char stringdata146[20];
    char stringdata147[14];
    char stringdata148[12];
    char stringdata149[12];
    char stringdata150[8];
    char stringdata151[8];
    char stringdata152[8];
    char stringdata153[8];
    char stringdata154[8];
    char stringdata155[8];
    char stringdata156[8];
    char stringdata157[8];
    char stringdata158[8];
    char stringdata159[8];
    char stringdata160[13];
    char stringdata161[9];
    char stringdata162[13];
    char stringdata163[18];
    char stringdata164[16];
    char stringdata165[12];
    char stringdata166[20];
    char stringdata167[26];
    char stringdata168[26];
    char stringdata169[13];
    char stringdata170[14];
    char stringdata171[14];
    char stringdata172[13];
    char stringdata173[12];
    char stringdata174[5];
    char stringdata175[10];
    char stringdata176[17];
    char stringdata177[15];
    char stringdata178[9];
    char stringdata179[10];
    char stringdata180[10];
    char stringdata181[14];
    char stringdata182[13];
    char stringdata183[16];
    char stringdata184[17];
    char stringdata185[15];
    char stringdata186[16];
    char stringdata187[13];
    char stringdata188[7];
    char stringdata189[9];
    char stringdata190[14];
    char stringdata191[10];
    char stringdata192[5];
    char stringdata193[5];
    char stringdata194[26];
    char stringdata195[18];
    char stringdata196[19];
    char stringdata197[21];
    char stringdata198[19];
    char stringdata199[9];
    char stringdata200[24];
    char stringdata201[19];
    char stringdata202[27];
    char stringdata203[5];
    char stringdata204[24];
    char stringdata205[24];
    char stringdata206[39];
    char stringdata207[36];
    char stringdata208[21];
    char stringdata209[43];
    char stringdata210[43];
    char stringdata211[37];
    char stringdata212[37];
    char stringdata213[31];
    char stringdata214[29];
    char stringdata215[42];
    char stringdata216[45];
    char stringdata217[25];
    char stringdata218[33];
    char stringdata219[33];
    char stringdata220[19];
    char stringdata221[18];
    char stringdata222[47];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowImplENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowImplENDCLASS_t qt_meta_stringdata_CLASSMainWindowImplENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "MainWindowImpl"
        QT_MOC_LITERAL(15, 9),  // "Moveimage"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 25),  // "on_action3D_Brush_toggled"
        QT_MOC_LITERAL(52, 51),  // "on_actionInterpolate_over_sel..."
        QT_MOC_LITERAL(104, 26),  // "on_actionRefresh_triggered"
        QT_MOC_LITERAL(131, 47),  // "on_actionExport_SPIERSview_an..."
        QT_MOC_LITERAL(179, 34),  // "on_actionMeasure_Volumes_trig..."
        QT_MOC_LITERAL(214, 25),  // "on_actionAb_out_triggered"
        QT_MOC_LITERAL(240, 38),  // "on_actionChange_downsampling_..."
        QT_MOC_LITERAL(279, 27),  // "on_tabWidget_currentChanged"
        QT_MOC_LITERAL(307, 5),  // "index"
        QT_MOC_LITERAL(313, 36),  // "on_CheckBoxRangeSelectedOnly_..."
        QT_MOC_LITERAL(350, 7),  // "checked"
        QT_MOC_LITERAL(358, 32),  // "on_CheckBoxRangeHardFill_toggled"
        QT_MOC_LITERAL(391, 40),  // "on_actionDistribute_over_rang..."
        QT_MOC_LITERAL(432, 32),  // "on_SpinBoxRangeBase_valueChanged"
        QT_MOC_LITERAL(465, 1),  // "v"
        QT_MOC_LITERAL(467, 31),  // "on_SpinBoxRangeTop_valueChanged"
        QT_MOC_LITERAL(499, 53),  // "on_actionHidden_masks_locked_..."
        QT_MOC_LITERAL(553, 46),  // "on_actionSegment_brush_applie..."
        QT_MOC_LITERAL(600, 25),  // "on_FindPolynomial_pressed"
        QT_MOC_LITERAL(626, 45),  // "on_actionShow_position_slice_..."
        QT_MOC_LITERAL(672, 39),  // "on_actionChange_slice_spacing..."
        QT_MOC_LITERAL(712, 37),  // "on_actionSet_slice_position_t..."
        QT_MOC_LITERAL(750, 23),  // "on_actionInfo_triggered"
        QT_MOC_LITERAL(774, 29),  // "on_actionExport_DXF_triggered"
        QT_MOC_LITERAL(804, 27),  // "on_OOTreeWidget_itemPressed"
        QT_MOC_LITERAL(832, 16),  // "QTreeWidgetItem*"
        QT_MOC_LITERAL(849, 4),  // "item"
        QT_MOC_LITERAL(854, 6),  // "column"
        QT_MOC_LITERAL(861, 31),  // "on_CurvesTreeWidget_itemPressed"
        QT_MOC_LITERAL(893, 41),  // "on_actionCreate_mask_from_cur..."
        QT_MOC_LITERAL(935, 41),  // "on_SliceSelectorList_itemSele..."
        QT_MOC_LITERAL(977, 50),  // "on_actionMaskCopy_selected_fr..."
        QT_MOC_LITERAL(1028, 45),  // "on_actionMaskCopy_all_from_pr..."
        QT_MOC_LITERAL(1074, 46),  // "on_actionMaskCopy_selected_fr..."
        QT_MOC_LITERAL(1121, 41),  // "on_actionMaskCopy_all_from_ne..."
        QT_MOC_LITERAL(1163, 56),  // "on_actionMask_Advance_slice_a..."
        QT_MOC_LITERAL(1220, 49),  // "on_actionMaskGo_back_one_slic..."
        QT_MOC_LITERAL(1270, 36),  // "on_actionOutput_SPIERSview_tr..."
        QT_MOC_LITERAL(1307, 39),  // "on_actionDelete_Output_Object..."
        QT_MOC_LITERAL(1347, 36),  // "on_actionNew_Output_Object_tr..."
        QT_MOC_LITERAL(1384, 42),  // "on_actionNew_Output_Object_Gr..."
        QT_MOC_LITERAL(1427, 37),  // "on_actionMove_item_to_Group_t..."
        QT_MOC_LITERAL(1465, 26),  // "on_actionUngroup_triggered"
        QT_MOC_LITERAL(1492, 36),  // "on_OOTreeWidget_itemSelection..."
        QT_MOC_LITERAL(1529, 42),  // "on_SegmentsTreeWidget_itemSel..."
        QT_MOC_LITERAL(1572, 39),  // "on_MasksTreeWidget_itemSelect..."
        QT_MOC_LITERAL(1612, 16),  // "on_OONew_clicked"
        QT_MOC_LITERAL(1629, 18),  // "on_OOGroup_clicked"
        QT_MOC_LITERAL(1648, 19),  // "on_OODelete_clicked"
        QT_MOC_LITERAL(1668, 15),  // "on_OOUp_clicked"
        QT_MOC_LITERAL(1684, 17),  // "on_OODown_clicked"
        QT_MOC_LITERAL(1702, 24),  // "on_OOTreeWidget_expanded"
        QT_MOC_LITERAL(1727, 11),  // "QModelIndex"
        QT_MOC_LITERAL(1739, 33),  // "on_OOTreeWidget_itemDoubleCli..."
        QT_MOC_LITERAL(1773, 45),  // "on_actionResize_keeping_curve..."
        QT_MOC_LITERAL(1819, 35),  // "on_actionLock_curve_shape_tri..."
        QT_MOC_LITERAL(1855, 62),  // "on_actionRemove_selected_curv..."
        QT_MOC_LITERAL(1918, 54),  // "on_actionCopy_from_current_sl..."
        QT_MOC_LITERAL(1973, 40),  // "on_CurvesTreeWidget_itemSelec..."
        QT_MOC_LITERAL(2014, 41),  // "on_actionCopyCurvesFromPrevio..."
        QT_MOC_LITERAL(2056, 37),  // "on_actionCopyCurvesFromNext_t..."
        QT_MOC_LITERAL(2094, 38),  // "on_actionCopyCurvesToCurrent_..."
        QT_MOC_LITERAL(2133, 20),  // "on_Curve_Add_pressed"
        QT_MOC_LITERAL(2154, 22),  // "on_CurveDelete_pressed"
        QT_MOC_LITERAL(2177, 22),  // "on_CurveMoveUp_pressed"
        QT_MOC_LITERAL(2200, 24),  // "on_CurveMoveDown_pressed"
        QT_MOC_LITERAL(2225, 32),  // "on_actionAdd_new_point_triggered"
        QT_MOC_LITERAL(2258, 43),  // "on_actionRemove_node_under_cu..."
        QT_MOC_LITERAL(2302, 43),  // "on_actionCurve_markers_as_cro..."
        QT_MOC_LITERAL(2346, 38),  // "on_CurvesTreeWidget_currentIt..."
        QT_MOC_LITERAL(2385, 7),  // "current"
        QT_MOC_LITERAL(2393, 8),  // "previous"
        QT_MOC_LITERAL(2402, 37),  // "on_CurvesTreeWidget_itemDoubl..."
        QT_MOC_LITERAL(2440, 43),  // "on_actionHistogram_shows_sele..."
        QT_MOC_LITERAL(2484, 29),  // "on_actionHistorgram_triggered"
        QT_MOC_LITERAL(2514, 27),  // "on_actionSettings_triggered"
        QT_MOC_LITERAL(2542, 31),  // "on_SpinBoxSparsity_valueChanged"
        QT_MOC_LITERAL(2574, 28),  // "on_SpinBoxOrder_valueChanged"
        QT_MOC_LITERAL(2603, 30),  // "on_SpinBoxRetries_valueChanged"
        QT_MOC_LITERAL(2634, 31),  // "on_SpinBoxContrast_valueChanged"
        QT_MOC_LITERAL(2666, 34),  // "on_LinearGreenSpinBox_valueCh..."
        QT_MOC_LITERAL(2701, 32),  // "on_LinearRedSpinBox_valueChanged"
        QT_MOC_LITERAL(2734, 37),  // "on_LinearGreenSpinBox_editing..."
        QT_MOC_LITERAL(2772, 33),  // "on_LinearBlueSpinBox_valueCha..."
        QT_MOC_LITERAL(2806, 35),  // "on_LinearGlobalSpinBox_valueC..."
        QT_MOC_LITERAL(2842, 20),  // "on_GenInvert_toggled"
        QT_MOC_LITERAL(2863, 23),  // "on_GenerateAuto_toggled"
        QT_MOC_LITERAL(2887, 25),  // "on_GenerateButton_clicked"
        QT_MOC_LITERAL(2913, 27),  // "on_actionNew_mask_triggered"
        QT_MOC_LITERAL(2941, 41),  // "on_actionDelete_selected_mask..."
        QT_MOC_LITERAL(2983, 37),  // "on_actionCreate_new_segment_t..."
        QT_MOC_LITERAL(3021, 43),  // "on_actionDelete_selected_segm..."
        QT_MOC_LITERAL(3065, 21),  // "on_SegmentAdd_pressed"
        QT_MOC_LITERAL(3087, 24),  // "on_SegmentDelete_pressed"
        QT_MOC_LITERAL(3112, 24),  // "on_SegmentMoveUp_pressed"
        QT_MOC_LITERAL(3137, 26),  // "on_SegmentMoveDown_pressed"
        QT_MOC_LITERAL(3164, 41),  // "on_actionLock_selected_segmen..."
        QT_MOC_LITERAL(3206, 43),  // "on_actionUnlock_selected_segm..."
        QT_MOC_LITERAL(3250, 45),  // "on_actionActivate_selected_se..."
        QT_MOC_LITERAL(3296, 47),  // "on_actionDeactivate_selected_..."
        QT_MOC_LITERAL(3344, 33),  // "on_SegmentsTreeWidget_itemPre..."
        QT_MOC_LITERAL(3378, 39),  // "on_SegmentsTreeWidget_itemDou..."
        QT_MOC_LITERAL(3418, 29),  // "on_SegmentsTreeWidget_pressed"
        QT_MOC_LITERAL(3448, 35),  // "on_SegmentsTreeWidget_doubleC..."
        QT_MOC_LITERAL(3484, 30),  // "on_MasksTreeWidget_itemPressed"
        QT_MOC_LITERAL(3515, 18),  // "on_MaskAdd_pressed"
        QT_MOC_LITERAL(3534, 21),  // "on_MaskDelete_pressed"
        QT_MOC_LITERAL(3556, 21),  // "on_MaskMoveUp_pressed"
        QT_MOC_LITERAL(3578, 23),  // "on_MaskMoveDown_pressed"
        QT_MOC_LITERAL(3602, 38),  // "on_actionLock_Selected_Masks_..."
        QT_MOC_LITERAL(3641, 40),  // "on_actionUnlock_Selected_Mask..."
        QT_MOC_LITERAL(3682, 38),  // "on_actionShow_Selected_Masks_..."
        QT_MOC_LITERAL(3721, 40),  // "on_actionUnShow_Selected_Mask..."
        QT_MOC_LITERAL(3762, 39),  // "on_actionWrite_Selected_Masks..."
        QT_MOC_LITERAL(3802, 41),  // "on_actionUnWrite_Selected_Mas..."
        QT_MOC_LITERAL(3844, 30),  // "on_MasksTreeWidget_itemChanged"
        QT_MOC_LITERAL(3875, 36),  // "on_MasksTreeWidget_itemDouble..."
        QT_MOC_LITERAL(3912, 31),  // "on_SpinBoxConverge_valueChanged"
        QT_MOC_LITERAL(3944, 25),  // "on_FirstFile_valueChanged"
        QT_MOC_LITERAL(3970, 24),  // "on_LastFile_valueChanged"
        QT_MOC_LITERAL(3995, 28),  // "on_DownsampleXY_valueChanged"
        QT_MOC_LITERAL(4024, 27),  // "on_DownsampleZ_valueChanged"
        QT_MOC_LITERAL(4052, 32),  // "on_PixelSensitivity_valueChanged"
        QT_MOC_LITERAL(4085, 28),  // "on_Edge_Down_MM_valueChanged"
        QT_MOC_LITERAL(4114, 28),  // "on_Edge_Left_MM_valueChanged"
        QT_MOC_LITERAL(4143, 22),  // "on_CheckMirror_toggled"
        QT_MOC_LITERAL(4166, 29),  // "on_Pixels_Per_MM_valueChanged"
        QT_MOC_LITERAL(4196, 29),  // "on_Slices_Per_MM_valueChanged"
        QT_MOC_LITERAL(4226, 37),  // "on_actionView_in_SPIERSview_t..."
        QT_MOC_LITERAL(4264, 26),  // "on_PosSpinBox_valueChanged"
        QT_MOC_LITERAL(4291, 27),  // "on_ZoomSpinBox_valueChanged"
        QT_MOC_LITERAL(4319, 25),  // "on_actionManual_triggered"
        QT_MOC_LITERAL(4345, 55),  // "on_actionGrey_out_curves_not_..."
        QT_MOC_LITERAL(4401, 39),  // "on_actionExport_Curves_as_CSV..."
        QT_MOC_LITERAL(4441, 39),  // "on_actionImport_Curves_as_CSV..."
        QT_MOC_LITERAL(4481, 43),  // "on_actionOutput_visible_image..."
        QT_MOC_LITERAL(4525, 33),  // "on_actionCode_on_GitHub_trigg..."
        QT_MOC_LITERAL(4559, 41),  // "on_actionBugIssueFeatureReque..."
        QT_MOC_LITERAL(4601, 12),  // "ScreenUpdate"
        QT_MOC_LITERAL(4614, 23),  // "Menu_Window_MainToolbox"
        QT_MOC_LITERAL(4638, 20),  // "Menu_Window_Generate"
        QT_MOC_LITERAL(4659, 16),  // "Menu_File_Import"
        QT_MOC_LITERAL(4676, 10),  // "InitStates"
        QT_MOC_LITERAL(4687, 19),  // "Zoom_Slider_Changed"
        QT_MOC_LITERAL(4707, 13),  // "Trans_Changed"
        QT_MOC_LITERAL(4721, 11),  // "Min_Changed"
        QT_MOC_LITERAL(4733, 11),  // "Max_Changed"
        QT_MOC_LITERAL(4745, 7),  // "Preset1"
        QT_MOC_LITERAL(4753, 7),  // "Preset2"
        QT_MOC_LITERAL(4761, 7),  // "Preset3"
        QT_MOC_LITERAL(4769, 7),  // "Preset4"
        QT_MOC_LITERAL(4777, 7),  // "Preset5"
        QT_MOC_LITERAL(4785, 7),  // "Preset6"
        QT_MOC_LITERAL(4793, 7),  // "Preset7"
        QT_MOC_LITERAL(4801, 7),  // "Preset8"
        QT_MOC_LITERAL(4809, 7),  // "Preset9"
        QT_MOC_LITERAL(4817, 7),  // "Preset0"
        QT_MOC_LITERAL(4825, 12),  // "Mode_Changed"
        QT_MOC_LITERAL(4838, 8),  // "QAction*"
        QT_MOC_LITERAL(4847, 12),  // "BrushChanged"
        QT_MOC_LITERAL(4860, 17),  // "BrightDownChanged"
        QT_MOC_LITERAL(4878, 15),  // "BrightUpChanged"
        QT_MOC_LITERAL(4894, 11),  // "SoftChanged"
        QT_MOC_LITERAL(4906, 19),  // "BoostSpinBoxChanged"
        QT_MOC_LITERAL(4926, 25),  // "BoostRadiusSpinBoxChanged"
        QT_MOC_LITERAL(4952, 25),  // "BoostAdjustSpinBoxChanged"
        QT_MOC_LITERAL(4978, 12),  // "TransToggled"
        QT_MOC_LITERAL(4991, 13),  // "SquareToggled"
        QT_MOC_LITERAL(5005, 13),  // "ThresholdFlag"
        QT_MOC_LITERAL(5019, 12),  // "SetMasksFlag"
        QT_MOC_LITERAL(5032, 11),  // "SetSegsFlag"
        QT_MOC_LITERAL(5044, 4),  // "cmac"
        QT_MOC_LITERAL(5049, 9),  // "LockShape"
        QT_MOC_LITERAL(5059, 16),  // "BuildRecentFiles"
        QT_MOC_LITERAL(5076, 14),  // "openRecentFile"
        QT_MOC_LITERAL(5091, 8),  // "openMore"
        QT_MOC_LITERAL(5100, 9),  // "q_pressed"
        QT_MOC_LITERAL(5110, 9),  // "a_pressed"
        QT_MOC_LITERAL(5120, 13),  // "right_pressed"
        QT_MOC_LITERAL(5134, 12),  // "left_pressed"
        QT_MOC_LITERAL(5147, 15),  // "LeftMaskChanged"
        QT_MOC_LITERAL(5163, 16),  // "RightMaskChanged"
        QT_MOC_LITERAL(5180, 14),  // "LeftSegChanged"
        QT_MOC_LITERAL(5195, 15),  // "RightSegChanged"
        QT_MOC_LITERAL(5211, 12),  // "SaveSettings"
        QT_MOC_LITERAL(5224, 6),  // "SaveAs"
        QT_MOC_LITERAL(5231, 8),  // "FileOpen"
        QT_MOC_LITERAL(5240, 13),  // "Menu_File_New"
        QT_MOC_LITERAL(5254, 9),  // "UndoTimer"
        QT_MOC_LITERAL(5264, 4),  // "Undo"
        QT_MOC_LITERAL(5269, 4),  // "Redo"
        QT_MOC_LITERAL(5274, 25),  // "Menu_Window_SliceSelector"
        QT_MOC_LITERAL(5300, 17),  // "Menu_Window_Masks"
        QT_MOC_LITERAL(5318, 18),  // "Menu_Window_Curves"
        QT_MOC_LITERAL(5337, 20),  // "Menu_Window_Segments"
        QT_MOC_LITERAL(5358, 18),  // "Menu_Window_Output"
        QT_MOC_LITERAL(5377, 8),  // "autosave"
        QT_MOC_LITERAL(5386, 23),  // "on_actionTEST_triggered"
        QT_MOC_LITERAL(5410, 18),  // "on_Measure_clicked"
        QT_MOC_LITERAL(5429, 26),  // "on_ShowCenter_stateChanged"
        QT_MOC_LITERAL(5456, 4),  // "arg1"
        QT_MOC_LITERAL(5461, 23),  // "on_CentreX_valueChanged"
        QT_MOC_LITERAL(5485, 23),  // "on_CentreY_valueChanged"
        QT_MOC_LITERAL(5509, 38),  // "on_HardeningRadiusSpinBox_val..."
        QT_MOC_LITERAL(5548, 35),  // "on_AdjustRadialSpinBox_valueC..."
        QT_MOC_LITERAL(5584, 20),  // "on_SetCentre_clicked"
        QT_MOC_LITERAL(5605, 42),  // "on_spinGradientsDistMinEffect..."
        QT_MOC_LITERAL(5648, 42),  // "on_spinGradientsDistMaxEffect..."
        QT_MOC_LITERAL(5691, 36),  // "on_spinGradientsDistMin_value..."
        QT_MOC_LITERAL(5728, 36),  // "on_spinGradientsDistMax_value..."
        QT_MOC_LITERAL(5765, 30),  // "on_chkGradientsPreview_toggled"
        QT_MOC_LITERAL(5796, 28),  // "on_actionThreshold_triggered"
        QT_MOC_LITERAL(5825, 41),  // "on_spinGradientsPointDensity_..."
        QT_MOC_LITERAL(5867, 44),  // "on_actionCycle_Bright_Segment..."
        QT_MOC_LITERAL(5912, 24),  // "on_action1_x_1_triggered"
        QT_MOC_LITERAL(5937, 32),  // "on_actionIncrease_Size_triggered"
        QT_MOC_LITERAL(5970, 32),  // "on_actionDecrease_Size_triggered"
        QT_MOC_LITERAL(6003, 18),  // "on_trainML_clicked"
        QT_MOC_LITERAL(6022, 17),  // "on_testML_clicked"
        QT_MOC_LITERAL(6040, 46)   // "on_actionSegment_brush_applie..."
    },
    "MainWindowImpl",
    "Moveimage",
    "",
    "on_action3D_Brush_toggled",
    "on_actionInterpolate_over_selected_slices_triggered",
    "on_actionRefresh_triggered",
    "on_actionExport_SPIERSview_and_Launch_triggered",
    "on_actionMeasure_Volumes_triggered",
    "on_actionAb_out_triggered",
    "on_actionChange_downsampling_triggered",
    "on_tabWidget_currentChanged",
    "index",
    "on_CheckBoxRangeSelectedOnly_toggled",
    "checked",
    "on_CheckBoxRangeHardFill_toggled",
    "on_actionDistribute_over_range_triggered",
    "on_SpinBoxRangeBase_valueChanged",
    "v",
    "on_SpinBoxRangeTop_valueChanged",
    "on_actionHidden_masks_locked_for_generation_triggered",
    "on_actionSegment_brush_applies_masks_triggered",
    "on_FindPolynomial_pressed",
    "on_actionShow_position_slice_selector_toggled",
    "on_actionChange_slice_spacing_triggered",
    "on_actionSet_slice_position_triggered",
    "on_actionInfo_triggered",
    "on_actionExport_DXF_triggered",
    "on_OOTreeWidget_itemPressed",
    "QTreeWidgetItem*",
    "item",
    "column",
    "on_CurvesTreeWidget_itemPressed",
    "on_actionCreate_mask_from_curve_triggered",
    "on_SliceSelectorList_itemSelectionChanged",
    "on_actionMaskCopy_selected_from_Previous_triggered",
    "on_actionMaskCopy_all_from_previous_triggered",
    "on_actionMaskCopy_selected_from_next_triggered",
    "on_actionMaskCopy_all_from_next_triggered",
    "on_actionMask_Advance_slice_after_copy_operation_toggled",
    "on_actionMaskGo_back_one_slice_after_copy_toggled",
    "on_actionOutput_SPIERSview_triggered",
    "on_actionDelete_Output_Object_triggered",
    "on_actionNew_Output_Object_triggered",
    "on_actionNew_Output_Object_Group_triggered",
    "on_actionMove_item_to_Group_triggered",
    "on_actionUngroup_triggered",
    "on_OOTreeWidget_itemSelectionChanged",
    "on_SegmentsTreeWidget_itemSelectionChanged",
    "on_MasksTreeWidget_itemSelectionChanged",
    "on_OONew_clicked",
    "on_OOGroup_clicked",
    "on_OODelete_clicked",
    "on_OOUp_clicked",
    "on_OODown_clicked",
    "on_OOTreeWidget_expanded",
    "QModelIndex",
    "on_OOTreeWidget_itemDoubleClicked",
    "on_actionResize_keeping_curve_shape_triggered",
    "on_actionLock_curve_shape_triggered",
    "on_actionRemove_selected_curves_from_selected_slices_triggered",
    "on_actionCopy_from_current_slice_to_selected_triggered",
    "on_CurvesTreeWidget_itemSelectionChanged",
    "on_actionCopyCurvesFromPrevious_triggered",
    "on_actionCopyCurvesFromNext_triggered",
    "on_actionCopyCurvesToCurrent_triggered",
    "on_Curve_Add_pressed",
    "on_CurveDelete_pressed",
    "on_CurveMoveUp_pressed",
    "on_CurveMoveDown_pressed",
    "on_actionAdd_new_point_triggered",
    "on_actionRemove_node_under_cursor_triggered",
    "on_actionCurve_markers_as_crosses_triggered",
    "on_CurvesTreeWidget_currentItemChanged",
    "current",
    "previous",
    "on_CurvesTreeWidget_itemDoubleClicked",
    "on_actionHistogram_shows_selected_triggered",
    "on_actionHistorgram_triggered",
    "on_actionSettings_triggered",
    "on_SpinBoxSparsity_valueChanged",
    "on_SpinBoxOrder_valueChanged",
    "on_SpinBoxRetries_valueChanged",
    "on_SpinBoxContrast_valueChanged",
    "on_LinearGreenSpinBox_valueChanged",
    "on_LinearRedSpinBox_valueChanged",
    "on_LinearGreenSpinBox_editingFinished",
    "on_LinearBlueSpinBox_valueChanged",
    "on_LinearGlobalSpinBox_valueChanged",
    "on_GenInvert_toggled",
    "on_GenerateAuto_toggled",
    "on_GenerateButton_clicked",
    "on_actionNew_mask_triggered",
    "on_actionDelete_selected_mask_s_triggered",
    "on_actionCreate_new_segment_triggered",
    "on_actionDelete_selected_segments_triggered",
    "on_SegmentAdd_pressed",
    "on_SegmentDelete_pressed",
    "on_SegmentMoveUp_pressed",
    "on_SegmentMoveDown_pressed",
    "on_actionLock_selected_segments_triggered",
    "on_actionUnlock_selected_segments_triggered",
    "on_actionActivate_selected_segments_triggered",
    "on_actionDeactivate_selected_segments_triggered",
    "on_SegmentsTreeWidget_itemPressed",
    "on_SegmentsTreeWidget_itemDoubleClicked",
    "on_SegmentsTreeWidget_pressed",
    "on_SegmentsTreeWidget_doubleClicked",
    "on_MasksTreeWidget_itemPressed",
    "on_MaskAdd_pressed",
    "on_MaskDelete_pressed",
    "on_MaskMoveUp_pressed",
    "on_MaskMoveDown_pressed",
    "on_actionLock_Selected_Masks_triggered",
    "on_actionUnlock_Selected_Masks_triggered",
    "on_actionShow_Selected_Masks_triggered",
    "on_actionUnShow_Selected_Masks_triggered",
    "on_actionWrite_Selected_Masks_triggered",
    "on_actionUnWrite_Selected_Masks_triggered",
    "on_MasksTreeWidget_itemChanged",
    "on_MasksTreeWidget_itemDoubleClicked",
    "on_SpinBoxConverge_valueChanged",
    "on_FirstFile_valueChanged",
    "on_LastFile_valueChanged",
    "on_DownsampleXY_valueChanged",
    "on_DownsampleZ_valueChanged",
    "on_PixelSensitivity_valueChanged",
    "on_Edge_Down_MM_valueChanged",
    "on_Edge_Left_MM_valueChanged",
    "on_CheckMirror_toggled",
    "on_Pixels_Per_MM_valueChanged",
    "on_Slices_Per_MM_valueChanged",
    "on_actionView_in_SPIERSview_triggered",
    "on_PosSpinBox_valueChanged",
    "on_ZoomSpinBox_valueChanged",
    "on_actionManual_triggered",
    "on_actionGrey_out_curves_not_no_current_slice_triggered",
    "on_actionExport_Curves_as_CSV_triggered",
    "on_actionImport_Curves_as_CSV_triggered",
    "on_actionOutput_visible_image_set_triggered",
    "on_actionCode_on_GitHub_triggered",
    "on_actionBugIssueFeatureRequest_triggered",
    "ScreenUpdate",
    "Menu_Window_MainToolbox",
    "Menu_Window_Generate",
    "Menu_File_Import",
    "InitStates",
    "Zoom_Slider_Changed",
    "Trans_Changed",
    "Min_Changed",
    "Max_Changed",
    "Preset1",
    "Preset2",
    "Preset3",
    "Preset4",
    "Preset5",
    "Preset6",
    "Preset7",
    "Preset8",
    "Preset9",
    "Preset0",
    "Mode_Changed",
    "QAction*",
    "BrushChanged",
    "BrightDownChanged",
    "BrightUpChanged",
    "SoftChanged",
    "BoostSpinBoxChanged",
    "BoostRadiusSpinBoxChanged",
    "BoostAdjustSpinBoxChanged",
    "TransToggled",
    "SquareToggled",
    "ThresholdFlag",
    "SetMasksFlag",
    "SetSegsFlag",
    "cmac",
    "LockShape",
    "BuildRecentFiles",
    "openRecentFile",
    "openMore",
    "q_pressed",
    "a_pressed",
    "right_pressed",
    "left_pressed",
    "LeftMaskChanged",
    "RightMaskChanged",
    "LeftSegChanged",
    "RightSegChanged",
    "SaveSettings",
    "SaveAs",
    "FileOpen",
    "Menu_File_New",
    "UndoTimer",
    "Undo",
    "Redo",
    "Menu_Window_SliceSelector",
    "Menu_Window_Masks",
    "Menu_Window_Curves",
    "Menu_Window_Segments",
    "Menu_Window_Output",
    "autosave",
    "on_actionTEST_triggered",
    "on_Measure_clicked",
    "on_ShowCenter_stateChanged",
    "arg1",
    "on_CentreX_valueChanged",
    "on_CentreY_valueChanged",
    "on_HardeningRadiusSpinBox_valueChanged",
    "on_AdjustRadialSpinBox_valueChanged",
    "on_SetCentre_clicked",
    "on_spinGradientsDistMinEffect_valueChanged",
    "on_spinGradientsDistMaxEffect_valueChanged",
    "on_spinGradientsDistMin_valueChanged",
    "on_spinGradientsDistMax_valueChanged",
    "on_chkGradientsPreview_toggled",
    "on_actionThreshold_triggered",
    "on_spinGradientsPointDensity_valueChanged",
    "on_actionCycle_Bright_Segment_Mask_triggered",
    "on_action1_x_1_triggered",
    "on_actionIncrease_Size_triggered",
    "on_actionDecrease_Size_triggered",
    "on_trainML_clicked",
    "on_testML_clicked",
    "on_actionSegment_brush_applies_locks_triggered"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowImplENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
     210,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1, 1274,    2, 0x08,    1 /* Private */,
       3,    1, 1277,    2, 0x08,    3 /* Private */,
       4,    0, 1280,    2, 0x08,    5 /* Private */,
       5,    0, 1281,    2, 0x08,    6 /* Private */,
       6,    0, 1282,    2, 0x08,    7 /* Private */,
       7,    0, 1283,    2, 0x08,    8 /* Private */,
       8,    0, 1284,    2, 0x08,    9 /* Private */,
       9,    0, 1285,    2, 0x08,   10 /* Private */,
      10,    1, 1286,    2, 0x08,   11 /* Private */,
      12,    1, 1289,    2, 0x08,   13 /* Private */,
      14,    1, 1292,    2, 0x08,   15 /* Private */,
      15,    0, 1295,    2, 0x08,   17 /* Private */,
      16,    1, 1296,    2, 0x08,   18 /* Private */,
      18,    1, 1299,    2, 0x08,   20 /* Private */,
      19,    0, 1302,    2, 0x08,   22 /* Private */,
      20,    0, 1303,    2, 0x08,   23 /* Private */,
      21,    0, 1304,    2, 0x08,   24 /* Private */,
      22,    1, 1305,    2, 0x08,   25 /* Private */,
      23,    0, 1308,    2, 0x08,   27 /* Private */,
      24,    0, 1309,    2, 0x08,   28 /* Private */,
      25,    1, 1310,    2, 0x08,   29 /* Private */,
      26,    0, 1313,    2, 0x08,   31 /* Private */,
      27,    2, 1314,    2, 0x08,   32 /* Private */,
      31,    2, 1319,    2, 0x08,   35 /* Private */,
      32,    0, 1324,    2, 0x08,   38 /* Private */,
      33,    0, 1325,    2, 0x08,   39 /* Private */,
      34,    0, 1326,    2, 0x08,   40 /* Private */,
      35,    0, 1327,    2, 0x08,   41 /* Private */,
      36,    0, 1328,    2, 0x08,   42 /* Private */,
      37,    0, 1329,    2, 0x08,   43 /* Private */,
      38,    1, 1330,    2, 0x08,   44 /* Private */,
      39,    1, 1333,    2, 0x08,   46 /* Private */,
      40,    0, 1336,    2, 0x08,   48 /* Private */,
      41,    0, 1337,    2, 0x08,   49 /* Private */,
      42,    0, 1338,    2, 0x08,   50 /* Private */,
      43,    0, 1339,    2, 0x08,   51 /* Private */,
      44,    0, 1340,    2, 0x08,   52 /* Private */,
      45,    0, 1341,    2, 0x08,   53 /* Private */,
      46,    0, 1342,    2, 0x08,   54 /* Private */,
      47,    0, 1343,    2, 0x08,   55 /* Private */,
      48,    0, 1344,    2, 0x08,   56 /* Private */,
      49,    0, 1345,    2, 0x08,   57 /* Private */,
      50,    0, 1346,    2, 0x08,   58 /* Private */,
      51,    0, 1347,    2, 0x08,   59 /* Private */,
      52,    0, 1348,    2, 0x08,   60 /* Private */,
      53,    0, 1349,    2, 0x08,   61 /* Private */,
      54,    1, 1350,    2, 0x08,   62 /* Private */,
      56,    2, 1353,    2, 0x08,   64 /* Private */,
      57,    0, 1358,    2, 0x08,   67 /* Private */,
      58,    1, 1359,    2, 0x08,   68 /* Private */,
      59,    0, 1362,    2, 0x08,   70 /* Private */,
      60,    0, 1363,    2, 0x08,   71 /* Private */,
      61,    0, 1364,    2, 0x08,   72 /* Private */,
      62,    0, 1365,    2, 0x08,   73 /* Private */,
      63,    0, 1366,    2, 0x08,   74 /* Private */,
      64,    0, 1367,    2, 0x08,   75 /* Private */,
      65,    0, 1368,    2, 0x08,   76 /* Private */,
      66,    0, 1369,    2, 0x08,   77 /* Private */,
      67,    0, 1370,    2, 0x08,   78 /* Private */,
      68,    0, 1371,    2, 0x08,   79 /* Private */,
      69,    0, 1372,    2, 0x08,   80 /* Private */,
      70,    0, 1373,    2, 0x08,   81 /* Private */,
      71,    0, 1374,    2, 0x08,   82 /* Private */,
      72,    2, 1375,    2, 0x08,   83 /* Private */,
      75,    2, 1380,    2, 0x08,   86 /* Private */,
      76,    1, 1385,    2, 0x08,   89 /* Private */,
      77,    1, 1388,    2, 0x08,   91 /* Private */,
      78,    0, 1391,    2, 0x08,   93 /* Private */,
      79,    1, 1392,    2, 0x08,   94 /* Private */,
      80,    1, 1395,    2, 0x08,   96 /* Private */,
      81,    1, 1398,    2, 0x08,   98 /* Private */,
      82,    1, 1401,    2, 0x08,  100 /* Private */,
      83,    1, 1404,    2, 0x08,  102 /* Private */,
      84,    1, 1407,    2, 0x08,  104 /* Private */,
      85,    0, 1410,    2, 0x08,  106 /* Private */,
      86,    1, 1411,    2, 0x08,  107 /* Private */,
      87,    1, 1414,    2, 0x08,  109 /* Private */,
      88,    1, 1417,    2, 0x08,  111 /* Private */,
      89,    1, 1420,    2, 0x08,  113 /* Private */,
      90,    0, 1423,    2, 0x08,  115 /* Private */,
      91,    0, 1424,    2, 0x08,  116 /* Private */,
      92,    0, 1425,    2, 0x08,  117 /* Private */,
      93,    0, 1426,    2, 0x08,  118 /* Private */,
      94,    0, 1427,    2, 0x08,  119 /* Private */,
      95,    0, 1428,    2, 0x08,  120 /* Private */,
      96,    0, 1429,    2, 0x08,  121 /* Private */,
      97,    0, 1430,    2, 0x08,  122 /* Private */,
      98,    0, 1431,    2, 0x08,  123 /* Private */,
      99,    0, 1432,    2, 0x08,  124 /* Private */,
     100,    0, 1433,    2, 0x08,  125 /* Private */,
     101,    0, 1434,    2, 0x08,  126 /* Private */,
     102,    0, 1435,    2, 0x08,  127 /* Private */,
     103,    2, 1436,    2, 0x08,  128 /* Private */,
     104,    2, 1441,    2, 0x08,  131 /* Private */,
     105,    1, 1446,    2, 0x08,  134 /* Private */,
     106,    1, 1449,    2, 0x08,  136 /* Private */,
     107,    2, 1452,    2, 0x08,  138 /* Private */,
     108,    0, 1457,    2, 0x08,  141 /* Private */,
     109,    0, 1458,    2, 0x08,  142 /* Private */,
     110,    0, 1459,    2, 0x08,  143 /* Private */,
     111,    0, 1460,    2, 0x08,  144 /* Private */,
     112,    0, 1461,    2, 0x08,  145 /* Private */,
     113,    0, 1462,    2, 0x08,  146 /* Private */,
     114,    0, 1463,    2, 0x08,  147 /* Private */,
     115,    0, 1464,    2, 0x08,  148 /* Private */,
     116,    0, 1465,    2, 0x08,  149 /* Private */,
     117,    0, 1466,    2, 0x08,  150 /* Private */,
     118,    2, 1467,    2, 0x08,  151 /* Private */,
     119,    2, 1472,    2, 0x08,  154 /* Private */,
     120,    1, 1477,    2, 0x08,  157 /* Private */,
     121,    1, 1480,    2, 0x08,  159 /* Private */,
     122,    1, 1483,    2, 0x08,  161 /* Private */,
     123,    1, 1486,    2, 0x08,  163 /* Private */,
     124,    1, 1489,    2, 0x08,  165 /* Private */,
     125,    1, 1492,    2, 0x08,  167 /* Private */,
     126,    1, 1495,    2, 0x08,  169 /* Private */,
     127,    1, 1498,    2, 0x08,  171 /* Private */,
     128,    1, 1501,    2, 0x08,  173 /* Private */,
     129,    1, 1504,    2, 0x08,  175 /* Private */,
     130,    1, 1507,    2, 0x08,  177 /* Private */,
     131,    0, 1510,    2, 0x08,  179 /* Private */,
     132,    1, 1511,    2, 0x08,  180 /* Private */,
     133,    1, 1514,    2, 0x08,  182 /* Private */,
     134,    0, 1517,    2, 0x08,  184 /* Private */,
     135,    0, 1518,    2, 0x08,  185 /* Private */,
     136,    0, 1519,    2, 0x08,  186 /* Private */,
     137,    0, 1520,    2, 0x08,  187 /* Private */,
     138,    0, 1521,    2, 0x08,  188 /* Private */,
     139,    0, 1522,    2, 0x08,  189 /* Private */,
     140,    0, 1523,    2, 0x08,  190 /* Private */,
     141,    0, 1524,    2, 0x08,  191 /* Private */,
     142,    0, 1525,    2, 0x08,  192 /* Private */,
     143,    0, 1526,    2, 0x08,  193 /* Private */,
     144,    0, 1527,    2, 0x08,  194 /* Private */,
     145,    0, 1528,    2, 0x08,  195 /* Private */,
     146,    1, 1529,    2, 0x08,  196 /* Private */,
     147,    1, 1532,    2, 0x08,  198 /* Private */,
     148,    1, 1535,    2, 0x08,  200 /* Private */,
     149,    1, 1538,    2, 0x08,  202 /* Private */,
     150,    0, 1541,    2, 0x08,  204 /* Private */,
     151,    0, 1542,    2, 0x08,  205 /* Private */,
     152,    0, 1543,    2, 0x08,  206 /* Private */,
     153,    0, 1544,    2, 0x08,  207 /* Private */,
     154,    0, 1545,    2, 0x08,  208 /* Private */,
     155,    0, 1546,    2, 0x08,  209 /* Private */,
     156,    0, 1547,    2, 0x08,  210 /* Private */,
     157,    0, 1548,    2, 0x08,  211 /* Private */,
     158,    0, 1549,    2, 0x08,  212 /* Private */,
     159,    0, 1550,    2, 0x08,  213 /* Private */,
     160,    1, 1551,    2, 0x08,  214 /* Private */,
     162,    1, 1554,    2, 0x08,  216 /* Private */,
     163,    1, 1557,    2, 0x08,  218 /* Private */,
     164,    1, 1560,    2, 0x08,  220 /* Private */,
     165,    1, 1563,    2, 0x08,  222 /* Private */,
     166,    1, 1566,    2, 0x08,  224 /* Private */,
     167,    1, 1569,    2, 0x08,  226 /* Private */,
     168,    1, 1572,    2, 0x08,  228 /* Private */,
     169,    0, 1575,    2, 0x08,  230 /* Private */,
     170,    0, 1576,    2, 0x08,  231 /* Private */,
     171,    0, 1577,    2, 0x08,  232 /* Private */,
     172,    0, 1578,    2, 0x08,  233 /* Private */,
     173,    0, 1579,    2, 0x08,  234 /* Private */,
     174,    0, 1580,    2, 0x08,  235 /* Private */,
     175,    0, 1581,    2, 0x08,  236 /* Private */,
     176,    0, 1582,    2, 0x08,  237 /* Private */,
     177,    0, 1583,    2, 0x08,  238 /* Private */,
     178,    0, 1584,    2, 0x08,  239 /* Private */,
     179,    0, 1585,    2, 0x08,  240 /* Private */,
     180,    0, 1586,    2, 0x08,  241 /* Private */,
     181,    0, 1587,    2, 0x08,  242 /* Private */,
     182,    0, 1588,    2, 0x08,  243 /* Private */,
     183,    1, 1589,    2, 0x08,  244 /* Private */,
     184,    1, 1592,    2, 0x08,  246 /* Private */,
     185,    1, 1595,    2, 0x08,  248 /* Private */,
     186,    1, 1598,    2, 0x08,  250 /* Private */,
     187,    0, 1601,    2, 0x08,  252 /* Private */,
     188,    0, 1602,    2, 0x08,  253 /* Private */,
     189,    0, 1603,    2, 0x08,  254 /* Private */,
     190,    0, 1604,    2, 0x08,  255 /* Private */,
     191,    0, 1605,    2, 0x08,  256 /* Private */,
     192,    0, 1606,    2, 0x08,  257 /* Private */,
     193,    0, 1607,    2, 0x08,  258 /* Private */,
     194,    0, 1608,    2, 0x08,  259 /* Private */,
     195,    0, 1609,    2, 0x08,  260 /* Private */,
     196,    0, 1610,    2, 0x08,  261 /* Private */,
     197,    0, 1611,    2, 0x08,  262 /* Private */,
     198,    0, 1612,    2, 0x08,  263 /* Private */,
     199,    0, 1613,    2, 0x08,  264 /* Private */,
     200,    0, 1614,    2, 0x08,  265 /* Private */,
     201,    0, 1615,    2, 0x08,  266 /* Private */,
     202,    1, 1616,    2, 0x08,  267 /* Private */,
     204,    1, 1619,    2, 0x08,  269 /* Private */,
     205,    1, 1622,    2, 0x08,  271 /* Private */,
     206,    1, 1625,    2, 0x08,  273 /* Private */,
     207,    1, 1628,    2, 0x08,  275 /* Private */,
     208,    0, 1631,    2, 0x08,  277 /* Private */,
     209,    1, 1632,    2, 0x08,  278 /* Private */,
     210,    1, 1635,    2, 0x08,  280 /* Private */,
     211,    1, 1638,    2, 0x08,  282 /* Private */,
     212,    1, 1641,    2, 0x08,  284 /* Private */,
     213,    1, 1644,    2, 0x08,  286 /* Private */,
     214,    0, 1647,    2, 0x08,  288 /* Private */,
     215,    1, 1648,    2, 0x08,  289 /* Private */,
     216,    0, 1651,    2, 0x08,  291 /* Private */,
     217,    0, 1652,    2, 0x08,  292 /* Private */,
     218,    0, 1653,    2, 0x08,  293 /* Private */,
     219,    0, 1654,    2, 0x08,  294 /* Private */,
     220,    0, 1655,    2, 0x08,  295 /* Private */,
     221,    0, 1656,    2, 0x08,  296 /* Private */,
     222,    0, 1657,    2, 0x08,  297 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 55,   11,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 28, 0x80000000 | 28,   73,   74,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void, 0x80000000 | 55,   11,
    QMetaType::Void, 0x80000000 | 55,   11,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 161,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,  203,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindowImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowImplENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowImplENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowImplENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindowImpl, std::true_type>,
        // method 'Moveimage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_action3D_Brush_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionInterpolate_over_selected_slices_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionRefresh_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionExport_SPIERSview_and_Launch_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionMeasure_Volumes_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionAb_out_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionChange_downsampling_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_tabWidget_currentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_CheckBoxRangeSelectedOnly_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_CheckBoxRangeHardFill_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionDistribute_over_range_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SpinBoxRangeBase_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SpinBoxRangeTop_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_actionHidden_masks_locked_for_generation_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSegment_brush_applies_masks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_FindPolynomial_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionShow_position_slice_selector_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionChange_slice_spacing_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSet_slice_position_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionInfo_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionExport_DXF_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OOTreeWidget_itemPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_CurvesTreeWidget_itemPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_actionCreate_mask_from_curve_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SliceSelectorList_itemSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionMaskCopy_selected_from_Previous_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionMaskCopy_all_from_previous_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionMaskCopy_selected_from_next_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionMaskCopy_all_from_next_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionMask_Advance_slice_after_copy_operation_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionMaskGo_back_one_slice_after_copy_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionOutput_SPIERSview_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionDelete_Output_Object_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionNew_Output_Object_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionNew_Output_Object_Group_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionMove_item_to_Group_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionUngroup_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OOTreeWidget_itemSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SegmentsTreeWidget_itemSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_MasksTreeWidget_itemSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OONew_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OOGroup_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OODelete_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OOUp_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OODown_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_OOTreeWidget_expanded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QModelIndex, std::false_type>,
        // method 'on_OOTreeWidget_itemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_actionResize_keeping_curve_shape_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionLock_curve_shape_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionRemove_selected_curves_from_selected_slices_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCopy_from_current_slice_to_selected_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_CurvesTreeWidget_itemSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCopyCurvesFromPrevious_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCopyCurvesFromNext_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCopyCurvesToCurrent_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_Curve_Add_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_CurveDelete_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_CurveMoveUp_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_CurveMoveDown_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionAdd_new_point_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionRemove_node_under_cursor_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCurve_markers_as_crosses_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_CurvesTreeWidget_currentItemChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        // method 'on_CurvesTreeWidget_itemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_actionHistogram_shows_selected_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionHistorgram_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionSettings_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SpinBoxSparsity_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SpinBoxOrder_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SpinBoxRetries_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SpinBoxContrast_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_LinearGreenSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_LinearRedSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_LinearGreenSpinBox_editingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_LinearBlueSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_LinearGlobalSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_GenInvert_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_GenerateAuto_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_GenerateButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionNew_mask_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionDelete_selected_mask_s_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCreate_new_segment_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionDelete_selected_segments_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SegmentAdd_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SegmentDelete_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SegmentMoveUp_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SegmentMoveDown_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionLock_selected_segments_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionUnlock_selected_segments_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionActivate_selected_segments_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionDeactivate_selected_segments_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SegmentsTreeWidget_itemPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SegmentsTreeWidget_itemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SegmentsTreeWidget_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QModelIndex, std::false_type>,
        // method 'on_SegmentsTreeWidget_doubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QModelIndex, std::false_type>,
        // method 'on_MasksTreeWidget_itemPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_MaskAdd_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_MaskDelete_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_MaskMoveUp_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_MaskMoveDown_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionLock_Selected_Masks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionUnlock_Selected_Masks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionShow_Selected_Masks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionUnShow_Selected_Masks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionWrite_Selected_Masks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionUnWrite_Selected_Masks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_MasksTreeWidget_itemChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_MasksTreeWidget_itemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SpinBoxConverge_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_FirstFile_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_LastFile_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_DownsampleXY_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_DownsampleZ_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_PixelSensitivity_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_Edge_Down_MM_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_Edge_Left_MM_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_CheckMirror_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_Pixels_Per_MM_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_Slices_Per_MM_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'on_actionView_in_SPIERSview_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_PosSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_ZoomSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_actionManual_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionGrey_out_curves_not_no_current_slice_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionExport_Curves_as_CSV_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionImport_Curves_as_CSV_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionOutput_visible_image_set_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCode_on_GitHub_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionBugIssueFeatureRequest_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ScreenUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_Window_MainToolbox'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_Window_Generate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_File_Import'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'InitStates'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Zoom_Slider_Changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'Trans_Changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'Min_Changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'Max_Changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'Preset1'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset2'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset3'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset4'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset5'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset6'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset7'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset8'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset9'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Preset0'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Mode_Changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAction *, std::false_type>,
        // method 'BrushChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'BrightDownChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'BrightUpChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'SoftChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'BoostSpinBoxChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'BoostRadiusSpinBoxChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'BoostAdjustSpinBoxChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'TransToggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SquareToggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ThresholdFlag'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SetMasksFlag'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SetSegsFlag'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'cmac'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'LockShape'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'BuildRecentFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openRecentFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openMore'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'q_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'a_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'right_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'left_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'LeftMaskChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'RightMaskChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'LeftSegChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'RightSegChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'SaveSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SaveAs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'FileOpen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_File_New'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'UndoTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Undo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Redo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_Window_SliceSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_Window_Masks'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_Window_Curves'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_Window_Segments'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Menu_Window_Output'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'autosave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionTEST_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_Measure_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ShowCenter_stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_CentreX_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_CentreY_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_HardeningRadiusSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_AdjustRadialSpinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SetCentre_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_spinGradientsDistMinEffect_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_spinGradientsDistMaxEffect_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_spinGradientsDistMin_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_spinGradientsDistMax_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_chkGradientsPreview_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_actionThreshold_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_spinGradientsPointDensity_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_actionCycle_Bright_Segment_Mask_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_action1_x_1_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionIncrease_Size_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionDecrease_Size_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_trainML_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_testML_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSegment_brush_applies_locks_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindowImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindowImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->Moveimage((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->on_action3D_Brush_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->on_actionInterpolate_over_selected_slices_triggered(); break;
        case 3: _t->on_actionRefresh_triggered(); break;
        case 4: _t->on_actionExport_SPIERSview_and_Launch_triggered(); break;
        case 5: _t->on_actionMeasure_Volumes_triggered(); break;
        case 6: _t->on_actionAb_out_triggered(); break;
        case 7: _t->on_actionChange_downsampling_triggered(); break;
        case 8: _t->on_tabWidget_currentChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->on_CheckBoxRangeSelectedOnly_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->on_CheckBoxRangeHardFill_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->on_actionDistribute_over_range_triggered(); break;
        case 12: _t->on_SpinBoxRangeBase_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->on_SpinBoxRangeTop_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->on_actionHidden_masks_locked_for_generation_triggered(); break;
        case 15: _t->on_actionSegment_brush_applies_masks_triggered(); break;
        case 16: _t->on_FindPolynomial_pressed(); break;
        case 17: _t->on_actionShow_position_slice_selector_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 18: _t->on_actionChange_slice_spacing_triggered(); break;
        case 19: _t->on_actionSet_slice_position_triggered(); break;
        case 20: _t->on_actionInfo_triggered((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 21: _t->on_actionExport_DXF_triggered(); break;
        case 22: _t->on_OOTreeWidget_itemPressed((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 23: _t->on_CurvesTreeWidget_itemPressed((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 24: _t->on_actionCreate_mask_from_curve_triggered(); break;
        case 25: _t->on_SliceSelectorList_itemSelectionChanged(); break;
        case 26: _t->on_actionMaskCopy_selected_from_Previous_triggered(); break;
        case 27: _t->on_actionMaskCopy_all_from_previous_triggered(); break;
        case 28: _t->on_actionMaskCopy_selected_from_next_triggered(); break;
        case 29: _t->on_actionMaskCopy_all_from_next_triggered(); break;
        case 30: _t->on_actionMask_Advance_slice_after_copy_operation_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 31: _t->on_actionMaskGo_back_one_slice_after_copy_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 32: _t->on_actionOutput_SPIERSview_triggered(); break;
        case 33: _t->on_actionDelete_Output_Object_triggered(); break;
        case 34: _t->on_actionNew_Output_Object_triggered(); break;
        case 35: _t->on_actionNew_Output_Object_Group_triggered(); break;
        case 36: _t->on_actionMove_item_to_Group_triggered(); break;
        case 37: _t->on_actionUngroup_triggered(); break;
        case 38: _t->on_OOTreeWidget_itemSelectionChanged(); break;
        case 39: _t->on_SegmentsTreeWidget_itemSelectionChanged(); break;
        case 40: _t->on_MasksTreeWidget_itemSelectionChanged(); break;
        case 41: _t->on_OONew_clicked(); break;
        case 42: _t->on_OOGroup_clicked(); break;
        case 43: _t->on_OODelete_clicked(); break;
        case 44: _t->on_OOUp_clicked(); break;
        case 45: _t->on_OODown_clicked(); break;
        case 46: _t->on_OOTreeWidget_expanded((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 47: _t->on_OOTreeWidget_itemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 48: _t->on_actionResize_keeping_curve_shape_triggered(); break;
        case 49: _t->on_actionLock_curve_shape_triggered((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 50: _t->on_actionRemove_selected_curves_from_selected_slices_triggered(); break;
        case 51: _t->on_actionCopy_from_current_slice_to_selected_triggered(); break;
        case 52: _t->on_CurvesTreeWidget_itemSelectionChanged(); break;
        case 53: _t->on_actionCopyCurvesFromPrevious_triggered(); break;
        case 54: _t->on_actionCopyCurvesFromNext_triggered(); break;
        case 55: _t->on_actionCopyCurvesToCurrent_triggered(); break;
        case 56: _t->on_Curve_Add_pressed(); break;
        case 57: _t->on_CurveDelete_pressed(); break;
        case 58: _t->on_CurveMoveUp_pressed(); break;
        case 59: _t->on_CurveMoveDown_pressed(); break;
        case 60: _t->on_actionAdd_new_point_triggered(); break;
        case 61: _t->on_actionRemove_node_under_cursor_triggered(); break;
        case 62: _t->on_actionCurve_markers_as_crosses_triggered(); break;
        case 63: _t->on_CurvesTreeWidget_currentItemChanged((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[2]))); break;
        case 64: _t->on_CurvesTreeWidget_itemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 65: _t->on_actionHistogram_shows_selected_triggered((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 66: _t->on_actionHistorgram_triggered((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 67: _t->on_actionSettings_triggered(); break;
        case 68: _t->on_SpinBoxSparsity_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 69: _t->on_SpinBoxOrder_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 70: _t->on_SpinBoxRetries_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 71: _t->on_SpinBoxContrast_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 72: _t->on_LinearGreenSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 73: _t->on_LinearRedSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 74: _t->on_LinearGreenSpinBox_editingFinished(); break;
        case 75: _t->on_LinearBlueSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 76: _t->on_LinearGlobalSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 77: _t->on_GenInvert_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 78: _t->on_GenerateAuto_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 79: _t->on_GenerateButton_clicked(); break;
        case 80: _t->on_actionNew_mask_triggered(); break;
        case 81: _t->on_actionDelete_selected_mask_s_triggered(); break;
        case 82: _t->on_actionCreate_new_segment_triggered(); break;
        case 83: _t->on_actionDelete_selected_segments_triggered(); break;
        case 84: _t->on_SegmentAdd_pressed(); break;
        case 85: _t->on_SegmentDelete_pressed(); break;
        case 86: _t->on_SegmentMoveUp_pressed(); break;
        case 87: _t->on_SegmentMoveDown_pressed(); break;
        case 88: _t->on_actionLock_selected_segments_triggered(); break;
        case 89: _t->on_actionUnlock_selected_segments_triggered(); break;
        case 90: _t->on_actionActivate_selected_segments_triggered(); break;
        case 91: _t->on_actionDeactivate_selected_segments_triggered(); break;
        case 92: _t->on_SegmentsTreeWidget_itemPressed((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 93: _t->on_SegmentsTreeWidget_itemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 94: _t->on_SegmentsTreeWidget_pressed((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 95: _t->on_SegmentsTreeWidget_doubleClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 96: _t->on_MasksTreeWidget_itemPressed((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 97: _t->on_MaskAdd_pressed(); break;
        case 98: _t->on_MaskDelete_pressed(); break;
        case 99: _t->on_MaskMoveUp_pressed(); break;
        case 100: _t->on_MaskMoveDown_pressed(); break;
        case 101: _t->on_actionLock_Selected_Masks_triggered(); break;
        case 102: _t->on_actionUnlock_Selected_Masks_triggered(); break;
        case 103: _t->on_actionShow_Selected_Masks_triggered(); break;
        case 104: _t->on_actionUnShow_Selected_Masks_triggered(); break;
        case 105: _t->on_actionWrite_Selected_Masks_triggered(); break;
        case 106: _t->on_actionUnWrite_Selected_Masks_triggered(); break;
        case 107: _t->on_MasksTreeWidget_itemChanged((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 108: _t->on_MasksTreeWidget_itemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 109: _t->on_SpinBoxConverge_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 110: _t->on_FirstFile_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 111: _t->on_LastFile_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 112: _t->on_DownsampleXY_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 113: _t->on_DownsampleZ_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 114: _t->on_PixelSensitivity_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 115: _t->on_Edge_Down_MM_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 116: _t->on_Edge_Left_MM_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 117: _t->on_CheckMirror_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 118: _t->on_Pixels_Per_MM_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 119: _t->on_Slices_Per_MM_valueChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 120: _t->on_actionView_in_SPIERSview_triggered(); break;
        case 121: _t->on_PosSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 122: _t->on_ZoomSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 123: _t->on_actionManual_triggered(); break;
        case 124: _t->on_actionGrey_out_curves_not_no_current_slice_triggered(); break;
        case 125: _t->on_actionExport_Curves_as_CSV_triggered(); break;
        case 126: _t->on_actionImport_Curves_as_CSV_triggered(); break;
        case 127: _t->on_actionOutput_visible_image_set_triggered(); break;
        case 128: _t->on_actionCode_on_GitHub_triggered(); break;
        case 129: _t->on_actionBugIssueFeatureRequest_triggered(); break;
        case 130: _t->ScreenUpdate(); break;
        case 131: _t->Menu_Window_MainToolbox(); break;
        case 132: _t->Menu_Window_Generate(); break;
        case 133: _t->Menu_File_Import(); break;
        case 134: _t->InitStates(); break;
        case 135: _t->Zoom_Slider_Changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 136: _t->Trans_Changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 137: _t->Min_Changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 138: _t->Max_Changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 139: _t->Preset1(); break;
        case 140: _t->Preset2(); break;
        case 141: _t->Preset3(); break;
        case 142: _t->Preset4(); break;
        case 143: _t->Preset5(); break;
        case 144: _t->Preset6(); break;
        case 145: _t->Preset7(); break;
        case 146: _t->Preset8(); break;
        case 147: _t->Preset9(); break;
        case 148: _t->Preset0(); break;
        case 149: _t->Mode_Changed((*reinterpret_cast< std::add_pointer_t<QAction*>>(_a[1]))); break;
        case 150: _t->BrushChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 151: _t->BrightDownChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 152: _t->BrightUpChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 153: _t->SoftChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 154: _t->BoostSpinBoxChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 155: _t->BoostRadiusSpinBoxChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 156: _t->BoostAdjustSpinBoxChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 157: _t->TransToggled(); break;
        case 158: _t->SquareToggled(); break;
        case 159: _t->ThresholdFlag(); break;
        case 160: _t->SetMasksFlag(); break;
        case 161: _t->SetSegsFlag(); break;
        case 162: _t->cmac(); break;
        case 163: _t->LockShape(); break;
        case 164: _t->BuildRecentFiles(); break;
        case 165: _t->openRecentFile(); break;
        case 166: _t->openMore(); break;
        case 167: _t->q_pressed(); break;
        case 168: _t->a_pressed(); break;
        case 169: _t->right_pressed(); break;
        case 170: _t->left_pressed(); break;
        case 171: _t->LeftMaskChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 172: _t->RightMaskChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 173: _t->LeftSegChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 174: _t->RightSegChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 175: _t->SaveSettings(); break;
        case 176: _t->SaveAs(); break;
        case 177: _t->FileOpen(); break;
        case 178: _t->Menu_File_New(); break;
        case 179: _t->UndoTimer(); break;
        case 180: _t->Undo(); break;
        case 181: _t->Redo(); break;
        case 182: _t->Menu_Window_SliceSelector(); break;
        case 183: _t->Menu_Window_Masks(); break;
        case 184: _t->Menu_Window_Curves(); break;
        case 185: _t->Menu_Window_Segments(); break;
        case 186: _t->Menu_Window_Output(); break;
        case 187: _t->autosave(); break;
        case 188: _t->on_actionTEST_triggered(); break;
        case 189: _t->on_Measure_clicked(); break;
        case 190: _t->on_ShowCenter_stateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 191: _t->on_CentreX_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 192: _t->on_CentreY_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 193: _t->on_HardeningRadiusSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 194: _t->on_AdjustRadialSpinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 195: _t->on_SetCentre_clicked(); break;
        case 196: _t->on_spinGradientsDistMinEffect_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 197: _t->on_spinGradientsDistMaxEffect_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 198: _t->on_spinGradientsDistMin_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 199: _t->on_spinGradientsDistMax_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 200: _t->on_chkGradientsPreview_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 201: _t->on_actionThreshold_triggered(); break;
        case 202: _t->on_spinGradientsPointDensity_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 203: _t->on_actionCycle_Bright_Segment_Mask_triggered(); break;
        case 204: _t->on_action1_x_1_triggered(); break;
        case 205: _t->on_actionIncrease_Size_triggered(); break;
        case 206: _t->on_actionDecrease_Size_triggered(); break;
        case 207: _t->on_trainML_clicked(); break;
        case 208: _t->on_testML_clicked(); break;
        case 209: _t->on_actionSegment_brush_applies_locks_triggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 149:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *MainWindowImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindowImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowImplENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::MainWindow"))
        return static_cast< Ui::MainWindow*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindowImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 210)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 210;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 210)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 210;
    }
    return _id;
}
QT_WARNING_POP
