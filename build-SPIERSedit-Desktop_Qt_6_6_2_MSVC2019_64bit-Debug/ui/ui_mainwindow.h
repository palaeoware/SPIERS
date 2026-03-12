/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "src/keysafespinbox.h"
#include "src/mainview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionA_command;
    QAction *actionMain_Toolbox;
    QAction *actionGeneration;
    QAction *actionMasks;
    QAction *actionSegments;
    QAction *actionCurves;
    QAction *actionSlice_Selector;
    QAction *actionSave_Settings;
    QAction *actionOutput_and_Render;
    QAction *actionExit;
    QAction *actionOpen;
    QAction *actionBright;
    QAction *actionSegment;
    QAction *actionRecalc;
    QAction *actionCurve;
    QAction *actionLock;
    QAction *actionMask;
    QAction *action1_x_1;
    QAction *action2_x_2;
    QAction *action3_x_3;
    QAction *action4_x_4;
    QAction *action5_x_5;
    QAction *action8_x_8;
    QAction *action15_x_15;
    QAction *action30_x_30;
    QAction *action50_x_50;
    QAction *action200_x_200;
    QAction *actionSquare;
    QAction *actionCircle;
    QAction *actionToggle_Source;
    QAction *actionThreshold;
    QAction *actionShowMasks;
    QAction *actionShowSegs;
    QAction *actionAb_out;
    QAction *actionKeyboard_Commands;
    QAction *actionMaskCopy_selected_from_Previous;
    QAction *actionMaskCopy_all_from_previous;
    QAction *actionMaskCopy_selected_from_next;
    QAction *actionMaskCopy_all_from_next;
    QAction *actionCopy_selected_on_and_advance;
    QAction *actionCopy_all_on_and_advance;
    QAction *actionCopy_selected_back_and_advance;
    QAction *actionCopy_all_back_and_adance;
    QAction *actionHidden_masks_locked_for_generation;
    QAction *actionSegment_brush_applies_masks;
    QAction *actionInvert_locking_for_this_slice;
    QAction *actionLock_mask_under_cursor;
    QAction *actionUnlock_mask_under_cursor;
    QAction *actionCreate_mask_from_curve;
    QAction *actionSwap_Segments;
    QAction *actionLock_segments;
    QAction *actionCopy_segment_from_next_slice;
    QAction *actionCopy_segment_from_previous_slice;
    QAction *actionCopyCurvesFromPrevious;
    QAction *actionCopy_selected_from_next_slice;
    QAction *actionCopy_all_from_previous_slice;
    QAction *actionCopyCurvesFromNext;
    QAction *actionCopyCurvesToCurrent;
    QAction *actionCurve_markers_as_crosses;
    QAction *actionCreate_curve_as_simple_circle;
    QAction *actionLock_curve_shape;
    QAction *actionResize_keeping_curve_shape;
    QAction *actionNew;
    QAction *actionImport;
    QAction *actionSave_As;
    QAction *actionMore;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionOutput;
    QAction *actionSave_Objects;
    QAction *actionLoad_Objects;
    QAction *actionOutput_SPIERSview;
    QAction *actionExport_DXF;
    QAction *actionAuto_Update;
    QAction *actionUpdate_View;
    QAction *actionLock_Selected_Masks;
    QAction *actionUnlock_Selected_Masks;
    QAction *actionShow_Selected_Masks;
    QAction *actionUnShow_Selected_Masks;
    QAction *actionWrite_Selected_Masks;
    QAction *actionUnWrite_Selected_Masks;
    QAction *actionLock_selected_segments;
    QAction *actionUnlock_selected_segments;
    QAction *actionActivate_selected_segments;
    QAction *actionDeactivate_selected_segments;
    QAction *actionNew_mask;
    QAction *actionDelete_selected_mask_s;
    QAction *actionCreate_new_segment;
    QAction *actionDelete_selected_segments;
    QAction *actionSettings;
    QAction *actionHistorgram;
    QAction *actionHistogram_shows_selected;
    QAction *actionAdd_new_point;
    QAction *actionRemove_node_under_cursor;
    QAction *actionCopy_from_current_slice_to_selected;
    QAction *actionRemove_selected_curves_from_selected_slices;
    QAction *actionNew_Output_Object;
    QAction *actionNew_Output_Object_Group;
    QAction *actionDelete_Output_Object;
    QAction *actionUngroup;
    QAction *actionMove_item_to_Group;
    QAction *actionMask_Advance_slice_after_copy_operation;
    QAction *actionMaskGo_back_one_slice_after_copy;
    QAction *actionInfo;
    QAction *actionShow_position_slice_selector;
    QAction *actionChange_slice_spacing;
    QAction *actionSet_slice_position;
    QAction *actionDistribute_over_range;
    QAction *actionChange_downsampling;
    QAction *actionMeasure_Volumes;
    QAction *actionExport_SPIERSview_and_Launch;
    QAction *actionRefresh;
    QAction *actionInterpolate_over_selected_slices;
    QAction *action3D_Brush;
    QAction *actionView_in_SPIERSview;
    QAction *actionUse_Old_Exporting_Code;
    QAction *actionManual;
    QAction *actionGrey_out_curves_not_no_current_slice;
    QAction *actionExport_Curves_as_CSV;
    QAction *actionImport_Curves_as_CSV;
    QAction *actionOutput_visible_image_set;
    QAction *actionTEST;
    QAction *actionBugIssueFeatureRequest;
    QAction *actionCode_on_GitHub;
    QAction *actionCycle_Bright_Segment_Mask;
    QAction *actionIncrease_Size;
    QAction *actionDecrease_Size;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_21;
    KeysafeSpinBox *ZoomSpinBox;
    QSlider *ZoomSlider;
    QVBoxLayout *verticalLayout_4;
    mainview *graphicsView;
    QHBoxLayout *_2;
    QSlider *SliderPos;
    QLabel *LabelImage;
    KeysafeSpinBox *PosSpinBox;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuOpen_Recent;
    QMenu *menuMode;
    QMenu *menuWindow;
    QMenu *menuBrush;
    QMenu *menuSegments;
    QMenu *menuMasks;
    QMenu *menuCurves;
    QMenu *menuHelp;
    QMenu *menuUndo;
    QMenu *menuOutput;
    QMenu *menuSlice_Spacing;
    QStatusBar *statusbar;
    QDockWidget *dockWidget_Main;
    QWidget *dockWidgetContents;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QSlider *verticalSlider;
    QSlider *verticalSlider_2;
    QSlider *verticalSlider_3;
    KeysafeSpinBox *SpinUp;
    KeysafeSpinBox *SpinDown;
    KeysafeSpinBox *SpinSoft;
    QGroupBox *groupBox_5;
    QHBoxLayout *hboxLayout;
    QGridLayout *gridLayout1;
    QLabel *label;
    QComboBox *MaskBoxLeft;
    QLabel *label_2;
    QComboBox *MaskBoxRight;
    QGroupBox *groupBox_6;
    QHBoxLayout *hboxLayout1;
    QGridLayout *gridLayout2;
    QLabel *label_4;
    QLabel *label_5;
    QGridLayout *gridLayout3;
    QComboBox *SegBoxLeft;
    QComboBox *SegBoxRight;
    QGroupBox *groupBox_2;
    QVBoxLayout *vboxLayout2;
    QScrollBar *MinSlider;
    QScrollBar *MaxSlider;
    QGroupBox *groupBox_3;
    QHBoxLayout *hboxLayout2;
    QSlider *TransSlider;
    QDockWidget *dockWidget_Generate;
    QWidget *dockWidgetContents_2;
    QVBoxLayout *vboxLayout3;
    QTabWidget *tabWidget;
    QWidget *tabLinear;
    QHBoxLayout *hboxLayout3;
    QVBoxLayout *vboxLayout4;
    QHBoxLayout *hboxLayout4;
    QVBoxLayout *vboxLayout5;
    KeysafeSpinBox *LinearRedSpinBox;
    QVBoxLayout *vboxLayout6;
    KeysafeSpinBox *LinearGreenSpinBox;
    QVBoxLayout *vboxLayout7;
    KeysafeSpinBox *LinearBlueSpinBox;
    QHBoxLayout *hboxLayout5;
    QLabel *label_23;
    QSlider *LinearGlobalSlider;
    KeysafeSpinBox *LinearGlobalSpinBox;
    QWidget *tabML;
    QVBoxLayout *verticalLayout_11;
    QHBoxLayout *horizontalLayout_15;
    QPushButton *testML;
    QPushButton *trainML;
    QWidget *tabRange;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayoutRange;
    QHBoxLayout *horizontalLayout_2;
    KeysafeSpinBox *SpinBoxRangeBase;
    KeysafeSpinBox *SpinBoxRangeTop;
    QHBoxLayout *horizontalLayout;
    QCheckBox *CheckBoxRangeSelectedOnly;
    QCheckBox *CheckBoxRangeHardFill;
    QWidget *tabLCE;
    QGridLayout *gridLayout_2;
    QLabel *label_30;
    KeysafeSpinBox *BoostSpinBox;
    QSlider *SliderBoostRadius;
    KeysafeSpinBox *BoostRadiusSpinBox;
    QSlider *SliderBoost;
    QLabel *label_29;
    QSlider *SliderBoostAdjust;
    QLabel *label_31;
    KeysafeSpinBox *BoostAdjustSpinBox;
    QWidget *tabBeamHardening;
    QVBoxLayout *verticalLayout_8;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_6;
    QCheckBox *ShowCenter;
    QPushButton *SetCentre;
    QPushButton *Measure;
    QHBoxLayout *horizontalLayout_5;
    KeysafeSpinBox *CentreX;
    KeysafeSpinBox *CentreY;
    QLabel *labelHardeningData;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_32;
    QSlider *SliderHardeningRadius;
    KeysafeSpinBox *HardeningRadiusSpinBox;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_33;
    QSlider *SliderBoostAdjust_2;
    KeysafeSpinBox *AdjustRadialSpinBox;
    QWidget *tabGradients;
    QVBoxLayout *verticalLayout_10;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_36;
    QSlider *SliderBoost_4;
    KeysafeSpinBox *spinGradientsDistMinEffect;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_37;
    QSlider *SliderBoost_5;
    KeysafeSpinBox *spinGradientsDistMaxEffect;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_38;
    QSlider *SliderBoost_6;
    KeysafeSpinBox *spinGradientsDistMin;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_39;
    QSlider *SliderBoost_7;
    KeysafeSpinBox *spinGradientsDistMax;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_40;
    QSlider *SliderBoost_8;
    KeysafeSpinBox *spinGradientsPointDensity;
    QSpacerItem *verticalSpacer_5;
    QHBoxLayout *horizontalLayout_13;
    QLabel *lblGradientsWarning;
    QCheckBox *chkGradientsPreview;
    QHBoxLayout *hboxLayout6;
    QCheckBox *GenInvert;
    QCheckBox *GenerateAuto;
    QPushButton *GenerateButton;
    QDockWidget *DockOutputSettings;
    QWidget *dockWidgetContents_8;
    QVBoxLayout *vboxLayout8;
    QTabWidget *OutputTabs;
    QWidget *tabSettings;
    QHBoxLayout *hboxLayout7;
    QGridLayout *gridLayout4;
    QLabel *label_12;
    QCheckBox *CheckMirror;
    QLabel *label_11;
    QLabel *label_16;
    QLabel *label_9;
    QLabel *label_10;
    KeysafeSpinBox *DownsampleXY;
    QLabel *label_17;
    QLabel *label_14;
    KeysafeSpinBox *DownsampleZ;
    QLabel *label_15;
    QLabel *label_13;
    QDoubleSpinBox *Pixels_Per_MM;
    QDoubleSpinBox *Slices_Per_MM;
    QDoubleSpinBox *Edge_Left_MM;
    QDoubleSpinBox *Edge_Down_MM;
    KeysafeSpinBox *PixelSensitivity;
    QLabel *label_3;
    KeysafeSpinBox *FirstFile;
    KeysafeSpinBox *LastFile;
    QSpacerItem *verticalSpacer;
    QWidget *tabObjects;
    QVBoxLayout *vboxLayout9;
    QTreeWidget *OOTreeWidget;
    QHBoxLayout *hboxLayout8;
    QLabel *label_18;
    KeysafeSpinBox *OOResample;
    QSpacerItem *spacerItem;
    QCheckBox *DirectCurves;
    QSpacerItem *spacerItem1;
    QLabel *label_19;
    QComboBox *NextKey;
    QHBoxLayout *hboxLayout9;
    QPushButton *OONew;
    QPushButton *OOGroup;
    QPushButton *OODelete;
    QPushButton *OOUp;
    QPushButton *OODown;
    QDockWidget *DockHist;
    QWidget *dockWidgetContents_7;
    QDockWidget *DockSegmentsSettings;
    QWidget *dockWidgetContents_6;
    QVBoxLayout *vboxLayout10;
    QTreeWidget *SegmentsTreeWidget;
    QHBoxLayout *hboxLayout10;
    QPushButton *SegmentAdd;
    QPushButton *SegmentDelete;
    QPushButton *SegmentMoveUp;
    QPushButton *SegmentMoveDown;
    QDockWidget *DockInfo;
    QWidget *dockWidgetContents_9;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_20;
    QLabel *InfoLabel;
    QPlainTextEdit *plainTextEdit;
    QDockWidget *DockMasksSettings;
    QWidget *dockWidgetContents_5;
    QVBoxLayout *vboxLayout11;
    QTreeWidget *MasksTreeWidget;
    QHBoxLayout *hboxLayout11;
    QPushButton *MaskAdd;
    QPushButton *MaskDelete;
    QPushButton *MaskMoveUp;
    QPushButton *MaskMoveDown;
    QDockWidget *DockCurvesSettings;
    QWidget *dockWidgetContents_4;
    QVBoxLayout *vboxLayout12;
    QTreeWidget *CurvesTreeWidget;
    QHBoxLayout *hboxLayout12;
    QPushButton *Curve_Add;
    QPushButton *CurveDelete;
    QPushButton *CurveMoveUp;
    QPushButton *CurveMoveDown;
    QDockWidget *SliceSelector;
    QWidget *dockWidgetContents_3;
    QVBoxLayout *verticalLayout_6;
    QListWidget *SliceSelectorList;
    QPushButton *SliceSelectAll;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(962, 2039);
        QSizePolicy sizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/resources/SPIERSeditIcon.svg"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setDockNestingEnabled(true);
        MainWindow->setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionA_command = new QAction(MainWindow);
        actionA_command->setObjectName("actionA_command");
        actionMain_Toolbox = new QAction(MainWindow);
        actionMain_Toolbox->setObjectName("actionMain_Toolbox");
        actionMain_Toolbox->setCheckable(true);
        actionGeneration = new QAction(MainWindow);
        actionGeneration->setObjectName("actionGeneration");
        actionGeneration->setCheckable(true);
        actionGeneration->setChecked(true);
        actionMasks = new QAction(MainWindow);
        actionMasks->setObjectName("actionMasks");
        actionMasks->setCheckable(true);
        actionMasks->setChecked(true);
        actionSegments = new QAction(MainWindow);
        actionSegments->setObjectName("actionSegments");
        actionSegments->setCheckable(true);
        actionCurves = new QAction(MainWindow);
        actionCurves->setObjectName("actionCurves");
        actionCurves->setCheckable(true);
        actionSlice_Selector = new QAction(MainWindow);
        actionSlice_Selector->setObjectName("actionSlice_Selector");
        actionSlice_Selector->setCheckable(true);
        actionSave_Settings = new QAction(MainWindow);
        actionSave_Settings->setObjectName("actionSave_Settings");
        actionSave_Settings->setEnabled(false);
        actionOutput_and_Render = new QAction(MainWindow);
        actionOutput_and_Render->setObjectName("actionOutput_and_Render");
        actionOutput_and_Render->setEnabled(false);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        actionBright = new QAction(MainWindow);
        actionBright->setObjectName("actionBright");
        actionBright->setCheckable(true);
        actionBright->setEnabled(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../bulb.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionBright->setIcon(icon1);
        actionBright->setShortcutContext(Qt::WindowShortcut);
        actionSegment = new QAction(MainWindow);
        actionSegment->setObjectName("actionSegment");
        actionSegment->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("../segment.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionSegment->setIcon(icon2);
        actionSegment->setShortcutContext(Qt::WindowShortcut);
        actionRecalc = new QAction(MainWindow);
        actionRecalc->setObjectName("actionRecalc");
        actionRecalc->setCheckable(true);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("../Recalc.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionRecalc->setIcon(icon3);
        actionCurve = new QAction(MainWindow);
        actionCurve->setObjectName("actionCurve");
        actionCurve->setCheckable(true);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("../Curves.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionCurve->setIcon(icon4);
        actionLock = new QAction(MainWindow);
        actionLock->setObjectName("actionLock");
        actionLock->setCheckable(true);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8("../Lock.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionLock->setIcon(icon5);
        actionMask = new QAction(MainWindow);
        actionMask->setObjectName("actionMask");
        actionMask->setCheckable(true);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8("../Masks.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionMask->setIcon(icon6);
        action1_x_1 = new QAction(MainWindow);
        action1_x_1->setObjectName("action1_x_1");
        action1_x_1->setEnabled(true);
        action2_x_2 = new QAction(MainWindow);
        action2_x_2->setObjectName("action2_x_2");
        action3_x_3 = new QAction(MainWindow);
        action3_x_3->setObjectName("action3_x_3");
        action4_x_4 = new QAction(MainWindow);
        action4_x_4->setObjectName("action4_x_4");
        action5_x_5 = new QAction(MainWindow);
        action5_x_5->setObjectName("action5_x_5");
        action8_x_8 = new QAction(MainWindow);
        action8_x_8->setObjectName("action8_x_8");
        action15_x_15 = new QAction(MainWindow);
        action15_x_15->setObjectName("action15_x_15");
        action30_x_30 = new QAction(MainWindow);
        action30_x_30->setObjectName("action30_x_30");
        action50_x_50 = new QAction(MainWindow);
        action50_x_50->setObjectName("action50_x_50");
        action200_x_200 = new QAction(MainWindow);
        action200_x_200->setObjectName("action200_x_200");
        actionSquare = new QAction(MainWindow);
        actionSquare->setObjectName("actionSquare");
        actionSquare->setCheckable(true);
        actionCircle = new QAction(MainWindow);
        actionCircle->setObjectName("actionCircle");
        actionCircle->setCheckable(true);
        actionCircle->setChecked(true);
        actionToggle_Source = new QAction(MainWindow);
        actionToggle_Source->setObjectName("actionToggle_Source");
        actionToggle_Source->setShortcutContext(Qt::WindowShortcut);
        actionThreshold = new QAction(MainWindow);
        actionThreshold->setObjectName("actionThreshold");
        actionThreshold->setCheckable(true);
        actionThreshold->setChecked(true);
        actionThreshold->setShortcutContext(Qt::WindowShortcut);
        actionShowMasks = new QAction(MainWindow);
        actionShowMasks->setObjectName("actionShowMasks");
        actionShowMasks->setCheckable(true);
        actionShowSegs = new QAction(MainWindow);
        actionShowSegs->setObjectName("actionShowSegs");
        actionShowSegs->setCheckable(true);
        actionAb_out = new QAction(MainWindow);
        actionAb_out->setObjectName("actionAb_out");
        actionKeyboard_Commands = new QAction(MainWindow);
        actionKeyboard_Commands->setObjectName("actionKeyboard_Commands");
        actionMaskCopy_selected_from_Previous = new QAction(MainWindow);
        actionMaskCopy_selected_from_Previous->setObjectName("actionMaskCopy_selected_from_Previous");
        actionMaskCopy_all_from_previous = new QAction(MainWindow);
        actionMaskCopy_all_from_previous->setObjectName("actionMaskCopy_all_from_previous");
        actionMaskCopy_selected_from_next = new QAction(MainWindow);
        actionMaskCopy_selected_from_next->setObjectName("actionMaskCopy_selected_from_next");
        actionMaskCopy_all_from_next = new QAction(MainWindow);
        actionMaskCopy_all_from_next->setObjectName("actionMaskCopy_all_from_next");
        actionCopy_selected_on_and_advance = new QAction(MainWindow);
        actionCopy_selected_on_and_advance->setObjectName("actionCopy_selected_on_and_advance");
        actionCopy_all_on_and_advance = new QAction(MainWindow);
        actionCopy_all_on_and_advance->setObjectName("actionCopy_all_on_and_advance");
        actionCopy_selected_back_and_advance = new QAction(MainWindow);
        actionCopy_selected_back_and_advance->setObjectName("actionCopy_selected_back_and_advance");
        actionCopy_all_back_and_adance = new QAction(MainWindow);
        actionCopy_all_back_and_adance->setObjectName("actionCopy_all_back_and_adance");
        actionHidden_masks_locked_for_generation = new QAction(MainWindow);
        actionHidden_masks_locked_for_generation->setObjectName("actionHidden_masks_locked_for_generation");
        actionHidden_masks_locked_for_generation->setCheckable(true);
        actionSegment_brush_applies_masks = new QAction(MainWindow);
        actionSegment_brush_applies_masks->setObjectName("actionSegment_brush_applies_masks");
        actionSegment_brush_applies_masks->setCheckable(true);
        actionInvert_locking_for_this_slice = new QAction(MainWindow);
        actionInvert_locking_for_this_slice->setObjectName("actionInvert_locking_for_this_slice");
        actionLock_mask_under_cursor = new QAction(MainWindow);
        actionLock_mask_under_cursor->setObjectName("actionLock_mask_under_cursor");
        actionUnlock_mask_under_cursor = new QAction(MainWindow);
        actionUnlock_mask_under_cursor->setObjectName("actionUnlock_mask_under_cursor");
        actionCreate_mask_from_curve = new QAction(MainWindow);
        actionCreate_mask_from_curve->setObjectName("actionCreate_mask_from_curve");
        actionSwap_Segments = new QAction(MainWindow);
        actionSwap_Segments->setObjectName("actionSwap_Segments");
        actionSwap_Segments->setVisible(false);
        actionLock_segments = new QAction(MainWindow);
        actionLock_segments->setObjectName("actionLock_segments");
        actionLock_segments->setCheckable(true);
        actionCopy_segment_from_next_slice = new QAction(MainWindow);
        actionCopy_segment_from_next_slice->setObjectName("actionCopy_segment_from_next_slice");
        actionCopy_segment_from_previous_slice = new QAction(MainWindow);
        actionCopy_segment_from_previous_slice->setObjectName("actionCopy_segment_from_previous_slice");
        actionCopyCurvesFromPrevious = new QAction(MainWindow);
        actionCopyCurvesFromPrevious->setObjectName("actionCopyCurvesFromPrevious");
        actionCopyCurvesFromPrevious->setEnabled(true);
        actionCopy_selected_from_next_slice = new QAction(MainWindow);
        actionCopy_selected_from_next_slice->setObjectName("actionCopy_selected_from_next_slice");
        actionCopy_all_from_previous_slice = new QAction(MainWindow);
        actionCopy_all_from_previous_slice->setObjectName("actionCopy_all_from_previous_slice");
        actionCopyCurvesFromNext = new QAction(MainWindow);
        actionCopyCurvesFromNext->setObjectName("actionCopyCurvesFromNext");
        actionCopyCurvesToCurrent = new QAction(MainWindow);
        actionCopyCurvesToCurrent->setObjectName("actionCopyCurvesToCurrent");
        actionCurve_markers_as_crosses = new QAction(MainWindow);
        actionCurve_markers_as_crosses->setObjectName("actionCurve_markers_as_crosses");
        actionCurve_markers_as_crosses->setCheckable(true);
        actionCreate_curve_as_simple_circle = new QAction(MainWindow);
        actionCreate_curve_as_simple_circle->setObjectName("actionCreate_curve_as_simple_circle");
        actionLock_curve_shape = new QAction(MainWindow);
        actionLock_curve_shape->setObjectName("actionLock_curve_shape");
        actionLock_curve_shape->setCheckable(true);
        actionResize_keeping_curve_shape = new QAction(MainWindow);
        actionResize_keeping_curve_shape->setObjectName("actionResize_keeping_curve_shape");
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName("actionNew");
        actionImport = new QAction(MainWindow);
        actionImport->setObjectName("actionImport");
        actionSave_As = new QAction(MainWindow);
        actionSave_As->setObjectName("actionSave_As");
        actionSave_As->setEnabled(false);
        actionMore = new QAction(MainWindow);
        actionMore->setObjectName("actionMore");
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName("actionUndo");
        actionUndo->setEnabled(false);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8("../undo.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionUndo->setIcon(icon7);
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName("actionRedo");
        actionRedo->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8("../redo.bmp"), QSize(), QIcon::Normal, QIcon::Off);
        actionRedo->setIcon(icon8);
        actionOutput = new QAction(MainWindow);
        actionOutput->setObjectName("actionOutput");
        actionOutput->setCheckable(true);
        actionSave_Objects = new QAction(MainWindow);
        actionSave_Objects->setObjectName("actionSave_Objects");
        actionSave_Objects->setEnabled(true);
        actionLoad_Objects = new QAction(MainWindow);
        actionLoad_Objects->setObjectName("actionLoad_Objects");
        actionOutput_SPIERSview = new QAction(MainWindow);
        actionOutput_SPIERSview->setObjectName("actionOutput_SPIERSview");
        actionExport_DXF = new QAction(MainWindow);
        actionExport_DXF->setObjectName("actionExport_DXF");
        actionExport_DXF->setEnabled(false);
        actionAuto_Update = new QAction(MainWindow);
        actionAuto_Update->setObjectName("actionAuto_Update");
        actionUpdate_View = new QAction(MainWindow);
        actionUpdate_View->setObjectName("actionUpdate_View");
        actionLock_Selected_Masks = new QAction(MainWindow);
        actionLock_Selected_Masks->setObjectName("actionLock_Selected_Masks");
        actionUnlock_Selected_Masks = new QAction(MainWindow);
        actionUnlock_Selected_Masks->setObjectName("actionUnlock_Selected_Masks");
        actionShow_Selected_Masks = new QAction(MainWindow);
        actionShow_Selected_Masks->setObjectName("actionShow_Selected_Masks");
        actionUnShow_Selected_Masks = new QAction(MainWindow);
        actionUnShow_Selected_Masks->setObjectName("actionUnShow_Selected_Masks");
        actionWrite_Selected_Masks = new QAction(MainWindow);
        actionWrite_Selected_Masks->setObjectName("actionWrite_Selected_Masks");
        actionUnWrite_Selected_Masks = new QAction(MainWindow);
        actionUnWrite_Selected_Masks->setObjectName("actionUnWrite_Selected_Masks");
        actionLock_selected_segments = new QAction(MainWindow);
        actionLock_selected_segments->setObjectName("actionLock_selected_segments");
        actionUnlock_selected_segments = new QAction(MainWindow);
        actionUnlock_selected_segments->setObjectName("actionUnlock_selected_segments");
        actionActivate_selected_segments = new QAction(MainWindow);
        actionActivate_selected_segments->setObjectName("actionActivate_selected_segments");
        actionDeactivate_selected_segments = new QAction(MainWindow);
        actionDeactivate_selected_segments->setObjectName("actionDeactivate_selected_segments");
        actionNew_mask = new QAction(MainWindow);
        actionNew_mask->setObjectName("actionNew_mask");
        actionNew_mask->setEnabled(false);
        actionDelete_selected_mask_s = new QAction(MainWindow);
        actionDelete_selected_mask_s->setObjectName("actionDelete_selected_mask_s");
        actionCreate_new_segment = new QAction(MainWindow);
        actionCreate_new_segment->setObjectName("actionCreate_new_segment");
        actionDelete_selected_segments = new QAction(MainWindow);
        actionDelete_selected_segments->setObjectName("actionDelete_selected_segments");
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName("actionSettings");
        actionHistorgram = new QAction(MainWindow);
        actionHistorgram->setObjectName("actionHistorgram");
        actionHistorgram->setCheckable(true);
        actionHistogram_shows_selected = new QAction(MainWindow);
        actionHistogram_shows_selected->setObjectName("actionHistogram_shows_selected");
        actionHistogram_shows_selected->setCheckable(true);
        actionHistogram_shows_selected->setChecked(false);
        actionHistogram_shows_selected->setEnabled(true);
        actionAdd_new_point = new QAction(MainWindow);
        actionAdd_new_point->setObjectName("actionAdd_new_point");
        actionRemove_node_under_cursor = new QAction(MainWindow);
        actionRemove_node_under_cursor->setObjectName("actionRemove_node_under_cursor");
        actionCopy_from_current_slice_to_selected = new QAction(MainWindow);
        actionCopy_from_current_slice_to_selected->setObjectName("actionCopy_from_current_slice_to_selected");
        actionRemove_selected_curves_from_selected_slices = new QAction(MainWindow);
        actionRemove_selected_curves_from_selected_slices->setObjectName("actionRemove_selected_curves_from_selected_slices");
        actionNew_Output_Object = new QAction(MainWindow);
        actionNew_Output_Object->setObjectName("actionNew_Output_Object");
        actionNew_Output_Object->setEnabled(true);
        actionNew_Output_Object_Group = new QAction(MainWindow);
        actionNew_Output_Object_Group->setObjectName("actionNew_Output_Object_Group");
        actionDelete_Output_Object = new QAction(MainWindow);
        actionDelete_Output_Object->setObjectName("actionDelete_Output_Object");
        actionUngroup = new QAction(MainWindow);
        actionUngroup->setObjectName("actionUngroup");
        actionMove_item_to_Group = new QAction(MainWindow);
        actionMove_item_to_Group->setObjectName("actionMove_item_to_Group");
        actionMask_Advance_slice_after_copy_operation = new QAction(MainWindow);
        actionMask_Advance_slice_after_copy_operation->setObjectName("actionMask_Advance_slice_after_copy_operation");
        actionMask_Advance_slice_after_copy_operation->setCheckable(true);
        actionMaskGo_back_one_slice_after_copy = new QAction(MainWindow);
        actionMaskGo_back_one_slice_after_copy->setObjectName("actionMaskGo_back_one_slice_after_copy");
        actionMaskGo_back_one_slice_after_copy->setCheckable(true);
        actionInfo = new QAction(MainWindow);
        actionInfo->setObjectName("actionInfo");
        actionInfo->setCheckable(true);
        actionShow_position_slice_selector = new QAction(MainWindow);
        actionShow_position_slice_selector->setObjectName("actionShow_position_slice_selector");
        actionShow_position_slice_selector->setCheckable(true);
        actionChange_slice_spacing = new QAction(MainWindow);
        actionChange_slice_spacing->setObjectName("actionChange_slice_spacing");
        actionSet_slice_position = new QAction(MainWindow);
        actionSet_slice_position->setObjectName("actionSet_slice_position");
        actionDistribute_over_range = new QAction(MainWindow);
        actionDistribute_over_range->setObjectName("actionDistribute_over_range");
        actionChange_downsampling = new QAction(MainWindow);
        actionChange_downsampling->setObjectName("actionChange_downsampling");
        actionChange_downsampling->setEnabled(false);
        actionMeasure_Volumes = new QAction(MainWindow);
        actionMeasure_Volumes->setObjectName("actionMeasure_Volumes");
        actionExport_SPIERSview_and_Launch = new QAction(MainWindow);
        actionExport_SPIERSview_and_Launch->setObjectName("actionExport_SPIERSview_and_Launch");
        actionRefresh = new QAction(MainWindow);
        actionRefresh->setObjectName("actionRefresh");
        actionInterpolate_over_selected_slices = new QAction(MainWindow);
        actionInterpolate_over_selected_slices->setObjectName("actionInterpolate_over_selected_slices");
        action3D_Brush = new QAction(MainWindow);
        action3D_Brush->setObjectName("action3D_Brush");
        action3D_Brush->setCheckable(true);
        actionView_in_SPIERSview = new QAction(MainWindow);
        actionView_in_SPIERSview->setObjectName("actionView_in_SPIERSview");
        actionUse_Old_Exporting_Code = new QAction(MainWindow);
        actionUse_Old_Exporting_Code->setObjectName("actionUse_Old_Exporting_Code");
        actionUse_Old_Exporting_Code->setCheckable(true);
        actionManual = new QAction(MainWindow);
        actionManual->setObjectName("actionManual");
        actionGrey_out_curves_not_no_current_slice = new QAction(MainWindow);
        actionGrey_out_curves_not_no_current_slice->setObjectName("actionGrey_out_curves_not_no_current_slice");
        actionGrey_out_curves_not_no_current_slice->setCheckable(true);
        actionExport_Curves_as_CSV = new QAction(MainWindow);
        actionExport_Curves_as_CSV->setObjectName("actionExport_Curves_as_CSV");
        actionExport_Curves_as_CSV->setVisible(false);
        actionImport_Curves_as_CSV = new QAction(MainWindow);
        actionImport_Curves_as_CSV->setObjectName("actionImport_Curves_as_CSV");
        actionImport_Curves_as_CSV->setVisible(false);
        actionOutput_visible_image_set = new QAction(MainWindow);
        actionOutput_visible_image_set->setObjectName("actionOutput_visible_image_set");
        actionTEST = new QAction(MainWindow);
        actionTEST->setObjectName("actionTEST");
        actionBugIssueFeatureRequest = new QAction(MainWindow);
        actionBugIssueFeatureRequest->setObjectName("actionBugIssueFeatureRequest");
        actionCode_on_GitHub = new QAction(MainWindow);
        actionCode_on_GitHub->setObjectName("actionCode_on_GitHub");
        actionCycle_Bright_Segment_Mask = new QAction(MainWindow);
        actionCycle_Bright_Segment_Mask->setObjectName("actionCycle_Bright_Segment_Mask");
        actionIncrease_Size = new QAction(MainWindow);
        actionIncrease_Size->setObjectName("actionIncrease_Size");
        actionDecrease_Size = new QAction(MainWindow);
        actionDecrease_Size->setObjectName("actionDecrease_Size");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        horizontalLayout_4 = new QHBoxLayout(centralwidget);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName("verticalLayout_5");
        label_21 = new QLabel(centralwidget);
        label_21->setObjectName("label_21");
        label_21->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_21->sizePolicy().hasHeightForWidth());
        label_21->setSizePolicy(sizePolicy1);
        label_21->setMinimumSize(QSize(38, 0));
        label_21->setMaximumSize(QSize(38, 16777215));
        label_21->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_21);

        ZoomSpinBox = new KeysafeSpinBox(centralwidget);
        ZoomSpinBox->setObjectName("ZoomSpinBox");
        ZoomSpinBox->setMinimumSize(QSize(40, 0));
        ZoomSpinBox->setMaximumSize(QSize(40, 16777215));
        QFont font;
        font.setPointSize(7);
        ZoomSpinBox->setFont(font);
        ZoomSpinBox->setFrame(true);
        ZoomSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        ZoomSpinBox->setKeyboardTracking(false);
        ZoomSpinBox->setMinimum(10);
        ZoomSpinBox->setMaximum(10000);
        ZoomSpinBox->setSingleStep(5);
        ZoomSpinBox->setValue(100);

        verticalLayout_5->addWidget(ZoomSpinBox);

        ZoomSlider = new QSlider(centralwidget);
        ZoomSlider->setObjectName("ZoomSlider");
        ZoomSlider->setMinimumSize(QSize(24, 0));
        ZoomSlider->setMaximumSize(QSize(24, 16777215));
        ZoomSlider->setMinimum(0);
        ZoomSlider->setMaximum(1500);
        ZoomSlider->setSingleStep(10);
        ZoomSlider->setPageStep(100);
        ZoomSlider->setValue(500);
        ZoomSlider->setTracking(false);
        ZoomSlider->setOrientation(Qt::Vertical);
        ZoomSlider->setInvertedAppearance(false);
        ZoomSlider->setTickPosition(QSlider::TicksBelow);
        ZoomSlider->setTickInterval(50);

        verticalLayout_5->addWidget(ZoomSlider);


        horizontalLayout_4->addLayout(verticalLayout_5);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        graphicsView = new mainview(centralwidget);
        graphicsView->setObjectName("graphicsView");
        graphicsView->viewport()->setProperty("cursor", QVariant(QCursor(Qt::CrossCursor)));
        graphicsView->setMouseTracking(true);
        graphicsView->setFocusPolicy(Qt::StrongFocus);
        graphicsView->setInteractive(true);
        graphicsView->setSceneRect(QRectF(0, 0, 0, 0));
        graphicsView->setAlignment(Qt::AlignCenter);
        graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
        graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);

        verticalLayout_4->addWidget(graphicsView);

        _2 = new QHBoxLayout();
        _2->setObjectName("_2");
        SliderPos = new QSlider(centralwidget);
        SliderPos->setObjectName("SliderPos");
        SliderPos->setEnabled(false);
        SliderPos->setOrientation(Qt::Horizontal);
        SliderPos->setTickPosition(QSlider::TicksAbove);
        SliderPos->setTickInterval(10);

        _2->addWidget(SliderPos);

        LabelImage = new QLabel(centralwidget);
        LabelImage->setObjectName("LabelImage");

        _2->addWidget(LabelImage);

        PosSpinBox = new KeysafeSpinBox(centralwidget);
        PosSpinBox->setObjectName("PosSpinBox");
        PosSpinBox->setEnabled(false);
        PosSpinBox->setKeyboardTracking(false);

        _2->addWidget(PosSpinBox);


        verticalLayout_4->addLayout(_2);


        horizontalLayout_4->addLayout(verticalLayout_4);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 962, 20));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuOpen_Recent = new QMenu(menuFile);
        menuOpen_Recent->setObjectName("menuOpen_Recent");
        menuMode = new QMenu(menubar);
        menuMode->setObjectName("menuMode");
        menuMode->setEnabled(false);
        menuWindow = new QMenu(menubar);
        menuWindow->setObjectName("menuWindow");
        menuWindow->setEnabled(false);
        menuBrush = new QMenu(menubar);
        menuBrush->setObjectName("menuBrush");
        menuBrush->setEnabled(true);
        menuSegments = new QMenu(menubar);
        menuSegments->setObjectName("menuSegments");
        menuSegments->setEnabled(false);
        menuMasks = new QMenu(menubar);
        menuMasks->setObjectName("menuMasks");
        menuMasks->setEnabled(false);
        menuCurves = new QMenu(menubar);
        menuCurves->setObjectName("menuCurves");
        menuCurves->setEnabled(false);
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        menuUndo = new QMenu(menubar);
        menuUndo->setObjectName("menuUndo");
        menuUndo->setEnabled(false);
        menuOutput = new QMenu(menubar);
        menuOutput->setObjectName("menuOutput");
        menuOutput->setEnabled(false);
        menuSlice_Spacing = new QMenu(menubar);
        menuSlice_Spacing->setObjectName("menuSlice_Spacing");
        menuSlice_Spacing->setEnabled(false);
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        statusbar->setSizeGripEnabled(false);
        MainWindow->setStatusBar(statusbar);
        dockWidget_Main = new QDockWidget(MainWindow);
        dockWidget_Main->setObjectName("dockWidget_Main");
        dockWidget_Main->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(dockWidget_Main->sizePolicy().hasHeightForWidth());
        dockWidget_Main->setSizePolicy(sizePolicy2);
        dockWidget_Main->setMinimumSize(QSize(180, 428));
        dockWidget_Main->setMaximumSize(QSize(180, 100000));
        dockWidget_Main->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidget_Main->setAllowedAreas(Qt::LeftDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        sizePolicy.setHeightForWidth(dockWidgetContents->sizePolicy().hasHeightForWidth());
        dockWidgetContents->setSizePolicy(sizePolicy);
        dockWidgetContents->setMinimumSize(QSize(180, 400));
        dockWidgetContents->setMaximumSize(QSize(180, 16777215));
        vboxLayout = new QVBoxLayout(dockWidgetContents);
        vboxLayout->setSpacing(0);
        vboxLayout->setObjectName("vboxLayout");
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName("vboxLayout1");
        vboxLayout1->setContentsMargins(2, 2, 2, 2);
        groupBox_7 = new QGroupBox(dockWidgetContents);
        groupBox_7->setObjectName("groupBox_7");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox_7->sizePolicy().hasHeightForWidth());
        groupBox_7->setSizePolicy(sizePolicy3);
        groupBox_7->setMinimumSize(QSize(120, 100));
        groupBox_7->setAlignment(Qt::AlignCenter);
        groupBox_7->setFlat(false);
        gridLayout = new QGridLayout(groupBox_7);
        gridLayout->setSpacing(2);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(2, 2, 2, 2);
        label_6 = new QLabel(groupBox_7);
        label_6->setObjectName("label_6");
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_6, 0, 0, 1, 1);

        label_7 = new QLabel(groupBox_7);
        label_7->setObjectName("label_7");
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_7, 0, 1, 1, 1);

        label_8 = new QLabel(groupBox_7);
        label_8->setObjectName("label_8");
        label_8->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_8, 0, 2, 1, 1);

        verticalSlider = new QSlider(groupBox_7);
        verticalSlider->setObjectName("verticalSlider");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(verticalSlider->sizePolicy().hasHeightForWidth());
        verticalSlider->setSizePolicy(sizePolicy4);
        verticalSlider->setLayoutDirection(Qt::RightToLeft);
        verticalSlider->setMinimum(1);
        verticalSlider->setValue(10);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setInvertedAppearance(true);
        verticalSlider->setInvertedControls(false);
        verticalSlider->setTickPosition(QSlider::TicksBothSides);

        gridLayout->addWidget(verticalSlider, 1, 0, 1, 1);

        verticalSlider_2 = new QSlider(groupBox_7);
        verticalSlider_2->setObjectName("verticalSlider_2");
        sizePolicy4.setHeightForWidth(verticalSlider_2->sizePolicy().hasHeightForWidth());
        verticalSlider_2->setSizePolicy(sizePolicy4);
        verticalSlider_2->setLayoutDirection(Qt::RightToLeft);
        verticalSlider_2->setMinimum(1);
        verticalSlider_2->setValue(10);
        verticalSlider_2->setOrientation(Qt::Vertical);
        verticalSlider_2->setInvertedAppearance(true);
        verticalSlider_2->setInvertedControls(false);
        verticalSlider_2->setTickPosition(QSlider::TicksBothSides);

        gridLayout->addWidget(verticalSlider_2, 1, 1, 1, 1);

        verticalSlider_3 = new QSlider(groupBox_7);
        verticalSlider_3->setObjectName("verticalSlider_3");
        sizePolicy4.setHeightForWidth(verticalSlider_3->sizePolicy().hasHeightForWidth());
        verticalSlider_3->setSizePolicy(sizePolicy4);
        verticalSlider_3->setLayoutDirection(Qt::RightToLeft);
        verticalSlider_3->setMinimum(0);
        verticalSlider_3->setMaximum(10);
        verticalSlider_3->setPageStep(1);
        verticalSlider_3->setValue(0);
        verticalSlider_3->setOrientation(Qt::Vertical);
        verticalSlider_3->setInvertedAppearance(true);
        verticalSlider_3->setInvertedControls(false);
        verticalSlider_3->setTickPosition(QSlider::TicksBothSides);

        gridLayout->addWidget(verticalSlider_3, 1, 2, 1, 1);

        SpinUp = new KeysafeSpinBox(groupBox_7);
        SpinUp->setObjectName("SpinUp");
        SpinUp->setEnabled(true);
        SpinUp->setFocusPolicy(Qt::WheelFocus);
        SpinUp->setReadOnly(false);
        SpinUp->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        SpinUp->setKeyboardTracking(false);
        SpinUp->setMinimum(1);
        SpinUp->setValue(10);

        gridLayout->addWidget(SpinUp, 2, 0, 1, 1);

        SpinDown = new KeysafeSpinBox(groupBox_7);
        SpinDown->setObjectName("SpinDown");
        SpinDown->setEnabled(true);
        SpinDown->setFocusPolicy(Qt::WheelFocus);
        SpinDown->setKeyboardTracking(false);
        SpinDown->setMinimum(1);
        SpinDown->setValue(10);

        gridLayout->addWidget(SpinDown, 2, 1, 1, 1);

        SpinSoft = new KeysafeSpinBox(groupBox_7);
        SpinSoft->setObjectName("SpinSoft");
        SpinSoft->setEnabled(true);
        SpinSoft->setFocusPolicy(Qt::WheelFocus);
        SpinSoft->setKeyboardTracking(false);
        SpinSoft->setMinimum(0);
        SpinSoft->setMaximum(10);
        SpinSoft->setValue(0);

        gridLayout->addWidget(SpinSoft, 2, 2, 1, 1);


        vboxLayout1->addWidget(groupBox_7);

        groupBox_5 = new QGroupBox(dockWidgetContents);
        groupBox_5->setObjectName("groupBox_5");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy5);
        groupBox_5->setMinimumSize(QSize(158, 81));
        groupBox_5->setAlignment(Qt::AlignCenter);
        hboxLayout = new QHBoxLayout(groupBox_5);
        hboxLayout->setSpacing(2);
        hboxLayout->setObjectName("hboxLayout");
        hboxLayout->setContentsMargins(2, 2, 2, 2);
        gridLayout1 = new QGridLayout();
        gridLayout1->setObjectName("gridLayout1");
        label = new QLabel(groupBox_5);
        label->setObjectName("label");

        gridLayout1->addWidget(label, 0, 0, 1, 1);

        MaskBoxLeft = new QComboBox(groupBox_5);
        MaskBoxLeft->setObjectName("MaskBoxLeft");
        sizePolicy5.setHeightForWidth(MaskBoxLeft->sizePolicy().hasHeightForWidth());
        MaskBoxLeft->setSizePolicy(sizePolicy5);

        gridLayout1->addWidget(MaskBoxLeft, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox_5);
        label_2->setObjectName("label_2");

        gridLayout1->addWidget(label_2, 1, 0, 1, 1);

        MaskBoxRight = new QComboBox(groupBox_5);
        MaskBoxRight->setObjectName("MaskBoxRight");

        gridLayout1->addWidget(MaskBoxRight, 1, 1, 1, 1);


        hboxLayout->addLayout(gridLayout1);


        vboxLayout1->addWidget(groupBox_5);

        groupBox_6 = new QGroupBox(dockWidgetContents);
        groupBox_6->setObjectName("groupBox_6");
        sizePolicy5.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy5);
        groupBox_6->setMinimumSize(QSize(158, 81));
        groupBox_6->setAlignment(Qt::AlignCenter);
        hboxLayout1 = new QHBoxLayout(groupBox_6);
        hboxLayout1->setSpacing(2);
        hboxLayout1->setObjectName("hboxLayout1");
        hboxLayout1->setContentsMargins(2, 2, 2, 2);
        gridLayout2 = new QGridLayout();
        gridLayout2->setObjectName("gridLayout2");
        label_4 = new QLabel(groupBox_6);
        label_4->setObjectName("label_4");

        gridLayout2->addWidget(label_4, 0, 0, 1, 1);

        label_5 = new QLabel(groupBox_6);
        label_5->setObjectName("label_5");
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy6);

        gridLayout2->addWidget(label_5, 1, 0, 1, 1);


        hboxLayout1->addLayout(gridLayout2);

        gridLayout3 = new QGridLayout();
        gridLayout3->setObjectName("gridLayout3");
        SegBoxLeft = new QComboBox(groupBox_6);
        SegBoxLeft->setObjectName("SegBoxLeft");

        gridLayout3->addWidget(SegBoxLeft, 0, 0, 1, 1);

        SegBoxRight = new QComboBox(groupBox_6);
        SegBoxRight->setObjectName("SegBoxRight");

        gridLayout3->addWidget(SegBoxRight, 1, 0, 1, 1);


        hboxLayout1->addLayout(gridLayout3);


        vboxLayout1->addWidget(groupBox_6);

        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName("groupBox_2");
        sizePolicy5.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy5);
        groupBox_2->setMinimumSize(QSize(158, 68));
        groupBox_2->setAlignment(Qt::AlignCenter);
        vboxLayout2 = new QVBoxLayout(groupBox_2);
        vboxLayout2->setSpacing(2);
        vboxLayout2->setObjectName("vboxLayout2");
        vboxLayout2->setContentsMargins(2, 2, 2, 2);
        MinSlider = new QScrollBar(groupBox_2);
        MinSlider->setObjectName("MinSlider");
        MinSlider->setMaximum(255);
        MinSlider->setPageStep(20);
        MinSlider->setValue(0);
        MinSlider->setOrientation(Qt::Horizontal);

        vboxLayout2->addWidget(MinSlider);

        MaxSlider = new QScrollBar(groupBox_2);
        MaxSlider->setObjectName("MaxSlider");
        MaxSlider->setMaximum(255);
        MaxSlider->setPageStep(20);
        MaxSlider->setValue(255);
        MaxSlider->setOrientation(Qt::Horizontal);

        vboxLayout2->addWidget(MaxSlider);


        vboxLayout1->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName("groupBox_3");
        sizePolicy5.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy5);
        groupBox_3->setMinimumSize(QSize(158, 51));
        groupBox_3->setAlignment(Qt::AlignCenter);
        hboxLayout2 = new QHBoxLayout(groupBox_3);
        hboxLayout2->setSpacing(2);
        hboxLayout2->setObjectName("hboxLayout2");
        hboxLayout2->setContentsMargins(2, 2, 2, 2);
        TransSlider = new QSlider(groupBox_3);
        TransSlider->setObjectName("TransSlider");
        TransSlider->setMinimum(0);
        TransSlider->setMaximum(15);
        TransSlider->setPageStep(4);
        TransSlider->setOrientation(Qt::Horizontal);
        TransSlider->setInvertedAppearance(false);
        TransSlider->setTickPosition(QSlider::TicksAbove);

        hboxLayout2->addWidget(TransSlider);


        vboxLayout1->addWidget(groupBox_3);


        vboxLayout->addLayout(vboxLayout1);

        dockWidget_Main->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget_Main);
        dockWidget_Generate = new QDockWidget(MainWindow);
        dockWidget_Generate->setObjectName("dockWidget_Generate");
        dockWidget_Generate->setEnabled(true);
        sizePolicy2.setHeightForWidth(dockWidget_Generate->sizePolicy().hasHeightForWidth());
        dockWidget_Generate->setSizePolicy(sizePolicy2);
        dockWidget_Generate->setMinimumSize(QSize(350, 228));
        dockWidget_Generate->setMaximumSize(QSize(350, 524287));
        dockWidget_Generate->setFloating(false);
        dockWidget_Generate->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidget_Generate->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName("dockWidgetContents_2");
        QSizePolicy sizePolicy7(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(dockWidgetContents_2->sizePolicy().hasHeightForWidth());
        dockWidgetContents_2->setSizePolicy(sizePolicy7);
        dockWidgetContents_2->setMinimumSize(QSize(350, 200));
        dockWidgetContents_2->setMaximumSize(QSize(350, 16777215));
        vboxLayout3 = new QVBoxLayout(dockWidgetContents_2);
        vboxLayout3->setObjectName("vboxLayout3");
        tabWidget = new QTabWidget(dockWidgetContents_2);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setEnabled(true);
        tabLinear = new QWidget();
        tabLinear->setObjectName("tabLinear");
        hboxLayout3 = new QHBoxLayout(tabLinear);
        hboxLayout3->setObjectName("hboxLayout3");
        hboxLayout3->setSizeConstraint(QLayout::SetDefaultConstraint);
        hboxLayout3->setContentsMargins(2, 2, 2, 2);
        vboxLayout4 = new QVBoxLayout();
        vboxLayout4->setObjectName("vboxLayout4");
        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setObjectName("hboxLayout4");
        vboxLayout5 = new QVBoxLayout();
        vboxLayout5->setObjectName("vboxLayout5");
        LinearRedSpinBox = new KeysafeSpinBox(tabLinear);
        LinearRedSpinBox->setObjectName("LinearRedSpinBox");
        LinearRedSpinBox->setKeyboardTracking(false);
        LinearRedSpinBox->setMaximum(255);
        LinearRedSpinBox->setValue(100);

        vboxLayout5->addWidget(LinearRedSpinBox);


        hboxLayout4->addLayout(vboxLayout5);

        vboxLayout6 = new QVBoxLayout();
        vboxLayout6->setObjectName("vboxLayout6");
        LinearGreenSpinBox = new KeysafeSpinBox(tabLinear);
        LinearGreenSpinBox->setObjectName("LinearGreenSpinBox");
        LinearGreenSpinBox->setKeyboardTracking(false);
        LinearGreenSpinBox->setMaximum(255);
        LinearGreenSpinBox->setValue(100);

        vboxLayout6->addWidget(LinearGreenSpinBox);


        hboxLayout4->addLayout(vboxLayout6);

        vboxLayout7 = new QVBoxLayout();
        vboxLayout7->setObjectName("vboxLayout7");
        LinearBlueSpinBox = new KeysafeSpinBox(tabLinear);
        LinearBlueSpinBox->setObjectName("LinearBlueSpinBox");
        LinearBlueSpinBox->setKeyboardTracking(false);
        LinearBlueSpinBox->setMaximum(255);
        LinearBlueSpinBox->setValue(100);

        vboxLayout7->addWidget(LinearBlueSpinBox);


        hboxLayout4->addLayout(vboxLayout7);


        vboxLayout4->addLayout(hboxLayout4);

        hboxLayout5 = new QHBoxLayout();
        hboxLayout5->setObjectName("hboxLayout5");
        label_23 = new QLabel(tabLinear);
        label_23->setObjectName("label_23");

        hboxLayout5->addWidget(label_23);

        LinearGlobalSlider = new QSlider(tabLinear);
        LinearGlobalSlider->setObjectName("LinearGlobalSlider");
        LinearGlobalSlider->setMinimum(1);
        LinearGlobalSlider->setMaximum(500);
        LinearGlobalSlider->setPageStep(30);
        LinearGlobalSlider->setOrientation(Qt::Horizontal);
        LinearGlobalSlider->setTickPosition(QSlider::TicksBothSides);
        LinearGlobalSlider->setTickInterval(50);

        hboxLayout5->addWidget(LinearGlobalSlider);

        LinearGlobalSpinBox = new KeysafeSpinBox(tabLinear);
        LinearGlobalSpinBox->setObjectName("LinearGlobalSpinBox");
        QSizePolicy sizePolicy8(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(LinearGlobalSpinBox->sizePolicy().hasHeightForWidth());
        LinearGlobalSpinBox->setSizePolicy(sizePolicy8);
        LinearGlobalSpinBox->setKeyboardTracking(false);
        LinearGlobalSpinBox->setMinimum(1);
        LinearGlobalSpinBox->setMaximum(500);

        hboxLayout5->addWidget(LinearGlobalSpinBox);


        vboxLayout4->addLayout(hboxLayout5);


        hboxLayout3->addLayout(vboxLayout4);

        tabWidget->addTab(tabLinear, QString());
        tabML = new QWidget();
        tabML->setObjectName("tabML");
        verticalLayout_11 = new QVBoxLayout(tabML);
        verticalLayout_11->setObjectName("verticalLayout_11");
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName("horizontalLayout_15");
        testML = new QPushButton(tabML);
        testML->setObjectName("testML");

        horizontalLayout_15->addWidget(testML);

        trainML = new QPushButton(tabML);
        trainML->setObjectName("trainML");

        horizontalLayout_15->addWidget(trainML);


        verticalLayout_11->addLayout(horizontalLayout_15);

        tabWidget->addTab(tabML, QString());
        tabRange = new QWidget();
        tabRange->setObjectName("tabRange");
        verticalLayout = new QVBoxLayout(tabRange);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayoutRange = new QVBoxLayout();
        verticalLayoutRange->setObjectName("verticalLayoutRange");

        verticalLayout->addLayout(verticalLayoutRange);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        SpinBoxRangeBase = new KeysafeSpinBox(tabRange);
        SpinBoxRangeBase->setObjectName("SpinBoxRangeBase");
        SpinBoxRangeBase->setKeyboardTracking(false);
        SpinBoxRangeBase->setMaximum(255);

        horizontalLayout_2->addWidget(SpinBoxRangeBase);

        SpinBoxRangeTop = new KeysafeSpinBox(tabRange);
        SpinBoxRangeTop->setObjectName("SpinBoxRangeTop");
        SpinBoxRangeTop->setKeyboardTracking(false);
        SpinBoxRangeTop->setMaximum(255);

        horizontalLayout_2->addWidget(SpinBoxRangeTop);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        CheckBoxRangeSelectedOnly = new QCheckBox(tabRange);
        CheckBoxRangeSelectedOnly->setObjectName("CheckBoxRangeSelectedOnly");

        horizontalLayout->addWidget(CheckBoxRangeSelectedOnly);

        CheckBoxRangeHardFill = new QCheckBox(tabRange);
        CheckBoxRangeHardFill->setObjectName("CheckBoxRangeHardFill");

        horizontalLayout->addWidget(CheckBoxRangeHardFill);


        verticalLayout->addLayout(horizontalLayout);

        tabWidget->addTab(tabRange, QString());
        tabLCE = new QWidget();
        tabLCE->setObjectName("tabLCE");
        gridLayout_2 = new QGridLayout(tabLCE);
        gridLayout_2->setObjectName("gridLayout_2");
        label_30 = new QLabel(tabLCE);
        label_30->setObjectName("label_30");
        label_30->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_30, 0, 0, 1, 1);

        BoostSpinBox = new KeysafeSpinBox(tabLCE);
        BoostSpinBox->setObjectName("BoostSpinBox");
        sizePolicy8.setHeightForWidth(BoostSpinBox->sizePolicy().hasHeightForWidth());
        BoostSpinBox->setSizePolicy(sizePolicy8);
        BoostSpinBox->setKeyboardTracking(false);
        BoostSpinBox->setMinimum(0);
        BoostSpinBox->setMaximum(50);
        BoostSpinBox->setValue(10);

        gridLayout_2->addWidget(BoostSpinBox, 0, 2, 1, 1);

        SliderBoostRadius = new QSlider(tabLCE);
        SliderBoostRadius->setObjectName("SliderBoostRadius");
        SliderBoostRadius->setMinimum(1);
        SliderBoostRadius->setMaximum(100);
        SliderBoostRadius->setValue(5);
        SliderBoostRadius->setOrientation(Qt::Horizontal);
        SliderBoostRadius->setTickPosition(QSlider::TicksBothSides);
        SliderBoostRadius->setTickInterval(10);

        gridLayout_2->addWidget(SliderBoostRadius, 1, 1, 1, 1);

        BoostRadiusSpinBox = new KeysafeSpinBox(tabLCE);
        BoostRadiusSpinBox->setObjectName("BoostRadiusSpinBox");
        sizePolicy8.setHeightForWidth(BoostRadiusSpinBox->sizePolicy().hasHeightForWidth());
        BoostRadiusSpinBox->setSizePolicy(sizePolicy8);
        BoostRadiusSpinBox->setKeyboardTracking(false);
        BoostRadiusSpinBox->setMinimum(1);
        BoostRadiusSpinBox->setMaximum(100);
        BoostRadiusSpinBox->setValue(5);

        gridLayout_2->addWidget(BoostRadiusSpinBox, 1, 2, 1, 1);

        SliderBoost = new QSlider(tabLCE);
        SliderBoost->setObjectName("SliderBoost");
        SliderBoost->setMinimum(0);
        SliderBoost->setMaximum(50);
        SliderBoost->setValue(10);
        SliderBoost->setOrientation(Qt::Horizontal);
        SliderBoost->setTickPosition(QSlider::TicksBothSides);
        SliderBoost->setTickInterval(5);

        gridLayout_2->addWidget(SliderBoost, 0, 1, 1, 1);

        label_29 = new QLabel(tabLCE);
        label_29->setObjectName("label_29");
        label_29->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_29, 1, 0, 1, 1);

        SliderBoostAdjust = new QSlider(tabLCE);
        SliderBoostAdjust->setObjectName("SliderBoostAdjust");
        SliderBoostAdjust->setMinimum(-100);
        SliderBoostAdjust->setMaximum(100);
        SliderBoostAdjust->setValue(0);
        SliderBoostAdjust->setOrientation(Qt::Horizontal);
        SliderBoostAdjust->setTickPosition(QSlider::TicksBothSides);
        SliderBoostAdjust->setTickInterval(10);

        gridLayout_2->addWidget(SliderBoostAdjust, 2, 1, 1, 1);

        label_31 = new QLabel(tabLCE);
        label_31->setObjectName("label_31");
        label_31->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_31, 2, 0, 1, 1);

        BoostAdjustSpinBox = new KeysafeSpinBox(tabLCE);
        BoostAdjustSpinBox->setObjectName("BoostAdjustSpinBox");
        sizePolicy8.setHeightForWidth(BoostAdjustSpinBox->sizePolicy().hasHeightForWidth());
        BoostAdjustSpinBox->setSizePolicy(sizePolicy8);
        BoostAdjustSpinBox->setKeyboardTracking(false);
        BoostAdjustSpinBox->setMinimum(-100);
        BoostAdjustSpinBox->setMaximum(100);
        BoostAdjustSpinBox->setValue(0);

        gridLayout_2->addWidget(BoostAdjustSpinBox, 2, 2, 1, 1);

        tabWidget->addTab(tabLCE, QString());
        tabBeamHardening = new QWidget();
        tabBeamHardening->setObjectName("tabBeamHardening");
        verticalLayout_8 = new QVBoxLayout(tabBeamHardening);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName("verticalLayout_7");
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        ShowCenter = new QCheckBox(tabBeamHardening);
        ShowCenter->setObjectName("ShowCenter");

        horizontalLayout_6->addWidget(ShowCenter);

        SetCentre = new QPushButton(tabBeamHardening);
        SetCentre->setObjectName("SetCentre");

        horizontalLayout_6->addWidget(SetCentre);

        Measure = new QPushButton(tabBeamHardening);
        Measure->setObjectName("Measure");

        horizontalLayout_6->addWidget(Measure);


        verticalLayout_7->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        CentreX = new KeysafeSpinBox(tabBeamHardening);
        CentreX->setObjectName("CentreX");
        CentreX->setKeyboardTracking(false);
        CentreX->setMaximum(9999);

        horizontalLayout_5->addWidget(CentreX);

        CentreY = new KeysafeSpinBox(tabBeamHardening);
        CentreY->setObjectName("CentreY");
        CentreY->setKeyboardTracking(false);
        CentreY->setMaximum(9999);

        horizontalLayout_5->addWidget(CentreY);


        verticalLayout_7->addLayout(horizontalLayout_5);

        labelHardeningData = new QLabel(tabBeamHardening);
        labelHardeningData->setObjectName("labelHardeningData");

        verticalLayout_7->addWidget(labelHardeningData);


        verticalLayout_8->addLayout(verticalLayout_7);

        verticalSpacer_4 = new QSpacerItem(20, 11, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_4);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        label_32 = new QLabel(tabBeamHardening);
        label_32->setObjectName("label_32");
        label_32->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_8->addWidget(label_32);

        SliderHardeningRadius = new QSlider(tabBeamHardening);
        SliderHardeningRadius->setObjectName("SliderHardeningRadius");
        SliderHardeningRadius->setMinimum(0);
        SliderHardeningRadius->setMaximum(2000);
        SliderHardeningRadius->setValue(5);
        SliderHardeningRadius->setOrientation(Qt::Horizontal);
        SliderHardeningRadius->setTickPosition(QSlider::TicksBothSides);
        SliderHardeningRadius->setTickInterval(10);

        horizontalLayout_8->addWidget(SliderHardeningRadius);

        HardeningRadiusSpinBox = new KeysafeSpinBox(tabBeamHardening);
        HardeningRadiusSpinBox->setObjectName("HardeningRadiusSpinBox");
        sizePolicy8.setHeightForWidth(HardeningRadiusSpinBox->sizePolicy().hasHeightForWidth());
        HardeningRadiusSpinBox->setSizePolicy(sizePolicy8);
        HardeningRadiusSpinBox->setKeyboardTracking(false);
        HardeningRadiusSpinBox->setMinimum(0);
        HardeningRadiusSpinBox->setMaximum(2000);
        HardeningRadiusSpinBox->setValue(5);

        horizontalLayout_8->addWidget(HardeningRadiusSpinBox);


        verticalLayout_8->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        label_33 = new QLabel(tabBeamHardening);
        label_33->setObjectName("label_33");
        label_33->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_9->addWidget(label_33);

        SliderBoostAdjust_2 = new QSlider(tabBeamHardening);
        SliderBoostAdjust_2->setObjectName("SliderBoostAdjust_2");
        SliderBoostAdjust_2->setMinimum(-100);
        SliderBoostAdjust_2->setMaximum(100);
        SliderBoostAdjust_2->setValue(0);
        SliderBoostAdjust_2->setOrientation(Qt::Horizontal);
        SliderBoostAdjust_2->setTickPosition(QSlider::TicksBothSides);
        SliderBoostAdjust_2->setTickInterval(10);

        horizontalLayout_9->addWidget(SliderBoostAdjust_2);

        AdjustRadialSpinBox = new KeysafeSpinBox(tabBeamHardening);
        AdjustRadialSpinBox->setObjectName("AdjustRadialSpinBox");
        sizePolicy8.setHeightForWidth(AdjustRadialSpinBox->sizePolicy().hasHeightForWidth());
        AdjustRadialSpinBox->setSizePolicy(sizePolicy8);
        AdjustRadialSpinBox->setKeyboardTracking(false);
        AdjustRadialSpinBox->setMinimum(-100);
        AdjustRadialSpinBox->setMaximum(100);
        AdjustRadialSpinBox->setValue(0);

        horizontalLayout_9->addWidget(AdjustRadialSpinBox);


        verticalLayout_8->addLayout(horizontalLayout_9);

        tabWidget->addTab(tabBeamHardening, QString());
        tabGradients = new QWidget();
        tabGradients->setObjectName("tabGradients");
        verticalLayout_10 = new QVBoxLayout(tabGradients);
        verticalLayout_10->setObjectName("verticalLayout_10");
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName("verticalLayout_9");
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        label_36 = new QLabel(tabGradients);
        label_36->setObjectName("label_36");
        label_36->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_7->addWidget(label_36);

        SliderBoost_4 = new QSlider(tabGradients);
        SliderBoost_4->setObjectName("SliderBoost_4");
        SliderBoost_4->setMinimum(-100);
        SliderBoost_4->setMaximum(100);
        SliderBoost_4->setValue(10);
        SliderBoost_4->setOrientation(Qt::Horizontal);
        SliderBoost_4->setTickPosition(QSlider::TicksBothSides);
        SliderBoost_4->setTickInterval(5);

        horizontalLayout_7->addWidget(SliderBoost_4);

        spinGradientsDistMinEffect = new KeysafeSpinBox(tabGradients);
        spinGradientsDistMinEffect->setObjectName("spinGradientsDistMinEffect");
        sizePolicy8.setHeightForWidth(spinGradientsDistMinEffect->sizePolicy().hasHeightForWidth());
        spinGradientsDistMinEffect->setSizePolicy(sizePolicy8);
        spinGradientsDistMinEffect->setKeyboardTracking(false);
        spinGradientsDistMinEffect->setMinimum(-100);
        spinGradientsDistMinEffect->setMaximum(100);
        spinGradientsDistMinEffect->setValue(0);

        horizontalLayout_7->addWidget(spinGradientsDistMinEffect);


        verticalLayout_9->addLayout(horizontalLayout_7);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        label_37 = new QLabel(tabGradients);
        label_37->setObjectName("label_37");
        label_37->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_10->addWidget(label_37);

        SliderBoost_5 = new QSlider(tabGradients);
        SliderBoost_5->setObjectName("SliderBoost_5");
        SliderBoost_5->setMinimum(-100);
        SliderBoost_5->setMaximum(100);
        SliderBoost_5->setValue(10);
        SliderBoost_5->setOrientation(Qt::Horizontal);
        SliderBoost_5->setTickPosition(QSlider::TicksBothSides);
        SliderBoost_5->setTickInterval(5);

        horizontalLayout_10->addWidget(SliderBoost_5);

        spinGradientsDistMaxEffect = new KeysafeSpinBox(tabGradients);
        spinGradientsDistMaxEffect->setObjectName("spinGradientsDistMaxEffect");
        sizePolicy8.setHeightForWidth(spinGradientsDistMaxEffect->sizePolicy().hasHeightForWidth());
        spinGradientsDistMaxEffect->setSizePolicy(sizePolicy8);
        spinGradientsDistMaxEffect->setKeyboardTracking(false);
        spinGradientsDistMaxEffect->setMinimum(-100);
        spinGradientsDistMaxEffect->setMaximum(100);
        spinGradientsDistMaxEffect->setValue(10);

        horizontalLayout_10->addWidget(spinGradientsDistMaxEffect);


        verticalLayout_9->addLayout(horizontalLayout_10);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_3);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        label_38 = new QLabel(tabGradients);
        label_38->setObjectName("label_38");
        label_38->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_11->addWidget(label_38);

        SliderBoost_6 = new QSlider(tabGradients);
        SliderBoost_6->setObjectName("SliderBoost_6");
        SliderBoost_6->setMinimum(0);
        SliderBoost_6->setMaximum(5000);
        SliderBoost_6->setValue(0);
        SliderBoost_6->setOrientation(Qt::Horizontal);
        SliderBoost_6->setTickPosition(QSlider::NoTicks);
        SliderBoost_6->setTickInterval(5);

        horizontalLayout_11->addWidget(SliderBoost_6);

        spinGradientsDistMin = new KeysafeSpinBox(tabGradients);
        spinGradientsDistMin->setObjectName("spinGradientsDistMin");
        sizePolicy8.setHeightForWidth(spinGradientsDistMin->sizePolicy().hasHeightForWidth());
        spinGradientsDistMin->setSizePolicy(sizePolicy8);
        spinGradientsDistMin->setKeyboardTracking(false);
        spinGradientsDistMin->setMinimum(0);
        spinGradientsDistMin->setMaximum(5000);
        spinGradientsDistMin->setValue(0);

        horizontalLayout_11->addWidget(spinGradientsDistMin);


        verticalLayout_9->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        label_39 = new QLabel(tabGradients);
        label_39->setObjectName("label_39");
        label_39->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_12->addWidget(label_39);

        SliderBoost_7 = new QSlider(tabGradients);
        SliderBoost_7->setObjectName("SliderBoost_7");
        SliderBoost_7->setMinimum(0);
        SliderBoost_7->setMaximum(5000);
        SliderBoost_7->setValue(100);
        SliderBoost_7->setOrientation(Qt::Horizontal);
        SliderBoost_7->setTickPosition(QSlider::NoTicks);
        SliderBoost_7->setTickInterval(5);

        horizontalLayout_12->addWidget(SliderBoost_7);

        spinGradientsDistMax = new KeysafeSpinBox(tabGradients);
        spinGradientsDistMax->setObjectName("spinGradientsDistMax");
        sizePolicy8.setHeightForWidth(spinGradientsDistMax->sizePolicy().hasHeightForWidth());
        spinGradientsDistMax->setSizePolicy(sizePolicy8);
        spinGradientsDistMax->setKeyboardTracking(false);
        spinGradientsDistMax->setMinimum(0);
        spinGradientsDistMax->setMaximum(5000);
        spinGradientsDistMax->setValue(100);

        horizontalLayout_12->addWidget(spinGradientsDistMax);


        verticalLayout_9->addLayout(horizontalLayout_12);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_2);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        label_40 = new QLabel(tabGradients);
        label_40->setObjectName("label_40");
        label_40->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_14->addWidget(label_40);

        SliderBoost_8 = new QSlider(tabGradients);
        SliderBoost_8->setObjectName("SliderBoost_8");
        SliderBoost_8->setMinimum(0);
        SliderBoost_8->setMaximum(20);
        SliderBoost_8->setValue(0);
        SliderBoost_8->setOrientation(Qt::Horizontal);
        SliderBoost_8->setTickPosition(QSlider::NoTicks);
        SliderBoost_8->setTickInterval(5);

        horizontalLayout_14->addWidget(SliderBoost_8);

        spinGradientsPointDensity = new KeysafeSpinBox(tabGradients);
        spinGradientsPointDensity->setObjectName("spinGradientsPointDensity");
        sizePolicy8.setHeightForWidth(spinGradientsPointDensity->sizePolicy().hasHeightForWidth());
        spinGradientsPointDensity->setSizePolicy(sizePolicy8);
        spinGradientsPointDensity->setKeyboardTracking(false);
        spinGradientsPointDensity->setMinimum(0);
        spinGradientsPointDensity->setMaximum(20);
        spinGradientsPointDensity->setValue(3);

        horizontalLayout_14->addWidget(spinGradientsPointDensity);


        verticalLayout_9->addLayout(horizontalLayout_14);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_5);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        lblGradientsWarning = new QLabel(tabGradients);
        lblGradientsWarning->setObjectName("lblGradientsWarning");
        QSizePolicy sizePolicy9(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy9.setHorizontalStretch(0);
        sizePolicy9.setVerticalStretch(0);
        sizePolicy9.setHeightForWidth(lblGradientsWarning->sizePolicy().hasHeightForWidth());
        lblGradientsWarning->setSizePolicy(sizePolicy9);
        lblGradientsWarning->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_13->addWidget(lblGradientsWarning);

        chkGradientsPreview = new QCheckBox(tabGradients);
        chkGradientsPreview->setObjectName("chkGradientsPreview");
        QSizePolicy sizePolicy10(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
        sizePolicy10.setHorizontalStretch(0);
        sizePolicy10.setVerticalStretch(0);
        sizePolicy10.setHeightForWidth(chkGradientsPreview->sizePolicy().hasHeightForWidth());
        chkGradientsPreview->setSizePolicy(sizePolicy10);

        horizontalLayout_13->addWidget(chkGradientsPreview);


        verticalLayout_9->addLayout(horizontalLayout_13);


        verticalLayout_10->addLayout(verticalLayout_9);

        tabWidget->addTab(tabGradients, QString());

        vboxLayout3->addWidget(tabWidget, 0, Qt::AlignVCenter);

        hboxLayout6 = new QHBoxLayout();
        hboxLayout6->setObjectName("hboxLayout6");
        hboxLayout6->setContentsMargins(2, 2, 2, 2);
        GenInvert = new QCheckBox(dockWidgetContents_2);
        GenInvert->setObjectName("GenInvert");

        hboxLayout6->addWidget(GenInvert);

        GenerateAuto = new QCheckBox(dockWidgetContents_2);
        GenerateAuto->setObjectName("GenerateAuto");

        hboxLayout6->addWidget(GenerateAuto);

        GenerateButton = new QPushButton(dockWidgetContents_2);
        GenerateButton->setObjectName("GenerateButton");

        hboxLayout6->addWidget(GenerateButton);


        vboxLayout3->addLayout(hboxLayout6);

        dockWidget_Generate->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, dockWidget_Generate);
        DockOutputSettings = new QDockWidget(MainWindow);
        DockOutputSettings->setObjectName("DockOutputSettings");
        DockOutputSettings->setEnabled(true);
        sizePolicy2.setHeightForWidth(DockOutputSettings->sizePolicy().hasHeightForWidth());
        DockOutputSettings->setSizePolicy(sizePolicy2);
        DockOutputSettings->setMinimumSize(QSize(350, 253));
        DockOutputSettings->setMaximumSize(QSize(350, 524287));
        DockOutputSettings->setFloating(false);
        DockOutputSettings->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        DockOutputSettings->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_8 = new QWidget();
        dockWidgetContents_8->setObjectName("dockWidgetContents_8");
        sizePolicy7.setHeightForWidth(dockWidgetContents_8->sizePolicy().hasHeightForWidth());
        dockWidgetContents_8->setSizePolicy(sizePolicy7);
        dockWidgetContents_8->setMinimumSize(QSize(350, 225));
        dockWidgetContents_8->setMaximumSize(QSize(350, 16777215));
        vboxLayout8 = new QVBoxLayout(dockWidgetContents_8);
        vboxLayout8->setObjectName("vboxLayout8");
        OutputTabs = new QTabWidget(dockWidgetContents_8);
        OutputTabs->setObjectName("OutputTabs");
        sizePolicy.setHeightForWidth(OutputTabs->sizePolicy().hasHeightForWidth());
        OutputTabs->setSizePolicy(sizePolicy);
        OutputTabs->setMinimumSize(QSize(0, 0));
        OutputTabs->setMaximumSize(QSize(16777215, 16777215));
        OutputTabs->setTabShape(QTabWidget::Rounded);
        tabSettings = new QWidget();
        tabSettings->setObjectName("tabSettings");
        hboxLayout7 = new QHBoxLayout(tabSettings);
        hboxLayout7->setObjectName("hboxLayout7");
        gridLayout4 = new QGridLayout();
        gridLayout4->setObjectName("gridLayout4");
        label_12 = new QLabel(tabSettings);
        label_12->setObjectName("label_12");
        label_12->setLayoutDirection(Qt::LeftToRight);
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_12, 4, 0, 1, 1);

        CheckMirror = new QCheckBox(tabSettings);
        CheckMirror->setObjectName("CheckMirror");
        CheckMirror->setLayoutDirection(Qt::LeftToRight);

        gridLayout4->addWidget(CheckMirror, 4, 1, 1, 1);

        label_11 = new QLabel(tabSettings);
        label_11->setObjectName("label_11");
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_11, 2, 0, 1, 1);

        label_16 = new QLabel(tabSettings);
        label_16->setObjectName("label_16");
        label_16->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_16, 6, 0, 1, 1);

        label_9 = new QLabel(tabSettings);
        label_9->setObjectName("label_9");
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_9, 1, 0, 1, 1);

        label_10 = new QLabel(tabSettings);
        label_10->setObjectName("label_10");
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_10, 3, 0, 1, 1);

        DownsampleXY = new KeysafeSpinBox(tabSettings);
        DownsampleXY->setObjectName("DownsampleXY");
        sizePolicy8.setHeightForWidth(DownsampleXY->sizePolicy().hasHeightForWidth());
        DownsampleXY->setSizePolicy(sizePolicy8);
        DownsampleXY->setKeyboardTracking(false);
        DownsampleXY->setMinimum(1);
        DownsampleXY->setMaximum(10);

        gridLayout4->addWidget(DownsampleXY, 8, 1, 1, 1);

        label_17 = new QLabel(tabSettings);
        label_17->setObjectName("label_17");
        label_17->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_17, 5, 0, 1, 1);

        label_14 = new QLabel(tabSettings);
        label_14->setObjectName("label_14");
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_14, 9, 0, 1, 1);

        DownsampleZ = new KeysafeSpinBox(tabSettings);
        DownsampleZ->setObjectName("DownsampleZ");
        sizePolicy8.setHeightForWidth(DownsampleZ->sizePolicy().hasHeightForWidth());
        DownsampleZ->setSizePolicy(sizePolicy8);
        DownsampleZ->setKeyboardTracking(false);
        DownsampleZ->setMinimum(1);
        DownsampleZ->setMaximum(10);

        gridLayout4->addWidget(DownsampleZ, 9, 1, 1, 1);

        label_15 = new QLabel(tabSettings);
        label_15->setObjectName("label_15");
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_15, 7, 0, 1, 1);

        label_13 = new QLabel(tabSettings);
        label_13->setObjectName("label_13");
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_13, 8, 0, 1, 1);

        Pixels_Per_MM = new QDoubleSpinBox(tabSettings);
        Pixels_Per_MM->setObjectName("Pixels_Per_MM");
        sizePolicy8.setHeightForWidth(Pixels_Per_MM->sizePolicy().hasHeightForWidth());
        Pixels_Per_MM->setSizePolicy(sizePolicy8);
        Pixels_Per_MM->setKeyboardTracking(false);
        Pixels_Per_MM->setDecimals(6);
        Pixels_Per_MM->setMaximum(1000000.000000000000000);

        gridLayout4->addWidget(Pixels_Per_MM, 0, 1, 1, 1);

        Slices_Per_MM = new QDoubleSpinBox(tabSettings);
        Slices_Per_MM->setObjectName("Slices_Per_MM");
        sizePolicy8.setHeightForWidth(Slices_Per_MM->sizePolicy().hasHeightForWidth());
        Slices_Per_MM->setSizePolicy(sizePolicy8);
        Slices_Per_MM->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        Slices_Per_MM->setKeyboardTracking(false);
        Slices_Per_MM->setDecimals(6);
        Slices_Per_MM->setMaximum(1000000.000000000000000);

        gridLayout4->addWidget(Slices_Per_MM, 1, 1, 1, 1);

        Edge_Left_MM = new QDoubleSpinBox(tabSettings);
        Edge_Left_MM->setObjectName("Edge_Left_MM");
        sizePolicy8.setHeightForWidth(Edge_Left_MM->sizePolicy().hasHeightForWidth());
        Edge_Left_MM->setSizePolicy(sizePolicy8);
        Edge_Left_MM->setKeyboardTracking(false);
        Edge_Left_MM->setDecimals(6);
        Edge_Left_MM->setMinimum(-10000.000000000000000);
        Edge_Left_MM->setMaximum(10000.000000000000000);

        gridLayout4->addWidget(Edge_Left_MM, 2, 1, 1, 1);

        Edge_Down_MM = new QDoubleSpinBox(tabSettings);
        Edge_Down_MM->setObjectName("Edge_Down_MM");
        sizePolicy8.setHeightForWidth(Edge_Down_MM->sizePolicy().hasHeightForWidth());
        Edge_Down_MM->setSizePolicy(sizePolicy8);
        Edge_Down_MM->setKeyboardTracking(false);
        Edge_Down_MM->setDecimals(6);
        Edge_Down_MM->setMinimum(-10000.000000000000000);
        Edge_Down_MM->setMaximum(10000.000000000000000);

        gridLayout4->addWidget(Edge_Down_MM, 3, 1, 1, 1);

        PixelSensitivity = new KeysafeSpinBox(tabSettings);
        PixelSensitivity->setObjectName("PixelSensitivity");
        sizePolicy8.setHeightForWidth(PixelSensitivity->sizePolicy().hasHeightForWidth());
        PixelSensitivity->setSizePolicy(sizePolicy8);
        PixelSensitivity->setKeyboardTracking(false);
        PixelSensitivity->setMinimum(1);
        PixelSensitivity->setMaximum(10000);

        gridLayout4->addWidget(PixelSensitivity, 7, 1, 1, 1);

        label_3 = new QLabel(tabSettings);
        label_3->setObjectName("label_3");
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout4->addWidget(label_3, 0, 0, 1, 1);

        FirstFile = new KeysafeSpinBox(tabSettings);
        FirstFile->setObjectName("FirstFile");
        sizePolicy8.setHeightForWidth(FirstFile->sizePolicy().hasHeightForWidth());
        FirstFile->setSizePolicy(sizePolicy8);
        FirstFile->setKeyboardTracking(false);
        FirstFile->setMinimum(1);

        gridLayout4->addWidget(FirstFile, 5, 1, 1, 1);

        LastFile = new KeysafeSpinBox(tabSettings);
        LastFile->setObjectName("LastFile");
        sizePolicy8.setHeightForWidth(LastFile->sizePolicy().hasHeightForWidth());
        LastFile->setSizePolicy(sizePolicy8);
        LastFile->setKeyboardTracking(false);
        LastFile->setMinimum(1);

        gridLayout4->addWidget(LastFile, 6, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout4->addItem(verticalSpacer, 10, 0, 1, 1);

        gridLayout4->setColumnStretch(1, 1);

        hboxLayout7->addLayout(gridLayout4);

        OutputTabs->addTab(tabSettings, QString());
        tabObjects = new QWidget();
        tabObjects->setObjectName("tabObjects");
        vboxLayout9 = new QVBoxLayout(tabObjects);
        vboxLayout9->setObjectName("vboxLayout9");
        OOTreeWidget = new QTreeWidget(tabObjects);
        OOTreeWidget->headerItem()->setText(4, QString());
        OOTreeWidget->setObjectName("OOTreeWidget");
        OOTreeWidget->setMouseTracking(true);
        OOTreeWidget->setAcceptDrops(true);
        OOTreeWidget->setDragEnabled(true);
        OOTreeWidget->setDragDropMode(QAbstractItemView::InternalMove);
        OOTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
        OOTreeWidget->setAnimated(false);

        vboxLayout9->addWidget(OOTreeWidget);

        hboxLayout8 = new QHBoxLayout();
        hboxLayout8->setObjectName("hboxLayout8");
        label_18 = new QLabel(tabObjects);
        label_18->setObjectName("label_18");
        label_18->setAlignment(Qt::AlignCenter);

        hboxLayout8->addWidget(label_18);

        OOResample = new KeysafeSpinBox(tabObjects);
        OOResample->setObjectName("OOResample");
        sizePolicy8.setHeightForWidth(OOResample->sizePolicy().hasHeightForWidth());
        OOResample->setSizePolicy(sizePolicy8);
        OOResample->setAlignment(Qt::AlignCenter);
        OOResample->setKeyboardTracking(false);
        OOResample->setMinimum(1);
        OOResample->setMaximum(100);
        OOResample->setValue(100);

        hboxLayout8->addWidget(OOResample);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout8->addItem(spacerItem);

        DirectCurves = new QCheckBox(tabObjects);
        DirectCurves->setObjectName("DirectCurves");

        hboxLayout8->addWidget(DirectCurves);

        spacerItem1 = new QSpacerItem(16, 23, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout8->addItem(spacerItem1);

        label_19 = new QLabel(tabObjects);
        label_19->setObjectName("label_19");
        label_19->setAlignment(Qt::AlignCenter);

        hboxLayout8->addWidget(label_19);

        NextKey = new QComboBox(tabObjects);
        NextKey->setObjectName("NextKey");
        sizePolicy8.setHeightForWidth(NextKey->sizePolicy().hasHeightForWidth());
        NextKey->setSizePolicy(sizePolicy8);
        NextKey->setMinimumSize(QSize(40, 0));
        NextKey->setMaximumSize(QSize(40, 16777215));

        hboxLayout8->addWidget(NextKey);


        vboxLayout9->addLayout(hboxLayout8);

        hboxLayout9 = new QHBoxLayout();
        hboxLayout9->setObjectName("hboxLayout9");
        OONew = new QPushButton(tabObjects);
        OONew->setObjectName("OONew");
        OONew->setMinimumSize(QSize(50, 16));

        hboxLayout9->addWidget(OONew);

        OOGroup = new QPushButton(tabObjects);
        OOGroup->setObjectName("OOGroup");

        hboxLayout9->addWidget(OOGroup);

        OODelete = new QPushButton(tabObjects);
        OODelete->setObjectName("OODelete");
        OODelete->setMinimumSize(QSize(50, 16));

        hboxLayout9->addWidget(OODelete);

        OOUp = new QPushButton(tabObjects);
        OOUp->setObjectName("OOUp");
        OOUp->setMinimumSize(QSize(50, 16));

        hboxLayout9->addWidget(OOUp);

        OODown = new QPushButton(tabObjects);
        OODown->setObjectName("OODown");
        OODown->setMinimumSize(QSize(50, 16));

        hboxLayout9->addWidget(OODown);


        vboxLayout9->addLayout(hboxLayout9);

        OutputTabs->addTab(tabObjects, QString());

        vboxLayout8->addWidget(OutputTabs);

        DockOutputSettings->setWidget(dockWidgetContents_8);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, DockOutputSettings);
        DockHist = new QDockWidget(MainWindow);
        DockHist->setObjectName("DockHist");
        sizePolicy2.setHeightForWidth(DockHist->sizePolicy().hasHeightForWidth());
        DockHist->setSizePolicy(sizePolicy2);
        DockHist->setMinimumSize(QSize(350, 128));
        DockHist->setMaximumSize(QSize(350, 524287));
        DockHist->setFloating(false);
        DockHist->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        DockHist->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_7 = new QWidget();
        dockWidgetContents_7->setObjectName("dockWidgetContents_7");
        sizePolicy7.setHeightForWidth(dockWidgetContents_7->sizePolicy().hasHeightForWidth());
        dockWidgetContents_7->setSizePolicy(sizePolicy7);
        dockWidgetContents_7->setMinimumSize(QSize(350, 100));
        dockWidgetContents_7->setMaximumSize(QSize(350, 16777215));
        DockHist->setWidget(dockWidgetContents_7);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, DockHist);
        DockSegmentsSettings = new QDockWidget(MainWindow);
        DockSegmentsSettings->setObjectName("DockSegmentsSettings");
        DockSegmentsSettings->setEnabled(true);
        sizePolicy2.setHeightForWidth(DockSegmentsSettings->sizePolicy().hasHeightForWidth());
        DockSegmentsSettings->setSizePolicy(sizePolicy2);
        DockSegmentsSettings->setMinimumSize(QSize(350, 128));
        DockSegmentsSettings->setMaximumSize(QSize(350, 524287));
        DockSegmentsSettings->setFloating(false);
        DockSegmentsSettings->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        DockSegmentsSettings->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_6 = new QWidget();
        dockWidgetContents_6->setObjectName("dockWidgetContents_6");
        sizePolicy7.setHeightForWidth(dockWidgetContents_6->sizePolicy().hasHeightForWidth());
        dockWidgetContents_6->setSizePolicy(sizePolicy7);
        dockWidgetContents_6->setMinimumSize(QSize(350, 100));
        dockWidgetContents_6->setMaximumSize(QSize(350, 16777215));
        vboxLayout10 = new QVBoxLayout(dockWidgetContents_6);
        vboxLayout10->setObjectName("vboxLayout10");
        SegmentsTreeWidget = new QTreeWidget(dockWidgetContents_6);
        SegmentsTreeWidget->headerItem()->setText(0, QString());
        SegmentsTreeWidget->headerItem()->setText(3, QString());
        SegmentsTreeWidget->headerItem()->setText(4, QString());
        SegmentsTreeWidget->headerItem()->setText(5, QString());
        SegmentsTreeWidget->setObjectName("SegmentsTreeWidget");
        SegmentsTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        SegmentsTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
        SegmentsTreeWidget->setTextElideMode(Qt::ElideMiddle);
        SegmentsTreeWidget->setRootIsDecorated(false);
        SegmentsTreeWidget->setUniformRowHeights(true);

        vboxLayout10->addWidget(SegmentsTreeWidget);

        hboxLayout10 = new QHBoxLayout();
        hboxLayout10->setObjectName("hboxLayout10");
        SegmentAdd = new QPushButton(dockWidgetContents_6);
        SegmentAdd->setObjectName("SegmentAdd");
        SegmentAdd->setMinimumSize(QSize(50, 16));

        hboxLayout10->addWidget(SegmentAdd);

        SegmentDelete = new QPushButton(dockWidgetContents_6);
        SegmentDelete->setObjectName("SegmentDelete");
        SegmentDelete->setMinimumSize(QSize(50, 16));

        hboxLayout10->addWidget(SegmentDelete);

        SegmentMoveUp = new QPushButton(dockWidgetContents_6);
        SegmentMoveUp->setObjectName("SegmentMoveUp");
        SegmentMoveUp->setMinimumSize(QSize(50, 16));

        hboxLayout10->addWidget(SegmentMoveUp);

        SegmentMoveDown = new QPushButton(dockWidgetContents_6);
        SegmentMoveDown->setObjectName("SegmentMoveDown");
        SegmentMoveDown->setMinimumSize(QSize(50, 16));

        hboxLayout10->addWidget(SegmentMoveDown);


        vboxLayout10->addLayout(hboxLayout10);

        DockSegmentsSettings->setWidget(dockWidgetContents_6);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, DockSegmentsSettings);
        DockInfo = new QDockWidget(MainWindow);
        DockInfo->setObjectName("DockInfo");
        sizePolicy2.setHeightForWidth(DockInfo->sizePolicy().hasHeightForWidth());
        DockInfo->setSizePolicy(sizePolicy2);
        DockInfo->setMinimumSize(QSize(350, 128));
        DockInfo->setMaximumSize(QSize(350, 524287));
        DockInfo->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        DockInfo->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_9 = new QWidget();
        dockWidgetContents_9->setObjectName("dockWidgetContents_9");
        sizePolicy7.setHeightForWidth(dockWidgetContents_9->sizePolicy().hasHeightForWidth());
        dockWidgetContents_9->setSizePolicy(sizePolicy7);
        dockWidgetContents_9->setMinimumSize(QSize(350, 100));
        dockWidgetContents_9->setMaximumSize(QSize(350, 16777215));
        verticalLayout_3 = new QVBoxLayout(dockWidgetContents_9);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_20 = new QLabel(dockWidgetContents_9);
        label_20->setObjectName("label_20");
        label_20->setScaledContents(true);
        label_20->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);

        horizontalLayout_3->addWidget(label_20);

        InfoLabel = new QLabel(dockWidgetContents_9);
        InfoLabel->setObjectName("InfoLabel");
        InfoLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        horizontalLayout_3->addWidget(InfoLabel);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout_3->addLayout(verticalLayout_2);

        plainTextEdit = new QPlainTextEdit(dockWidgetContents_9);
        plainTextEdit->setObjectName("plainTextEdit");

        verticalLayout_3->addWidget(plainTextEdit);

        DockInfo->setWidget(dockWidgetContents_9);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, DockInfo);
        DockMasksSettings = new QDockWidget(MainWindow);
        DockMasksSettings->setObjectName("DockMasksSettings");
        DockMasksSettings->setEnabled(true);
        sizePolicy2.setHeightForWidth(DockMasksSettings->sizePolicy().hasHeightForWidth());
        DockMasksSettings->setSizePolicy(sizePolicy2);
        DockMasksSettings->setMinimumSize(QSize(350, 128));
        DockMasksSettings->setMaximumSize(QSize(350, 524287));
        DockMasksSettings->setFloating(true);
        DockMasksSettings->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        DockMasksSettings->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_5 = new QWidget();
        dockWidgetContents_5->setObjectName("dockWidgetContents_5");
        sizePolicy7.setHeightForWidth(dockWidgetContents_5->sizePolicy().hasHeightForWidth());
        dockWidgetContents_5->setSizePolicy(sizePolicy7);
        dockWidgetContents_5->setMinimumSize(QSize(350, 100));
        dockWidgetContents_5->setMaximumSize(QSize(350, 16777215));
        vboxLayout11 = new QVBoxLayout(dockWidgetContents_5);
        vboxLayout11->setObjectName("vboxLayout11");
        MasksTreeWidget = new QTreeWidget(dockWidgetContents_5);
        MasksTreeWidget->setObjectName("MasksTreeWidget");
        MasksTreeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        MasksTreeWidget->setAlternatingRowColors(false);
        MasksTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
        MasksTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        MasksTreeWidget->setTextElideMode(Qt::ElideMiddle);
        MasksTreeWidget->setRootIsDecorated(false);
        MasksTreeWidget->setUniformRowHeights(true);
        MasksTreeWidget->setColumnCount(6);
        MasksTreeWidget->header()->setMinimumSectionSize(28);

        vboxLayout11->addWidget(MasksTreeWidget);

        hboxLayout11 = new QHBoxLayout();
        hboxLayout11->setObjectName("hboxLayout11");
        MaskAdd = new QPushButton(dockWidgetContents_5);
        MaskAdd->setObjectName("MaskAdd");
        MaskAdd->setMinimumSize(QSize(50, 16));

        hboxLayout11->addWidget(MaskAdd);

        MaskDelete = new QPushButton(dockWidgetContents_5);
        MaskDelete->setObjectName("MaskDelete");
        MaskDelete->setMinimumSize(QSize(50, 16));

        hboxLayout11->addWidget(MaskDelete);

        MaskMoveUp = new QPushButton(dockWidgetContents_5);
        MaskMoveUp->setObjectName("MaskMoveUp");
        MaskMoveUp->setMinimumSize(QSize(50, 16));

        hboxLayout11->addWidget(MaskMoveUp);

        MaskMoveDown = new QPushButton(dockWidgetContents_5);
        MaskMoveDown->setObjectName("MaskMoveDown");
        MaskMoveDown->setMinimumSize(QSize(50, 16));

        hboxLayout11->addWidget(MaskMoveDown);


        vboxLayout11->addLayout(hboxLayout11);

        DockMasksSettings->setWidget(dockWidgetContents_5);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, DockMasksSettings);
        DockCurvesSettings = new QDockWidget(MainWindow);
        DockCurvesSettings->setObjectName("DockCurvesSettings");
        DockCurvesSettings->setEnabled(true);
        sizePolicy2.setHeightForWidth(DockCurvesSettings->sizePolicy().hasHeightForWidth());
        DockCurvesSettings->setSizePolicy(sizePolicy2);
        DockCurvesSettings->setMinimumSize(QSize(350, 128));
        DockCurvesSettings->setMaximumSize(QSize(350, 524287));
        DockCurvesSettings->setFloating(true);
        DockCurvesSettings->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        DockCurvesSettings->setAllowedAreas(Qt::RightDockWidgetArea);
        dockWidgetContents_4 = new QWidget();
        dockWidgetContents_4->setObjectName("dockWidgetContents_4");
        sizePolicy7.setHeightForWidth(dockWidgetContents_4->sizePolicy().hasHeightForWidth());
        dockWidgetContents_4->setSizePolicy(sizePolicy7);
        dockWidgetContents_4->setMinimumSize(QSize(350, 100));
        dockWidgetContents_4->setMaximumSize(QSize(350, 16777215));
        vboxLayout12 = new QVBoxLayout(dockWidgetContents_4);
        vboxLayout12->setObjectName("vboxLayout12");
        CurvesTreeWidget = new QTreeWidget(dockWidgetContents_4);
        CurvesTreeWidget->setObjectName("CurvesTreeWidget");
        CurvesTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
        CurvesTreeWidget->setTextElideMode(Qt::ElideMiddle);
        CurvesTreeWidget->setRootIsDecorated(false);
        CurvesTreeWidget->setUniformRowHeights(true);

        vboxLayout12->addWidget(CurvesTreeWidget);

        hboxLayout12 = new QHBoxLayout();
        hboxLayout12->setObjectName("hboxLayout12");
        Curve_Add = new QPushButton(dockWidgetContents_4);
        Curve_Add->setObjectName("Curve_Add");
        Curve_Add->setMinimumSize(QSize(50, 16));

        hboxLayout12->addWidget(Curve_Add);

        CurveDelete = new QPushButton(dockWidgetContents_4);
        CurveDelete->setObjectName("CurveDelete");
        CurveDelete->setMinimumSize(QSize(50, 16));

        hboxLayout12->addWidget(CurveDelete);

        CurveMoveUp = new QPushButton(dockWidgetContents_4);
        CurveMoveUp->setObjectName("CurveMoveUp");
        CurveMoveUp->setMinimumSize(QSize(50, 16));

        hboxLayout12->addWidget(CurveMoveUp);

        CurveMoveDown = new QPushButton(dockWidgetContents_4);
        CurveMoveDown->setObjectName("CurveMoveDown");
        CurveMoveDown->setMinimumSize(QSize(50, 16));

        hboxLayout12->addWidget(CurveMoveDown);


        vboxLayout12->addLayout(hboxLayout12);

        DockCurvesSettings->setWidget(dockWidgetContents_4);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, DockCurvesSettings);
        SliceSelector = new QDockWidget(MainWindow);
        SliceSelector->setObjectName("SliceSelector");
        SliceSelector->setEnabled(true);
        sizePolicy2.setHeightForWidth(SliceSelector->sizePolicy().hasHeightForWidth());
        SliceSelector->setSizePolicy(sizePolicy2);
        SliceSelector->setMinimumSize(QSize(180, 68));
        SliceSelector->setMaximumSize(QSize(180, 100000));
        SliceSelector->setFloating(false);
        SliceSelector->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        SliceSelector->setAllowedAreas(Qt::LeftDockWidgetArea);
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName("dockWidgetContents_3");
        sizePolicy7.setHeightForWidth(dockWidgetContents_3->sizePolicy().hasHeightForWidth());
        dockWidgetContents_3->setSizePolicy(sizePolicy7);
        dockWidgetContents_3->setMinimumSize(QSize(180, 40));
        dockWidgetContents_3->setMaximumSize(QSize(180, 16777215));
        verticalLayout_6 = new QVBoxLayout(dockWidgetContents_3);
        verticalLayout_6->setObjectName("verticalLayout_6");
        SliceSelectorList = new QListWidget(dockWidgetContents_3);
        SliceSelectorList->setObjectName("SliceSelectorList");
        sizePolicy.setHeightForWidth(SliceSelectorList->sizePolicy().hasHeightForWidth());
        SliceSelectorList->setSizePolicy(sizePolicy);
        SliceSelectorList->setSelectionMode(QAbstractItemView::ExtendedSelection);

        verticalLayout_6->addWidget(SliceSelectorList);

        SliceSelectAll = new QPushButton(dockWidgetContents_3);
        SliceSelectAll->setObjectName("SliceSelectAll");

        verticalLayout_6->addWidget(SliceSelectAll);

        SliceSelector->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(Qt::LeftDockWidgetArea, SliceSelector);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        toolBar->setEnabled(true);
        toolBar->setOrientation(Qt::Horizontal);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuUndo->menuAction());
        menubar->addAction(menuMode->menuAction());
        menubar->addAction(menuBrush->menuAction());
        menubar->addAction(menuMasks->menuAction());
        menubar->addAction(menuSegments->menuAction());
        menubar->addAction(menuCurves->menuAction());
        menubar->addAction(menuSlice_Spacing->menuAction());
        menubar->addAction(menuOutput->menuAction());
        menubar->addAction(menuWindow->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionImport);
        menuFile->addAction(actionOpen);
        menuFile->addAction(menuOpen_Recent->menuAction());
        menuFile->addAction(actionSave_As);
        menuFile->addAction(actionSave_Settings);
        menuFile->addSeparator();
        menuFile->addAction(actionChange_downsampling);
        menuFile->addSeparator();
        menuFile->addAction(actionSettings);
        menuFile->addSeparator();
        menuFile->addAction(actionRefresh);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuFile->addSeparator();
        menuOpen_Recent->addSeparator();
        menuOpen_Recent->addAction(actionMore);
        menuMode->addAction(actionBright);
        menuMode->addAction(actionSegment);
        menuMode->addAction(actionRecalc);
        menuMode->addAction(actionCurve);
        menuMode->addAction(actionLock);
        menuMode->addAction(actionMask);
        menuMode->addAction(actionCycle_Bright_Segment_Mask);
        menuMode->addSeparator();
        menuMode->addAction(actionToggle_Source);
        menuMode->addAction(actionThreshold);
        menuMode->addSeparator();
        menuMode->addAction(actionShowMasks);
        menuMode->addAction(actionShowSegs);
        menuMode->addSeparator();
        menuMode->addAction(actionInvert_locking_for_this_slice);
        menuMode->addAction(actionHistogram_shows_selected);
        menuWindow->addAction(actionMain_Toolbox);
        menuWindow->addAction(actionSlice_Selector);
        menuWindow->addSeparator();
        menuWindow->addAction(actionGeneration);
        menuWindow->addAction(actionMasks);
        menuWindow->addAction(actionSegments);
        menuWindow->addAction(actionCurves);
        menuWindow->addAction(actionOutput);
        menuWindow->addAction(actionHistorgram);
        menuWindow->addAction(actionInfo);
        menuBrush->addAction(action1_x_1);
        menuBrush->addAction(action2_x_2);
        menuBrush->addAction(action3_x_3);
        menuBrush->addAction(action4_x_4);
        menuBrush->addAction(action5_x_5);
        menuBrush->addAction(action8_x_8);
        menuBrush->addAction(action15_x_15);
        menuBrush->addAction(action30_x_30);
        menuBrush->addAction(action50_x_50);
        menuBrush->addAction(action200_x_200);
        menuBrush->addSeparator();
        menuBrush->addAction(actionIncrease_Size);
        menuBrush->addAction(actionDecrease_Size);
        menuBrush->addSeparator();
        menuBrush->addAction(actionSquare);
        menuBrush->addAction(action3D_Brush);
        menuSegments->addAction(actionCreate_new_segment);
        menuSegments->addAction(actionDelete_selected_segments);
        menuSegments->addAction(actionSwap_Segments);
        menuSegments->addSeparator();
        menuSegments->addAction(actionCopy_segment_from_next_slice);
        menuSegments->addAction(actionCopy_segment_from_previous_slice);
        menuSegments->addSeparator();
        menuSegments->addAction(actionLock_selected_segments);
        menuSegments->addAction(actionUnlock_selected_segments);
        menuSegments->addAction(actionActivate_selected_segments);
        menuSegments->addAction(actionDeactivate_selected_segments);
        menuSegments->addSeparator();
        menuSegments->addAction(actionDistribute_over_range);
        menuMasks->addAction(actionNew_mask);
        menuMasks->addAction(actionDelete_selected_mask_s);
        menuMasks->addSeparator();
        menuMasks->addAction(actionMaskCopy_selected_from_Previous);
        menuMasks->addAction(actionMaskCopy_all_from_previous);
        menuMasks->addAction(actionMaskCopy_selected_from_next);
        menuMasks->addAction(actionMaskCopy_all_from_next);
        menuMasks->addAction(actionMask_Advance_slice_after_copy_operation);
        menuMasks->addAction(actionMaskGo_back_one_slice_after_copy);
        menuMasks->addSeparator();
        menuMasks->addAction(actionHidden_masks_locked_for_generation);
        menuMasks->addAction(actionSegment_brush_applies_masks);
        menuMasks->addSeparator();
        menuMasks->addAction(actionLock_Selected_Masks);
        menuMasks->addAction(actionUnlock_Selected_Masks);
        menuMasks->addAction(actionShow_Selected_Masks);
        menuMasks->addAction(actionUnShow_Selected_Masks);
        menuMasks->addSeparator();
        menuMasks->addAction(actionCreate_mask_from_curve);
        menuCurves->addAction(actionCopyCurvesFromPrevious);
        menuCurves->addAction(actionCopy_from_current_slice_to_selected);
        menuCurves->addAction(actionCopyCurvesFromNext);
        menuCurves->addAction(actionCopyCurvesToCurrent);
        menuCurves->addSeparator();
        menuCurves->addAction(actionCurve_markers_as_crosses);
        menuCurves->addAction(actionLock_curve_shape);
        menuCurves->addAction(actionResize_keeping_curve_shape);
        menuCurves->addSeparator();
        menuCurves->addAction(actionAdd_new_point);
        menuCurves->addAction(actionRemove_node_under_cursor);
        menuCurves->addAction(actionRemove_selected_curves_from_selected_slices);
        menuCurves->addSeparator();
        menuCurves->addAction(actionInterpolate_over_selected_slices);
        menuCurves->addSeparator();
        menuCurves->addAction(actionGrey_out_curves_not_no_current_slice);
        menuCurves->addSeparator();
        menuCurves->addAction(actionExport_Curves_as_CSV);
        menuCurves->addAction(actionImport_Curves_as_CSV);
        menuHelp->addAction(actionManual);
        menuHelp->addAction(actionCode_on_GitHub);
        menuHelp->addAction(actionBugIssueFeatureRequest);
        menuHelp->addSeparator();
        menuHelp->addAction(actionAb_out);
        menuUndo->addAction(actionUndo);
        menuUndo->addAction(actionRedo);
        menuOutput->addAction(actionNew_Output_Object);
        menuOutput->addAction(actionDelete_Output_Object);
        menuOutput->addAction(actionMove_item_to_Group);
        menuOutput->addAction(actionNew_Output_Object_Group);
        menuOutput->addAction(actionUngroup);
        menuOutput->addSeparator();
        menuOutput->addAction(actionOutput_SPIERSview);
        menuOutput->addAction(actionUse_Old_Exporting_Code);
        menuOutput->addAction(actionExport_SPIERSview_and_Launch);
        menuOutput->addAction(actionView_in_SPIERSview);
        menuOutput->addAction(actionMeasure_Volumes);
        menuOutput->addSeparator();
        menuOutput->addAction(actionOutput_visible_image_set);
        menuSlice_Spacing->addAction(actionShow_position_slice_selector);
        menuSlice_Spacing->addAction(actionChange_slice_spacing);
        menuSlice_Spacing->addAction(actionSet_slice_position);
        toolBar->addAction(actionBright);
        toolBar->addAction(actionSegment);
        toolBar->addAction(actionRecalc);
        toolBar->addAction(actionCurve);
        toolBar->addAction(actionLock);
        toolBar->addAction(actionMask);
        toolBar->addSeparator();
        toolBar->addAction(actionUndo);
        toolBar->addAction(actionRedo);
        toolBar->addSeparator();
        toolBar->addAction(actionSquare);
        toolBar->addAction(action3D_Brush);

        retranslateUi(MainWindow);
        QObject::connect(verticalSlider, SIGNAL(valueChanged(int)), SpinUp, SLOT(setValue(int)));
        QObject::connect(SpinUp, SIGNAL(valueChanged(int)), verticalSlider, SLOT(setValue(int)));
        QObject::connect(SpinDown, SIGNAL(valueChanged(int)), verticalSlider_2, SLOT(setValue(int)));
        QObject::connect(verticalSlider_2, SIGNAL(valueChanged(int)), SpinDown, SLOT(setValue(int)));
        QObject::connect(verticalSlider_3, SIGNAL(valueChanged(int)), SpinSoft, SLOT(setValue(int)));
        QObject::connect(SpinSoft, SIGNAL(valueChanged(int)), verticalSlider_3, SLOT(setValue(int)));
        QObject::connect(LinearGlobalSlider, SIGNAL(valueChanged(int)), LinearGlobalSpinBox, SLOT(setValue(int)));
        QObject::connect(LinearGlobalSpinBox, SIGNAL(valueChanged(int)), LinearGlobalSlider, SLOT(setValue(int)));
        QObject::connect(SliceSelectAll, &QPushButton::clicked, SliceSelectorList, qOverload<>(&QListWidget::selectAll));
        QObject::connect(SliderBoost, SIGNAL(valueChanged(int)), BoostSpinBox, SLOT(setValue(int)));
        QObject::connect(SliderBoostRadius, SIGNAL(valueChanged(int)), BoostRadiusSpinBox, SLOT(setValue(int)));
        QObject::connect(BoostSpinBox, SIGNAL(valueChanged(int)), SliderBoost, SLOT(setValue(int)));
        QObject::connect(BoostRadiusSpinBox, SIGNAL(valueChanged(int)), SliderBoostRadius, SLOT(setValue(int)));
        QObject::connect(SliderBoostAdjust, SIGNAL(valueChanged(int)), BoostAdjustSpinBox, SLOT(setValue(int)));
        QObject::connect(BoostAdjustSpinBox, SIGNAL(valueChanged(int)), SliderBoostAdjust, SLOT(setValue(int)));
        QObject::connect(SliderHardeningRadius, SIGNAL(valueChanged(int)), HardeningRadiusSpinBox, SLOT(setValue(int)));
        QObject::connect(HardeningRadiusSpinBox, SIGNAL(valueChanged(int)), SliderHardeningRadius, SLOT(setValue(int)));
        QObject::connect(SliderBoostAdjust_2, SIGNAL(valueChanged(int)), AdjustRadialSpinBox, SLOT(setValue(int)));
        QObject::connect(AdjustRadialSpinBox, SIGNAL(valueChanged(int)), SliderBoostAdjust_2, SLOT(setValue(int)));
        QObject::connect(SliderBoost_4, SIGNAL(valueChanged(int)), spinGradientsDistMinEffect, SLOT(setValue(int)));
        QObject::connect(spinGradientsDistMinEffect, SIGNAL(valueChanged(int)), SliderBoost_4, SLOT(setValue(int)));
        QObject::connect(SliderBoost_8, SIGNAL(sliderMoved(int)), spinGradientsPointDensity, SLOT(setValue(int)));
        QObject::connect(spinGradientsPointDensity, SIGNAL(valueChanged(int)), SliderBoost_8, SLOT(setValue(int)));
        QObject::connect(SliderBoost_7, SIGNAL(sliderMoved(int)), spinGradientsDistMax, SLOT(setValue(int)));
        QObject::connect(spinGradientsDistMax, SIGNAL(valueChanged(int)), SliderBoost_7, SLOT(setValue(int)));
        QObject::connect(SliderBoost_6, SIGNAL(sliderMoved(int)), spinGradientsDistMin, SLOT(setValue(int)));
        QObject::connect(spinGradientsDistMin, SIGNAL(valueChanged(int)), SliderBoost_6, SLOT(setValue(int)));
        QObject::connect(SliderBoost_5, SIGNAL(sliderMoved(int)), spinGradientsDistMaxEffect, SLOT(setValue(int)));
        QObject::connect(spinGradientsDistMaxEffect, SIGNAL(valueChanged(int)), SliderBoost_5, SLOT(setValue(int)));

        tabWidget->setCurrentIndex(1);
        OutputTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SPIERSEdit - Version 2.21 - No files loaded", nullptr));
        actionA_command->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
        actionMain_Toolbox->setText(QCoreApplication::translate("MainWindow", "Main Toolbox", nullptr));
#if QT_CONFIG(shortcut)
        actionMain_Toolbox->setShortcut(QCoreApplication::translate("MainWindow", "F1", nullptr));
#endif // QT_CONFIG(shortcut)
        actionGeneration->setText(QCoreApplication::translate("MainWindow", "Generation", nullptr));
#if QT_CONFIG(shortcut)
        actionGeneration->setShortcut(QCoreApplication::translate("MainWindow", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMasks->setText(QCoreApplication::translate("MainWindow", "Masks", nullptr));
#if QT_CONFIG(shortcut)
        actionMasks->setShortcut(QCoreApplication::translate("MainWindow", "F4", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSegments->setText(QCoreApplication::translate("MainWindow", "Segments", nullptr));
#if QT_CONFIG(shortcut)
        actionSegments->setShortcut(QCoreApplication::translate("MainWindow", "F5", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCurves->setText(QCoreApplication::translate("MainWindow", "Curves", nullptr));
#if QT_CONFIG(shortcut)
        actionCurves->setShortcut(QCoreApplication::translate("MainWindow", "F6", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSlice_Selector->setText(QCoreApplication::translate("MainWindow", "Slice Selector", nullptr));
#if QT_CONFIG(shortcut)
        actionSlice_Selector->setShortcut(QCoreApplication::translate("MainWindow", "F2", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_Settings->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_Settings->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+V", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOutput_and_Render->setText(QCoreApplication::translate("MainWindow", "Output and Render", nullptr));
#if QT_CONFIG(shortcut)
        actionOutput_and_Render->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionBright->setText(QCoreApplication::translate("MainWindow", "Bright", nullptr));
#if QT_CONFIG(tooltip)
        actionBright->setToolTip(QCoreApplication::translate("MainWindow", "Brightness Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        actionBright->setStatusTip(QCoreApplication::translate("MainWindow", "Brightness Mode", nullptr));
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(shortcut)
        actionBright->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+B", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSegment->setText(QCoreApplication::translate("MainWindow", "Segment", nullptr));
#if QT_CONFIG(tooltip)
        actionSegment->setToolTip(QCoreApplication::translate("MainWindow", "Segmentation (Pure Colour) Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        actionSegment->setStatusTip(QCoreApplication::translate("MainWindow", "Segmentation (Pure Colour) Mode", nullptr));
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(shortcut)
        actionSegment->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRecalc->setText(QCoreApplication::translate("MainWindow", "Recalc", nullptr));
#if QT_CONFIG(statustip)
        actionRecalc->setStatusTip(QCoreApplication::translate("MainWindow", "Recalculate Mode", nullptr));
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(whatsthis)
        actionRecalc->setWhatsThis(QCoreApplication::translate("MainWindow", "Recalculate Mode", nullptr));
#endif // QT_CONFIG(whatsthis)
#if QT_CONFIG(shortcut)
        actionRecalc->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCurve->setText(QCoreApplication::translate("MainWindow", "Curve", nullptr));
#if QT_CONFIG(tooltip)
        actionCurve->setToolTip(QCoreApplication::translate("MainWindow", "Curves Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        actionCurve->setStatusTip(QCoreApplication::translate("MainWindow", "Curves Mode", nullptr));
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(shortcut)
        actionCurve->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+C", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLock->setText(QCoreApplication::translate("MainWindow", "Lock / Select", nullptr));
#if QT_CONFIG(tooltip)
        actionLock->setToolTip(QCoreApplication::translate("MainWindow", "Lock/Selection Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        actionLock->setStatusTip(QCoreApplication::translate("MainWindow", "Lock/Selection Mode", nullptr));
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(shortcut)
        actionLock->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMask->setText(QCoreApplication::translate("MainWindow", "Mask", nullptr));
        actionMask->setIconText(QCoreApplication::translate("MainWindow", "Mask ", nullptr));
#if QT_CONFIG(tooltip)
        actionMask->setToolTip(QCoreApplication::translate("MainWindow", "Mask Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        actionMask->setStatusTip(QCoreApplication::translate("MainWindow", "Mask Mode", nullptr));
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(shortcut)
        actionMask->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        action1_x_1->setText(QCoreApplication::translate("MainWindow", "1 x 1", nullptr));
#if QT_CONFIG(shortcut)
        action1_x_1->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+1", nullptr));
#endif // QT_CONFIG(shortcut)
        action2_x_2->setText(QCoreApplication::translate("MainWindow", "2 x 2", nullptr));
#if QT_CONFIG(shortcut)
        action2_x_2->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+2", nullptr));
#endif // QT_CONFIG(shortcut)
        action3_x_3->setText(QCoreApplication::translate("MainWindow", "3 x 3", nullptr));
#if QT_CONFIG(shortcut)
        action3_x_3->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+3", nullptr));
#endif // QT_CONFIG(shortcut)
        action4_x_4->setText(QCoreApplication::translate("MainWindow", "4 x 4", nullptr));
#if QT_CONFIG(shortcut)
        action4_x_4->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+4", nullptr));
#endif // QT_CONFIG(shortcut)
        action5_x_5->setText(QCoreApplication::translate("MainWindow", "5 x 5", nullptr));
#if QT_CONFIG(shortcut)
        action5_x_5->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+5", nullptr));
#endif // QT_CONFIG(shortcut)
        action8_x_8->setText(QCoreApplication::translate("MainWindow", "8 x 8", nullptr));
#if QT_CONFIG(shortcut)
        action8_x_8->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+6", nullptr));
#endif // QT_CONFIG(shortcut)
        action15_x_15->setText(QCoreApplication::translate("MainWindow", "15 x 15", nullptr));
#if QT_CONFIG(shortcut)
        action15_x_15->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+7", nullptr));
#endif // QT_CONFIG(shortcut)
        action30_x_30->setText(QCoreApplication::translate("MainWindow", "30 x 30", nullptr));
#if QT_CONFIG(shortcut)
        action30_x_30->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+8", nullptr));
#endif // QT_CONFIG(shortcut)
        action50_x_50->setText(QCoreApplication::translate("MainWindow", "50 x 50", nullptr));
#if QT_CONFIG(shortcut)
        action50_x_50->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+9", nullptr));
#endif // QT_CONFIG(shortcut)
        action200_x_200->setText(QCoreApplication::translate("MainWindow", "200 x 200", nullptr));
#if QT_CONFIG(shortcut)
        action200_x_200->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+0", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSquare->setText(QCoreApplication::translate("MainWindow", "Square Brush", nullptr));
#if QT_CONFIG(shortcut)
        actionSquare->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Return", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCircle->setText(QCoreApplication::translate("MainWindow", "Circle", nullptr));
#if QT_CONFIG(shortcut)
        actionCircle->setShortcut(QCoreApplication::translate("MainWindow", "Return", nullptr));
#endif // QT_CONFIG(shortcut)
        actionToggle_Source->setText(QCoreApplication::translate("MainWindow", "Toggle Source", nullptr));
#if QT_CONFIG(shortcut)
        actionToggle_Source->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Space", nullptr));
#endif // QT_CONFIG(shortcut)
        actionThreshold->setText(QCoreApplication::translate("MainWindow", "Threshold", nullptr));
#if QT_CONFIG(shortcut)
        actionThreshold->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+T", nullptr));
#endif // QT_CONFIG(shortcut)
        actionShowMasks->setText(QCoreApplication::translate("MainWindow", "Always Show Masks", nullptr));
        actionShowSegs->setText(QCoreApplication::translate("MainWindow", "Always Show Segments", nullptr));
        actionAb_out->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        actionKeyboard_Commands->setText(QCoreApplication::translate("MainWindow", "Keyboard Commands", nullptr));
        actionMaskCopy_selected_from_Previous->setText(QCoreApplication::translate("MainWindow", "Copy from previous slice to selected", nullptr));
#if QT_CONFIG(shortcut)
        actionMaskCopy_selected_from_Previous->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F1", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMaskCopy_all_from_previous->setText(QCoreApplication::translate("MainWindow", "Copy from current slice to selected", nullptr));
#if QT_CONFIG(shortcut)
        actionMaskCopy_all_from_previous->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F2", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMaskCopy_selected_from_next->setText(QCoreApplication::translate("MainWindow", "Copy from next slice to selected", nullptr));
#if QT_CONFIG(shortcut)
        actionMaskCopy_selected_from_next->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F3", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMaskCopy_all_from_next->setText(QCoreApplication::translate("MainWindow", "Copy from selected slice to current", nullptr));
#if QT_CONFIG(shortcut)
        actionMaskCopy_all_from_next->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F4", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_selected_on_and_advance->setText(QCoreApplication::translate("MainWindow", "Copy selected on and advance", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_selected_on_and_advance->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F1", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_all_on_and_advance->setText(QCoreApplication::translate("MainWindow", "Copy all on and advance", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_all_on_and_advance->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F2", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_selected_back_and_advance->setText(QCoreApplication::translate("MainWindow", "Copy selected back and advance", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_selected_back_and_advance->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F12", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_all_back_and_adance->setText(QCoreApplication::translate("MainWindow", "Copy all back and advance", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_all_back_and_adance->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F11", nullptr));
#endif // QT_CONFIG(shortcut)
        actionHidden_masks_locked_for_generation->setText(QCoreApplication::translate("MainWindow", "Hidden masks locked for generation", nullptr));
#if QT_CONFIG(shortcut)
        actionHidden_masks_locked_for_generation->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+H", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSegment_brush_applies_masks->setText(QCoreApplication::translate("MainWindow", "Segment brush applies masks", nullptr));
#if QT_CONFIG(shortcut)
        actionSegment_brush_applies_masks->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
        actionInvert_locking_for_this_slice->setText(QCoreApplication::translate("MainWindow", "Invert lock / selection", nullptr));
        actionLock_mask_under_cursor->setText(QCoreApplication::translate("MainWindow", "Lock mask under cursor", nullptr));
#if QT_CONFIG(shortcut)
        actionLock_mask_under_cursor->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+K", nullptr));
#endif // QT_CONFIG(shortcut)
        actionUnlock_mask_under_cursor->setText(QCoreApplication::translate("MainWindow", "Unlock mask under cursor", nullptr));
#if QT_CONFIG(shortcut)
        actionUnlock_mask_under_cursor->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+U", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCreate_mask_from_curve->setText(QCoreApplication::translate("MainWindow", "Mask from curve", nullptr));
#if QT_CONFIG(shortcut)
        actionCreate_mask_from_curve->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSwap_Segments->setText(QCoreApplication::translate("MainWindow", "Swap left/right segments", nullptr));
#if QT_CONFIG(shortcut)
        actionSwap_Segments->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+W", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLock_segments->setText(QCoreApplication::translate("MainWindow", "Lock segments", nullptr));
#if QT_CONFIG(shortcut)
        actionLock_segments->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_segment_from_next_slice->setText(QCoreApplication::translate("MainWindow", "Copy selected from next slice", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_segment_from_next_slice->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F5", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_segment_from_previous_slice->setText(QCoreApplication::translate("MainWindow", "Copy selected from previous slice", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_segment_from_previous_slice->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F6", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopyCurvesFromPrevious->setText(QCoreApplication::translate("MainWindow", "Copy from previous slice to selected", nullptr));
#if QT_CONFIG(shortcut)
        actionCopyCurvesFromPrevious->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F7", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_selected_from_next_slice->setText(QCoreApplication::translate("MainWindow", "Copy selected from next slice", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_selected_from_next_slice->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_all_from_previous_slice->setText(QCoreApplication::translate("MainWindow", "Copy all from previous slice", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_all_from_previous_slice->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F5", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopyCurvesFromNext->setText(QCoreApplication::translate("MainWindow", "Copy from next slice to selected", nullptr));
#if QT_CONFIG(shortcut)
        actionCopyCurvesFromNext->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F7", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopyCurvesToCurrent->setText(QCoreApplication::translate("MainWindow", "Copy from selected slice to current", nullptr));
#if QT_CONFIG(shortcut)
        actionCopyCurvesToCurrent->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F8", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCurve_markers_as_crosses->setText(QCoreApplication::translate("MainWindow", "Curve markers as crosses", nullptr));
#if QT_CONFIG(shortcut)
        actionCurve_markers_as_crosses->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+X", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCreate_curve_as_simple_circle->setText(QCoreApplication::translate("MainWindow", "Create curve as simple circle", nullptr));
#if QT_CONFIG(shortcut)
        actionCreate_curve_as_simple_circle->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+I", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLock_curve_shape->setText(QCoreApplication::translate("MainWindow", "Lock curve shape", nullptr));
#if QT_CONFIG(shortcut)
        actionLock_curve_shape->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+E", nullptr));
#endif // QT_CONFIG(shortcut)
        actionResize_keeping_curve_shape->setText(QCoreApplication::translate("MainWindow", "Resize curves on selected slices...", nullptr));
        actionNew->setText(QCoreApplication::translate("MainWindow", "New...", nullptr));
        actionImport->setText(QCoreApplication::translate("MainWindow", "Import SPIERSedit 1.1...", nullptr));
        actionSave_As->setText(QCoreApplication::translate("MainWindow", "Save As...", nullptr));
        actionMore->setText(QCoreApplication::translate("MainWindow", "More...", nullptr));
        actionUndo->setText(QCoreApplication::translate("MainWindow", "Undo", nullptr));
#if QT_CONFIG(shortcut)
        actionUndo->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Z", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRedo->setText(QCoreApplication::translate("MainWindow", "Redo", nullptr));
#if QT_CONFIG(shortcut)
        actionRedo->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Y", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOutput->setText(QCoreApplication::translate("MainWindow", "Output/Render", nullptr));
#if QT_CONFIG(shortcut)
        actionOutput->setShortcut(QCoreApplication::translate("MainWindow", "F7", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_Objects->setText(QCoreApplication::translate("MainWindow", "Save Output Configuration", nullptr));
        actionLoad_Objects->setText(QCoreApplication::translate("MainWindow", "Load Output Configuration", nullptr));
        actionOutput_SPIERSview->setText(QCoreApplication::translate("MainWindow", "Export SPIERSview", nullptr));
        actionExport_DXF->setText(QCoreApplication::translate("MainWindow", "Export DXF", nullptr));
#if QT_CONFIG(shortcut)
        actionExport_DXF->setShortcut(QCoreApplication::translate("MainWindow", "F11", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAuto_Update->setText(QCoreApplication::translate("MainWindow", "Auto Update", nullptr));
        actionUpdate_View->setText(QCoreApplication::translate("MainWindow", "Update View", nullptr));
        actionLock_Selected_Masks->setText(QCoreApplication::translate("MainWindow", "Lock Selected Masks", nullptr));
        actionUnlock_Selected_Masks->setText(QCoreApplication::translate("MainWindow", "Unlock Selected Masks", nullptr));
        actionShow_Selected_Masks->setText(QCoreApplication::translate("MainWindow", "Show Selected Masks", nullptr));
        actionUnShow_Selected_Masks->setText(QCoreApplication::translate("MainWindow", "Hide Selected Masks", nullptr));
        actionWrite_Selected_Masks->setText(QCoreApplication::translate("MainWindow", "Write Selected Masks", nullptr));
        actionUnWrite_Selected_Masks->setText(QCoreApplication::translate("MainWindow", "UnWrite Selected Masks", nullptr));
        actionLock_selected_segments->setText(QCoreApplication::translate("MainWindow", "Lock selected segments", nullptr));
        actionUnlock_selected_segments->setText(QCoreApplication::translate("MainWindow", "Unlock selected segments", nullptr));
        actionActivate_selected_segments->setText(QCoreApplication::translate("MainWindow", "Activate selected segments", nullptr));
        actionDeactivate_selected_segments->setText(QCoreApplication::translate("MainWindow", "Deactivate selected segments", nullptr));
        actionNew_mask->setText(QCoreApplication::translate("MainWindow", "New mask", nullptr));
        actionDelete_selected_mask_s->setText(QCoreApplication::translate("MainWindow", "Delete selected mask(s)", nullptr));
        actionCreate_new_segment->setText(QCoreApplication::translate("MainWindow", "Create new segment", nullptr));
        actionDelete_selected_segments->setText(QCoreApplication::translate("MainWindow", "Delete selected segment(s)", nullptr));
        actionSettings->setText(QCoreApplication::translate("MainWindow", "Advanced prefs...", nullptr));
        actionHistorgram->setText(QCoreApplication::translate("MainWindow", "Histogram", nullptr));
#if QT_CONFIG(shortcut)
        actionHistorgram->setShortcut(QCoreApplication::translate("MainWindow", "F8", nullptr));
#endif // QT_CONFIG(shortcut)
        actionHistogram_shows_selected->setText(QCoreApplication::translate("MainWindow", "Histogram shows selected", nullptr));
        actionAdd_new_point->setText(QCoreApplication::translate("MainWindow", "Add new node under cursor", nullptr));
#if QT_CONFIG(shortcut)
        actionAdd_new_point->setShortcut(QCoreApplication::translate("MainWindow", "=", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRemove_node_under_cursor->setText(QCoreApplication::translate("MainWindow", "Remove node under cursor", nullptr));
#if QT_CONFIG(shortcut)
        actionRemove_node_under_cursor->setShortcut(QCoreApplication::translate("MainWindow", "-", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCopy_from_current_slice_to_selected->setText(QCoreApplication::translate("MainWindow", "Copy from current slice to selected", nullptr));
#if QT_CONFIG(shortcut)
        actionCopy_from_current_slice_to_selected->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F8", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRemove_selected_curves_from_selected_slices->setText(QCoreApplication::translate("MainWindow", "Remove curves from selected slices", nullptr));
        actionNew_Output_Object->setText(QCoreApplication::translate("MainWindow", "New Output Object", nullptr));
        actionNew_Output_Object_Group->setText(QCoreApplication::translate("MainWindow", "Group", nullptr));
#if QT_CONFIG(shortcut)
        actionNew_Output_Object_Group->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+G", nullptr));
#endif // QT_CONFIG(shortcut)
        actionDelete_Output_Object->setText(QCoreApplication::translate("MainWindow", "Delete Output Object", nullptr));
        actionUngroup->setText(QCoreApplication::translate("MainWindow", "Remove from Group", nullptr));
#if QT_CONFIG(shortcut)
        actionUngroup->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+U", nullptr));
#endif // QT_CONFIG(shortcut)
        actionMove_item_to_Group->setText(QCoreApplication::translate("MainWindow", "Move Selected to Group", nullptr));
        actionMask_Advance_slice_after_copy_operation->setText(QCoreApplication::translate("MainWindow", "Advance one slice after copy", nullptr));
        actionMaskGo_back_one_slice_after_copy->setText(QCoreApplication::translate("MainWindow", "Go back one slice after copy", nullptr));
        actionInfo->setText(QCoreApplication::translate("MainWindow", "Info", nullptr));
#if QT_CONFIG(shortcut)
        actionInfo->setShortcut(QCoreApplication::translate("MainWindow", "F9", nullptr));
#endif // QT_CONFIG(shortcut)
        actionShow_position_slice_selector->setText(QCoreApplication::translate("MainWindow", "Show position in slice selector", nullptr));
#if QT_CONFIG(shortcut)
        actionShow_position_slice_selector->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionChange_slice_spacing->setText(QCoreApplication::translate("MainWindow", "Change slice spacing", nullptr));
        actionSet_slice_position->setText(QCoreApplication::translate("MainWindow", "Set slice position", nullptr));
#if QT_CONFIG(shortcut)
        actionSet_slice_position->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionDistribute_over_range->setText(QCoreApplication::translate("MainWindow", "Distribute over range", nullptr));
        actionChange_downsampling->setText(QCoreApplication::translate("MainWindow", "Change downsampling...", nullptr));
        actionMeasure_Volumes->setText(QCoreApplication::translate("MainWindow", "Measure Volumes", nullptr));
        actionExport_SPIERSview_and_Launch->setText(QCoreApplication::translate("MainWindow", "Export SPIERSview and Launch", nullptr));
        actionRefresh->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
        actionInterpolate_over_selected_slices->setText(QCoreApplication::translate("MainWindow", "Interpolate over selected slices", nullptr));
#if QT_CONFIG(shortcut)
        actionInterpolate_over_selected_slices->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+I", nullptr));
#endif // QT_CONFIG(shortcut)
        action3D_Brush->setText(QCoreApplication::translate("MainWindow", "3D Brush", nullptr));
#if QT_CONFIG(shortcut)
        action3D_Brush->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+D", nullptr));
#endif // QT_CONFIG(shortcut)
        actionView_in_SPIERSview->setText(QCoreApplication::translate("MainWindow", "View in SPIERSview", nullptr));
#if QT_CONFIG(shortcut)
        actionView_in_SPIERSview->setShortcut(QCoreApplication::translate("MainWindow", "F12", nullptr));
#endif // QT_CONFIG(shortcut)
        actionUse_Old_Exporting_Code->setText(QCoreApplication::translate("MainWindow", "Use Old Exporting Code", nullptr));
        actionManual->setText(QCoreApplication::translate("MainWindow", "Online Manual", nullptr));
        actionGrey_out_curves_not_no_current_slice->setText(QCoreApplication::translate("MainWindow", "Grey-out curves not on current slice", nullptr));
        actionExport_Curves_as_CSV->setText(QCoreApplication::translate("MainWindow", "Export Curves as CSV", nullptr));
        actionImport_Curves_as_CSV->setText(QCoreApplication::translate("MainWindow", "Import Curves as CSV", nullptr));
        actionOutput_visible_image_set->setText(QCoreApplication::translate("MainWindow", "Export Working Image Set...", nullptr));
        actionTEST->setText(QCoreApplication::translate("MainWindow", "TEST", nullptr));
        actionBugIssueFeatureRequest->setText(QCoreApplication::translate("MainWindow", "Report Bug/Issue/Feature Request", nullptr));
        actionCode_on_GitHub->setText(QCoreApplication::translate("MainWindow", "Code on GitHub", nullptr));
        actionCycle_Bright_Segment_Mask->setText(QCoreApplication::translate("MainWindow", "Cycle Bright/Segment/Mask", nullptr));
#if QT_CONFIG(shortcut)
        actionCycle_Bright_Segment_Mask->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+C", nullptr));
#endif // QT_CONFIG(shortcut)
        actionIncrease_Size->setText(QCoreApplication::translate("MainWindow", "Increase Size", nullptr));
#if QT_CONFIG(shortcut)
        actionIncrease_Size->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+=", nullptr));
#endif // QT_CONFIG(shortcut)
        actionDecrease_Size->setText(QCoreApplication::translate("MainWindow", "Decrease Size", nullptr));
#if QT_CONFIG(shortcut)
        actionDecrease_Size->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+-", nullptr));
#endif // QT_CONFIG(shortcut)
        label_21->setText(QCoreApplication::translate("MainWindow", "Zoom", nullptr));
        ZoomSpinBox->setSuffix(QCoreApplication::translate("MainWindow", "%", nullptr));
        LabelImage->setText(QCoreApplication::translate("MainWindow", "Image:", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuOpen_Recent->setTitle(QCoreApplication::translate("MainWindow", "Open Recent", nullptr));
        menuMode->setTitle(QCoreApplication::translate("MainWindow", "Mode", nullptr));
        menuWindow->setTitle(QCoreApplication::translate("MainWindow", "Window", nullptr));
        menuBrush->setTitle(QCoreApplication::translate("MainWindow", "Brush", nullptr));
        menuSegments->setTitle(QCoreApplication::translate("MainWindow", "Segments", nullptr));
        menuMasks->setTitle(QCoreApplication::translate("MainWindow", "Masks", nullptr));
        menuCurves->setTitle(QCoreApplication::translate("MainWindow", "Curves", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
        menuUndo->setTitle(QCoreApplication::translate("MainWindow", "Undo", nullptr));
        menuOutput->setTitle(QCoreApplication::translate("MainWindow", "Output", nullptr));
        menuSlice_Spacing->setTitle(QCoreApplication::translate("MainWindow", "Slice Spacing", nullptr));
        dockWidget_Main->setWindowTitle(QCoreApplication::translate("MainWindow", "Main Toolbox (F1)", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "Brightness Mode", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Up", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Down", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Soft", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "Masks", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "L", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "R", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "Segments", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "L", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "R", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Source Min/Max levels", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Source transparency", nullptr));
        dockWidget_Generate->setWindowTitle(QCoreApplication::translate("MainWindow", "Generation (F3)", nullptr));
        LinearRedSpinBox->setPrefix(QCoreApplication::translate("MainWindow", "R: ", nullptr));
        LinearGreenSpinBox->setPrefix(QCoreApplication::translate("MainWindow", "G: ", nullptr));
        LinearBlueSpinBox->setPrefix(QCoreApplication::translate("MainWindow", "B: ", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "Global", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLinear), QCoreApplication::translate("MainWindow", "Linear", nullptr));
        testML->setText(QCoreApplication::translate("MainWindow", "TestML", nullptr));
        trainML->setText(QCoreApplication::translate("MainWindow", "Train ML", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabML), QCoreApplication::translate("MainWindow", "ML", nullptr));
        SpinBoxRangeBase->setPrefix(QCoreApplication::translate("MainWindow", "Base: ", nullptr));
        SpinBoxRangeTop->setPrefix(QCoreApplication::translate("MainWindow", "Top: ", nullptr));
        CheckBoxRangeSelectedOnly->setText(QCoreApplication::translate("MainWindow", "Selected only", nullptr));
        CheckBoxRangeHardFill->setText(QCoreApplication::translate("MainWindow", "Hard Fill", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRange), QCoreApplication::translate("MainWindow", "Range", nullptr));
        label_30->setText(QCoreApplication::translate("MainWindow", "Boost:", nullptr));
        label_29->setText(QCoreApplication::translate("MainWindow", "Radius", nullptr));
        label_31->setText(QCoreApplication::translate("MainWindow", "Adjust", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLCE), QCoreApplication::translate("MainWindow", "LCE", nullptr));
        ShowCenter->setText(QCoreApplication::translate("MainWindow", "Show Details", nullptr));
        SetCentre->setText(QCoreApplication::translate("MainWindow", "Set to Image Centre", nullptr));
        Measure->setText(QCoreApplication::translate("MainWindow", "Measure Sample", nullptr));
        CentreX->setPrefix(QCoreApplication::translate("MainWindow", "Centre X: ", nullptr));
        CentreY->setPrefix(QCoreApplication::translate("MainWindow", "Centre Y: ", nullptr));
        labelHardeningData->setText(QCoreApplication::translate("MainWindow", "No measurements made", nullptr));
        label_32->setText(QCoreApplication::translate("MainWindow", "Min Radius", nullptr));
        label_33->setText(QCoreApplication::translate("MainWindow", "Adjust", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabBeamHardening), QCoreApplication::translate("MainWindow", "Radial", nullptr));
        label_36->setText(QCoreApplication::translate("MainWindow", "Effect (dist min)", nullptr));
        label_37->setText(QCoreApplication::translate("MainWindow", "Effect (dist max)", nullptr));
        label_38->setText(QCoreApplication::translate("MainWindow", "Dist min", nullptr));
        label_39->setText(QCoreApplication::translate("MainWindow", "Dist max", nullptr));
        label_40->setText(QCoreApplication::translate("MainWindow", "Point Density", nullptr));
        lblGradientsWarning->setText(QCoreApplication::translate("MainWindow", "Select ONE curve to apply gradient.", nullptr));
        chkGradientsPreview->setText(QCoreApplication::translate("MainWindow", "Preview", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabGradients), QCoreApplication::translate("MainWindow", "Gradients", nullptr));
        GenInvert->setText(QCoreApplication::translate("MainWindow", "Invert", nullptr));
        GenerateAuto->setText(QCoreApplication::translate("MainWindow", "Auto", nullptr));
        GenerateButton->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        DockOutputSettings->setWindowTitle(QCoreApplication::translate("MainWindow", "Output (F7)", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Sequence front to back?", nullptr));
        CheckMirror->setText(QString());
        label_11->setText(QCoreApplication::translate("MainWindow", "mm/slice Left:", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "Files: First", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Slices/mm:", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "mm/slice Down:", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "Files: Last", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Bin: Z", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "Pixel Sens.:", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "Bin: XY", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Pixels/mm:", nullptr));
        OutputTabs->setTabText(OutputTabs->indexOf(tabSettings), QCoreApplication::translate("MainWindow", "Settings", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = OOTreeWidget->headerItem();
        ___qtreewidgetitem->setText(5, QCoreApplication::translate("MainWindow", "Merge", nullptr));
        ___qtreewidgetitem->setText(3, QCoreApplication::translate("MainWindow", "Fidelity", nullptr));
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "Key", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "Col", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "Object Name", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "Fidelity", nullptr));
        OOResample->setSuffix(QCoreApplication::translate("MainWindow", "%", nullptr));
        DirectCurves->setText(QCoreApplication::translate("MainWindow", "Direct Curves", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "Next Key", nullptr));
        OONew->setText(QCoreApplication::translate("MainWindow", "New", nullptr));
        OOGroup->setText(QCoreApplication::translate("MainWindow", "Group", nullptr));
        OODelete->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        OOUp->setText(QCoreApplication::translate("MainWindow", "Up", nullptr));
        OODown->setText(QCoreApplication::translate("MainWindow", "Down", nullptr));
        OutputTabs->setTabText(OutputTabs->indexOf(tabObjects), QCoreApplication::translate("MainWindow", "Objects", nullptr));
        DockHist->setWindowTitle(QCoreApplication::translate("MainWindow", "Histogram (F8)", nullptr));
        DockSegmentsSettings->setWindowTitle(QCoreApplication::translate("MainWindow", "Segments (F5)", nullptr));
        QTreeWidgetItem *___qtreewidgetitem1 = SegmentsTreeWidget->headerItem();
        ___qtreewidgetitem1->setText(2, QCoreApplication::translate("MainWindow", "Col", nullptr));
        ___qtreewidgetitem1->setText(1, QCoreApplication::translate("MainWindow", "Segment Name", nullptr));
        SegmentAdd->setText(QCoreApplication::translate("MainWindow", "New", nullptr));
        SegmentDelete->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        SegmentMoveUp->setText(QCoreApplication::translate("MainWindow", "Up", nullptr));
        SegmentMoveDown->setText(QCoreApplication::translate("MainWindow", "Down", nullptr));
        DockInfo->setWindowTitle(QCoreApplication::translate("MainWindow", "Info (F9)", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "Pos (Source) [Interp.]:\n"
"Mask (Seg) [Lock/Sel]:\n"
"Source Images:\n"
"Interpreted Images:", nullptr));
        InfoLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        DockMasksSettings->setWindowTitle(QCoreApplication::translate("MainWindow", "Masks (F4)", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = MasksTreeWidget->headerItem();
        ___qtreewidgetitem2->setText(5, QCoreApplication::translate("MainWindow", "Lock", nullptr));
        ___qtreewidgetitem2->setText(4, QCoreApplication::translate("MainWindow", "Show", nullptr));
        ___qtreewidgetitem2->setText(3, QCoreApplication::translate("MainWindow", "On Col.", nullptr));
        ___qtreewidgetitem2->setText(2, QCoreApplication::translate("MainWindow", "Off Col.", nullptr));
        ___qtreewidgetitem2->setText(1, QCoreApplication::translate("MainWindow", "Mask Name", nullptr));
        ___qtreewidgetitem2->setText(0, QCoreApplication::translate("MainWindow", "Bnt.", nullptr));
        MaskAdd->setText(QCoreApplication::translate("MainWindow", "New", nullptr));
        MaskDelete->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        MaskMoveUp->setText(QCoreApplication::translate("MainWindow", "Up", nullptr));
        MaskMoveDown->setText(QCoreApplication::translate("MainWindow", "Down", nullptr));
        DockCurvesSettings->setWindowTitle(QCoreApplication::translate("MainWindow", "Curves (F6)", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = CurvesTreeWidget->headerItem();
        ___qtreewidgetitem3->setText(5, QCoreApplication::translate("MainWindow", "End Slice", nullptr));
        ___qtreewidgetitem3->setText(4, QCoreApplication::translate("MainWindow", "Start Slice", nullptr));
        ___qtreewidgetitem3->setText(3, QCoreApplication::translate("MainWindow", "Segment", nullptr));
        ___qtreewidgetitem3->setText(2, QCoreApplication::translate("MainWindow", "Mode", nullptr));
        ___qtreewidgetitem3->setText(1, QCoreApplication::translate("MainWindow", "Col", nullptr));
        ___qtreewidgetitem3->setText(0, QCoreApplication::translate("MainWindow", "Curve Name", nullptr));
        Curve_Add->setText(QCoreApplication::translate("MainWindow", "New", nullptr));
        CurveDelete->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        CurveMoveUp->setText(QCoreApplication::translate("MainWindow", "Up", nullptr));
        CurveMoveDown->setText(QCoreApplication::translate("MainWindow", "Down", nullptr));
        SliceSelector->setWindowTitle(QCoreApplication::translate("MainWindow", "Slice Selector (F2) ", nullptr));
        SliceSelectAll->setText(QCoreApplication::translate("MainWindow", "Select All", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
