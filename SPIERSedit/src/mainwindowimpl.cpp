/**
 * @file
 * Source: MainWindowImpl
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

#include <math.h>
#include "dialogaboutimpl.h"
#include "importdialogimpl.h"
#include "curves.h"
#include "resampleimpl.h"
#include "mainwindowimpl.h"
#include "copyingimpl.h"
#include "globals.h"
#include "display.h"
#include "myrangescene.h"
#include "mygraphicsview.h"
#include "brush.h"
#include "fileio.h"
#include "moreimpl.h"
#include "undo.h"
#include "contrastimpl.h"
#include "deletemaskdialogimpl.h"
#include "settingsimpl.h"
#include "backthread.h"
#include "histogram.h"
#include "../../SPIERScommon/src/netmodule.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QShortcut>
#include <QTimer>
#include <QTime>
#include <QInputDialog>
#include <QPicture>
#include <QPainter>
#include <QHeaderView>
#include <QFileInfo>
#include <QMutexLocker>
#include <QTextStream>
#include <QStandardPaths>

bool temptestflag = false;

MainWindowImpl *AppMainWindow;
//Contains all main window manipulation code - e.g. dock handling

MainWindowImpl::MainWindowImpl(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
{
    DontRedoZoom = false;
    mainwin = this;
    ReadSuperGlobals();
    AppMainWindow = this;
    setupUi(this);
    setStatusBar(nullptr);

    showMaximized();

    SetUpDocks();

    //Connect all the menu commands
    //For commands which rely on F keys that are non functional on macOS define backup shortcuts
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::Key_F1));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_1));
    actionMain_Toolbox->setShortcuts(shortcuts);
    QObject::connect(actionMain_Toolbox, SIGNAL(triggered()), this, SLOT(Menu_Window_MainToolbox()));

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F2));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_2));
    actionSlice_Selector->setShortcuts(shortcuts);
    QObject::connect(actionSlice_Selector, SIGNAL(triggered()), this, SLOT(Menu_Window_SliceSelector()));

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F3));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_3));
    actionGeneration->setShortcuts(shortcuts);
    QObject::connect(actionGeneration, SIGNAL(triggered()), this, SLOT(Menu_Window_Generate()));

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F4));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_4));
    actionMasks->setShortcuts(shortcuts);
    QObject::connect(actionMasks, SIGNAL(triggered()), this, SLOT(Menu_Window_Masks()));

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F5));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_5));
    actionSegments->setShortcuts(shortcuts);
    QObject::connect(actionSegments, SIGNAL(triggered()), this, SLOT(Menu_Window_Segments()));

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F6));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_6));
    actionCurves->setShortcuts(shortcuts);
    QObject::connect(actionCurves, SIGNAL(triggered()), this, SLOT(Menu_Window_Curves()));

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F7));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_7));
    actionOutput->setShortcuts(shortcuts);
    QObject::connect(actionOutput, SIGNAL(triggered()), this, SLOT(Menu_Window_Output()));

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F8));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_8));
    actionHistorgram->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F9));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_9));
    actionInfo->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::Key_F12));
    shortcuts.append(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    actionView_in_SPIERSview->setShortcuts(shortcuts);

    QObject::connect(actionImport, SIGNAL(triggered()), this, SLOT(Menu_File_Import())); //file/open
    QObject::connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit())); //quit
    QObject::connect(actionToggle_Source, SIGNAL(triggered()), this, SLOT(TransToggled())); //quit
    QObject::connect(actionSquare, SIGNAL(triggered()), this, SLOT(SquareToggled())); //quit
    QObject::connect(actionThreshold, SIGNAL(triggered()), this, SLOT(ThresholdFlag())); //quit
    QObject::connect(actionShowMasks, SIGNAL(triggered()), this, SLOT(SetMasksFlag())); //quit
    QObject::connect(actionShowSegs, SIGNAL(triggered()), this, SLOT(SetSegsFlag())); //quit
    QObject::connect(actionSave_Settings, SIGNAL(triggered()), this, SLOT(SaveSettings())); //quit
    QObject::connect(actionSave_As, SIGNAL(triggered()), this, SLOT(SaveAs())); //quit
    QObject::connect(actionOpen, SIGNAL(triggered()), this, SLOT(FileOpen())); //quit
    QObject::connect(actionNew, SIGNAL(triggered()), this, SLOT(Menu_File_New())); //quit
    QObject::connect(actionUndo, SIGNAL(triggered()), this, SLOT(Undo())); //quit
    QObject::connect(actionRedo, SIGNAL(triggered()), this, SLOT(Redo())); //quit

    //connect other stuff
    QObject::connect(ZoomSlider, SIGNAL(valueChanged(int)), this, SLOT(Zoom_Slider_Changed(int)));
    QObject::connect(SliderPos, SIGNAL(valueChanged(int)), this, SLOT(Moveimage(int)));
    QObject::connect(TransSlider, SIGNAL(valueChanged(int)), this, SLOT(Trans_Changed(int)));
    QObject::connect(MinSlider, SIGNAL(valueChanged(int)), this, SLOT(Min_Changed(int)));
    QObject::connect(MaxSlider, SIGNAL(valueChanged(int)), this, SLOT(Max_Changed(int)));
    QObject::connect(BrushSize, SIGNAL(valueChanged(int)), this, SLOT(BrushChanged(int)));
    QObject::connect(SpinDown, SIGNAL(valueChanged(int)), this, SLOT(BrightDownChanged(int)));
    QObject::connect(SpinUp, SIGNAL(valueChanged(int)), this, SLOT(BrightUpChanged(int)));
    QObject::connect(SpinSoft, SIGNAL(valueChanged(int)), this, SLOT(SoftChanged(int)));
    QObject::connect(MaskBoxLeft, SIGNAL(currentIndexChanged(int)), this, SLOT(LeftMaskChanged(int)));
    QObject::connect(MaskBoxRight, SIGNAL(currentIndexChanged(int)), this, SLOT(RightMaskChanged(int)));
    QObject::connect(SegBoxLeft, SIGNAL(currentIndexChanged(int)), this, SLOT(LeftSegChanged(int)));
    QObject::connect(SegBoxRight, SIGNAL(currentIndexChanged(int)), this, SLOT(RightSegChanged(int)));

    QActionGroup *myActionGroup = new QActionGroup(this);
    // These actions were created via qt designer
    myActionGroup->addAction(actionBright);
    myActionGroup->addAction(actionLock);
    myActionGroup->addAction(actionCurve);
    myActionGroup->addAction(actionMask);
    myActionGroup->addAction(actionSegment);
    myActionGroup->addAction(actionRecalc);

    QObject::connect(myActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(Mode_Changed(QAction *)));

    QAction *shortcutq = new QAction("q_pressed", this);
    shortcutq->setShortcut(tr("q"));
    QAction *shortcuta = new QAction("q_pressed", this);
    shortcuta->setShortcut(tr("a"));
    shortcutright  = new QAction("right_pressed", this);
    shortcutright->setShortcut(tr("."));
    shortcutleft = new QAction("left_pressed", this);
    shortcutleft->setShortcut(tr(","));
    shortcutright2  = new QAction("right_pressed", this);
    shortcutright2->setShortcut(tr("ctrl+."));
    shortcutleft2 = new QAction("left_pressed", this);
    shortcutleft2->setShortcut(tr("ctrl+,"));
    shortcutspace = new QAction("space_pressed", this);
    shortcutspace->setShortcut(tr("space"));

    QObject::connect(shortcutq, SIGNAL(triggered()), this, SLOT(q_pressed()));
    QObject::connect(shortcuta, SIGNAL(triggered()), this, SLOT(a_pressed()));
    QObject::connect(shortcutright, SIGNAL(triggered()), this, SLOT(right_pressed()));
    QObject::connect(shortcutleft, SIGNAL(triggered()), this, SLOT(left_pressed()));
    QObject::connect(shortcutright2, SIGNAL(triggered()), this, SLOT(right_pressed()));
    QObject::connect(shortcutleft2, SIGNAL(triggered()), this, SLOT(left_pressed()));
    QObject::connect(shortcutspace, SIGNAL(triggered()), this, SLOT(TransToggled()));

    escapeFlag = false;

    addAction(shortcutq);
    addAction(shortcuta);
    addAction(shortcutright);
    addAction(shortcutleft);
    addAction(shortcutleft2);
    addAction(shortcutright2);
    addAction(shortcutspace);

    QObject::connect(action1_x_1, SIGNAL(triggered()), this, SLOT(Preset1()));
    QObject::connect(action2_x_2, SIGNAL(triggered()), this, SLOT(Preset2()));
    QObject::connect(action3_x_3, SIGNAL(triggered()), this, SLOT(Preset3()));
    QObject::connect(action4_x_4, SIGNAL(triggered()), this, SLOT(Preset4()));
    QObject::connect(action5_x_5, SIGNAL(triggered()), this, SLOT(Preset5()));
    QObject::connect(action8_x_8, SIGNAL(triggered()), this, SLOT(Preset6()));
    QObject::connect(action15_x_15, SIGNAL(triggered()), this, SLOT(Preset7()));
    QObject::connect(action30_x_30, SIGNAL(triggered()), this, SLOT(Preset8()));
    QObject::connect(action50_x_50, SIGNAL(triggered()), this, SLOT(Preset9()));
    QObject::connect(action200_x_200, SIGNAL(triggered()), this, SLOT(Preset0()));
    QObject::connect(actionMore, SIGNAL(triggered()), this, SLOT(openMore()));

    QTimer *timer = new QTimer(this); //Set up timer to do the screenupdates
    connect(timer, SIGNAL(timeout()), this, SLOT(ScreenUpdate()));
    Active = false;
    timer->start(50); //whenever I get a chance!

    timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(UndoTimer()));
    timer2->start(UndoTimerSetting * 1000);

    //and the autosave timer
    QTimer *timer3 = new QTimer(this); //Set up timer to do the screenupdates
    connect(timer3, SIGNAL(timeout()), this, SLOT(autosave()));
    timer3->start(AutoSaveFrequency * 1000 * 60); //convert mins to ms

    tabwidget = tabWidget;
    BuildRecentFiles();  //do the menus
    InitStates();

    //set up key combo for outputitems
    QStringList items;
    items << "[-]" << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" << "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";
    items << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9";

    NextKey->clear();
    NextKey->addItems(items);
    NextKey->setCurrentIndex(1);

    OutputRegroupMode = false;
    NoUpdateSelectionFlag = false;
    MasksMoveBack = false;
    MasksMoveForward = false;

    actionBright->setIcon(QIcon(":/icons/bulb.bmp"));
    actionSegment->setIcon(QIcon(":/icons/segment.bmp"));
    actionRecalc->setIcon(QIcon(":/icons/Recalc.bmp"));
    actionCurve->setIcon(QIcon(":/icons/Curves.bmp"));
    actionLock->setIcon(QIcon(":/icons/Lock.bmp"));
    actionMask->setIcon(QIcon(":/icons/Masks.bmp"));
    actionUndo->setIcon(QIcon(":/icons/undo.bmp"));
    actionRedo->setIcon(QIcon(":/icons/redo.bmp"));

    bodgeflag = false;

    //sort out mask headings issue
    MasksTreeWidget->headerItem()->setText(0, "Btn.");
    MasksTreeWidget->headerItem()->setText(1, "Mask Name");
    MasksTreeWidget->headerItem()->setText(2, "Col A");
    MasksTreeWidget->setColumnWidth(2, 20);
    MasksTreeWidget->headerItem()->setText(3, "Col B");
    MasksTreeWidget->setColumnWidth(3, 20);
    MasksTreeWidget->headerItem()->setText(4, "Show");
    MasksTreeWidget->headerItem()->setText(5, "Lock");
    MasksTreeWidget->headerItem()->setText(6, "");

    SegmentsTreeWidget->headerItem()->setText(0, "");
    SegmentsTreeWidget->headerItem()->setText(1, "Segment Name");
    SegmentsTreeWidget->headerItem()->setText(2, "Col");
    SegmentsTreeWidget->headerItem()->setText(3, "");
    SegmentsTreeWidget->headerItem()->setText(4, "");

    OOTreeWidget->headerItem()->setText(0, "Object Name");
    OOTreeWidget->headerItem()->setText(1, "Col");
    OOTreeWidget->headerItem()->setText(2, "Key");
    OOTreeWidget->headerItem()->setText(3, "Fidelity");
    OOTreeWidget->headerItem()->setText(4, "");
    OOTreeWidget->headerItem()->setText(5, "Merge");

    CurvesTreeWidget->headerItem()->setText(0, "Curve Name");
    CurvesTreeWidget->headerItem()->setText(1, "Col.");
    CurvesTreeWidget->headerItem()->setText(2, "Mode");
    CurvesTreeWidget->headerItem()->setText(3, "Segment");
    CurvesTreeWidget->headerItem()->setText(4, "Start");
    CurvesTreeWidget->headerItem()->setText(5, "End");

    MasksTreeWidget->header()->setSectionsMovable(false);
    CurvesTreeWidget->header()->setSectionsMovable(false);
    OOTreeWidget->header()->setSectionsMovable(false);
    SegmentsTreeWidget->header()->setSectionsMovable(false);

    MasksTreeWidget->header()->setSectionResizeMode(QHeaderView::Fixed);
    CurvesTreeWidget->header()->setSectionResizeMode(QHeaderView::Fixed);
    OOTreeWidget->header()->setSectionResizeMode(QHeaderView::Fixed);
    SegmentsTreeWidget->header()->setSectionResizeMode(QHeaderView::Fixed);

    //now set up the range graphics scene
    //RangeGraphicsView=(QGraphicsView *) new mygraphicsview;
    mygraphicsview *n = new mygraphicsview;
    RangeGraphicsView = (QGraphicsView *) n;
    verticalLayoutRange->addWidget(RangeGraphicsView);
    rangescene = new myrangescene;
    QGraphicsRectItem *rect = new QGraphicsRectItem(0, 0, 255, 255);
    rect->setPen(Qt::NoPen);
    QLinearGradient g(0, 0, 255, 0);
    g.setColorAt(0, Qt::black);
    g.setColorAt(1, Qt::white);
    rect->setBrush(QBrush(g));
    rangescene->addItem(rect);
    RangeGraphicsView->setScene(rangescene);
    rangescene->Refresh();
    RangeGraphicsView->fitInView(0, 0, 255, 255);

    // Makes sure you can see tab labels when docked with decent size font
    mainwin->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    mainwin->setWindowTitle(QString(PRODUCTNAME) + " - Version " + QString(SOFTWARE_VERSION) + " - No files loaded");

    pausetimers = false;

    //Disable copy segments
    actionCopy_segment_from_previous_slice->setVisible(false);
    actionCopy_segment_from_next_slice->setVisible(false);
    DirectCurves->setVisible(false);

    //Install event handler at application level - this catches all wheel events and
    //sends them to the graphics view
    qApp->installEventFilter(graphicsView);

    ExportingImages = false;
    DoubleClickTimer.start();
}



void MainWindowImpl::autosave()
{
    if (pausetimers) return;
    if (Active)
    {
        WriteSettings();
    }
}

void MainWindowImpl::MakeUndo(QString type)
//type is type of curve operation. If its empty, not a curve undo
{
    bool allcurves = false;
    if (type.length() != 0) allcurves = true; //
    bool OK = false;
    if (Active)
    {
        if (allcurves) CurvesUndoDirty = true;
        //anything dirty?
        if (MasksUndoDirty || LocksUndoDirty || CurvesUndoDirty) OK = true;
        else for (int i = 0; i < SegmentCount; i++) if (Segments[i]->UndoDirty) OK = true;

        //if so, add a new undo event, and kill all the redo events
        if (OK)
        {
            qDeleteAll(RedoEvents.begin(), RedoEvents.end());
            RedoEvents.clear();
            if (allcurves)
            {
                UndoEvent *u = new UndoEvent(-1, -2);
                u->Type = type;
                UndoEvents.prepend(u);
            }
            else
                UndoEvents.prepend(new UndoEvent(-1, -1));
        }

        //cull undo list if it gets too long
        while (TotalUndoSize > (UndoMem * 1024 * 1024)) //as many times as necessary to free the memory
            delete UndoEvents.takeLast(); //remove last from list and delete

        if (UndoEvents.count() > 1) actionUndo->setEnabled(true);
    }
    if (RedoEvents.count() > 0) actionRedo->setEnabled(true);
    else actionRedo->setEnabled(false);
    if (allcurves) CurvesUndoDirty = true;
}

void MainWindowImpl::UndoTimer()
{
    //The Undo timer has fired.
    if (pausetimers) return;
    MakeUndo("");
}

void MainWindowImpl::Undo()
{
    bool OK = false;
    if (Active)
    {
        if (UndoEvents.count() < 2) Error("Not enough undo objects");

        //Back to last or one before? Depends if any changes since last...
        if (MasksUndoDirty || LocksUndoDirty || CurvesUndoDirty) OK = true;
        else for (int i = 0; i < SegmentCount; i++) if (Segments[i]->UndoDirty) OK = true;

        if (OK) UndoEvents.prepend(new UndoEvent(-1, -1)); //Store changes as an undo event

        //if it's a curve operation, ask!
        bool proceed = false;
        if (UndoEvents[1]->Type.length() == 0) proceed = true;
        if (proceed == false) if (QMessageBox::question(this, "Undo", "Undo curve command operation (" + UndoEvents[1]->Type + ")? There is no Redo!",
                                                            QMessageBox::Yes | QMessageBox::No)
                                      == QMessageBox::Yes) proceed = true;
        if (proceed == true)
        {
            UndoEvents[1]->Undo(this);

            bool flag = false;
            if (UndoEvents[1]->Type.length() > 0) flag = true;
            RedoEvents.prepend(UndoEvents.takeFirst()); //remove first from list and delete - put it on redo list
            if (UndoEvents[0]->FileNumber >= 0) RedoEvents.prepend(UndoEvents.takeFirst()); //remove first from list and delete - put it on redo list

            if (flag)
            {
                if (RedoEvents.count()) qDeleteAll(RedoEvents.begin(), RedoEvents.end());
                RedoEvents.clear();
            }

            ShowImage(graphicsView);
            if (UndoEvents.count() > 1) actionUndo->setEnabled(true);
            else actionUndo->setEnabled(false);
        }
    }
    if (RedoEvents.count() > 0) actionRedo->setEnabled(true);
    else actionRedo->setEnabled(false);
}

void MainWindowImpl::Redo()
{
    if (Active)
    {
        if (RedoEvents.count() < 1) Error("Not enough redo objects");

        //Ignore dirtyness

        RedoEvents[0]->Redo(this);
        UndoEvents.prepend(RedoEvents.takeFirst()); //remove first from list and delete - put it on undo list
        if (UndoEvents[0]->FileNumber >= 0 && RedoEvents.count() > 0 ) // we just did a move event - redo next too
            //second condition is just a sanity check
        {
            RedoEvents[0]->Redo(this);
            UndoEvents.prepend(RedoEvents.takeFirst()); //remove first from list and delete - put it on undo list
        }

        ShowImage(graphicsView);
        if (UndoEvents.count() > 1) actionUndo->setEnabled(true);
        else actionUndo->setEnabled(false);
        if (RedoEvents.count() > 0) actionRedo->setEnabled(true);
        else actionRedo->setEnabled(false);
    }
}

void MainWindowImpl::SaveSettings()
{
    WriteAllData(CurrentFile);
    WriteSettings();
    WriteSuperGlobals();
    Message("Settings saved");
}
void MainWindowImpl::LeftMaskChanged(int index)
{
    if (clearing == false)
    {
        int old = SelectedMask;
        SelectedMask = (MaskBoxLeft->itemData(index)).toInt();

        RefreshOneMaskItem(MasksSettings[old]->widgetitem,  old);
        RefreshOneMaskItem(MasksSettings[SelectedMask]->widgetitem,  SelectedMask);
    }
    return;
}

void MainWindowImpl::RightMaskChanged(int index)
{
    if (clearing == false)
    {
        int old = SelectedRMask;
        SelectedRMask = (MaskBoxRight->itemData(index)).toInt();
        RefreshOneMaskItem(MasksSettings[old]->widgetitem,  old);
        RefreshOneMaskItem(MasksSettings[SelectedRMask]->widgetitem,  SelectedRMask);
        return;
    }
}


void MainWindowImpl::LeftSegChanged(int index)
{
    if (clearing == false)
    {
        int old = CurrentSegment;
        CurrentSegment = (SegBoxLeft->itemData(index)).toInt();
        if (old >= 0) RefreshOneSegmentItem(Segments[old]->widgetitem,  old);
        if (CurrentSegment >= 0) RefreshOneSegmentItem(Segments[CurrentSegment]->widgetitem,  CurrentSegment);
        SetUpGenerationToolbox(CurrentSegment);
        ShowImage(graphicsView); //might update if grey mode
        rangescene->Refresh();
    }
}
void MainWindowImpl::RightSegChanged(int index)
{

    if (clearing == false)
    {
        int old = CurrentRSegment;
        CurrentRSegment = (SegBoxRight->itemData(index)).toInt();
        if (old >= 0)RefreshOneSegmentItem(Segments[old]->widgetitem,  old);
        if (CurrentRSegment >= 0) RefreshOneSegmentItem(Segments[CurrentRSegment]->widgetitem,  CurrentRSegment);
        ShowImage(graphicsView); //might update if grey mode
    }
}

void MainWindowImpl::ScreenUpdate()
{
    if (pausetimers) return;
    if (Active)
    {
        if (MoveFlag) Brush.draw(LastMouseX, LastMouseY);
        if (ChangeFlag) ShowImage(graphicsView);
        MoveFlag = false;
        ChangeFlag = false; //reset flags
    }
    else
    {
        //this is the initial startup check - was a command line parameter passed?
        if (openfile.length() >= 2)
        {
            //qDebug()<<"Here"<<openfile;
            QMutexLocker locker(&mutex);

            QFile f(openfile);
            if (f.exists() == false)
            {
                Message("File " + openfile + " does not exist");
                return;
            }
            FullSettingsFileName = openfile.replace("\\", "/");
            //qDebug()<<"Here2"<<FullSettingsFileName;
            ReadSettings();

            SetUpGUIFromSettings();
            RecentFile(openfile);

            Start();

            openfile = "";
        }
    }
}

void MainWindowImpl::q_pressed()
{
    ZoomSlider->setValue(ZoomSlider->value() + 20);
}

void MainWindowImpl::a_pressed()
{
    ZoomSlider->setValue(ZoomSlider->value() - 20);
}

void MainWindowImpl::right_pressed()
{
    SliderPos->setValue(CurrentFile + 2);
}

void MainWindowImpl::left_pressed()
{
    SliderPos->setValue(CurrentFile);
}

void MainWindowImpl:: MouseZoom(int delta)
{
    if ( QGuiApplication::keyboardModifiers() == Qt::CTRL)
    {
        QList<QListWidgetItem *> selected = SliceSelectorList->selectedItems();
        int selectedRow = SliceSelectorList->row(selected[0]);
        int newRow = selectedRow - (delta / 10);
        if (newRow < 0) newRow = 0;
        if (newRow > SliceSelectorList->count() - 1) newRow = SliceSelectorList->count() - 1;
        SliceSelectorList->setCurrentRow(newRow, QItemSelectionModel::ClearAndSelect);
    }
    else if (QGuiApplication::keyboardModifiers() == Qt::SHIFT)
    {
        CurrentFile += delta / 10;
        SliderPos->setValue(CurrentFile);
    }
    else ZoomSlider->setValue(ZoomSlider->value() + delta / 5);
}

void MainWindowImpl::SetMasksFlag()
{
    if (actionShowMasks->isChecked()) MasksFlag = true;
    else MasksFlag = false;
    ShowImage(graphicsView);
}

void MainWindowImpl::SetSegsFlag()
{
    if (actionShowSegs->isChecked()) SegsFlag = true;
    else SegsFlag = false;
    ShowImage(graphicsView);
}

void MainWindowImpl::BrightUpChanged(int trans)
//Transparency setting changed - set global, redraw
{
    BrightUp = trans;
}

void MainWindowImpl::BrightDownChanged(int trans)
//Transparency setting changed - set global, redraw
{
    BrightDown = trans;
}
void MainWindowImpl::SoftChanged(int trans)
//Transparency setting changed - set global, redraw
{
    BrightSoft = trans;
}
void MainWindowImpl::BrushChanged(int trans)
//Transparency setting changed - set global, redraw
{
    Brush_Size = trans;
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

void MainWindowImpl::Mode_Changed(QAction *temp2)
{
    Q_UNUSED(temp2);
    if (actionBright->isChecked()) CurrentMode = 0;
    if (actionMask->isChecked()) CurrentMode = 1;
    if (actionCurve->isChecked()) CurrentMode = 2;
    if (actionLock->isChecked()) CurrentMode = 3;
    if (actionSegment->isChecked()) CurrentMode = 4;
    if (actionRecalc->isChecked()) CurrentMode = 5;
    //0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc
    ShowImage(graphicsView);
}

MainWindowImpl::~MainWindowImpl()
{
//  qDebug()<<"In destructor";
    QMutexLocker locker(&mutex);
//  qDebug()<<"In destructor1";
    if (Active) WriteSettings();
//  qDebug()<<"In destructor2";
    WriteSuperGlobals();
//  qDebug()<<"In destructor3";
    WriteAllData(CurrentFile);
//  qDebug()<<"In destructor4";

//  qDebug()<<"In destructor5";
    //Free all 'newed' items in Lists
    qDeleteAll(Segments.begin(), Segments.end());
//  qDebug()<<"In destructor6";
    qDeleteAll(MasksSettings.begin(), MasksSettings.end());
//  qDebug()<<"In destructor7";
    qDeleteAll(Curves.begin(), Curves.end());
//  qDebug()<<"In destructor8";
    qDeleteAll(OutputObjects.begin(), OutputObjects.end());
//  qDebug()<<"In destructor9";
    //qDeleteAll(GA.begin(), GA.end());
    if (RedoEvents.count()) qDeleteAll(RedoEvents.begin(), RedoEvents.end());
//  qDebug()<<"In destructor10";
    qDeleteAll(UndoEvents.begin(), UndoEvents.end());

//  qDebug()<<"Exited successfully";
    return;
}


void MainWindowImpl::Preset1()
{
    BrushSize->setValue(1);
}
void MainWindowImpl::Preset2()
{
    BrushSize->setValue(2);
}
void MainWindowImpl::Preset3()
{
    BrushSize->setValue(3);
}
void MainWindowImpl::Preset4()
{
    BrushSize->setValue(4);
}
void MainWindowImpl::Preset5()
{
    BrushSize->setValue(5);
}
void MainWindowImpl::Preset6()
{
    BrushSize->setValue(8);
}
void MainWindowImpl::Preset7()
{
    BrushSize->setValue(15);
}
void MainWindowImpl::Preset8()
{
    BrushSize->setValue(30);
}
void MainWindowImpl::Preset9()
{
    BrushSize->setValue(50);
}
void MainWindowImpl::Preset0()
{
    BrushSize->setValue(200);
}
void MainWindowImpl::cmac()
{
    if (actionCurve_markers_as_crosses->isChecked()) CurveMarkersAsCrosses = true;
    else CurveMarkersAsCrosses = false;
    ShowImage(graphicsView);
}
void MainWindowImpl::LockShape()
{
    if (actionLock_curve_shape->isChecked()) CurveShapeLocked = true;
    else CurveShapeLocked = false;
}


void MainWindowImpl::Zoom_Slider_Changed(int zoom)
//Zoom slider changed - convert to real zoom, set zoom box
{
    if (DontRedoZoom) return;
    CurrentZoom = (pow(10.0, (static_cast<double>(zoom)) / 500 + 1)) / 100;
    DontRedoZoom = true;
    ZoomSpinBox->setValue(static_cast<int>(CurrentZoom * 100));
    DontRedoZoom = false;
    ShowImage(graphicsView);
}


void MainWindowImpl::on_ZoomSpinBox_valueChanged(int zoom)
{
    if (DontRedoZoom) return;
    //handle converting the slider to the value typed
    double t = log10(static_cast<double>(zoom));
    int slider = static_cast<int>((t - 1.00) * 500.00);
    CurrentZoom = zoom / 100;
    DontRedoZoom = true;
    ZoomSpinBox->setValue(static_cast<int>(CurrentZoom * 100.00));
    ZoomSlider->setValue(slider);
    DontRedoZoom = false;
    ShowImage(graphicsView);
}

void MainWindowImpl::Trans_Changed(int trans)
//Transparency setting changed - set global, redraw
{
    Trans = trans;
    ShowImage(graphicsView);
}

void MainWindowImpl::Min_Changed(int val)
//Min/max slider handlers
{
    CMin = val;
    if (MaxSlider->value() < (val))
    {
        MaxSlider->setValue(val);
        MaxSlider->update();
    }
    ShowImage(graphicsView);
}

void MainWindowImpl::Max_Changed(int val)
//Min/max slider handlers
{
    CMax = val;
    if (MinSlider->value() > val)
    {
        MinSlider->setValue(val);
        MinSlider->update();
    }
    ShowImage(graphicsView);
}


void MainWindowImpl::InitStates()
//set up window/menu states in accord with default show/hide status.
{
    actionMain_Toolbox->setChecked(true);
    actionGeneration->setChecked(true);
    clearing = false;
    //Do other inits
}

void MainWindowImpl::SquareToggled()
{
    if (actionSquare->isChecked())
        SquareBrush = true;
    else
        SquareBrush = false;

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


void MainWindowImpl::TransToggled()
{
    // if it's not full - make it full, save last value. OTHERWISE - revert to last value
    if (Trans < 15)
    {
        LastTrans = Trans;
        Trans = 15;
    }
    else Trans = LastTrans;
    TransSlider->setValue(Trans);
}

void MainWindowImpl::ThresholdFlag()
{
    if (actionThreshold->isChecked()) ThreshFlag = true;
    else ThreshFlag = false;
    ShowImage(graphicsView);
}

void MainWindowImpl::Menu_Window_MainToolbox()
{
    if (actionMain_Toolbox->isChecked()) // show the toolbox
        dockWidget_Main->setVisible(true);
    else                                // hide it
        dockWidget_Main->setVisible(false);

    MenuToolboxChecked = actionMain_Toolbox->isChecked();
}

void MainWindowImpl::Menu_Window_Masks()
{
    if (actionMasks->isChecked()) // show the toolbox
        DockMasksSettings->setVisible(true);
    else                                // hide it
        DockMasksSettings->setVisible(false);

    MenuMasksChecked = actionMasks->isChecked();

}

void MainWindowImpl::Menu_Window_SliceSelector()
{
    if (actionSlice_Selector->isChecked()) // show the toolbox
        SliceSelector->setVisible(true);
    else                                // hide it
        SliceSelector->setVisible(false);

    MenuSliceSelectorChecked = actionSlice_Selector->isChecked();
}

void MainWindowImpl::Menu_Window_Segments()
{
    if (actionSegments->isChecked()) // show the toolbox
        DockSegmentsSettings->setVisible(true);
    else                                // hide it
        DockSegmentsSettings->setVisible(false);

    MenuSegsChecked = actionSegments->isChecked();
}

void MainWindowImpl::Menu_Window_Output()
{
    if (actionOutput->isChecked()) // show the toolbox
        DockOutputSettings->setVisible(true);
    else                                // hide it
        DockOutputSettings->setVisible(false);
    MenuOutputChecked = actionOutput->isChecked();
}

void MainWindowImpl::Menu_Window_Curves()
{
    if (actionCurves->isChecked()) // show the toolbox
        DockCurvesSettings->setVisible(true);
    else                                // hide it
        DockCurvesSettings->setVisible(false);

    MenuCurvesChecked = actionCurves->isChecked();
}


void MainWindowImpl::Menu_Window_Generate()
{
    if (actionGeneration->isChecked()) // show the toolbox
        dockWidget_Generate->setVisible(true);
    else                                // hide it
        dockWidget_Generate->setVisible(false);
    MenuGenChecked = actionGeneration->isChecked();
}


void MainWindowImpl::FileOpen()
{
    QMutexLocker locker(&mutex);

    QString file = QFileDialog::getOpenFileName(
                       this,
                       "Select SPIERSedit settings file",
                       QDir::homePath(),
                       "SPIERSedit files (*.spe)");


    //Now we do a whole load of initialisation!
    if (file.isNull()) return; //if nothing there, cancel
    if (Active) WriteSettings();
    FullSettingsFileName = file;

    ReadSettings();

    //Now do set up - same as for
    if (Active)
    {
        Active = false;
        Brush.Brush_Flag_Restart();
        ClearImages();
    }

    SetUpGUIFromSettings();
    RecentFile(file);

    Start();
}

void MainWindowImpl::openRecentFile()
{
    QMutexLocker locker(&mutex);
    QAction *action = qobject_cast<QAction *>(sender());

    if (Active) WriteSettings();
    QString fname = action->statusTip(); //bit of a bodge to extract name!
    FullSettingsFileName = fname;
    //Got filename - go as per file open (code copied!)

    QFileInfo fi (FullSettingsFileName);
    if (fi.exists())
    {
        ReadSettings();

        if (Active)
        {
            Active = false;
            Brush.Brush_Flag_Restart();
            ClearImages();
        }

        SetUpGUIFromSettings();
        RecentFile(fname);

        Start();
    }
    else
    {
        Message("File " + FullSettingsFileName + " not found");
        for (int i = 0; i < RecentFileList.count(); i++)
        {
            if (RecentFileList[i].File == fname)
            {
                RecentFileList.removeAt(i);
                BuildRecentFiles();
                break;
            }
        }
    }
}

void MainWindowImpl::openMore()
{
    QMutexLocker locker(&mutex);
    moreimpl mdialog;
    mdialog.exec();

    //do some error checking and ways out!
    if (mdialog.Cancelled == true) return;

    FullSettingsFileName = mdialog.fname;
    //Got filename - go as per file open (code copied!)

    ReadSettings();
    if (Active)
    {
        Active = false;
        Brush.Brush_Flag_Restart();
        ClearImages();
    }

    SetUpGUIFromSettings();
    RecentFile(mdialog.fname);

    Start();
}

void MainWindowImpl::Start()
//runs after new dataset loaded
{
    //Make sure undo and redo are cleared
    qDeleteAll(RedoEvents.begin(), RedoEvents.end());
    RedoEvents.clear();
    qDeleteAll(UndoEvents.begin(), UndoEvents.end());
    UndoEvents.clear();
    actionUndo->setEnabled(false);
    actionRedo->setEnabled(false);

    //remove any marker pointers

    MarkerList.clear();

    InitImage(graphicsView);

    fwidth = cwidth / ColMonoScale;
    if ((fwidth % 4) == 0) fwidth4 = fwidth; // no problem with width
    else fwidth4 = (fwidth / 4) * 4 + 4;
    fheight = cheight / ColMonoScale;

    LoadAllData(CurrentFile);
    SliceSelectorList->setCurrentRow(CurrentFile);

    cwidth4 = cwidth;
    if (GreyImage)
    {
        if ((cwidth % 4) != 0) cwidth4 = (cwidth / 4) * 4 + 4;
    }

    BuildRecentFiles();
    dirty.resize(fheight * fwidth);

    setWindowTitle(QString(PRODUCTNAME) + " - Version " + QString(SOFTWARE_VERSION) + " - " + SettingsFileName + " - " + Files[CurrentFile]);
    Active = true; //flag that we are GO

    ShowImage(graphicsView);

    //set initial undo point
    MasksUndoDirty = true;
    LocksUndoDirty = true;
    CurvesUndoDirty = true;
    for (int i = 0; i < SegmentCount; i++) Segments[i]->UndoDirty = true;
    UndoEvents.prepend(new UndoEvent(-1, -1));
    rangescene->Refresh();

    ResetFilesDirty();
}

void MainWindowImpl::BuildRecentFiles()
{
//  QAction *recentFileAction;
    int lastsep;
    QString name;
    int count = 0;
    //first delete any current menu items under recent

    QList <QAction *> currentactions = menuOpen_Recent->actions();

    menuOpen_Recent->actions();

    foreach (QAction *thisact, currentactions)
    {
        //remove from menu
        menuOpen_Recent->removeAction(thisact);
        if (thisact->text() != "More...")
        {
            //disconnect
            thisact->disconnect();
            //delete object
            delete thisact;
        }
    }


    //now add all
    foreach (RecentFiles rf, RecentFileList)
    {
        count++;
        if (count > 9)
        {
            menuOpen_Recent->addAction(actionMore);
            break;
        }

        lastsep = qMax(rf.File.lastIndexOf("\\"), rf.File.lastIndexOf("/")); //this is last separator in path
        name = rf.File.mid(lastsep + 1);
        name = name.left(name.length() - 4);
        QAction *recentFileAction = new QAction(name, this);
        //recentFileAction->setVisible(true);
        recentFileAction->setStatusTip(rf.File);
        //recentFileAction
        connect(recentFileAction, SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
        menuOpen_Recent->addAction(recentFileAction);
    }


}


void MainWindowImpl::SaveAs()
//Makes a copy
{

    QMutexLocker locker(&mutex);
    bool ok;
    QString file = QInputDialog::getText(this, tr("Save As"),
                                         tr("Enter new filename:"), QLineEdit::Normal,
                                         "", &ok);
    if (!ok || file.isEmpty()) return;

    if (QString::compare(".spe", file.right(4), Qt::CaseInsensitive) == 0) file = file.left(file.length() - 4);


    int lastsep = qMax(FullSettingsFileName.lastIndexOf("\\"), FullSettingsFileName.lastIndexOf("/"));
    QString newdirname = FullSettingsFileName.left(lastsep + 1) + file;
    QString newfilename = FullSettingsFileName.left(lastsep + 1) + file + ".spe";
    QString olddirname = FullSettingsFileName.left(FullSettingsFileName.length() - 4);
    QString rootdirname = FullSettingsFileName.left(lastsep);

    //watch for sneaky stuff
    if (file.contains(".") || file.contains("/") || file.contains("\\"))
    {
        Message("Illegal Filename - try again");
        return;
    }

    //now see if it exists -  just test the file, not the directory
    QFile testfile(newfilename);
    if (testfile.exists())
    {
        //no overwrite facility
        Message("SPIERSedit file " + file + ".spe already exists. SPIERSedit will not overwrite existing files - delete the old one manually before proceeding if you really want to do this.");
        return;
    }

    //now check we can create it OK
    if (!testfile.open(QIODevice::WriteOnly))
    {
        Message("ERROR - can't create SPIERSedit settings file " + file + ".spe - filename probably invalid");
        return;
    }

    testfile.close();
    testfile.remove(); //delete test version

    //OK, now we are go! Just copy the files, set the settings, call writesettings. Done!
    WriteSettings(); //have to save current one first...
    QDir fromdir(olddirname);
    QDir todir(newdirname);
    QDir rootdir(rootdirname);

    QDir newdir(rootdirname);
    if (newdir.mkdir(file) == false)
    {
        QString outstring = "ERROR - can't create SPIERSedit subdirectory ";
        outstring.append(todir.canonicalPath());
        outstring.append(" - please check directory permiossions");
        Message(outstring);
        return;
    }

    CopyingImpl cop;
    cop.Copy(fromdir, todir);

    SettingsFileName = file;
    FullSettingsFileName = newfilename;

    WriteSettings();
    RecentFile(FullSettingsFileName);
    BuildRecentFiles();
    WriteSuperGlobals();
    setWindowTitle(QString(PRODUCTNAME) + " - Version " + QString(SOFTWARE_VERSION) + " - " + SettingsFileName + " - " + Files[CurrentFile]);
}

void MainWindowImpl::Menu_File_Import()
{
    QMutexLocker locker(&mutex);

    //Select files
    if (Active)
        WriteSettings();

    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select source images for SPIERS 1.1 dataset",
                            "D:/Research/3dFiles/Grinding/Acaen/5-2/Cut",
                            "Images (*.bmp)");

    // If nothing there, cancel
    if (files.isEmpty() || files.count() == 0)
        return;

    // Now we do a whole load of initialisation!
    std::sort(files.begin(), files.end());

    // Show the 2nd stage dialog
    ImportDialogImpl impdialog;
    impdialog.exec();

    // Do some error checking and ways out!
    if (impdialog.Cancelled == true)
        return;

    //check to see if dataset actually exists - look for settings.dat
    QString Fname = files.at(0);
    int lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    QString setname = Fname.left(lastsep);
    setname.append("/imed/settings.dat");
    QFile settings(setname);
    if (!(settings.exists()))
    {
        Message("ERROR - couldn't find a SPIERS 1.1 dataset here (imed/settings.dat missing)");
        return;
    }

    setname = Fname.left(lastsep);
    setname.append("/");
    setname.append(impdialog.fname);
    setname.append(".spe");

    QFile settings2(setname);
    if (settings2.exists())
    {
        QString outstring = "";
        outstring.append("ERROR - SPIERS edit file ");
        outstring.append(setname);
        outstring.append(" already exists!");
        Message(outstring);
        return;
    }

    //Finally check the filename is legal
    //How? Just check

    if (!settings2.open(QIODevice::WriteOnly))
    {
        QString outstring = "ERROR - can't create SPIERS edit settings file ";
        outstring.append(setname);
        outstring.append(" - filename probably invalid");
        Message(outstring);
        return;
    }

    //looks OK
    settings2.close();
    settings2.remove(); //delete test version
    FullSettingsFileName = Fname.left(lastsep) + "/" + impdialog.fname + ".spe";
    SettingsFileName = impdialog.fname;
    FileNotes = impdialog.notes; //get globals in

    setname = Fname.left(lastsep) + "/imed";
    QDir fromdir(setname);
    setname = Fname.left(lastsep) + "/" + impdialog.fname;
    QDir todir(setname);
    QDir newdir(Fname.left(lastsep));
    if (newdir.mkdir(impdialog.fname) == false)
    {
        QString outstring = "ERROR - can't create SPIERS edit subdirectory ";
        outstring.append(todir.canonicalPath());
        outstring.append(" - please check directory permiossions");
        Message(outstring);
        return;
    }

    //Now do set up
    if (Active)
    {
        Active = false;
        Brush.Brush_Flag_Restart();
        ClearImages();
    }
    Files = files; //best make a copy I think
    FileCount = Files.count();

    GetSettings(fromdir); //applies defaults, tries to load. Need to do this here to get curves ready to read in!


    CopyingImpl cop;
    cop.Copy(fromdir, todir); //move files, import the curves


    SetUpGUIFromSettings();


    RecentFile(FullSettingsFileName);
    FullFiles = Files;

    Start();


    WriteSuperGlobals();
    Message("SPIERS 1.1 dataset successfully converted to new format!");
}

void MainWindowImpl::Menu_File_New()  //create from scratch

{
    if (Active)
        WriteSettings();

    //Select files
    QStringList files = QFileDialog::getOpenFileNames(
                            nullptr,
                            "Select source images for dataset",
                            QString(QStandardPaths::DesktopLocation),
                            "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)");

    // If nothing there, cancel
    if (files.isEmpty() || files.count() == 0)
        return;

    if (files[0].endsWith(".tif") || files[0].endsWith(".tiff"))
        Message("Tiff support is a relatively recent addition to SPIERSedit, and has not been extensively tested. Please can you submit an issue on the Palaeoware SPIERS GitHub repository should you encounter any issues.");

    // Now we do a whole load of initialisation!
    std::sort(files.begin(), files.end());

    //OK, no more caching
    //Show the 2nd stage dialog
    ImportDialogImpl impdialog;
    impdialog.HideCopy();
    impdialog.exec();

    //do some error checking and ways out!
    if (impdialog.Cancelled == true)
        return;

    QString Fname = files.at(0);
    int lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    QString setname = Fname.left(lastsep);
    setname.append("\\");
    setname.append(impdialog.fname);
    setname.append(".spe");

    QFile settings2(setname);
    if (settings2.exists())
    {
        QString outstring = "";
        outstring.append("ERROR - SPIERS edit file ");
        outstring.append(setname);
        outstring.append(" already exists!");
        Message(outstring);
        return;
    }

    //Finally check the filename is legal
    //How? Just try creating

    if (!settings2.open(QIODevice::WriteOnly))
    {
        QString outstring = "ERROR - can't create SPIERS edit settings file ";
        outstring.append(setname);
        outstring.append(" - filename probably invalid");
        Message(outstring);
        return;
    }

    //looks OK
    mutex.lock();
    settings2.close();
    settings2.remove(); //delete test version

    FullSettingsFileName = Fname.left(lastsep) + "/" + impdialog.fname + ".spe";
    SettingsFileName = impdialog.fname;
    FileNotes = impdialog.notes; //get globals in

    QDir temp(Fname.left(lastsep)); //root directory
    if (temp.mkdir(SettingsFileName) == false)   //make my new subdirectory, check worked OK
    {
        QString outstring = "ERROR - can't create SPIERS edit data subdirectory ";
        outstring.append(Fname.left(lastsep) + "/" + SettingsFileName);
        outstring.append(" - filename probably invalid");
        Message(outstring);
        mutex.unlock();
        return;
    };

    //Now do set up
    if (Active)
    {
        Active = false;
        Brush.Brush_Flag_Restart();
        ClearImages();
    }
    FullFiles = Files = files; //best make a copy I think
    FileCount = Files.count();
    ResetFilesDirty();


    ApplyDefaultSettings(); //applies defaults, tries to load

    ColMonoScale = impdialog.spinBox->value();
    zsparsity = impdialog.spinBoxZ->value();
    OutputMirroring = impdialog.CheckMirrored->isChecked();
    if (zsparsity > 1)
    {
        Files.clear();
        for (int i = 0; i < FullFiles.count(); i += zsparsity)
        {
            Files.append(FullFiles[i]);
        }
        FileCount = Files.count();
    }

    SetUpGUIFromSettings();
    //Raise the slice selector so it is clear this is tabbed but present
    mainwin->SliceSelector->raise();
    DockOutputSettings->setVisible(true);

    //Set up a single output object
    OutputObject *o = new OutputObject("");

    QString sname = "Object";
    o->ComponentSegments.append(0);
    o->ComponentMasks.append(0);
    o->Name = "Object";
    o->Key = 0;
    o->Parent = -1;
    OutputObjects.append(o);
    OutputObjectsCount++;

    RefreshOO();

    //set up greyscale files - use progress bar window
    CopyingImpl cop;
    cop.GenerateAllBlank();

    RecentFile(FullSettingsFileName);

    Start();

    mutex.unlock();
}

void MainWindowImpl::on_Slices_Per_MM_valueChanged(double value)
{
    SlicePerMM = value;
    if (Active)
        for (int i = 0; i < FileCount; i++)
            SliceSelectorList->item(i)->setText(TextForSliceSelectorBox(i));
}

void MainWindowImpl::on_Pixels_Per_MM_valueChanged(double value)
{
    PixPerMM = value;
}

void MainWindowImpl::on_Edge_Down_MM_valueChanged(double value)
{
    SkewDown = value;
}

void MainWindowImpl::on_Edge_Left_MM_valueChanged(double value )
{
    SkewLeft = value;
}

void MainWindowImpl::on_CheckMirror_toggled(bool checked)
{
    ResetFilesDirty();
    OutputMirroring = checked;
}

void MainWindowImpl::on_FirstFile_valueChanged(int value)
{
    FirstOutputFile = value - 1;
    if (FirstOutputFile > LastOutputFile) LastFile->setValue(FirstOutputFile + 1);
}

void MainWindowImpl::on_LastFile_valueChanged(int value)
{
    LastOutputFile = value - 1;
    if (LastOutputFile < FirstOutputFile) FirstFile->setValue(LastOutputFile + 1);
}

void MainWindowImpl::on_DownsampleXY_valueChanged(int value)
{
    ResetFilesDirty();
    XYDownsample = value;
}

void MainWindowImpl::on_DownsampleZ_valueChanged(int value)
{
    ResetFilesDirty();
    ZDownsample = value;
}

void MainWindowImpl::on_PixelSensitivity_valueChanged(int value)
{
    ResetFilesDirty();
    PixSens = value;
}

void MainWindowImpl::on_MasksTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (DoubleClickTimer.elapsed() < 100) return; //avoid double calls
    //invert a yes/no
    for (int i = 0; i <= MaxUsedMask; i++)
        if (item == MasksSettings[i]->widgetitem)   //found it
        {
            //if (column==4)    MasksSettings[i]->Write=!(MasksSettings[i]->Write);
            if (column == 4)  MasksSettings[i]->Show = !(MasksSettings[i]->Show);
            if (column == 5)  MasksSettings[i]->Lock = !(MasksSettings[i]->Lock);
            if (column == 1)
            {
                QString temp =
                    QInputDialog::getText (this, "", "", QLineEdit::Normal, MasksSettings[i]->Name);

                if (temp != "") MasksSettings[i]->Name = temp;
                RefreshOO();
            }

            if (column == 3)   //foreground
            {
                QColor newcol;

                newcol = QColorDialog::getColor(QColor(MasksSettings[i]->ForeColour[0], MasksSettings[i]->ForeColour[1], MasksSettings[i]->ForeColour[2]));
                if (newcol.isValid())
                {
                    MasksSettings[i]->ForeColour[0] = newcol.red();
                    MasksSettings[i]->ForeColour[1] = newcol.green();
                    MasksSettings[i]->ForeColour[2] = newcol.blue();

                    int c = MasksSettings[i]->Contrast + 1;
                    if (c == 6) c = 20000; //big value, want 0's!
                    MasksSettings[i]->BackColour[0] = MasksSettings[i]->ForeColour[0] / c;
                    MasksSettings[i]->BackColour[1] = MasksSettings[i]->ForeColour[1] / c;
                    MasksSettings[i]->BackColour[2] = MasksSettings[i]->ForeColour[2] / c;
                }
            }


            if (column == 2)   //background - custom dialog
            {
                ContrastImpl cdialog(i);
                cdialog.exec();
            }
            RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
            MasksTreeWidget->resizeColumnToContents(1); //resize text column
            ShowImage(graphicsView);
        }
    DoubleClickTimer.restart();
}

void MainWindowImpl::on_MasksTreeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(item);
    Q_UNUSED(column);
    return;
}


void MainWindowImpl::on_actionLock_Selected_Masks_triggered()
{
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) != nullptr)
            if ((MasksSettings[i]->widgetitem)->isSelected())
            {
                MasksSettings[i]->Lock = true;
                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
            }
    }
}

void MainWindowImpl::on_actionUnlock_Selected_Masks_triggered()
{
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) != nullptr)
            if ((MasksSettings[i]->widgetitem)->isSelected())
            {
                MasksSettings[i]->Lock = false;
                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
            }
    }
}

void MainWindowImpl::on_actionShow_Selected_Masks_triggered()
{
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) != nullptr)
            if ((MasksSettings[i]->widgetitem)->isSelected())
            {
                MasksSettings[i]->Show = true;
                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
            }
    }
    ShowImage(graphicsView);
}

void MainWindowImpl::on_actionUnShow_Selected_Masks_triggered()
{
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) != nullptr)
            if ((MasksSettings[i]->widgetitem)->isSelected())
            {
                MasksSettings[i]->Show = false;
                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
            }
    }
    ShowImage(graphicsView);
}

void MainWindowImpl::on_actionWrite_Selected_Masks_triggered()
{
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) != nullptr)
            if ((MasksSettings[i]->widgetitem)->isSelected())
            {
                MasksSettings[i]->Write = true;
                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
            }
    }
}

void MainWindowImpl::on_actionUnWrite_Selected_Masks_triggered()
{
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) != nullptr)
            if ((MasksSettings[i]->widgetitem)->isSelected())
            {
                MasksSettings[i]->Write = false;
                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
            }
    }
}

void MainWindowImpl::on_MaskMoveUp_pressed()
{
    //Move selected mask up
    MasksTreeWidget->setUpdatesEnabled(false);
    if ((MasksTreeWidget->selectedItems()).count() != 1)
        Message("Select a SINGLE mask");
    else
    {
        for (int i = 0;  i <= MaxUsedMask; i++)
        {
            if ((MasksSettings[i]->widgetitem) != nullptr)
                if ((MasksSettings[i]->widgetitem)->isSelected())
                {
                    //found selected
                    //swap with one above in list
                    QTreeWidgetItem *search = MasksTreeWidget->itemAbove(MasksSettings[i]->widgetitem);
                    if (search)
                        for (int j = 0;  j <= MaxUsedMask; j++)
                            if ((MasksSettings[j]->widgetitem) == search)
                            {
                                //found it
                                int temp = MasksSettings[i]->ListOrder;
                                QTreeWidgetItem *temp2 = MasksSettings[i]->widgetitem;
                                MasksSettings[i]->ListOrder = MasksSettings[j]->ListOrder;
                                MasksSettings[i]->widgetitem = MasksSettings[j]->widgetitem;

                                MasksSettings[j]->ListOrder = temp;
                                MasksSettings[j]->widgetitem = temp2;

                                /*                  int index=MasksTreeWidget->indexOfTopLevelItem(MasksSettings[i]->widgetitem);
                                                    QTreeWidgetItem *temp1=MasksTreeWidget->takeTopLevelItem(index);
                                                    MasksTreeWidget->insertTopLevelItem(index-1,temp1);
                                */

                                MasksTreeWidget->setFocus(Qt::OtherFocusReason);
                                MasksSettings[i]->widgetitem->setSelected(true);
                                MasksSettings[j]->widgetitem->setSelected(false);
                                MasksTreeWidget->scrollToItem(MasksSettings[j]->widgetitem);
                                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
                                RefreshOneMaskItem(MasksSettings[j]->widgetitem, j);
                                MasksTreeWidget->setUpdatesEnabled(true);
                                RefreshMasksBoxes();
                                return;

                            }
                }
        }
    }
    MasksTreeWidget->setUpdatesEnabled(true);
}

void MainWindowImpl::on_MaskMoveDown_pressed()
{
    //Move selected mask down
//  QTime t;
//  t.start();
    MasksTreeWidget->setUpdatesEnabled(false);

    if ((MasksTreeWidget->selectedItems()).count() != 1)
        Message("Select a SINGLE mask");
    else
    {
        for (int i = 0;  i <= MaxUsedMask; i++)
        {
            if ((MasksSettings[i]->widgetitem) != nullptr)
                if ((MasksSettings[i]->widgetitem)->isSelected())
                {
                    //found selected
                    //swap with one above in list
                    QTreeWidgetItem *search = MasksTreeWidget->itemBelow(MasksSettings[i]->widgetitem);
                    if (search)
                        for (int j = 0;  j <= MaxUsedMask; j++)
                            if ((MasksSettings[j]->widgetitem) == search)
                            {
                                //found it
                                int temp = MasksSettings[i]->ListOrder;
                                QTreeWidgetItem *temp2 = MasksSettings[i]->widgetitem;
                                MasksSettings[i]->ListOrder = MasksSettings[j]->ListOrder;
                                MasksSettings[i]->widgetitem = MasksSettings[j]->widgetitem;

                                MasksSettings[j]->ListOrder = temp;
                                MasksSettings[j]->widgetitem = temp2;

                                //now do it in the widget
                                //find index
                                //int index=MasksTreeWidget->indexOfTopLevelItem(MasksSettings[i]->widgetitem);
                                //QTreeWidgetItem *temp1=MasksTreeWidget->takeTopLevelItem(index);
                                //MasksTreeWidget->insertTopLevelItem(index+1,temp1);
                                MasksTreeWidget->setFocus(Qt::OtherFocusReason);
                                MasksSettings[i]->widgetitem->setSelected(true);
                                MasksSettings[j]->widgetitem->setSelected(false);
                                MasksTreeWidget->scrollToItem(MasksSettings[j]->widgetitem);
                                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
                                RefreshOneMaskItem(MasksSettings[j]->widgetitem, j);
                                MasksTreeWidget->setUpdatesEnabled(true);
                                RefreshMasksBoxes();
                                return;
                            }
                }
        }
    }
    MasksTreeWidget->setUpdatesEnabled(true);
}

void MainWindowImpl::on_MaskAdd_pressed()
{
    QString s;
    QTextStream(&s) << "Mask " << MaxUsedMask + 1;
    Mask *m = new Mask(s);
    MasksSettings.append(m);

    QStringList strings;
    strings.append("");
    strings.append(MasksSettings[MaxUsedMask + 1]->Name);
    strings.append("");
    strings.append("");
    strings.append("");
    strings.append("");
    strings.append("");

    QTreeWidgetItem *item = new QTreeWidgetItem(strings);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    MasksTreeWidget->addTopLevelItem(item);

    MasksSettings[MaxUsedMask + 1]->widgetitem = item; //store pointer
    RefreshOneMaskItem(item, MaxUsedMask + 1);
    MaxUsedMask++;
    RefreshMasksBoxes();

    //create colours, BG first

}

void MainWindowImpl::on_MaskDelete_pressed()
{
    if (MaxUsedMask == 0)
    {
        Message("Can't delete the last mask!");
        return;
    }
    QList <int> list;
    for (int i = 0;  i <= MaxUsedMask; i++)
    {
        if ((MasksSettings[i]->widgetitem) != nullptr)
            if ((MasksSettings[i]->widgetitem)->isSelected())
            {
                list.append(i);
            }
    }
    if (list.count() > 0)
    {
        DeleteMaskDialogImpl dialog(list);
        dialog.exec();

        CleanseOO();
        ShowImage(graphicsView);
        RefreshMasks();
        RefreshMasksBoxes();
        ResetUndo();
    }
}

void MainWindowImpl::on_SegmentsTreeWidget_pressed(QModelIndex index)
{
    Q_UNUSED(index);
    return; //not used
}

void MainWindowImpl::on_SegmentsTreeWidget_doubleClicked(QModelIndex index)
{
    Q_UNUSED(index);
    return; //not used
}


void MainWindowImpl::on_actionLock_selected_segments_triggered()
{
    for (int i = 0;  i < SegmentCount; i++)
    {
        if ((Segments[i]->widgetitem) != nullptr)
            if ((Segments[i]->widgetitem)->isSelected())
            {
                Segments[i]->Locked = true;
                RefreshOneSegmentItem(Segments[i]->widgetitem, i);
            }
    }
}

void MainWindowImpl::on_actionUnlock_selected_segments_triggered()
{
    for (int i = 0;  i < SegmentCount; i++)
    {
        if ((Segments[i]->widgetitem) != nullptr) if ((Segments[i]->widgetitem)->isSelected())
            {
                Segments[i]->Locked = false;
                RefreshOneSegmentItem(Segments[i]->widgetitem, i);
            }
    }
}

void MainWindowImpl::on_actionActivate_selected_segments_triggered()
{
    for (int i = 0;  i < SegmentCount; i++)
    {
        if ((Segments[i]->widgetitem) != nullptr) if ((Segments[i]->widgetitem)->isSelected())
            {
                Segments[i]->Activated = true;
                RefreshOneSegmentItem(Segments[i]->widgetitem, i);
            }
    }
    ShowImage(graphicsView);
}


void MainWindowImpl::on_actionDeactivate_selected_segments_triggered()
{
    for (int i = 0;  i < SegmentCount; i++)
    {
        if ((Segments[i]->widgetitem) != nullptr) if ((Segments[i]->widgetitem)->isSelected())
            {
                Segments[i]->Activated = false;
                RefreshOneSegmentItem(Segments[i]->widgetitem, i);
            }
    }
    ShowImage(graphicsView);
}



void MainWindowImpl::on_SegmentMoveUp_pressed()
{
    //Move selected segment up list - code from masks equivalent
    SegmentsTreeWidget->setUpdatesEnabled(false);

    if ((SegmentsTreeWidget->selectedItems()).count() != 1)
        Message("Select a SINGLE segment");
    else
    {
        for (int i = 0;  i < SegmentCount; i++)
        {
            if ((Segments[i]->widgetitem) != nullptr)
                if ((Segments[i]->widgetitem)->isSelected())
                {
                    //found selected
                    //swap with one above in list
                    QTreeWidgetItem *search = SegmentsTreeWidget->itemAbove(Segments[i]->widgetitem);
                    if (search)
                        for (int j = 0;  j < SegmentCount; j++)
                            if ((Segments[j]->widgetitem) == search)
                            {
                                //found it
                                int temp = Segments[i]->ListOrder;
                                QTreeWidgetItem *temp2 = Segments[i]->widgetitem;
                                Segments[i]->ListOrder = Segments[j]->ListOrder;
                                Segments[i]->widgetitem = Segments[j]->widgetitem;

                                Segments[j]->ListOrder = temp;
                                Segments[j]->widgetitem = temp2;

                                //now do it in the widget
                                SegmentsTreeWidget->setFocus(Qt::OtherFocusReason);
                                Segments[i]->widgetitem->setSelected(true);
                                Segments[j]->widgetitem->setSelected(false);
                                SegmentsTreeWidget->scrollToItem(Segments[j]->widgetitem);
                                RefreshOneSegmentItem(Segments[i]->widgetitem, i);
                                RefreshOneSegmentItem(Segments[j]->widgetitem, j);
                                SegmentsTreeWidget->setUpdatesEnabled(true);
                                RefreshSegmentsBoxes();
                                return;
                            }
                }
        }
    }
    SegmentsTreeWidget->setUpdatesEnabled(false);
}

void MainWindowImpl::on_SegmentMoveDown_pressed()
{
    //Move selected segment down list - code from masks equivalent
    SegmentsTreeWidget->setUpdatesEnabled(false);

    if ((SegmentsTreeWidget->selectedItems()).count() != 1)
        Message("Select a SINGLE segment");
    else
    {
        for (int i = 0;  i < SegmentCount; i++)
        {
            if ((Segments[i]->widgetitem) != nullptr)
                if ((Segments[i]->widgetitem)->isSelected())
                {
                    //found selected
                    //swap with one above in list
                    QTreeWidgetItem *search = SegmentsTreeWidget->itemBelow(Segments[i]->widgetitem);
                    if (search)
                        for (int j = 0;  j < SegmentCount; j++)
                            if ((Segments[j]->widgetitem) == search)
                            {
                                //found it
                                int temp = Segments[i]->ListOrder;
                                QTreeWidgetItem *temp2 = Segments[i]->widgetitem;
                                Segments[i]->ListOrder = Segments[j]->ListOrder;
                                Segments[i]->widgetitem = Segments[j]->widgetitem;

                                Segments[j]->ListOrder = temp;
                                Segments[j]->widgetitem = temp2;

                                //now do it in the widget
                                SegmentsTreeWidget->setFocus(Qt::OtherFocusReason);
                                Segments[i]->widgetitem->setSelected(true);
                                Segments[j]->widgetitem->setSelected(false);
                                SegmentsTreeWidget->scrollToItem(Segments[j]->widgetitem);
                                RefreshOneSegmentItem(Segments[i]->widgetitem, i);
                                RefreshOneSegmentItem(Segments[j]->widgetitem, j);
                                SegmentsTreeWidget->setUpdatesEnabled(true);
                                RefreshSegmentsBoxes();
                                return;
                            }
                }
        }
    }
    SegmentsTreeWidget->setUpdatesEnabled(false);
}


//These are menu equivs to my buttons in mask/seg toolboxes - passthrough slots
void MainWindowImpl::on_actionNew_mask_triggered()
{
    on_MaskAdd_pressed();
}
void MainWindowImpl::on_actionDelete_selected_mask_s_triggered()
{
    on_MaskDelete_pressed();
}
void MainWindowImpl::on_actionCreate_new_segment_triggered()
{
    on_SegmentAdd_pressed();
}
void MainWindowImpl::on_actionDelete_selected_segments_triggered()
{
    on_SegmentDelete_pressed();
}

void MainWindowImpl::on_actionSettings_triggered()
{
    SettingsImpl Dialog;
    Dialog.exec();
    BuildRecentFiles();
}

void MainWindowImpl::on_actionHistorgram_triggered(bool checked)
{
    if (checked) // show the toolbox
        DockHist->setVisible(true);
    else                                // hide it
        DockHist->setVisible(false);

    MenuHistChecked = checked;
}

void MainWindowImpl::on_actionHistogram_shows_selected_triggered(bool checked)
{
    MenuHistSelectedOnly = checked;
    ShowImage(graphicsView);
}


void MainWindowImpl::on_actionCurve_markers_as_crosses_triggered()
{
    cmac();
}




void MainWindowImpl::on_actionLock_curve_shape_triggered(bool checked)
{
    CurveShapeLocked = checked;
}




void MainWindowImpl::on_actionNew_Output_Object_triggered()
{
    on_OONew_clicked();
}

void MainWindowImpl::on_actionNew_Output_Object_Group_triggered()
{
    on_OOGroup_clicked();
}

void MainWindowImpl::on_actionDelete_Output_Object_triggered()
{
    on_OODelete_clicked();
}



void MainWindowImpl::on_actionInfo_triggered(bool checked)
{
    if (checked) // show the toolbox
        DockInfo->setVisible(true);
    else                                // hide it
        DockInfo->setVisible(false);

    MenuInfoChecked = checked;
}


void MainWindowImpl::on_actionHidden_masks_locked_for_generation_triggered()
{
    if (actionHidden_masks_locked_for_generation->isChecked())
        HiddenMasksLockedForGeneration = true;
    else HiddenMasksLockedForGeneration = false;
}

void MainWindowImpl::on_actionSegment_brush_applies_masks_triggered()
{
    if (actionSegment_brush_applies_masks->isChecked()) SegmentBrushAppliesMasks = true;
    else SegmentBrushAppliesMasks = false;
}



void MainWindowImpl::on_CheckBoxRangeSelectedOnly_toggled(bool checked)
{
    RangeSelectedOnly = checked;
}

void MainWindowImpl::on_CheckBoxRangeHardFill_toggled(bool checked)
{
    RangeHardFill = checked;
}



void MainWindowImpl::on_tabWidget_currentChanged(int index)
{
    if (index == 2) GenInvert->setVisible(false);
    else GenInvert->setVisible(true);
}

void MainWindowImpl::on_actionChange_downsampling_triggered()
{
    if (!Active) return;
    resampleImpl dialog;
    dialog.exec();
}

void MainWindowImpl::on_actionAb_out_triggered()
{
    DialogAboutImpl d;
    d.exec();
}



void MainWindowImpl::on_actionTEST_triggered()
{

    foreach (OutputObject *o, OutputObjects)
    {
        o->ComponentSegments.append(1);
    }
    RefreshOO();
}
