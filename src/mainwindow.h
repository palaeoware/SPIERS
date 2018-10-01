#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
//#include "SVGlwidget.h"
#include "gl3widget.h"
#include <QtWidgets/QLabel>
#include <QTimer>
#include <QtWidgets/QListWidgetItem>
#include <QHBoxLayout>
#include <QtWidgets/QShortcut>

class QTreeWidgetItem;

class vtkObject;
class SPV;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void UpdateGL();
    void RefreshObjects();
    Ui::MainWindow *ui;
    void EnableRenderCommands();
    void DisableRenderCommands();
    //SVGlWidget *widget;
    GlWidget *gl3widget;
    QHBoxLayout *gllayout;

    void RefreshInfo();
    //void UpdateScaleEnabling();


    void setSamples(int i);
    void setSpecificProgress(int p);
    void setSpecificLabel(QString t);
public slots:
    void showSpecificProgress();
protected:
    bool eventFilter(QObject *object, QEvent *event);
private:

    int specificprogress;
    QString specificlabel;
    bool FilterKeys; //set to false to turn off interception of keys needed for type-in boxes

    QList<int>Framenumbs;
    void StripDownForVoxml();
    void closeEvent(QCloseEvent *event);

    QLabel *scalelabel, *ktrlabel;

    QTimer *StartTimer; //bodgy timer to get open working
    QTimer *SpinTimer; //bodgy timer to get open working
    QTimer *PBtimer; //update to progress bar

    QTime *time; //used by spin timer
    void UnsetAllStereo();
    void DrawChildObjects(QList <bool> selflags, int parent);
    void RefreshOneItem(QTreeWidgetItem *item, int i);
    void ActionKey(QChar C);
    void SetResample();
    void SetIslands(int i);
    void SetSmoothing(int i);
    void SetTrans(int v);
    void RefreshPieces();
    void KillSPV(int i);
    void wheelEvent(QWheelEvent *event);
    void ApplyAnimStep();
    void AnimSaveImage();
    QString AnimOutputDir;
    QString DegConvert(float angle);
    QString TransConvert(float trans);

    void SetShininess(int s);
private slots:

    void on_actionSet_Resampling_triggered();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionAnaglyph_Stereo_triggered();
    void on_actionSplit_Stereo_triggered();
    void on_actionNo_Stereo_triggered();
    void on_actionQuadBuffer_Stereo_triggered();
    void on_actionMute_Colours_triggered();
    void on_actionRotate_Clockwise_triggered();
    void on_actionRotate_Anticlockwise_triggered();
    void on_actionLarge_Rotate_Clockwise_triggered();
    void on_actionLarge_Rotate_Anticlockwise_triggered();
    void on_actionMove_away_from_viewer_triggered();
    void on_actionMove_towards_viewer_triggered();
    void on_actionLarge_Move_Closer_triggered();
    void on_actionLarge_Move_Away_triggered();
    void on_actionScreen_Capture_triggered();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_actionBugIssueFeatureRequest_triggered();
    void on_actionCode_on_GitHub_triggered();

    void on_actionVery_Low_triggered();
    void on_actionLow_triggered();
    void on_actionMedium_triggered();
    void on_actionHigh_triggered();
    void on_actionVery_High_triggered();

    void on_actionOff_triggered();
    void on_actionVery_Weak_triggered();
    void on_actionWeak_triggered();
    void on_actionMedium_2_triggered();
    void on_actionStrongish_triggered();
    void on_actionStrong_triggered();
    void on_actionStrongest_triggered();

    void on_actionOff_2_triggered();
    void on_actionRemove_Tiny_triggered();
    void on_actionRemove_Small_triggered();
    void on_actionRemove_Medium_triggered();
    void on_actionRemove_Large_triggered();
    void on_actionRemove_All_triggered();

    void on_actionOpaque_2_triggered();
    void on_action75_Opaque_2_triggered();
    void on_action50_Opaque_triggered();
    void on_action25_Opaque_triggered();
    void on_action62_Opaque_triggered();
    void on_actionObject_Panel_triggered();
    void on_actionProgress_Bars_triggered();
    void on_actionAbout_triggered();
    void on_actionQuick_Guide_triggered();
    void on_actionSave_Changes_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave_Presurfaced_triggered();

    void on_actionResurface_Now_triggered();
    void on_actionAuto_Resurface_triggered();
    void on_actionDXF_triggered();
    void on_actionSTL_triggered();
    void on_actionShow_All_triggered();
    void on_actionHide_All_triggered();
    void on_actionInvert_Show_triggered();
    void on_actionSave_Memory_triggered();
    void on_actionImport_SPV_triggered();
    void on_actionImport_Replacement_triggered();
    void on_actionPieces_Panel_triggered();
    void on_actionOrthographic_View_triggered();

    void on_actionGroup_triggered();
    void on_actionUngroup_triggered();
    void on_actionMove_to_group_triggered();
    void on_action_Move_Up_triggered();
    void on_actionMove_Down_triggered();
    void on_PiecesList_itemSelectionChanged();
    void on_actionSelect_All_triggered();
    void on_actionSelect_None_triggered();
    void on_actionIncrease_Size_triggered();
    void on_actionDecrease_Size_triggered();
    void on_actionReset_Size_triggered();
    void on_PiecesList_itemDoubleClicked(QListWidgetItem *);

    void on_actionAANone_triggered();
    void on_action2x_triggered();
    void on_action4x_triggered();
    void on_action8x_triggered();
    void on_action16x_triggered();
    void UnsetAllAA();
    void StartTimer_fired();
    void SpinTimer_fired();
    void on_ClipStart_valueChanged(int clip);
    void on_ClipDepth_valueChanged(int clip);
    void on_ClipAngle_valueChanged(int clip);
    void on_actionAnimation_Panel_triggered();
    void on_actionClip_Controls_triggered();
    void on_actionRemove_Piece_triggered();
    void on_actionRescale_by_triggered();
    void on_actionInfo_triggered();
    void RedoInfoText();
    void on_infoTreeWidget_itemSelectionChanged();
    void on_infoTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void deleteinfo();
    void on_actionShow_Scale_Grid_triggered();

    void on_actionSave_Finalised_As_triggered();

    void on_actionBackground_Colour_triggered();
    void on_actionManual_triggered();

    void on_actionScale_Grid_Colour_triggered();
    void on_actionMinor_Grid_Colour_triggered();
    void on_actionSet_Image_Folder_triggered();
    void on_actionSave_Image_and_Apply_Step_triggered();
    void on_actionApply_Step_triggered();
    void on_actionApply_Steps_triggered();
    void on_actionApply_Multiple_Steps_Saving_Images_triggered();
    void on_SingleStepButton_pressed();
    void on_SingleStepSaveButton_pressed();
    void on_actionReset_to_default_position_triggered();
    void on_actionSet_new_default_position_triggered();

    void on_actionMatte_triggered();
    void on_actionSlightly_Shiny_triggered();
    void on_actionFairly_Shiny_triggered();
    void on_actionShiny_triggered();
    void on_actionShininessCustom_triggered();
    void on_actionTransparency_Custom_triggered();
    void on_actionSmoothing_Custom_triggered();
    void on_actionIsland_Removal_Custom_triggered();
    void on_actionBounding_Box_triggered();
    void on_MultipleStepButton_pressed();
};

extern MainWindow *MainWin;

#endif // MAINWINDOW_H
