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
#include "dialogaboutimpl.h"

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
    MainWindowImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~MainWindowImpl();
        void LogText(QString text);
        void RedrawImage();
        void RedrawJustDecorations();
        void RedrawJustCropBox();
        void RedrawJustAM();
        void on_actionLock_File_triggered(bool checked);
        void BuildRecentFiles();
        int width, height;

private slots:
        void on_actionLoad_settings_file_triggered();
        void on_actionCompress_dataset_triggered();
        void on_actionSwap_Image_With_Next_triggered();
        void on_actionRotate_Clockwise_Less_2_triggered();
        void on_actionReset_Scene_triggered();
        void on_actionReset_Image_triggered();
        void on_actionHide_Image_triggered();
        void on_actionShow_Image_triggered();
        void on_actionShow_All_triggered();
        void on_actionAdvance_to_hidden_triggered();
        void on_actionRetreat_To_Hidden_triggered();
        void on_actionSave_Backup_triggered();
        void on_actionSave_triggered();
        void on_actionLock_Forward_triggered(bool checked);
        void on_actionLock_Back_triggered(bool checked);
        void on_actionMove_Forward_Back_triggered();
        void on_actionApply_propogation_triggered();
        void on_actionPropogate_mode_triggered(bool checked);
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
        void on_horizontalSlider_valueChanged(int value);
        void on_actionZoom_In_triggered();
        void on_actionZoom_Out_triggered();
        void on_actionZoom_100_triggered();
        void on_actionFit_Window_triggered();
        void on_actionNext_Image_triggered();
        void on_actionPrevious_Image_triggered();
        void on_actionOpen_triggered();
        void on_actionInfo_triggered(bool checked);
        void on_actionManual_triggered();

        void selectMarker();
        void changeRed(int value);
        void changeGreen(int value);
        void changeBlue(int value);
        void changeShape();
        void addMarker();
        void removeMarker();
        void on_actionAdd_Markers_triggered(bool checked);
        void on_actionAuto_align_triggered (bool checked);
        void aMTopLeftXChanged(int value);
        void aMTopLeftYChanged(int value);
        void aMWidthChanged(int value);
        void aMHeightChanged(int value);
        void aMThicknessChanged(int value);
        void aMHorizChanged(int value);
        void aMVertChanged(int value);
        void executeAlign_triggered();
        void openRecentFile();
        void changeMarkerSize(int size);
        void markersLockToggled();
        void autoMarkersToggled();
        void autoMarkersAlign();
        void autoMarkersGrid ();
        void setupAlign_triggered();
        void okClicked();
        void resizeCropW(int value);
        void resizeCropH(int value);
        void Clear_List();

private:
        QString files_directory;
        CustomScene *scene;
        QGraphicsPixmapItem *pixMapPointer;
        QPointF topleft;
        void rotate (qreal rotateAngle);
        void resize(qreal sizeChange);
        void lateralShift(qreal shiftSize);
        void redrawShift();
        void verticalShift(qreal shiftSize);
        QDir Directory;
        int PropogateImage, PropogateStep, LockImage;
        int redValue, greenValue, blueValue;
        int verbose;
        QStringList dirlist;
        QDockWidget *markersDialogue;
        QDockWidget *info, *cropDock, *aMOptions, *autoAlign;
        QDial *red, *red2;
        QDial *green, *green2;
        QDial *blue, *blue2;
        QGridLayout *aMGridLayout;
        QVBoxLayout *markerLayout, *infoLayout, *cropLayout, *aMVertLayout, *autoLayout;
        QHBoxLayout *horizontalLayout1, *horizontalLayout2, *horizontalLayout3, *horizontalLayout4, *horizontalLayout5, *horizontalLayout6, *horizontalLayout7, *horizontalLayout8;
        QSpinBox *mThickness, *mSize, *cropWidth, *cropHeight;
        QSpinBox *aMTopLeftX, *aMTopLeftY, *aMWidth, *aMHeight, *aMThickness, *aMHoriz, *aMVert;
        QWidget *layoutwidget, *layout2widget, *layout3widget, *layout4widget, *layout5widget;
        QPlainTextEdit *notes;
        void RedrawDecorations();
        QRectF sceneRectangle;
        QPushButton *lockMarkers, *autoMarkers, *grid, *setupAlign;
};

extern MainWindowImpl *TheMainWindow;
#endif




