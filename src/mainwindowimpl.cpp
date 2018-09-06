#include <QAction>
#include <QColor>
#include <QCursor>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneEvent>
#include <QImageWriter>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>
#include <QProgressBar>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#include <QTime>
#include <QUrl>
#include <QVBoxLayout>

#include <math.h>
#include <qbitmap.h>

#include "about.h"
#include "globals.h"
#include "mainwindowimpl.h"
#include "version.h"

#define PI 3.14159265
#define TOLERANCE 20
#define ALPHA(pointer, offset) pointer[offset+3]
#define RED(pointer, offset) pointer[offset+2]
#define GREEN(pointer, offset) pointer[offset+1]
#define BLUE(pointer, offset) pointer[offset]

MainWindowImpl *TheMainWindow;

// Constructor
MainWindowImpl::MainWindowImpl(QWidget *parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
{
    //Set up initial variables
    setupUi(this);
    setWindowIcon(QIcon (":/alignicon.png"));

    QString version;
    version.sprintf("%d.%d.%d", MAJORVERSION, MINORVERSION, PATCHVERSION);
    setWindowTitle(QString(PRODUCTNAME) + " v" + version);

    showMaximized();

    filesDirectoryString = "";
    scene = new CustomScene;
    graphicsView->setScene(scene);
    graphicsView->setMouseTracking(true);
    sceneRectangle = scene->sceneRect();
    currentImage = -1;
    cropping = 0;
    markersUp = 0;
    cropUp = 0;
    infoChecked = 0;
    markersLocked = 0;
    rectPointer = NULL;
    amRectPointer = NULL;
    autoMarkersGroup = NULL;
    setupFlag = 0;
    verbose = 0;

    //Marker docking window
    markersDialogue = new QDockWidget("Markers (F1)", this);
    markersDialogue->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    markersDialogue->setFeatures(QDockWidget::DockWidgetMovable);
    markersDialogue->setFeatures(QDockWidget::DockWidgetFloatable);

    //Features to go in docking window
    markerList = new QListWidget(markersDialogue);

    QLabel *title = new QLabel("Marker Colour");

    red = new QDial(markersDialogue);
    red->setNotchesVisible(true);
    red->setMaximum(255);
    red->setSingleStep(2);
    QLabel *redLabel = new QLabel("Red");

    green = new QDial(markersDialogue);
    green->setNotchesVisible(true);
    green->setMaximum(255);
    green->setSingleStep(2);
    QLabel *greenLabel = new QLabel("Green");

    blue = new QDial(markersDialogue);
    blue->setNotchesVisible(true);
    blue->setMaximum(255);
    blue->setSingleStep(2);
    QLabel *blueLabel = new QLabel("Blue");

    connect(red, SIGNAL(valueChanged(int)), this, SLOT(changeRed(int) ));
    connect(green, SIGNAL(valueChanged(int)), this, SLOT(changeGreen(int) ));
    connect(blue, SIGNAL(valueChanged(int)), this, SLOT(changeBlue(int) ));

    QLabel *sizeLabel = new QLabel("Marker size:");
    QLabel *thicknessLabel = new QLabel("Line thickness:");
    mThickness = new QSpinBox;
    mSize = new QSpinBox;
    mThickness->setMinimum(1);
    mSize->setMinimum(1);

    swapButton = new QPushButton("&Toggle shape", markersDialogue);
    addMarker = new QPushButton("Add marker", markersDialogue);
    removeMarker = new QPushButton("Remove marker", markersDialogue);
    QPushButton *align = new QPushButton("AM: &Align", markersDialogue);
    grid = new QPushButton("AM: &Edit Grid", markersDialogue);
    grid->setCheckable(true);

    lockMarkers = new QPushButton("&Lock markers", markersDialogue);
    lockMarkers->setCheckable(true);

    autoMarkers = new QPushButton("Auto markers", markersDialogue);
    autoMarkers->setCheckable(true);

    //Initial setup of Markers and markerlist
    for (int i = 0; i < 5; i++) {
        MarkerData *append = new MarkerData(new QRectF((qreal)(i * 20), (qreal)(i * 20), 10., 10.), 0);
        markers.append(append);
        QString output;
        output.sprintf("Marker - %d", (i + 1));
        markerList->addItem(output);
    }

    //Set up layout for dialogue, adding list and dials
    markerLayout = new QVBoxLayout;

    horizontalLayout1 = new QHBoxLayout;
    horizontalLayout1->addWidget(sizeLabel);
    horizontalLayout1->addWidget(mSize);

    horizontalLayout2 = new QHBoxLayout;
    horizontalLayout2->addWidget(thicknessLabel);
    horizontalLayout2->addWidget(mThickness);

    horizontalLayout6 = new QHBoxLayout;
    horizontalLayout6->addWidget(addMarker);
    horizontalLayout6->addWidget(removeMarker);

    horizontalLayout7 = new QHBoxLayout;
    horizontalLayout7->addWidget(swapButton);
    horizontalLayout7->addWidget(lockMarkers);

    horizontalLayout8 = new QHBoxLayout;
    horizontalLayout8->addWidget(align);
    horizontalLayout8->addWidget(grid);

    markerLayout->addWidget(markerList);

    markerLayout->addLayout(horizontalLayout6);
    markerLayout->addLayout(horizontalLayout7);
    markerLayout->addWidget(autoMarkers);
    markerLayout->addLayout(horizontalLayout8);

    markerLayout->addLayout(horizontalLayout1);
    markerLayout->addLayout(horizontalLayout2);

    markerLayout->addWidget(title);
    markerLayout->addWidget(redLabel);
    markerLayout->addWidget(red);

    markerLayout->addWidget(greenLabel);
    markerLayout->addWidget(green);

    markerLayout->addWidget(blueLabel);
    markerLayout->addWidget(blue);

    //Dockwidget has inbuilt protected layout, so apply layout to widget and addMarker this docker
    //Can't use set widget for more than one widget
    layoutWidgetOne = new QWidget;
    layoutWidgetOne->setLayout(markerLayout);
    layoutWidgetOne->setMaximumWidth(500);
    markersDialogue->setWidget(layoutWidgetOne);

    //Add docker but hide until needed
    addDockWidget(Qt::RightDockWidgetArea, markersDialogue);
    markersDialogue->hide();

    //Set initial RGB values to white
    red->setValue(255);
    green->setValue(255);
    blue->setValue(255);

    //Set up Crop window
    cropDock = new QDockWidget("Crop (F2)", this);
    cropDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    cropDock->setFeatures(QDockWidget::DockWidgetMovable);
    cropDock->setFeatures(QDockWidget::DockWidgetFloatable);

    cropLayout = new QVBoxLayout;

    QLabel *cropLabel = new QLabel("Crop Box Dimensions:");

    horizontalLayout4 = new QHBoxLayout;
    QLabel *cropWidthLabel = new QLabel("Width:");
    cropWidth = new QSpinBox;
    cropWidth->setMaximum(6000);
    horizontalLayout4->addWidget(cropWidthLabel);
    horizontalLayout4->addWidget(cropWidth);

    horizontalLayout5 = new QHBoxLayout;
    QLabel *cropHeightLabel = new QLabel("Height:");
    cropHeight = new QSpinBox;
    cropHeight->setMaximum(6000);
    horizontalLayout5->addWidget(cropHeightLabel);
    horizontalLayout5->addWidget(cropHeight);

    cropLayout->addWidget(cropLabel);
    cropLayout->addLayout(horizontalLayout4);
    cropLayout->addLayout(horizontalLayout5);

    QLabel *cropLabel2 = new QLabel("Crop Box Colour:");
    cropLayout->addWidget(cropLabel2);

    //Can't use same dials as for markers so new below - change to value of RGB constants when switch to crop mode
    red2 = new QDial(markersDialogue);
    red2->setNotchesVisible(true);
    red2->setMaximum(255);
    red2->setSingleStep(2);
    QLabel *redLabel2 = new QLabel("Red");

    green2 = new QDial(markersDialogue);
    green2->setNotchesVisible(true);
    green2->setMaximum(255);
    green2->setSingleStep(2);
    QLabel *greenLabel2 = new QLabel("Green");

    blue2 = new QDial(markersDialogue);
    blue2->setNotchesVisible(true);
    blue2->setMaximum(255);
    blue2->setSingleStep(2);
    QLabel *blueLabel2 = new QLabel("Blue");

    connect(red2, SIGNAL(valueChanged(int)), this, SLOT(changeRed(int) ));
    connect(green2, SIGNAL(valueChanged(int)), this, SLOT(changeGreen(int) ));
    connect(blue2, SIGNAL(valueChanged(int)), this, SLOT(changeBlue(int) ));

    cropLayout->addWidget(redLabel2);
    cropLayout->addWidget(red2);

    cropLayout->addWidget(greenLabel2);
    cropLayout->addWidget(green2);

    cropLayout->addWidget(blueLabel2);
    cropLayout->addWidget(blue2);

    QPushButton *executeCrop = new QPushButton("Crop", cropDock);

    cropLayout->addWidget(executeCrop);

    layoutWidgetThree = new QWidget;
    layoutWidgetThree->setLayout(cropLayout);
    layoutWidgetThree->setMaximumWidth(500);
    cropDock->setWidget(layoutWidgetThree);

    addDockWidget(Qt::RightDockWidgetArea, cropDock);
    cropDock->hide();

    //Set up Info Window
    info = new QDockWidget("Info(F9)", this);
    info->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    info->setFeatures(QDockWidget::DockWidgetMovable);
    info->setFeatures(QDockWidget::DockWidgetFloatable);

    infoLayout = new QVBoxLayout;

    horizontalLayout3 = new QHBoxLayout;
    QLabel *infoCat = new QLabel("Mode:\nPosition:\nImage:\nImage Dimensions:\n");
    infoLabel = new QLabel("TextLabel");
    horizontalLayout3->addWidget(infoCat);
    horizontalLayout3->addWidget(infoLabel);

    infoLayout->addLayout(horizontalLayout3);

    //Text box
    notes = new QPlainTextEdit();

    infoLayout->addWidget(notes);

    layoutWidgetTwo = new QWidget;
    layoutWidgetTwo->setLayout(infoLayout);
    layoutWidgetTwo->setMaximumWidth(500);
    layoutWidgetTwo->setMaximumHeight(200);
    info->setWidget(layoutWidgetTwo);

    addDockWidget(Qt::RightDockWidgetArea, info);
    info->hide();

    //Auto align window
    autoAlign = new QDockWidget("Auto Align (F3)", this);
    autoAlign->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    autoAlign->setFeatures(QDockWidget::DockWidgetMovable);
    autoAlign->setFeatures(QDockWidget::DockWidgetFloatable);

    autoLayout = new QVBoxLayout;

    setupAlign = new QPushButton("Setup", autoAlign);
    setupAlign->setCheckable(true);
    autoLayout->addWidget(setupAlign);

    executeAlign = new QPushButton("Align", autoAlign);
    autoLayout->addWidget(executeAlign);

    fileList = new QListWidget(autoAlign);
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    autoLayout->addWidget(fileList);

    resetImage = new QPushButton("Reset Image", autoAlign);
    autoLayout->addWidget(resetImage);

    layoutWidgetFive = new QWidget;
    layoutWidgetFive->setLayout(autoLayout);
    autoAlign->setWidget(layoutWidgetFive);
    autoAlign->setMaximumWidth(500);
    addDockWidget(Qt::LeftDockWidgetArea, autoAlign);
    autoAlign->hide();

    //AutoMarkers options tab
    aMOptions = new QDockWidget("Auto Markers Grid Options", this);
    aMOptions->setFeatures(QDockWidget::DockWidgetFloatable);

    QLabel *Am1 = new QLabel("Grid: Top Left X");
    aMTopLeftX = new QSpinBox;
    aMTopLeftX->setMinimum(1);

    QLabel *Am2 = new QLabel("Grid: Top Left Y");
    aMTopLeftY = new QSpinBox;
    aMTopLeftY->setMinimum(1);

    QLabel *Am3 = new QLabel("Grid: Width");
    aMWidth = new QSpinBox;
    aMWidth->setMinimum(1);
    aMWidth->setMaximum(10000);

    QLabel *Am4 = new QLabel("Grid: Height");
    aMHeight = new QSpinBox;
    aMHeight->setMinimum(1);
    aMHeight->setMaximum(10000);

    QLabel *Am5 = new QLabel("Line Thickness");
    aMThickness = new QSpinBox;
    aMThickness->setMinimum(1);

    QLabel *Am6 = new QLabel("Number Of Horizontal Gridlines");
    aMHoriz = new QSpinBox;
    aMHoriz->setMinimum(0);

    QLabel *Am7 = new QLabel("Number Of Vertical Gridlines");
    aMVert = new QSpinBox;
    aMVert->setMinimum(0);

    QPushButton *ok = new QPushButton("&OK", aMOptions);

    aMGridLayout = new QGridLayout;

    aMGridLayout->addWidget(Am1, 1, 1);
    aMGridLayout->addWidget(aMTopLeftX, 1, 2);
    aMGridLayout->addWidget(Am2, 2, 1);
    aMGridLayout->addWidget(aMTopLeftY, 2, 2);
    aMGridLayout->addWidget(Am3, 3, 1);
    aMGridLayout->addWidget(aMWidth, 3, 2);
    aMGridLayout->addWidget(Am4, 4, 1);
    aMGridLayout->addWidget(aMHeight, 4, 2);
    aMGridLayout->addWidget(Am6, 5, 1);
    aMGridLayout->addWidget(aMHoriz, 5, 2);
    aMGridLayout->addWidget(Am7, 6, 1);
    aMGridLayout->addWidget(aMVert, 6, 2);
    aMGridLayout->addWidget(Am5, 7, 1);
    aMGridLayout->addWidget(aMThickness, 7, 2);

    aMVertLayout = new QVBoxLayout;
    aMVertLayout->addLayout(aMGridLayout);
    aMVertLayout->addWidget(ok);

    layoutWidgetFour = new QWidget;
    layoutWidgetFour->setLayout(aMVertLayout);
    aMOptions->setWidget(layoutWidgetFour);

    aMOptions->setFloating(true);
    aMOptions->move(10, 10);

    aMOptions->hide();

    //Build recently used files list
    readSuperGlobals();
    buildRecentFiles();

    TheMainWindow = this;

    //Link custom slots to signals for the widgets in dockers
    connect(markerList, SIGNAL(itemSelectionChanged () ), this, SLOT(selectMarker() ));
    connect(addMarker, SIGNAL(clicked () ), this, SLOT(addMarkerSlot() ));
    connect(removeMarker, SIGNAL(clicked () ), this, SLOT(removeMarkerSlot() ));
    connect(swapButton, SIGNAL(clicked ()), this, SLOT(changeShape() ));
    connect(mThickness, SIGNAL(valueChanged(int)), this, SLOT(selectMarker() ));
    connect(mSize, SIGNAL(valueChanged(int)), this, SLOT(changeMarkerSize(int) ));
    connect(lockMarkers, SIGNAL(clicked()), this, SLOT(markersLockToggled() ));
    connect(autoMarkers, SIGNAL(clicked()), this, SLOT(autoMarkersToggled() ));
    connect(align, SIGNAL(clicked()), this, SLOT(autoMarkersAlign() ));
    connect(grid, SIGNAL(clicked()), this, SLOT(autoMarkersGrid () ));
    connect(executeCrop, SIGNAL(clicked ()), this, SLOT(on_actionCrop_triggered() ));
    connect(cropWidth, SIGNAL(valueChanged(int)), this, SLOT(resizeCropW(int) ));
    connect(cropHeight, SIGNAL(valueChanged(int)), this, SLOT(resizeCropH(int) ));
    connect(ok, SIGNAL(clicked ()), this, SLOT(okClicked() ));
    connect(executeAlign, SIGNAL(clicked ()), this, SLOT(executeAlignTriggered() ));
    connect(resetImage, SIGNAL(clicked ()), this, SLOT(on_actionReset_Image_triggered() ));
    connect(setupAlign, SIGNAL(clicked ()), this, SLOT(setupAlignTriggered() ));
    connect(aMTopLeftX, SIGNAL(valueChanged(int)), this, SLOT(aMTopLeftXChanged(int) ));
    connect(aMTopLeftY, SIGNAL(valueChanged(int)), this, SLOT(aMTopLeftYChanged(int) ));
    connect(aMWidth, SIGNAL(valueChanged(int)), this, SLOT(aMWidthChanged(int) ));
    connect(aMHeight, SIGNAL(valueChanged(int)), this, SLOT(aMHeightChanged(int) ));
    connect(aMThickness, SIGNAL(valueChanged(int)), this, SLOT(aMThicknessChanged(int) ));
    connect(aMHoriz, SIGNAL(valueChanged(int)), this, SLOT(aMHorizChanged(int) ));
    connect(aMVert, SIGNAL(valueChanged(int)), this, SLOT(aMVertChanged(int) ));

    //Keyboard shortcuts impossible in designer
    actionShift_Up->setShortcut(Qt::Key_Up);
    actionShift_Down->setShortcut(Qt::Key_Down);
    actionShift_Up_Less->setShortcut(Qt::SHIFT + Qt::Key_Up);
    actionShift_Down_Less->setShortcut(Qt::SHIFT + Qt::Key_Down);
    actionMove_Up->setShortcut(Qt::ALT + Qt::Key_Up);
    actionMove_Down->setShortcut(Qt::ALT + Qt::Key_Down);
    actionRotate_Anticlockwise_Less->setShortcut(Qt::SHIFT + Qt::Key_1);
    actionRotate_Clockwise_Less_2->setShortcut(Qt::SHIFT + Qt::Key_0);
    actionShrink_Less->setShortcut(Qt::SHIFT + Qt::Key_BracketLeft);
    actionEnlarge_Less->setShortcut(Qt::SHIFT + Qt::Key_BracketRight);
    QKeySequence sc("CTRL+SHIFT+0");
    actionRotate_Clockwise_More->setShortcut(sc);

}

//Reads values from the regsitry.
void readSuperGlobals()
{
    //Clear recentfilelist just in case already exists
    recentFileList.clear();
    QString t1;
    //New settings to be read from the registry
    QSettings settings("Mark Sutton", "SPIERSalign 2.0");
    int size = settings.beginReadArray("RecentFiles");
    //Read files from registry
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString rf;
        rf = settings.value("fileName").toString();
        recentFileList.append(rf);
    }
    settings.endArray();
}

//Write values to registry - only called in destructor as recentfiles is stored while program runs
void writeSuperGlobals()
{
    //New settings to be written
    QSettings settings("Mark Sutton", "SPIERSalign 2.0");
    settings.beginWriteArray("RecentFiles");
    int loop;
    if (recentFileList.size() > 20) loop = 20;
    else loop = recentFileList.size();

    for (int i = 0; i < loop; ++i) {
        //RFileList written into setting array & registry
        settings.setArrayIndex(i);
        settings.setValue("fileName", recentFileList.at(i));
    }
    settings.endArray();
}

//Moves a file to the top of recentFileList
void recentFile(QString fname)
{
    int n;
    //move this file to the top of the list
    for (n = 0; n < recentFileList.count(); n++) {
        if (recentFileList[n] == fname) {
            recentFileList.removeAt(n);
            break;
        }
    }
    QString rf(fname);
    recentFileList.prepend(rf);
    TheMainWindow->buildRecentFiles();
}


void showInfo(int x, int y)
{
    QString out;
    QTextStream o(&out);
    int lastsep;

    QLabel *label = infoLabel;

    if (markersLocked == 1 && autoMarkersUp == 0)o << "Markers locked\n";
    else if (markersLocked == 1 && autoMarkersUp == 1)o << "Auto markers\n";
    else if (cropUp == 1)o << "Crop\n";
    else if (TheMainWindow->actionPropogate_Mode->isChecked())o << "Propogate Mode\n";
    else if (TheMainWindow->actionLock_Forward->isChecked())o << "Forward Lock\n";
    else if (TheMainWindow->actionLock_Back->isChecked())o << "Backward Lock\n";
    else o << "Normal Align Mode\n";


    if (x < 0 || x > TheMainWindow->width || y < 0 || y > TheMainWindow->height )o << " (---, ---)\n";
    else o << "(" << x << "," << y << ")\n";

    QString fn = imageList[currentImage]->fileName;
    lastsep = fn.lastIndexOf("/");
    fn = fn.mid(lastsep + 1);

    o << fn << "\n";

    o << TheMainWindow->width << " x " << TheMainWindow->height << " pixels.\n";

    label->setText(out);
}


//Rebuilds GUI recent file list, called when list is changed
void MainWindowImpl::buildRecentFiles()
{

    int lastsep;
    QString name;
    //Delete current menu items under recent
    QList <QAction *> currentactions = menuOpen_RecentFile->actions();
    foreach (QAction *thisact, currentactions) {
        //Remove from menu
        menuOpen_RecentFile->removeAction(thisact);
        if (thisact->text() != "More...") {
            //Disconnect
            thisact->disconnect();
            //Delete object
            delete thisact;
        }
    }
    //Add all
    foreach (QString rf, recentFileList) {
        //Last separator in path
        lastsep = rf.lastIndexOf("/");
        name = rf.mid(lastsep + 1);
        QAction *recentFileAction = new QAction(name, this);
        recentFileAction->setStatusTip(rf);
        connect(recentFileAction, SIGNAL(triggered()), this, SLOT(openRecentFile()));
        menuOpen_RecentFile->addAction(recentFileAction);
    }

    QAction *ClearList = new QAction("Clear List", this);
    connect(ClearList, SIGNAL(triggered()), this, SLOT(clearList()));

    menuOpen_RecentFile->addSeparator();
    menuOpen_RecentFile->addAction(ClearList);

}

//Clear RUF list
void MainWindowImpl::clearList()
{
    recentFileList.clear();
    writeSuperGlobals();
    TheMainWindow->buildRecentFiles();
}

//Open a file from the recently used files list slot
void MainWindowImpl::openRecentFile()
{
    //Get pointer to the relevant action
    QAction *action = qobject_cast<QAction *>(sender());
    //Bodge to extract name stored in status tip
    QString fname = action->statusTip();
    fullSettingsFileName = fname;
    on_actionOpen_triggered();
}

//Select marker (selection changed) slot
void MainWindowImpl::selectMarker()
{
    if (markersLocked == 1)return;
    if (selectedMarker < 0)selectedMarker = 0;
    selectedMarker = markerList->currentRow();
    redrawJustDecorations();
}

//Change marker size slot
void MainWindowImpl::changeMarkerSize(int size)
{
    if (markersLocked == 1)return;
    if (selectedMarker < 0)selectedMarker = 0;
    QSizeF newSize((qreal)size, (qreal)size);
    for (int i = 0; i < markers.count(); i++) {
        markers[i]->markerRect->setSize(newSize);
    }
    redrawJustDecorations();
}

void MainWindowImpl::markersLockToggled()
{
    if (lockMarkers->isChecked() == true) {

        markersLocked = 1;
        markerList->clearSelection();
        markerList->setEnabled(false);
        mThickness->setEnabled(false);
        mSize->setEnabled(false);
        addMarker->setEnabled(false);
        removeMarker->setEnabled(false);
        swapButton->setEnabled(false);
        redrawImage();
        showInfo(-1, -1);
    } else                              {

        markersLocked = 0;
        markerList->setEnabled(true);
        mThickness->setEnabled(true);
        mSize->setEnabled(true);
        addMarker->setEnabled(true);
        removeMarker->setEnabled(true);
        swapButton->setEnabled(true);
        redrawImage();
        showInfo(-1, -1);
    }
}

void MainWindowImpl::autoMarkersToggled()
{
    if (autoMarkers->isChecked() == true) {
        lockMarkers->toggle();
        lockMarkers->setEnabled(false);
        markerList->clearSelection();
        markersLockToggled();
        autoMarkersUp = 1;
        showInfo(-1, -1);
        int x = width / 8;
        int y = height / 8;

        gridOutline = new QRect(x, y, width - (2 * x), height - (2 * y));
        autoMarkersGroup = new QGraphicsItemGroup();

        redrawImage();

        if (aMTopLeftX->value() < 2 && aMTopLeftY->value() < 2) {
            aMTopLeftX->setMaximum(width - 50);
            aMTopLeftY->setMaximum(height - 50);
            aMWidth->setValue(gridOutline->width());
            aMHeight->setValue(gridOutline->height());
            aMHoriz->setValue(5);
            aMVert->setValue(5);
            aMThickness->setValue(2);
            aMTopLeftX->setValue(gridOutline->x());
            aMTopLeftY->setValue(gridOutline->y());
        }
    }

    else {
        grid->setChecked(false);
        autoMarkersGrid ();
        aMOptions->hide();
        lockMarkers->toggle();
        lockMarkers->setEnabled(true);
        autoMarkersUp = 0;
        showInfo(-1, -1);
        delete gridOutline;
        delete autoMarkersGroup;
        autoMarkersGroup = NULL;
        markersLockToggled();
        linePointers.clear();
        QApplication::restoreOverrideCursor();
        redrawImage();
    }
}

void MainWindowImpl::okClicked()
{
    grid->setChecked(false);
    autoMarkersGrid ();
}


void MainWindowImpl::setupAlignTriggered()
{
    if (setupAlign->isChecked() == true) {
        if (cropUp == 1) {
            setupAlign->setChecked(false);
            return;
        }
        if (autoMarkers->isChecked() == true)autoMarkers->setChecked(false);
        markersDialogue->setEnabled(false);
        executeAlign->setDisabled(true);
        if (autoEdgeOne == NULL)autoEdgeOne = new QRect((width / 4), (height / 4), 500, 100);
        if (autoEdgeTwo == NULL)autoEdgeTwo = new QRect((width - width / 4), (height / 4), 100, 500);
        setupFlag = 1;
        redrawImage();
        QMessageBox::about(0, "Auto-align Setup",
                           "Please drag, rotate and resize both boxes so each contains 3/4 of one edge on all the slices you wish to align. The edges should be a minimum of 45 degrees apart. When complete click the Setup button again.");
    } else {
        double angle = atan2(setupM.m12(), setupM.m11());
        double angle2 = atan2(setupM2.m12(), setupM2.m11());
        if (fabs(angle) > 1.13 || fabs(angle2) > 1.13) {
            QMessageBox::warning(0, "Error", "Autoalign only works if the horizontal box remains sub-horizontal, and the vertical sub-vertical. Please change the setup so this is the case. ", QMessageBox::Ok);
            setupAlign->setChecked(true);
            return;
        }
        markersDialogue->setEnabled(true);
        executeAlign->setDisabled(false);
        setupFlag = 0;
        redrawImage();

        QMessageBox notice;
        notice.addButton(QMessageBox::Ok);
        QPushButton *verboseMode = notice.addButton(tr("Verbose Mode"), QMessageBox::ActionRole);
        notice.setText("Now you're ready to align - select the slices you would like to be aligned in the Auto Align panel, click align, and will be aligned to the current image (that you are viewing). This may take a few minutes. (Choose VM for verbose mode, largely for testing - this will slow progress down).");
        notice.exec();
        if (notice.clickedButton() == verboseMode)verbose = 1;
        else verbose = 0;
    }
}

void MainWindowImpl::executeAlignTriggered()
{
    if (cropUp == 1) {
        setupAlign->setChecked(false);
        return;
    }
    if (autoEdgeOne == NULL || autoEdgeTwo == NULL) {
        QMessageBox::warning(0, "Error", "Please define edge regions by clicking the Setup on the Auto Align panel.", QMessageBox::Ok);
        return;
    }

//Current image a+bx and next imabe a+bx for 2 edges on each....
    double CIa1 = 0., CIa2 = 0., CIb1 = 0., CIb2 = 0.;
    double NIa1 = 0., NIa2 = 0., NIb1 = 0., NIb2 = 0.;
    int CI = currentImage;

//Work out selection from list
    int start = -1, end = -1;
    for (int i = 0; i < fileList->count(); i++) {
        if (fileList->item(i)->isSelected()) {
            if (start == -1)start = i;
            end = i;
        }
    }
//Make sure selectionis valid
    if (start == -1 || end == -1) {
        QMessageBox::about(0, "Auto-align error", "Please select - at minimum - a single slice to align");
        return;
    }
    if (start == end && start == currentImage) {
        QMessageBox::about(0, "Auto-align error", "You only have the current image - against which others are aligned - selected. Please select some to align");
        return;
    }

//Work out edges on currentImage - then for loop to compare rest to this.
    redrawImage();
    QImage alignImage = pixMapPointer->pixmap().toImage();
    if (alignImage.isGrayscale() == true)alignImage = pixMapPointer->pixmap().toImage().convertToFormat(QImage::Format_RGB32);

    int alignHeight = alignImage.height();
    int alignWidth = alignImage.width();


///////Horizontal edge
    int numberPoints = (autoEdgeOne->width()) / 5;
    int mappedI, mappedK, mappedJ;
    int warning = 0;
    int n = -1;
    int points[5000][2];

    QColor colorRGB;

    QBrush brush(Qt::NoBrush);
    QPen marker;
    marker.setCosmetic(true);
    QColor colour(redValue, greenValue, blueValue);
    marker.setWidth(3);
    marker.setStyle(Qt::SolidLine);
    marker.setColor(colour);

//Move along horizontal edge 5 at a time
    for (int i = autoEdgeOne->left(); i < autoEdgeOne->right(); i = i + 5) {
        n++;
        int biggestDiff = 0;
        //Move down pixels for each 'vertical test line' find maximum change from summed pixels above and summed below
        for (int j = autoEdgeOne->top(); j < autoEdgeOne->bottom(); j++) {
            //Variables for summing RGB levels of pixels above and below...
            int totalRabove = 0, totalGabove = 0, totalBabove = 0, totalRbelow = 0, totalGbelow = 0, totalBbelow = 0, totalDiff = 0;
            //For each pixel asses difference in blocks of ten above and below
            //if((j-10)<0||(j+10)>alignHeight){QMessageBox::warning(0,"Error","Please move the horizontal edge box a minimum of ten pixels away from the edge of the image.", QMessageBox::Ok);return;}

            //Reminder:j is y, i is x. Cool?
            for (int k = (j - 10); k <= j; k++) {
                setupM.map(i, k, &mappedI, &mappedK);
                if (mappedK < 0 && warning == 0) {
                    QMessageBox::warning(0, "Error",
                                         "The horizontal AutoAlign setup box is less than ten pixels from the upper image boundary. we'll deal with this, but if the align doesn't work try moving it further from the edge of the image.",
                                         QMessageBox::Ok);
                    warning = 1;
                }
                if (mappedI < 0 || mappedI > alignWidth) {
                    QMessageBox::warning(0, "Error", "The horizontal AutoAlign setup box goes off the image at the left or right. Please fix.", QMessageBox::Ok);
                    return;
                }
                if (mappedK < 0)mappedK = 0;
                colorRGB = alignImage.pixel(mappedI, mappedK);
                totalRabove += colorRGB.red();
                totalGabove += colorRGB.green();
                totalBabove += colorRGB.blue();
            }

            for (int k = j; k <= (j + 10); k++) {
                setupM.map(i, k, &mappedI, &mappedK);
                if (mappedK > alignHeight && warning == 0) {
                    QMessageBox::warning(0, "Error",
                                         "The horizontal AutoAlign setup box is less than ten pixels from the lower image boundary. we'll deal with this, but if the align doesn't work try moving it further from the edge of the image.",
                                         QMessageBox::Ok);
                    warning = 1;
                }
                if (mappedI < 0 || mappedI > alignWidth) {
                    QMessageBox::warning(0, "Error", "The horizontal AutoAlign setup box goes off the image at the left or right. Please fix.", QMessageBox::Ok);
                    return;
                }
                if (mappedK > alignHeight)mappedK = alignHeight - 1;
                colorRGB = alignImage.pixel(mappedI, mappedK);
                totalRbelow += colorRGB.red();
                totalGbelow += colorRGB.green();
                totalBbelow += colorRGB.blue();
            }

            int rDifference = abs(totalRbelow - totalRabove);
            int bDifference = abs(totalBbelow - totalBabove);
            int gDifference = abs(totalBbelow - totalBabove);
            totalDiff = rDifference + bDifference + gDifference;

            //Save point here unmapped so sort them by how far they are off line i.e. by x or y prior to rotating htem to match line
            if (totalDiff > biggestDiff) {
                biggestDiff = totalDiff;
                points[n][0] = i;
                points[n][1] = j;
            }
        }

    }

//All points of maximum contrast based on RGB values stored in array --> do shit
//Sort data (insertion sort)
    int x, y, mark;
    for (int l = 1; l < numberPoints; l++) {
        y = points[l][1];
        x = points[l][0];
        mark = 0;
        while (y < points[mark][1] && mark < l) mark++;
        if (mark < l) {
            for (int m = l; m > mark; m--) {
                points[m][0] = points[m - 1][0];
                points[m][1] = points[m - 1][1];

            }
            points[mark][0] = x;
            points[mark][1] = y;
        }
    }

//Tolerance = defined at top - percentage of points that we lop off top and bottom when creating a straight line. ATM this is 20.
    float startAt = (((double)TOLERANCE / 100.) * (float)numberPoints);
    float endAt = (numberPoints - (((double)TOLERANCE / 100.) * (float)numberPoints));

//Best fit line of these points (20%-80% middle of range, which is sorted horizontally)
    double Sx = 0., Sy = 0., Sxx = 0., Sxy = 0., delta = 0.;
    int number = 0;
    for (int l = (int)startAt; l < (int)endAt; l++) {
        //Now map the points to the matrix...
        setupM.map(points[l][0], points[l][1], &mappedI, &mappedJ);
        //QGraphicsItem *tempPointer3=scene->addEllipse(mappedI,mappedJ,2.,2.,marker);
        //tempPointer3->setZValue(1);
        //Then do equations
        Sx += mappedI;
        Sy += mappedJ;
        Sxx = Sxx + (mappedI * mappedI);
        Sxy = Sxy + (mappedJ * mappedI);
        number++;
    }

    delta = ((double)number * Sxx) - (Sx * Sx);

    double a = (Sxx * Sy - Sx * Sxy) / delta;
    double b = ((double)number * Sxy - Sx * Sy) / delta;

//Everything else should be corrected according to this...
    CIa1 = a;
    CIb1 = b;

//Display lines to test
//y=a+bx
    double x1, x2, y1, y2;
    x1 = alignWidth - (alignWidth / 4);
    y1 = a + (b * x1);
    x2 = (alignWidth / 4);
    y2 = a + (b * x2);
    QGraphicsLineItem *tempPointer(scene->addLine(x1, y1, x2, y2, marker));
    tempPointer->setZValue(1);

////////Vertical edge...
    numberPoints = (autoEdgeTwo->height()) / 5;
    warning = 0;
    n = -1;
    int points2[5000][2];

    for (int i = autoEdgeTwo->top(); i < autoEdgeTwo->bottom(); i = i + 5) {
        n++;
        int biggestDiff = 0;
        for (int j = autoEdgeTwo->right(); j > autoEdgeTwo->left(); j--) {
            int totalRleft = 0, totalGleft = 0, totalBleft = 0, totalRright = 0, totalGright = 0, totalBright = 0;
            int totalDiff = 0;
            //This time j is x, i is y. Cool?
            for (int k = (j - 10); k <= j; k++) {
                setupM2.map(k, i, &mappedK, &mappedI);
                if (mappedK < 0 && warning == 0) {
                    QMessageBox::warning(0, "Error",
                                         "The vertical AutoAlign setup box is less than ten pixels from the upper image boundary. we'll deal with this, but if the align doesn't work try moving it further from the edge of the image.",
                                         QMessageBox::Ok);
                    warning = 1;
                }
                if (mappedI < 0 || mappedI > alignHeight) {
                    QMessageBox::warning(0, "Error", "The vertical AutoAlign setup box goes off the image at the left or right. Please fix.", QMessageBox::Ok);
                    return;
                }
                if (mappedK < 0)mappedK = 0;
                colorRGB = alignImage.pixel(mappedK, mappedI);
                totalRleft += colorRGB.red();
                totalGleft += colorRGB.green();
                totalBleft = +colorRGB.blue();
            }
            for (int k = j; k <= (j + 10); k++) {
                setupM2.map(k, i, &mappedK, &mappedI);
                if (mappedK > alignWidth && warning == 0) {
                    QMessageBox::warning(0, "Error",
                                         "The vertical AutoAlign setup box is less than ten pixels from the upper image boundary. we'll deal with this, but if the align doesn't work try moving it further from the edge of the image.",
                                         QMessageBox::Ok);
                    warning = 1;
                }
                if (mappedI < 0 || mappedI > alignHeight) {
                    QMessageBox::warning(0, "Error", "The vertical AutoAlign setup box goes off the image at the left or right. Please fix.", QMessageBox::Ok);
                    return;
                }
                if (mappedK > alignWidth)mappedK = alignWidth - 1;
                colorRGB = alignImage.pixel(mappedK, mappedI);
                totalRright += colorRGB.red();
                totalGright += colorRGB.green();
                totalBright = +colorRGB.blue();
            }

            int rDifference = abs(totalRleft - totalRright);
            int bDifference = abs(totalBleft - totalBright);
            int gDifference = abs(totalBleft - totalBright);
            totalDiff = rDifference + bDifference + gDifference;

            if (totalDiff > biggestDiff) {
                biggestDiff = totalDiff;
                points2[n][0] = j; //x
                points2[n][1] = i; //y
            }

        }
    }


    //Sort data as per above... This time knock out outliers in x, not y (this sorted by x too...)
    for (int l = 1; l < numberPoints; l++) {
        y = points2[l][1];
        x = points2[l][0];
        mark = 0;
        while (x < points2[mark][0] && mark < l) mark++;
        if (mark < l) {
            for (int m = l; m > mark; m--) {
                points2[m][0] = points2[m - 1][0];
                points2[m][1] = points2[m - 1][1];

            }
            points2[mark][0] = x;
            points2[mark][1] = y;
        }
    }

    startAt = (((double)TOLERANCE / 100.) * (float)numberPoints);
    endAt = (numberPoints - (((double)TOLERANCE / 100.) * (float)numberPoints));

    number = 0;
//Best fit line of that point
    Sx = 0., Sy = 0., Sxx = 0., Sxy = 0., delta = 0.;
    for (int l = (int)startAt; l < (int)endAt; l++) {
        setupM2.map(points2[l][0], points2[l][1], &mappedI, &mappedJ);
        Sx += mappedI;
        Sy += mappedJ;
        Sxx = Sxx + (mappedI * mappedI);
        Sxy = Sxy + (mappedJ * mappedI);
        number++;
    }

    delta = ((double)number * Sxx) - (Sx * Sx);

    a = (Sxx * Sy - Sx * Sxy) / delta;
    b = ((double)number * Sxy - Sx * Sy) / delta;

    if (a != a || b != b) {
        QMessageBox::warning(0, "NaN", "It would appear that one of your control lines is perfectly vertical - if so please rotate so this is not the case. If not, please email.", QMessageBox::Ok);
        return;
    }

    CIa2 = a;
    CIb2 = b;

//Work out corner/point at which line cross: x=(CIa1-CIa2)/(CIb2-CIb1);
    float CIcornerX, CIcornerY;

    CIcornerX = (CIa1 - CIa2) / (CIb2 - CIb1);
    CIcornerY = CIa1 + (CIb1 * CIcornerX);

//Display lines to test
//y=a+bx
    y1 = alignHeight - (alignHeight / 4);
    x1 = (y1 - a) / b;
    y2 = (alignHeight / 4);
    x2 = (y2 - a) / b;
    QGraphicsLineItem *tempPointer5 = scene->addLine(x1, y1, x2, y2, marker);
    tempPointer5->setZValue(1);
    QGraphicsItem *tempPointer6 = scene->addEllipse(CIcornerX, CIcornerY, 4., 4., marker);
    tempPointer6->setZValue(1);

//Check that this has worked on control image. Not much use otherwise...
    if ((QMessageBox::question(0, "Control edges found",
                               "Are the edges correctly marked on the image? This is the image all others will be aligned to. If the edges are incorrect cancel, and try another slice, or adjusting the setup boxes.",
                               QMessageBox::Ok, QMessageBox::Cancel)) == 4194304) {
        redrawImage();
        return;
    }

//Create progress bar
    QProgressBar progress;
    progress.setRange (start, end);
    progress.setAlignment(Qt::AlignHCenter);
    statusbar->addPermanentWidget(&progress);

    for (int z = start; z <= end; z++) {
        if (z != CI) {
            progress.setValue(z);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

            QString output = QString("Aligning - Processing %1/%2").arg(z - start).arg(end - start);

            currentImage = z;
            redrawImage();

            statusbar->showMessage(output);
            //pixmap pointer = current scene image
            alignImage = pixMapPointer->pixmap().toImage();
            warning = 0;
            n = -1;
            //Move along horizontal edge 5 at a time
            for (int i = autoEdgeOne->left(); i < autoEdgeOne->right(); i = i + 5) {
                n++;
                int biggestDiff = 0;
                //Move down pixels for each 'vertical test line' find maximum change from summed pixels above and summed below
                for (int j = autoEdgeOne->top(); j < autoEdgeOne->bottom(); j++) {
                    //Variables for summing RGB levels of pixels above and below...
                    int totalRabove = 0, totalGabove = 0, totalBabove = 0, totalRbelow = 0, totalGbelow = 0, totalBbelow = 0, totalDiff = 0;
                    //For each pixel asses difference in blocks of ten above and below
                    //if((j-10)<0||(j+10)>alignHeight){QMessageBox::warning(0,"Error","Please move the horizontal edge box a minimum of ten pixels away from the edge of the image.", QMessageBox::Ok);return;}

                    //Reminder:j is y, i is x. Cool?
                    for (int k = (j - 10); k <= j; k++) {
                        setupM.map(i, k, &mappedI, &mappedK);
                        if (mappedK < 0)mappedK = 0;
                        colorRGB = alignImage.pixel(mappedI, mappedK);
                        totalRabove += colorRGB.red();
                        totalGabove += colorRGB.green();
                        totalBabove += colorRGB.blue();
                    }
                    warning = 0;
                    for (int k = j; k <= (j + 10); k++) {
                        setupM.map(i, k, &mappedI, &mappedK);
                        if (mappedK > alignHeight)mappedK = alignHeight - 1;
                        colorRGB = alignImage.pixel(mappedI, mappedK);
                        totalRbelow += colorRGB.red();
                        totalGbelow += colorRGB.green();
                        totalBbelow += colorRGB.blue();
                    }

                    int rDifference = abs(totalRbelow - totalRabove);
                    int bDifference = abs(totalBbelow - totalBabove);
                    int gDifference = abs(totalBbelow - totalBabove);
                    totalDiff = rDifference + bDifference + gDifference;

                    //Save point here unmapped so sort them by how far they are off line i.e. by x or y prior to rotating htem to match line
                    if (totalDiff > biggestDiff) {
                        biggestDiff = totalDiff;
                        points[n][0] = i;
                        points[n][1] = j;
                    }

                }

            }


            //All points of maximum contrast based on RGB values stored in array --> do shit
            //Sort data (insertion sort)
            int x, y, mark;
            for (int l = 1; l < numberPoints; l++) {
                y = points[l][1];
                x = points[l][0];
                mark = 0;
                while (y < points[mark][1] && mark < l) mark++;
                if (mark < l) {
                    for (int m = l; m > mark; m--) {
                        points[m][0] = points[m - 1][0];
                        points[m][1] = points[m - 1][1];

                    }
                    points[mark][0] = x;
                    points[mark][1] = y;
                }
            }

            //Tolerance = defined at top - percentage of points that we lop off top and bottom when creating a straight line. ATM this is 20.
            float startAt = (((double)TOLERANCE / 100.) * (float)numberPoints);
            float endAt = (numberPoints - (((double)TOLERANCE / 100.) * (float)numberPoints));

            //Best fit line of these points (20%-80% middle of range, which is sorted horizontally)
            double Sx = 0., Sy = 0., Sxx = 0., Sxy = 0., delta = 0.;
            number = 0;
            for (int l = (int)startAt; l < (int)endAt; l++) {
                //Now map the points to the matrix...
                setupM.map(points[l][0], points[l][1], &mappedI, &mappedJ);
                //Then do equations
                Sx += mappedI;
                Sy += mappedJ;
                Sxx = Sxx + (mappedI * mappedI);
                Sxy = Sxy + (mappedJ * mappedI);
                number++;
            }

            delta = ((double)number * Sxx) - (Sx * Sx);

            a = (Sxx * Sy - Sx * Sxy) / delta;
            b = ((double)number * Sxy - Sx * Sy) / delta;

            //Everything else should be corrected according to this...
            NIa1 = a;
            NIb1 = b;

            if (verbose == 1) {
                //Display lines to test
                //y=a+bx
                //double x1,x2,y1,y2;
                x1 = alignWidth - (alignWidth / 4);
                y1 = a + (b * x1);
                x2 = (alignWidth / 4);
                y2 = a + (b * x2);

                QGraphicsLineItem *tempPointer(scene->addLine(x1, y1, x2, y2, marker));
                tempPointer->setZValue(1);
            }

            ////////Vertical edge...
            numberPoints = (autoEdgeTwo->height()) / 5;
            warning = 0;
            n = -1;
            int points2[5000][2];

            for (int i = autoEdgeTwo->top(); i < autoEdgeTwo->bottom(); i = i + 5) {
                n++;
                int biggestDiff = 0;
                for (int j = autoEdgeTwo->right(); j > autoEdgeTwo->left(); j--) {
                    int totalRleft = 0, totalGleft = 0, totalBleft = 0, totalRright = 0, totalGright = 0, totalBright = 0;
                    int totalDiff = 0;
                    //This time j is x, i is y. Cool?
                    for (int k = (j - 10); k <= j; k++) {
                        setupM2.map(k, i, &mappedK, &mappedI);
                        if (mappedK < 0)mappedK = 0;
                        if (mappedK > alignWidth)mappedK = alignWidth;
                        colorRGB = alignImage.pixel(mappedK, mappedI);
                        totalRleft += colorRGB.red();
                        totalGleft += colorRGB.green();
                        totalBleft = +colorRGB.blue();
                    }
                    for (int k = j; k <= (j + 10); k++) {
                        setupM2.map(k, i, &mappedK, &mappedI);
                        if (mappedK > alignWidth)mappedK = alignWidth;
                        if (mappedK < 0)mappedK = 0;
                        colorRGB = alignImage.pixel(mappedK, mappedI);
                        totalRright += colorRGB.red();
                        totalGright += colorRGB.green();
                        totalBright = +colorRGB.blue();
                    }

                    int rDifference = abs(totalRleft - totalRright);
                    int bDifference = abs(totalBleft - totalBright);
                    int gDifference = abs(totalBleft - totalBright);
                    totalDiff = rDifference + bDifference + gDifference;

                    if (totalDiff > biggestDiff) {
                        biggestDiff = totalDiff;
                        points2[n][0] = j; //x
                        points2[n][1] = i; //y
                    }

                }
            }


            //Sort data as per above... This time knock out outliers in x, not y (this sorted by x too...)
            for (int l = 1; l < numberPoints; l++) {
                y = points2[l][1];
                x = points2[l][0];
                mark = 0;
                while (x < points2[mark][0] && mark < l) mark++;
                if (mark < l) {
                    for (int m = l; m > mark; m--) {
                        points2[m][0] = points2[m - 1][0];
                        points2[m][1] = points2[m - 1][1];

                    }
                    points2[mark][0] = x;
                    points2[mark][1] = y;
                }
            }

            startAt = (((double)TOLERANCE / 100.) * (float)numberPoints);
            endAt = (numberPoints - (((double)TOLERANCE / 100.) * (float)numberPoints));

            number = 0;
            //Best fit line of that point
            Sx = 0., Sy = 0., Sxx = 0., Sxy = 0., delta = 0.;
            for (int l = (int)startAt; l < (int)endAt; l++) {
                setupM2.map(points2[l][0], points2[l][1], &mappedI, &mappedJ);
                Sx += mappedI;
                Sy += mappedJ;
                Sxx = Sxx + (mappedI * mappedI);
                Sxy = Sxy + (mappedJ * mappedI);
                number++;
            }

            delta = ((double)number * Sxx) - (Sx * Sx);

            a = (Sxx * Sy - Sx * Sxy) / delta;
            b = ((double)number * Sxy - Sx * Sy) / delta;

            NIa2 = a;
            NIb2 = b;

            if (a != a
                    || b != b)QMessageBox::warning(0, "NaN",
                                                       "It would appear that one of the lines here is perfectly vertical - Auto Align will skip the image. Please take a note of its number - a small rotation and repeat Auto Align on this slice should fix the problem. If no line appears vertical, please email.",
                                                       QMessageBox::Ok);
            else {
                //Now align the image compared to CIa1 etc. - first rotate
                //First work out corner of this image:

                float NIcornerX, NIcornerY;

                NIcornerX = (NIa1 - NIa2) / (NIb2 - NIb1);
                NIcornerY = NIa1 + (NIb1 * NIcornerX);

                if (verbose == 1) {
                    //Display lines to test
                    //y=a+bx
                    y1 = alignHeight - (alignHeight / 4);
                    x1 = (y1 - a) / b;
                    y2 = (alignHeight / 4);
                    x2 = (y2 - a) / b;

                    QGraphicsLineItem *tempPointer2 = scene->addLine(x1, y1, x2, y2, marker);
                    tempPointer2->setZValue(1);

                    QGraphicsItem *tempPointer4 = scene->addEllipse(NIcornerX, NIcornerY, 4, 4, marker);
                    tempPointer4->setZValue(1);
                }


                ///////////////////////////////////////////////////////
                //Now shift...
                verticalShift(CIcornerY - NIcornerY);
                lateralShift(CIcornerX - NIcornerX);
                //qDebug()<<CIcornerY-NIcornerY<<CIcornerX-NIcornerX;
                redrawImage();

                //Now rotate:
                //Calculate angle...
                /*Not working - try something else entirely.
                double angleBetween=(atanf((NIb1-CIb1)/(1.+(CIb1*NIb1))))*(180./PI);
                double angleBetween2=(atanf((NIb2-CIb2)/(1.+(CIb2*NIb2))))*(180./PI);

                qDebug()<<angleBetween<<angleBetween2;
                qDebug()<<"CI stats:"<<CIa1<<CIb1<<CIa2<<CIb2;
                qDebug()<<"NI stats:"<<NIa1<<NIb1<<NIa2<<NIb2;*/

                //Angle for horizontal line from gradient
                //Use absolue angle to remove quadrant issues - if line near vertical this is a problem.
                /*float angleNI, angleCI;
                angleCI=atan(CIb1);
                angleNI=atan(NIb1);*/

                //Angle for vertical line
                /*float angleNI2, angleCI2;
                angleCI2=atan(CIb2);
                angleNI2=atan(NIb2);*/
                //Still giving quadrant issues, pain in arse to fix. Trying below/

                //Difference between both...
                //double angleBetween=(angleCI-angleNI)*(180./PI);
                //double angleBetween2=(angleCI2-angleNI2)*(180./PI);

                double angleBetween = (atan((NIb1 - CIb1) / (1. + (CIb1 * NIb1)))) * (-180. / PI);
                double angleBetween2 = (atan((NIb2 - CIb2) / (1. + (CIb2 * NIb2)))) * (-180. / PI);

                //qDebug()<<angleBetween<<angleBetween2<<(atan((NIb1-CIb1)/(1.+(CIb1*NIb1))))*(-180./PI)<<(atan((NIb2-CIb2)/(1.+(CIb2*NIb2))))*(-180./PI);

                //Stop it giving 176 rather than 4, for example.
                //if(angleBetween>150)angleBetween=180-angleBetween;
                //if(angleBetween2>150)angleBetween2=180-angleBetween2;

                //Average
                double avangle = (angleBetween + angleBetween2) / 2;
                if (avangle > 70.)avangle = 0.;

                if (verbose == 1) {
                    QString lineInfo =
                        QString("Horizontal line - y=%1x + %2, vertical line - y=%3x + %4. Control image horizontal line - y=%5x + %6, vertical - y=%7x + %8. Intersection x=%9, y=%10 (Control x=%11, y=%12) Lateral shift=%13. Vertical shift=%14. Rotation=%15.").arg(
                            NIb1, 0, 'g', 4).arg(NIa1, 0, 'g', 4).arg(NIb2, 0, 'g', 4).arg(NIa2, 0, 'g', 4).arg(CIb1, 0, 'g', 4).arg(CIa1, 0, 'g', 4).arg(CIb2, 0, 'g', 4).arg(CIa2, 0, 'g', 4).arg(NIcornerX, 0, 'g',
                                    4).arg(NIcornerY, 0, 'g', 4).arg(CIcornerX, 0, 'g', 4).arg(CIcornerY, 0, 'g', 4).arg(CIcornerX - NIcornerX, 0, 'g', 4).arg(CIcornerY - NIcornerY, 0, 'g', 4).arg(avangle, 0, 'g', 4);
                    QMessageBox::question(this, "Line found", lineInfo, QMessageBox::Ok);
                }

                //Apply rotation
                QImage Rotate(imageList[currentImage]->fileName);

                //Apply by drawing QImage with painter - allows for shifts.
                QImage ToDraw(width, height, QImage::Format_RGB32);
                QPainter paint;
                paint.begin(&ToDraw);

                //Transform here so rotates around middle - allows translate
                paint.setWorldTransform(imageList[currentImage]->m);
                paint.setRenderHint(QPainter::SmoothPixmapTransform);
                paint.translate((CIcornerX), (CIcornerY));
                paint.rotate(avangle);
                paint.translate(-(CIcornerX), -(CIcornerY));
                imageList[currentImage]->m = paint.worldTransform();
                QPointF leftCorner(0.0, 0.0);

                paint.drawImage(leftCorner, Rotate);
                paint.end();

                QString savename = imageList[currentImage]->fileName + ".xxx";
                if (imageList[currentImage]->format == 0)ToDraw.save(savename, "BMP", 100);
                if (imageList[currentImage]->format == 1)ToDraw.save(savename, "JPG", 100);
                if (imageList[currentImage]->format == 2)ToDraw.save(savename, "PNG", 50);

                /*rotate(angleBetween);


                //The shift
                double midX=((double)alignWidth/2);
                double midY=((double)alignHeight/2);

                //Vertical shift
                double VS=((CIa1+(CIb1*midX))-(NIa1+(NIb1*midX)));

                //Lateral shift
                //y=a+bx --> (y-a)/b=x
                double LS=((midY-CIa2)/CIb2)-((midY-NIa2)/NIb2);

                verticalShift(VS);
                lateralShift(LS);*/

                redrawImage();
            }//ends the else...
        }//end the if
    }//ends the for
    currentImage = CI;
    redrawImage();
}//ends the function

void MainWindowImpl::autoMarkersGrid ()
{
    if (!autoMarkers->isChecked()) {
        grid->setChecked(false);
        return;
    }
    if (grid->isChecked() == true) aMOptions->show();
    if (grid->isChecked() == false) aMOptions->hide();

}

void MainWindowImpl::aMTopLeftXChanged(int value)
{
    if (value < 2 || aMTopLeftY->value() < 2)return;
    gridOutline->setRect(value, aMTopLeftY->value(), aMWidth->value(), aMHeight->value());
    redrawJustAM();
}

void MainWindowImpl::aMTopLeftYChanged(int value)
{
    if (value < 2 || aMTopLeftX->value() < 2)return;
    gridOutline->setRect(aMTopLeftX->value(), value, aMWidth->value(), aMHeight->value());
    redrawJustAM();
}

void MainWindowImpl::aMWidthChanged(int value)
{
    gridOutline->setWidth(value);
    redrawJustAM();
}

void MainWindowImpl::aMHeightChanged(int value)
{
    gridOutline->setHeight(value);
    redrawJustAM();
}

void MainWindowImpl::aMThicknessChanged(int value)
{
    Q_UNUSED(value);
    redrawJustAM();
}

void MainWindowImpl::aMHorizChanged(int value)
{
    Q_UNUSED(value);
    redrawJustAM();
}

void MainWindowImpl::aMVertChanged(int value)
{
    Q_UNUSED(value);
    redrawJustAM();
}


void MainWindowImpl::autoMarkersAlign()
{
    if (aM.isIdentity() || autoMarkersUp == 0)return;

    QTransform aMi;

    if (aM.isInvertible())aMi = aM.inverted();
    else   {
        QMessageBox::warning(0, "Error", "Matrix is not invertible. You should never see this, please email", QMessageBox::Ok);
        return;
    }

    //If current image has already been moved need to fix
    if (!imageList[currentImage]->m.isIdentity()) {
        //Scaling
        qreal m11 = imageList[currentImage]->m.m11();
        //Translation
        qreal m31 = imageList[currentImage]->m.m31();
        qreal m32 = imageList[currentImage]->m.m32();
        //Shearing
        qreal m12 = imageList[currentImage]->m.m12();

        long double angle = atan2(m12, m11);

        aMi.rotateRadians(angle);

        aMi.translate(m31, m32);

        /*QString errormessage;
        QTextStream(&errormessage)<<"M21\t"<<imageList[currentImage]->m.m21()<<"M12\t"<<imageList[currentImage]->m.m12();
        QMessageBox::warning(0,"Error",errormessage, QMessageBox::Ok);*/
    }

    imageList[currentImage]->m = aMi;
    QImage OldImage(imageList[currentImage]->fileName);
    QImage ToDraw(width, height, QImage::Format_RGB32);
    QPainter paint;
    paint.begin(&ToDraw);

    paint.setWorldTransform(imageList[currentImage]->m);
    paint.setRenderHint(QPainter::SmoothPixmapTransform);

    QPointF leftCorner(0.0, 0.0);

    paint.drawImage(leftCorner, OldImage);

    paint.end();

    if (OldImage.format() == QImage::Format_Indexed8) {

        QImage tempToDraw(width, height, QImage::Format_Indexed8);

        QVector <QRgb> clut(256);
        for (int i = 0; i < 256; i++)
            clut[i] = qRgb(i, i, i);

        int w4; //this is width rounded up to nearest multiple of 4 - trust me, you need this!
        if ((width % 4) == 0) w4 = width; // no problem with width
        else w4 = (width / 4) * 4 + 4; //round up
        tempToDraw.setColorTable(clut);
        //set up pointers to base of array - enables very fast access to data
        uchar *data1 = (uchar *) ToDraw.bits(); //image is the source (XRGB) image
        uchar *data2 = (uchar *) tempToDraw.bits();

        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                data2[y * w4 + x] = BLUE(data1, 4 * (y * width + x));


        ToDraw = tempToDraw;
    }

    QString savename = imageList[currentImage]->fileName + ".xxx";
    if (imageList[currentImage]->format == 0)ToDraw.save(savename, "BMP", 100);
    if (imageList[currentImage]->format == 1)ToDraw.save(savename, "JPG", 100);
    if (imageList[currentImage]->format == 2)ToDraw.save(savename, "PNG", 50);

    aM.reset();

    redrawImage();



}

//Add marker slot
void MainWindowImpl::addMarkerSlot()
{
    if (markersLocked == 1)return;
    MarkerData *append = new MarkerData(new QRectF((qreal)(10), (qreal)(10), (qreal)mSize->value(), (qreal)mSize->value()), 0);
    markers.append(append);
    QString output;
    output.sprintf("Marker - %d", markers.count());
    markerList->addItem(output);
    markerList->setCurrentRow(markers.count() - 1);
    selectedMarker = (markers.count() - 1);
    redrawJustDecorations();
}

//Remove marker slot
void MainWindowImpl::removeMarkerSlot()
{
    if (markersLocked == 1)return;
    if (markers.count() == 5) {
        QMessageBox::warning(0, "Error", "5 is the minimum number of markers.", QMessageBox::Ok);
        return;
    }

    delete markers[markers.count()];
    markers.removeLast();
    markerList->takeItem((markers.count()));

    redrawImage();
}

//Change marker shape slot
void MainWindowImpl::changeShape()
{
    if (markersLocked == 1)return;
    if (markers[selectedMarker]->shape == 0) {
        markers[selectedMarker]->shape = 1;
        markers[selectedMarker]->linePoint.setX(markers[selectedMarker]->markerRect->x() + 200.);
        markers[selectedMarker]->linePoint.setY(markers[selectedMarker]->markerRect->top());
    } else if (markers[selectedMarker]->shape == 1) {
        markers[selectedMarker]->shape = 0;
    }
    redrawImage();
}

//Red change slot
void MainWindowImpl::changeRed(int value)
{
    redValue = value;
    redrawImage();
}

//Green change slot
void MainWindowImpl::changeGreen(int value)
{
    greenValue = value;
    redrawImage();
}

//Blue change slot
void MainWindowImpl::changeBlue(int value)
{
    blueValue = value;
    redrawImage();
}


//Destructor
MainWindowImpl::~MainWindowImpl()
{
    if (currentImage != -1) {
        writeSuperGlobals();
        actionSave_Backup->trigger();
        int i = 0;

        //Write to settings file
        on_actionSave_triggered();

        qDeleteAll(imageList.begin(), imageList.end());
        imageList.clear();
        linePointers.clear();
        for (i = 0; i < markers.count(); i++) {
            delete markers[i];
        }
        //Should delete all child layouts etc.
        delete markersDialogue;
        delete cropDock;
        delete info;
    }
}

//Open files - set up program and enable menus
void MainWindowImpl::on_actionOpen_triggered()
{
    int i, x, j = 0;
    //Reset all variables and menus so opening another image set is posssible
    if (currentImage != -1) {
        filesDirectoryString = "";
        currentImage = -1;
        rectPointer = NULL;
        amRectPointer = NULL;

        //Write to settings file
        on_actionSave_triggered();

        qDeleteAll(imageList.begin(), imageList.end());
        imageList.clear();
        linePointers.clear();
        qDeleteAll(markers.begin(), markers.end());
        markers.clear();
        markerList->clear();
        QList <QGraphicsItem *> tokill = scene->items();

        for (i = 0; i < tokill.count(); i++) {
            scene->removeItem(tokill[i]);
            delete tokill[i];
        }
        markersDialogue->hide();
        for (i = 0; i < 5; i++) {
            MarkerData *append = new MarkerData(new QRectF((qreal)(i * 20), (qreal)(i * 20), 10., 10.), 0);
            markers.append(append);
            QString output;
            output.sprintf("Marker - %d", (i + 1));
            markerList->addItem(output);
        }

        spinBox->setEnabled(false);
        horizontalSlider->setEnabled(false);
        menuNavigate->setEnabled(false);
        menuLocking_Propagation->setEnabled(false);
        menuWindows->setEnabled(false);
        menuTransform->setEnabled(false);
        menuMagnification->setEnabled(false);
        actionSave->setEnabled(false);
        actionSave_Backup->setEnabled(false);
        actionLoad_Settings_File->setEnabled(false);
        actionCompress_Dataset->setEnabled(false);

        actionSelect_Marker->setEnabled(false);

        actionLock_Forward->setChecked(false);
        actionLock_Back->setChecked(false);
        actionLock_File->setChecked(false);
        actionPropogate_Mode->setChecked(false);

        markersUp = 0;
        infoChecked = 0;
        cropUp = 0;
        markersDialogue->hide();
        info->hide();
        cropDock->hide();
        autoAlign->hide();

        delete (fileList);
        fileList = new QListWidget(autoAlign);
        autoLayout->addWidget(fileList);

        setupM.reset();
        setupM2.reset();
        aM.reset();

        actionCreate_Crop_Area->setChecked(false);
        actionAdd_Markers->setChecked(false);
        actionInfo->setChecked(false);

        notes->clear();

        QString version;
        version.sprintf("%d.%d.%d", MAJORVERSION, MINORVERSION, PATCHVERSION);
        this->setWindowTitle(QString(PRODUCTNAME) + " v" + version);
    }
    currentImage = 0;

    if (fullSettingsFileName.isEmpty()) {
        filesDirectoryString = QFileDialog::getExistingDirectory(this, tr("Select folder containing image files"),
                                                                 "d:/", QFileDialog::ShowDirsOnly);
        if (filesDirectoryString == "") return; //dialogue cancelled
        filesDirectory = filesDirectoryString; //construct directory object
    }

    else {
        filesDirectoryString = fullSettingsFileName;
        filesDirectory = filesDirectoryString;
        fullSettingsFileName = "";
    }

    recentFile(filesDirectoryString);

    QStringList FilterList;
    FilterList << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";
    drectoryFileList = filesDirectory.entryList(FilterList, QDir::Files, QDir::Name);

    if (drectoryFileList.count() == 0) {
        QMessageBox::warning(0, "Error", "No image files in this folder.", QMessageBox::Ok);
        return;
    }


    for (i = 0; i < drectoryFileList.count(); i++) {
        ImageData *newimage = new ImageData(filesDirectoryString + "/" + drectoryFileList[i]); //create data structure
        imageList.append(newimage);
        fileList->addItem(drectoryFileList[i]);
    }

    //Read from settings and apply matrices
    x = imageList.count();
    QString filename = filesDirectory.absolutePath() + "/settings.txt";

    if (QFile::exists(filename) == true) {
        int numberMarkers = 0;


        QFile settings(filename);
        settings.open(QFile::ReadOnly);
        QTextStream read(&settings);
        i = -1;

        while (!read.atEnd()) {
            i++;
            QString line = read.readLine();
            QStringList list = line.split("\t");
            if (i < x) {
                //Check image names have not been modified
                if (!list[0].endsWith(drectoryFileList[i], Qt::CaseInsensitive)) {
                    if ((QMessageBox::question(0, "Error",
                                               "Image sequence has been modified. This will prevent the dataset loading correctly. If this is because you have appended images to the end of the dataset click OK, otherwise click cancel to return.",
                                               QMessageBox::Ok, QMessageBox::Cancel)) == 4194304)return;
                    else {
                        if (list.size() == 1)numberMarkers = line.toInt();
                        else {
                            numberMarkers = list[0].toInt();
                            int size = list[1].toInt();
                            mSize->setValue(size);
                            QSizeF newSize((qreal)size, (qreal)size);
                            for (int i = 0; i < markers.count(); i++)markers[i]->markerRect->setSize(newSize);

                            mThickness->setValue(list[2].toInt());
                            red->setValue(list[3].toInt());
                            green->setValue(list[4].toInt());
                            blue ->setValue(list[5].toInt());
                            currentImage = list[6].toInt();
                            if (currentImage < 0)currentImage = 0;
                        }
                        i = x;
                    }
                } else {
                    qreal m11, m12, m21, m22, mdx, mdy;
                    mdx = list[1].toDouble();
                    mdy = list[2].toDouble();
                    m11 = list[3].toDouble();
                    m22 = list[4].toDouble();
                    m21 = list[5].toDouble();
                    m12 = list[6].toDouble();
                    imageList[i]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);
                    imageList[i]->hidden = list[7].toInt();
                }
            } else if (i == x) {
                //Can take this if out when everyone has settings files written by the new version which save colour etc.
                if (list.size() == 1)numberMarkers = line.toInt();
                else {
                    numberMarkers = list[0].toInt();
                    int size = list[1].toInt();
                    mSize->setValue(size);
                    QSizeF newSize((qreal)size, (qreal)size);
                    for (int i = 0; i < markers.count(); i++)markers[i]->markerRect->setSize(newSize);

                    mThickness->setValue(list[2].toInt());
                    red->setValue(list[3].toInt());
                    green->setValue(list[4].toInt());
                    blue ->setValue(list[5].toInt());
                    currentImage = list[6].toInt();
                    if (currentImage < 0)currentImage = 0;
                    if (list.size() > 7) {
                        if (list[7].toInt() > 0)lockMarkers->toggle();
                        markersLockToggled();
                        actionConstant_autosave->setChecked(list[8].toInt());
                    }

                }
            }

            else if (i > x && i <= (x + 5)) {
                markers[j]->markerRect->moveTo(list[0].toDouble(), list[1].toDouble());
                markers[j]->shape = list[2].toInt();
                markers[j]->linePoint.setX(list[3].toDouble());
                markers[j]->linePoint.setY(list[4].toDouble());
                j++;
            }

            else if (i > (x + 5) && i <= (x + numberMarkers)) {
                MarkerData *append = new MarkerData(new QRectF(list[0].toDouble(), list[1].toDouble(), (qreal)mSize->value(), (qreal)mSize->value()), list[2].toInt());
                markers.append(append);
                QString output;
                output.sprintf("Marker - %d", (j + 1));
                markerList->addItem(output);
                markers[j]->linePoint.setX(list[3].toDouble());
                markers[j]->linePoint.setY(list[4].toDouble());
                j++;
            } else notes->appendPlainText(line);
        }
        settings.flush();
        settings.close();
    } else {
        for (i = 0; i < imageList.count(); i++)imageList[i]->hidden = false;
        mSize->setValue(10);
        mThickness->setValue(5);
    }

    for (i = 0; i < imageList.count(); i++) {
        imageList[i]->format = -1;
        if (imageList[i]->fileName.endsWith(".png", Qt::CaseInsensitive))imageList[i]->format = 2;
        if (imageList[i]->fileName.endsWith(".jpg", Qt::CaseInsensitive) || imageList[i]->fileName.endsWith(".jpeg", Qt::CaseInsensitive))imageList[i]->format = 1;
        if (imageList[i]->fileName.endsWith(".bmp", Qt::CaseInsensitive))imageList[i]->format = 0;
        if (imageList[i]->format == -1) {
            QMessageBox::warning(0, "Error", "Please check extensions - should be either .jpg, .jpeg, .bmp or .png", QMessageBox::Ok);
            return;
        }
    }

    //Set maxima for slider and spin box
    horizontalSlider->setMaximum(drectoryFileList.count());
    horizontalSlider->setTickInterval(drectoryFileList.count() / 20);
    spinBox->setMaximum(drectoryFileList.count());

    //Enable menu
    spinBox->setEnabled(true);
    spinBox->setValue(currentImage + 1);
    horizontalSlider->setEnabled(true);
    menuNavigate->setEnabled(true);
    menuLocking_Propagation->setEnabled(true);
    menuTransform->setEnabled(true);
    menuWindows->setEnabled(true);
    menuMagnification->setEnabled(true);
    actionSave->setEnabled(true);
    actionSave_Backup->setEnabled(true);
    actionLoad_Settings_File->setEnabled(true);
    actionCompress_Dataset->setEnabled(true);

    QImage Dimensions(imageList[currentImage]->fileName);
    width = Dimensions.width();
    height = Dimensions.height();

    currentScale = 1;
    redrawImage();
    on_actionFit_Window_triggered();

    actionAdd_Markers->trigger();
    actionInfo->trigger();
    actionAuto_Align->trigger();
}

void MainWindowImpl::LogText(QString text)
{
    QString filename = filesDirectory.absolutePath() + "/log.txt";
    QFile log(filename);

    log.open(QFile::Append);

    QTextStream write(&log);
    write << text;

    log.close();
}

//Redraw Image
void  MainWindowImpl::redrawImage()
{
    QString Outstring;
    QTextStream out(&Outstring);

    out << "Starting.. Current Image is " << currentImage << "\t";

    LogText(Outstring);

    if (currentImage < 0)return;
    LogText("*1\t");
    //Dismantles group and also destroys group item
    if (autoMarkersGroup != NULL && autoMarkersGroup->scene() != 0) {
        scene->destroyItemGroup(autoMarkersGroup);
        linePointers.clear();
    }
    LogText("*2\t");
    if (rectPointer != NULL) {
        scene->removeItem(rectPointer);
        delete rectPointer;
    }
    LogText("*3\t");
    //Clear old items
    QList <QGraphicsItem *> tokill = scene->items();
    LogText("*4\t");

    for (int i = 0; i < tokill.count(); i++) {
        scene->removeItem(tokill[i]);
        delete tokill[i];
    }

    LogText("*5\t");
    //Ensure only image is scene so when markers are moved off the edge it doesn't introduce unnecessary scrolls
    scene->setSceneRect(0.0, 0.0, width, height);
    //Do it based on height and width of firt image - can do it so each image is nicely centred using newimage.height & width
    //But this means the images are no longer aligned, this prevents proper alignment...
    LogText("*6\t");

    //Title bar

    QString version;
    version.sprintf("%d.%d.%d - ", MAJORVERSION, MINORVERSION, PATCHVERSION);
    QString output = version + imageList[currentImage]->fileName;
    QString output2;
    output2.sprintf(" - (%d/%d)", currentImage + 1, imageList.count());
    this->setWindowTitle(QString(PRODUCTNAME) + " v" + output + output2);

    LogText("*7\t");
    //Rescale view
    graphicsView->resetMatrix ();
    LogText("*8\t");
    graphicsView->scale(currentScale, currentScale);
    LogText("*9\t");
    //Draw image
    QPixmap newimage;
    QString loadname = imageList[currentImage]->fileName + ".xxx";
    if (QFile::exists(loadname) == true)newimage.load(loadname);
    else newimage.load(imageList[currentImage]->fileName);
    LogText("*10\t");
    pixMapPointer = scene->addPixmap(newimage);
    pixMapPointer->setZValue(0);
    LogText("*11\t");
    showInfo(-1, -1);
    if (actionConstant_autosave->isChecked())on_actionSave_triggered();
    LogText("*12\t");
    redrawDecorations();
    LogText("...Done\n");
}

//Redraw crop area and markers
void  MainWindowImpl::redrawDecorations()
{
    QBrush brush(Qt::NoBrush);
    QColor colour(redValue, greenValue, blueValue);
    statusbar->clearMessage();
    if (imageList[currentImage]->hidden == true)statusbar->showMessage("This image is hidden.");
    if (actionPropogate_Mode->isChecked())statusbar->showMessage("propogation mode");
    if (actionLock_Back->isChecked() && !actionPropogate_Mode->isChecked())statusbar->showMessage("Backward File Locking mode");
    if (actionLock_Forward->isChecked() && !actionPropogate_Mode->isChecked())statusbar->showMessage("Forward File Locking mode");

    if (setupFlag == 1) {
        QPen pen;
        pen.setCosmetic(true);
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);
        pen.setColor(colour);

        //rectPointer is removed with other graphics items above - no need to kill here.
        suRectPointer = scene->addRect(*autoEdgeOne, pen, brush);
        suRectPointer2 = scene->addRect(*autoEdgeTwo, pen, brush);
        suRectPointer->setZValue(1);
        suRectPointer2->setZValue(1);
        suRectPointer->setTransform(setupM);
        suRectPointer2->setTransform(setupM2);
    }

    if (autoMarkersUp == 1) {
        autoMarkersGroup = new QGraphicsItemGroup();
        scene->addItem(autoMarkersGroup);

        QPen amPen;
        amPen.setCosmetic(true);
        amPen.setWidth(aMThickness->value());
        amPen.setColor(colour);

        amRectPointer = scene->addRect(*gridOutline, amPen, brush);
        //autoMarkersGroup->addToGroup(scene->addRect(*gridOutline,amPen,brush));

        autoMarkersGroup->addToGroup(amRectPointer);

        //Change 5 below for more or less lines in grid - eventually user set...
        int L = gridOutline->left();
        int R = gridOutline->right();
        int JH = (R - L) / (aMVert->value() + 1);

        int T = gridOutline->top();
        int B = gridOutline->bottom();
        int JV = (B - T) / (aMHoriz->value() + 1);

        //Fix rounding errors so there isn't a last line very close to edge
        int z1 = ((R - L) % (aMVert->value() + 1)) + 1;
        int z2 = ((B - T) % (aMHoriz->value() + 1)) + 1;

        for (int i = L; i < (R - z1); i += JH) {
            QGraphicsLineItem *line = scene->addLine(i, T, i, B, amPen);
            line->setZValue(1);
            autoMarkersGroup->addToGroup(line);
            linePointers.append(line);
        }

        for (int i = T; i < (B - z2); i += JV) {
            QGraphicsLineItem *line = scene->addLine(L, i, R, i, amPen);
            line->setZValue(1);
            autoMarkersGroup->addToGroup(line);
            linePointers.append(line);
        }
        if (actionLock_File->isChecked())autoMarkersGroup->setVisible(false);
        autoMarkersGroup->setZValue(1);
    }

    if (actionCreate_Crop_Area->isChecked() && cropArea != NULL) {

        QPen pen;
        pen.setCosmetic(true);
        pen.setWidth(1);
        pen.setStyle(Qt::DashLine);
        pen.setColor(colour);

        //rectPointer is removed with other graphics items above - no need to kill here.
        rectPointer = scene->addRect(*cropArea, pen, brush);
        rectPointer->setZValue(1);

        if (cropping != 1) {
            QString output = "Crop mode enabled - crop area";
            QString output2;
            output2.sprintf(" - %d pixels wide, %d pixels high)", cropArea->width(), cropArea->height());
            statusbar->showMessage(output + output2);
        }
    }

    if (actionAdd_Markers->isChecked() && setupFlag != 1) {

        QPen marker;

        QColor icolour((255 - redValue), (255 - greenValue), (255 - blueValue));
        marker.setWidth(mThickness->value());
        marker.setStyle(Qt::SolidLine);
        marker.setCosmetic(true);

        for (int i = 0; i < markers.count(); i++) {
            if (i == selectedMarker && markersLocked != 1) marker.setColor(icolour);
            else marker.setColor(colour);
            if (markers[i]->shape == 1)markers[i]->markerPointer = scene->addLine(markers[i]->markerRect->x(), markers[i]->markerRect->top(), markers[i]->linePoint.x(), markers[i]->linePoint.y(), marker);
            else markers[i]->markerPointer = scene->addEllipse(*markers[i]->markerRect, marker, brush);
            if (autoMarkersUp == 1 && !actionLock_File->isChecked())autoMarkersGroup->addToGroup(markers[i]->markerPointer);
            else markers[i]->markerPointer->setZValue(1);
        }
    }

    if (autoMarkersUp == 1)autoMarkersGroup->setTransform(aM);

}

//Redraw Just markers
void  MainWindowImpl::redrawJustDecorations()
{
    if (currentImage < 0)return;
    int i, numberMarkers = markers.count();
    QBrush brush(Qt::NoBrush);
    for (i = 0; i < numberMarkers; i++)if (markers[i]->markerPointer != NULL)scene->removeItem(markers[i]->markerPointer);
    for (i = 0; i < numberMarkers; i++)delete markers[i]->markerPointer;

    QPen marker;
    marker.setCosmetic(true);
    QColor colour(redValue, greenValue, blueValue);
    QColor icolour((255 - redValue), (255 - greenValue), (255 - blueValue));
    marker.setWidth(mThickness->value());
    marker.setStyle(Qt::SolidLine);

    for (i = 0; i < numberMarkers; i++) {
        if (i == selectedMarker && markersLocked != 1)marker.setColor(icolour);
        else marker.setColor(colour);
        if (markers[i]->shape == 1)markers[i]->markerPointer = scene->addLine(markers[i]->markerRect->x(), markers[i]->markerRect->top(), markers[i]->linePoint.x(), markers[i]->linePoint.y(), marker);
        else markers[i]->markerPointer = scene->addEllipse(*markers[i]->markerRect, marker, brush);
        markers[i]->markerPointer->setZValue(1);
    }
}

//Redraw just Auto markers
void  MainWindowImpl::redrawJustAM()
{
    if (currentImage < 0)return;

    if (autoMarkersUp == 1) {
        if (autoMarkersGroup != NULL && autoMarkersGroup->scene() != 0)scene->destroyItemGroup(autoMarkersGroup);
        if (amRectPointer != NULL) {
            scene->removeItem(amRectPointer);
            delete amRectPointer;
        }
        int i, numberMarkers = markers.count();

        for (i = 0; i < numberMarkers; i++)if (markers[i]->markerPointer != NULL)scene->removeItem(markers[i]->markerPointer);
        for (i = 0; i < numberMarkers; i++)delete markers[i]->markerPointer;
        int numberLines = linePointers.count();
        for (i = 0; i < numberLines; i++)if (linePointers[i] != NULL)scene->removeItem(linePointers[i]);
        for (i = 0; i < numberLines; i++)delete linePointers[i];
        linePointers.clear();
        QBrush brush(Qt::NoBrush);
        QColor colour(redValue, greenValue, blueValue);
        statusbar->clearMessage();

        if (imageList[currentImage]->hidden == true)statusbar->showMessage("This image is hidden.");
        if (actionPropogate_Mode->isChecked())statusbar->showMessage("propogation mode");
        if (actionLock_Back->isChecked() && !actionPropogate_Mode->isChecked())statusbar->showMessage("Backward File Locking mode");
        if (actionLock_Forward->isChecked() && !actionPropogate_Mode->isChecked())statusbar->showMessage("Forward File Locking mode");

        autoMarkersGroup = new QGraphicsItemGroup();

        scene->addItem(autoMarkersGroup);

        QPen amPen;
        amPen.setCosmetic(true);
        amPen.setWidth(aMThickness->value());
        amPen.setColor(colour);

        amRectPointer = scene->addRect(*gridOutline, amPen, brush);


        autoMarkersGroup->addToGroup(amRectPointer);

        //Change 5 below for more or less lines in grid - eventually user set...
        int L = gridOutline->left();
        int R = gridOutline->right();
        int JH = (R - L) / (aMVert->value() + 1);

        int T = gridOutline->top();
        int B = gridOutline->bottom();
        int JV = (B - T) / (aMHoriz->value() + 1);

        //Fix rounding errors so there isn't a last line very close to edge
        int z1 = ((R - L) % (aMVert->value() + 1)) + 1;
        int z2 = ((B - T) % (aMHoriz->value() + 1)) + 1;

        for (int i = L; i < (R - z1); i += JH) {
            QGraphicsLineItem *line = scene->addLine(i, T, i, B, amPen);
            line->setZValue(1);
            autoMarkersGroup->addToGroup(line);
            linePointers.append(line);
        }

        for (int i = T; i < (B - z2); i += JV) {
            QGraphicsLineItem *line = scene->addLine(L, i, R, i, amPen);
            line->setZValue(1);
            autoMarkersGroup->addToGroup(line);
            linePointers.append(line);
        }


        QPen marker;
        marker.setCosmetic(true);
        marker.setWidth(mThickness->value());
        marker.setStyle(Qt::SolidLine);
        marker.setColor(colour);

        for (i = 0; i < numberMarkers; i++) {

            if (markers[i]->shape == 1)markers[i]->markerPointer = scene->addLine(markers[i]->markerRect->x(), markers[i]->markerRect->top(), markers[i]->linePoint.x(), markers[i]->linePoint.y(), marker);
            else markers[i]->markerPointer = scene->addEllipse(*markers[i]->markerRect, marker, brush);
            autoMarkersGroup->addToGroup(markers[i]->markerPointer);
        }


        autoMarkersGroup->setZValue(1);

        autoMarkersGroup->setTransform(aM);
    }

    if (setupFlag == 1) {
        if (suRectPointer != NULL) {
            scene->removeItem(suRectPointer);
            delete suRectPointer;
        }
        if (suRectPointer2 != NULL) {
            scene->removeItem(suRectPointer2);
            delete suRectPointer2;
        }

        QBrush brush(Qt::NoBrush);
        QPen pen;
        pen.setCosmetic(true);
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);

        QColor colour(redValue, greenValue, blueValue);
        pen.setColor(colour);

        //rectPointer is removed with other graphics items above - no need to kill here.
        suRectPointer = scene->addRect(*autoEdgeOne, pen, brush);
        suRectPointer2 = scene->addRect(*autoEdgeTwo, pen, brush);
        suRectPointer->setZValue(1);
        suRectPointer2->setZValue(1);
        suRectPointer->setTransform(setupM);
        suRectPointer2->setTransform(setupM2);

    }
}


//Redraw just crop box
void  MainWindowImpl::redrawJustCropBox()
{
    if (cropUp == 0)return;
    if (rectPointer != NULL) {
        scene->removeItem(rectPointer);
        delete rectPointer;
    }
    QColor colour(redValue, greenValue, blueValue);
    QBrush brush(Qt::NoBrush);
    QPen pen;
    pen.setCosmetic(true);
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    pen.setColor(colour);

    rectPointer = scene->addRect(*cropArea, pen, brush);
    rectPointer->setZValue(1);
    QString output = "Crop mode enabled - crop area";
    QString output2;
    output2.sprintf(" - %d pixels wide, %d pixels high)", cropArea->width(), cropArea->height());
    cropWidth->setValue(cropArea->width());
    cropHeight->setValue(cropArea->height());
    statusbar->showMessage(output + output2);
}

void MainWindowImpl::on_actionInfo_triggered(bool checked)
{
    if (checked == true) {
        info->show();
        infoChecked = 1;
    }
    if (checked == false) {
        info->hide();
        infoChecked = 0;
    }
}

void MainWindowImpl::on_actionAuto_Align_triggered (bool checked)
{
    if (checked == true) {
        autoAlign->show();
    }
    if (checked == false) {
        autoAlign->hide();
    }
}

//Setup markers
void MainWindowImpl::on_actionAdd_Markers_triggered(bool checked)
{

    if (checked == true) {
        if (actionCreate_Crop_Area->isChecked()) {

            actionCreate_Crop_Area->setChecked(false);
            delete cropArea;
            cropUp = 0;
            cropDock->hide();
            //If this is not NULLed here still points to cropArea and when you try delete it in redraw image it crashes....
            rectPointer = NULL;
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            redrawImage();
        }

        selectedMarker = 0;
        layoutWidgetTwo->setMaximumHeight(200);
        markersUp = 1;
        red->setValue(redValue);
        green->setValue(greenValue);
        blue->setValue(blueValue);


        markersDialogue->show();
    }

    if (checked == false) {
        markersUp = 0;
        markersDialogue->hide();
    }

    redrawImage();
}

// Select markers
void MainWindowImpl::on_actionSelect_Marker_triggered()
{
    selectedMarker++;
    if (selectedMarker >= markerList->count())selectedMarker = 0;
    markerList->setCurrentRow(selectedMarker);
    redrawImage();
}

//Move selected markers


void MainWindowImpl::on_actionMove_Marker_Left_triggered()
{
    markers[selectedMarker]->markerRect->translate(-2., 0.);
    redrawImage();
}

void MainWindowImpl::on_actionMove_Marker_Right_triggered()
{
    markers[selectedMarker]->markerRect->translate(2., 0.);
    redrawImage();
}

void MainWindowImpl::on_actionMove_Marker_Up_triggered()
{
    markers[selectedMarker]->markerRect->translate(0., -2.);
    redrawImage();
}

void MainWindowImpl::on_actionMove_Marker_Down_triggered()
{
    markers[selectedMarker]->markerRect->translate(0., 2.);
    redrawImage();
}

//Zoom In
void MainWindowImpl::on_actionZoom_In_triggered()
{
    currentScale += .1;
    redrawImage();
}

//Zoom out
void MainWindowImpl::on_actionZoom_Out_triggered()
{
    if (currentScale > .2)currentScale -= .1;
    redrawImage();
}

//Zoom 100%
void MainWindowImpl::on_actionZoom_100_triggered()
{
    currentScale = 1.0;
    redrawImage();
}

//Zoom fit
void MainWindowImpl::on_actionFit_Window_triggered()
{
    graphicsView->fitInView(pixMapPointer, Qt::KeepAspectRatio);
    QTransform m;
    m = graphicsView->transform();
    currentScale = m.m11();
    redrawImage();
}

//Rotate C/W

void MainWindowImpl::on_actionRotate_Clockwise_More_triggered()
{

    rotate(10.);
}

void MainWindowImpl::on_actionRotate_Clockwise_triggered()
{
    rotate(1.);
}

void MainWindowImpl::on_actionRotate_Clockwise_Less_2_triggered()
{
    rotate(.05);
}

//Rotate A/CW
void MainWindowImpl::on_actionRotate_Anticlockwise_More_triggered()
{
    rotate(-10);
}

void MainWindowImpl::on_actionRotate_Anticlockwise_triggered()
{
    rotate(-1.);
}

void MainWindowImpl::on_actionRotate_Anticlockwise_Less_triggered()
{
    rotate(-.05);
}

////Rotate function
void MainWindowImpl::rotate (qreal rotateAngle)
{
    if (actionLock_File->isChecked() || currentImage == -1)return;
    QImage Rotate(imageList[currentImage]->fileName);

    //Apply by drawing QImage with painter - allows for shifts.
    QImage ToDraw(width, height, QImage::Format_RGB32);
    QPainter paint;
    paint.begin(&ToDraw);

    //Transform here so rotates around middle - allows translate
    paint.setWorldTransform(imageList[currentImage]->m);
    paint.setRenderHint(QPainter::SmoothPixmapTransform);
    paint.translate((width / 2), (height / 2));
    paint.rotate(rotateAngle);
    paint.translate(-(width / 2), -(height / 2));
    imageList[currentImage]->m = paint.worldTransform();
    QPointF leftCorner(0.0, 0.0);

    paint.drawImage(leftCorner, Rotate);

    //For propogation mode - record transformations in propogation list.
    if (actionPropogate_Mode->isChecked()) {
        PropogationData *newdata = new PropogationData();
        propogation.append(newdata);
        propogation[propogateStep]->transformation = 1;
        propogation[propogateStep]->value = rotateAngle;
        propogateStep++;
    }

    paint.end();

    if (Rotate.format() == QImage::Format_Indexed8) {

        QImage tempToDraw(width, height, QImage::Format_Indexed8);

        QVector <QRgb> clut(256);
        for (int i = 0; i < 256; i++)
            clut[i] = qRgb(i, i, i);

        int w4; //this is width rounded up to nearest multiple of 4 - trust me, you need this!
        if ((width % 4) == 0) w4 = width; // no problem with width
        else w4 = (width / 4) * 4 + 4; //round up
        tempToDraw.setColorTable(clut);
        //set up pointers to base of array - enables very fast access to data
        uchar *data1 = (uchar *) ToDraw.bits(); //image is the source (XRGB) image
        uchar *data2 = (uchar *) tempToDraw.bits();

        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                data2[y * w4 + x] = BLUE(data1, 4 * (y * width + x));


        ToDraw = tempToDraw;
    }

    QString savename = imageList[currentImage]->fileName + ".xxx";
    if (imageList[currentImage]->format == 0)ToDraw.save(savename, "BMP", 100);
    if (imageList[currentImage]->format == 1)ToDraw.save(savename, "JPG", 100);
    if (imageList[currentImage]->format == 2)ToDraw.save(savename, "PNG", 50);

    redrawImage();
}
////

//Enlarge
void MainWindowImpl::on_actionEnlarge_More_triggered()
{
    resize(1.05);
}

void MainWindowImpl::on_actionEnlarge_triggered()
{
    resize(1.01);
}

void MainWindowImpl::on_actionEnlarge_Less_triggered()
{
    resize(1.001);
}

//Shrink

void MainWindowImpl::on_actionShrink_More_triggered()
{
    resize(0.95);
}

void MainWindowImpl::on_actionShrink_triggered()
{
    resize(0.99);
}

void MainWindowImpl::on_actionShrink_Less_triggered()
{
    resize(0.999);
}

////Enlarge/Shrink Function
void MainWindowImpl::resize(qreal sizeChange)
{
    if (actionLock_File->isChecked() || currentImage == -1)return;
    QImage Enlarge(imageList[currentImage]->fileName);

    //Apply transformation here to allow them to occur around centre point
    QImage ToDraw(width, height, QImage::Format_RGB32);
    QPainter paint;
    paint.begin(&ToDraw);
    paint.setWorldTransform(imageList[currentImage]->m);
    paint.setRenderHint(QPainter::SmoothPixmapTransform);
    paint.translate((width / 2), (height / 2));
    paint.scale(sizeChange, sizeChange);
    paint.translate(-(width / 2), -(height / 2));
    imageList[currentImage]->m = paint.worldTransform();
    QPointF leftCorner(0.0, 0.0);
    paint.drawImage(leftCorner, Enlarge);


    //For propogation mode
    if (actionPropogate_Mode->isChecked()) {
        PropogationData *newdata = new PropogationData();
        propogation.append(newdata);
        propogation[propogateStep]->transformation = 2;
        propogation[propogateStep]->value = sizeChange;
        propogateStep++;
    }

    paint.end();

    if (Enlarge.format() == QImage::Format_Indexed8) {

        QImage tempToDraw(width, height, QImage::Format_Indexed8);

        QVector <QRgb> clut(256);
        for (int i = 0; i < 256; i++)
            clut[i] = qRgb(i, i, i);

        int w4; //this is width rounded up to nearest multiple of 4 - trust me, you need this!
        if ((width % 4) == 0) w4 = width; // no problem with width
        else w4 = (width / 4) * 4 + 4; //round up
        tempToDraw.setColorTable(clut);
        //set up pointers to base of array - enables very fast access to data
        uchar *data1 = (uchar *) ToDraw.bits(); //image is the source (XRGB) image
        uchar *data2 = (uchar *) tempToDraw.bits();

        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                data2[y * w4 + x] = BLUE(data1, 4 * (y * width + x));


        ToDraw = tempToDraw;
    }


    QString savename = imageList[currentImage]->fileName + ".xxx";

    if (imageList[currentImage]->format == 0)ToDraw.save(savename, "BMP", 100);
    if (imageList[currentImage]->format == 1)ToDraw.save(savename, "JPG", 100);
    if (imageList[currentImage]->format == 2)ToDraw.save(savename, "PNG", 50);
    redrawImage();
}
////

//Shift Right

void MainWindowImpl::on_actionShift_Right_Less_triggered()
{
    if (imageList[currentImage]->m.m12() == 0.)rotate(.00001);
    lateralShift(.5);
}

void MainWindowImpl::on_actionShift_Right_triggered()
{
    lateralShift(1.);
}

void MainWindowImpl::on_actionShift_Right_More_triggered()
{
    lateralShift(10.);
}

//Shift Left

void MainWindowImpl::on_actionShift_Left_less_triggered()
{
    if (imageList[currentImage]->m.m12() == 0.)rotate(.00001);
    lateralShift(-.5);
}

void MainWindowImpl::on_actionShift_Left_triggered()
{
    lateralShift(-1);
}

void MainWindowImpl::on_actionShift_Left_More_triggered()
{
    lateralShift(-10);
}

////Lateral shift function
void MainWindowImpl::lateralShift(qreal shiftSize)
{
    if (actionLock_File->isChecked() || currentImage == -1)return;
    //Manually edit matrix to make pure hoizontal movement, not movement in the dX of the matrix
    qreal m11, m12, m21, m22, mdx, mdy;
    mdx = imageList[currentImage]->m.dx();
    mdy = imageList[currentImage]->m.dy();
    m11 = imageList[currentImage]->m.m11();
    m22 = imageList[currentImage]->m.m22();
    m21 = imageList[currentImage]->m.m21();
    m12 = imageList[currentImage]->m.m12();
    mdx += shiftSize;
    imageList[currentImage]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);


    //For propogation mode
    if (actionPropogate_Mode->isChecked()) {
        PropogationData *newdata = new PropogationData();
        propogation.append(newdata);
        propogation[propogateStep]->transformation = 3;
        propogation[propogateStep]->value = shiftSize;
        propogateStep++;
    }


    redrawShift();
    redrawImage();
}

//Shift Up

void MainWindowImpl::on_actionShift_Up_More_triggered()
{
    verticalShift(-10.);
}

void MainWindowImpl::on_actionShift_Up_triggered()
{
    verticalShift(-1.);
}

void MainWindowImpl::on_actionShift_Up_Less_triggered()
{
    verticalShift(-.5);
}

//Shift Down

void MainWindowImpl::on_actionShift_Down_More_triggered()
{
    verticalShift(10.);
}

void MainWindowImpl::on_actionShift_Down_triggered()
{
    verticalShift(1.);
}

void MainWindowImpl::on_actionShift_Down_Less_triggered()
{
    verticalShift(.5);
}

////Vertical shift function
void MainWindowImpl::verticalShift(qreal shiftSize)
{
    if (actionLock_File->isChecked() || currentImage == -1)return;
    QImage Shift(imageList[currentImage]->fileName);

    //Manually edit matrix to make pure hoizontal movement, not movement in the dX of the matrix
    qreal m11, m12, m21, m22, mdx, mdy;
    mdx = imageList[currentImage]->m.dx();
    mdy = imageList[currentImage]->m.dy();
    m11 = imageList[currentImage]->m.m11();
    m22 = imageList[currentImage]->m.m22();
    m21 = imageList[currentImage]->m.m21();
    m12 = imageList[currentImage]->m.m12();
    mdy += shiftSize;
    imageList[currentImage]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);

    //For propogation mode
    if (actionPropogate_Mode->isChecked()) {
        PropogationData *newdata = new PropogationData();
        propogation.append(newdata);
        propogation[propogateStep]->transformation = 4;
        propogation[propogateStep]->value = shiftSize;
        propogateStep++;
    }


    redrawShift();
    redrawImage();
}

//Function to redraw the shifts for translation (both lateral and vertical need this code)
void MainWindowImpl::redrawShift()
{
    QImage Shift(imageList[currentImage]->fileName);
    QImage Shifted(width, height, QImage::Format_RGB32);
    QPainter paint;
    paint.begin(&Shifted);
    paint.setRenderHint(QPainter::SmoothPixmapTransform);
    paint.setWorldTransform(imageList[currentImage]->m);
    //0.0 because matrix applies is in fact mdx,mdy
    QPointF leftCorner(0.0, 0.0);
    paint.drawImage(leftCorner, Shift);

    paint.end();


    if (Shift.format() == QImage::Format_Indexed8) {

        QImage tempToDraw(width, height, QImage::Format_Indexed8);

        QVector <QRgb> clut(256);
        for (int i = 0; i < 256; i++)
            clut[i] = qRgb(i, i, i);

        int w4; //this is width rounded up to nearest multiple of 4 - trust me, you need this!
        if ((width % 4) == 0) w4 = width; // no problem with width
        else w4 = (width / 4) * 4 + 4; //round up
        tempToDraw.setColorTable(clut);
        //set up pointers to base of array - enables very fast access to data
        uchar *data1 = (uchar *) Shifted.bits(); //image is the source (XRGB) image
        uchar *data2 = (uchar *) tempToDraw.bits();

        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
                data2[y * w4 + x] = BLUE(data1, 4 * (y * width + x));


        Shifted = tempToDraw;
    }

    QString savename = imageList[currentImage]->fileName + ".xxx";
    if (imageList[currentImage]->format == 0)Shifted.save(savename, "BMP", 100);
    if (imageList[currentImage]->format == 1)Shifted.save(savename, "JPG", 100);
    if (imageList[currentImage]->format == 2)Shifted.save(savename, "PNG", 50);
}

//Set up to record changes in propogation mode
void MainWindowImpl::on_actionPropogate_Mode_triggered(bool checked)
{
    if (checked == true) {
        if (autoMarkersUp == 1) {
            QMessageBox::warning(0, "Error", "Automarkers doesn't work in propagate mode, please turn off and try again. ", QMessageBox::Ok);
            actionPropogate_Mode->setChecked(false);
            return;
        }


        QMessageBox msgBox;
        msgBox.setText("Entering propagation mode");
        msgBox.setInformativeText("Would you like to propagate forwards (apply changes on all files to the end of the dataset) or backwards (apply changes between here and the start)?");
        QPushButton *backButton = msgBox.addButton("Backwards", QMessageBox::AcceptRole);
        QPushButton *forwardButton = msgBox.addButton("Forward", QMessageBox::AcceptRole);
        QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);

        msgBox.setDefaultButton(forwardButton);

        msgBox.exec();

        if (msgBox.clickedButton() != cancelButton) {
            int flag = 0;
            lockImage = currentImage;

            if (msgBox.clickedButton() == forwardButton) {
                if (currentImage == imageList.count() - 1) {
                    QMessageBox::warning(0, "Error", "I'm afraid it's not possible to propogate forward from the last image - perhaps time for a coffee?", QMessageBox::Ok);
                    actionPropogate_Mode->setChecked(false);
                    return;
                }
                if (currentImage == 0)flag = 1;
                actionApply_Propogation->setEnabled(true);
                actionLock_Forward->setChecked(true);
                actionLock_Back->setChecked(false);
                actionLock_File->setChecked(false);

                actionLock_Forward->setEnabled(false);
                actionLock_Back->setEnabled(false);
                actionLock_File->setEnabled(false);
            }
            if (msgBox.clickedButton() == backButton) {
                if (currentImage == 0) {
                    QMessageBox::warning(0, "Error", "I'm afraid it's not possible to propogate back from the first image - perhaps time for a coffee?", QMessageBox::Ok);
                    actionPropogate_Mode->setChecked(false);
                    return;
                }
                if (currentImage == imageList.count() - 1)flag = 1;
                actionApply_Propogation->setEnabled(true);
                actionLock_Forward->setChecked(false);
                actionLock_Back->setChecked(true);
                actionLock_File->setChecked(true);

                actionLock_Forward->setEnabled(false);
                actionLock_Back->setEnabled(false);
                actionLock_File->setEnabled(false);
            }


            if (flag == 0) {
                horizontalSlider->setMaximum(lockImage + 1);
                spinBox->setMaximum(lockImage + 1);
                horizontalSlider->setMinimum(lockImage);
                spinBox->setMinimum(lockImage);
                propogateStep = 0;
                propogateImage = currentImage;
                redrawImage();
            }
            if (flag == 1) {
                actionLock_File->setChecked(false);
                horizontalSlider->setMaximum(lockImage + 1);
                spinBox->setMaximum(lockImage + 1);
                horizontalSlider->setMinimum(lockImage + 1);
                spinBox->setMinimum(lockImage + 1);
                propogateStep = 0;
                propogateImage = currentImage;
                redrawImage();
            }
        } else {
            actionPropogate_Mode->setChecked(false);
        }
    }

    if (checked == false) {
        actionApply_Propogation->setEnabled(false);
        if (currentImage < 2) {
            horizontalSlider->setEnabled(true);
            spinBox->setEnabled(true);
            horizontalSlider->setEnabled(true);
            spinBox->setEnabled(true);
        }
        qDeleteAll(propogation.begin(), propogation.end());
        propogation.clear();
        redrawImage();
        horizontalSlider->setMaximum(drectoryFileList.count());
        spinBox->setMaximum(drectoryFileList.count());
        horizontalSlider->setMinimum(1);
        spinBox->setMinimum(1);

        actionLock_Forward->setChecked(false);
        actionLock_Back->setChecked(false);
        actionLock_File->setChecked(false);


        actionLock_Forward->setEnabled(true);
        actionLock_Back->setEnabled(true);
        actionLock_File->setEnabled(true);

    }
    showInfo(-1, -1);
}

void MainWindowImpl::on_actionApply_Propogation_triggered()
{
    //Setup progress bar
    QProgressBar progress;
    progress.setRange (propogateImage + 1, imageList.count());
    progress.setAlignment(Qt::AlignHCenter);
    statusbar->addPermanentWidget(&progress);

    if (actionLock_Forward->isChecked()) {
        //Loop through images
        for (int i = (propogateImage + 1); i < imageList.count(); i++) {
            //Update message on status bar & progress bar
            progress.setValue(i);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

            QString output = "propogation - ";// + imageList[i]->fileName;
            QString output2;
            output2.sprintf("Processing (%d/%d)", i + 1, imageList.count());
            statusbar->showMessage(output + output2);

            //Start painter
            QImage ApplyProp(imageList[i]->fileName);
            QImage ToDraw(width, height, QImage::Format_RGB32);
            QPainter paint;
            paint.begin(&ToDraw);
            paint.setRenderHint(QPainter::SmoothPixmapTransform);
            //For each images loop through the transformation list
            for (int j = 0; j < propogation.count(); j++) {
                //Rotate
                if (propogation[j]->transformation == 1) {
                    paint.setWorldTransform(imageList[i]->m);
                    paint.translate((width / 2), (height / 2));
                    paint.rotate(propogation[j]->value);
                    paint.translate(-(width / 2), -(height / 2));
                    imageList[i]->m = paint.worldTransform();
                }
                //Resize
                if (propogation[j]->transformation == 2) {
                    paint.setWorldTransform(imageList[i]->m);
                    paint.translate((width / 2), (height / 2));
                    paint.scale(propogation[j]->value, propogation[j]->value);
                    paint.translate(-(width / 2), -(height / 2));
                    imageList[i]->m = paint.worldTransform();
                }
                //Lateral shift
                if (propogation[j]->transformation == 3) {
                    qreal m11, m12, m21, m22, mdx, mdy;
                    mdx = imageList[i]->m.dx();
                    mdy = imageList[i]->m.dy();
                    m11 = imageList[i]->m.m11();
                    m22 = imageList[i]->m.m22();
                    m21 = imageList[i]->m.m21();
                    m12 = imageList[i]->m.m12();
                    mdx += propogation[j]->value;
                    imageList[i]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);
                }
                //Vertical shift
                if (propogation[j]->transformation == 4) {
                    qreal m11, m12, m21, m22, mdx, mdy;
                    mdx = imageList[i]->m.dx();
                    mdy = imageList[i]->m.dy();
                    m11 = imageList[i]->m.m11();
                    m22 = imageList[i]->m.m22();
                    m21 = imageList[i]->m.m21();
                    m12 = imageList[i]->m.m12();
                    mdy += propogation[j]->value;
                    imageList[i]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);
                }
            }
            //Finally draw and save new file
            paint.setWorldTransform(imageList[i]->m);
            QPointF leftCorner(0.0, 0.0);
            paint.drawImage(leftCorner, ApplyProp);
            paint.end();

            if (ApplyProp.format() == QImage::Format_Indexed8) {

                QImage tempToDraw(width, height, QImage::Format_Indexed8);

                QVector <QRgb> clut(256);
                for (int i = 0; i < 256; i++)
                    clut[i] = qRgb(i, i, i);

                int w4; //this is width rounded up to nearest multiple of 4 - trust me, you need this!
                if ((width % 4) == 0) w4 = width; // no problem with width
                else w4 = (width / 4) * 4 + 4; //round up
                tempToDraw.setColorTable(clut);
                //set up pointers to base of array - enables very fast access to data
                uchar *data1 = (uchar *) ToDraw.bits(); //image is the source (XRGB) image
                uchar *data2 = (uchar *) tempToDraw.bits();

                for (int x = 0; x < width; x++)
                    for (int y = 0; y < height; y++)
                        data2[y * w4 + x] = BLUE(data1, 4 * (y * width + x));


                ToDraw = tempToDraw;
            }

            QString savename = imageList[i]->fileName + ".xxx";
            if (imageList[i]->format == 0)ToDraw.save(savename, "BMP", 100);
            if (imageList[i]->format == 1)ToDraw.save(savename, "JPG", 100);
            if (imageList[i]->format == 2)ToDraw.save(savename, "PNG", 50);
        }
    } else if (actionLock_Back->isChecked()) {
        int k = (propogateImage + 1);
        int l = 0;
        //Loop through images
        for (int i = propogateImage - 2; i >= 0; i--) {
            k++;
            l++;
            //Update message on status bar & progress bar
            progress.setValue(k);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

            QString output = "propogation - ";
            QString output2;
            output2.sprintf("Processing (%d/%d)", l, imageList.count());
            statusbar->showMessage(output + output2);

            //Start painter
            QImage ApplyProp(imageList[i]->fileName);
            QImage ToDraw(width, height, QImage::Format_RGB32);
            QPainter paint;
            paint.begin(&ToDraw);
            paint.setRenderHint(QPainter::SmoothPixmapTransform);
            //For each images loop through the transformation list
            for (int j = 0; j < propogation.count(); j++) {
                //Rotate
                if (propogation[j]->transformation == 1) {
                    paint.setWorldTransform(imageList[i]->m);
                    paint.translate((width / 2), (height / 2));
                    paint.rotate(propogation[j]->value);
                    paint.translate(-(width / 2), -(height / 2));
                    imageList[i]->m = paint.worldTransform();
                }
                //Resize
                if (propogation[j]->transformation == 2) {
                    paint.setWorldTransform(imageList[i]->m);
                    paint.translate((width / 2), (height / 2));
                    paint.scale(propogation[j]->value, propogation[j]->value);
                    paint.translate(-(width / 2), -(height / 2));
                    imageList[i]->m = paint.worldTransform();
                }
                //Lateral shift
                if (propogation[j]->transformation == 3) {
                    qreal m11, m12, m21, m22, mdx, mdy;
                    mdx = imageList[i]->m.dx();
                    mdy = imageList[i]->m.dy();
                    m11 = imageList[i]->m.m11();
                    m22 = imageList[i]->m.m22();
                    m21 = imageList[i]->m.m21();
                    m12 = imageList[i]->m.m12();
                    mdx += propogation[j]->value;
                    imageList[i]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);
                }
                //Vertical shift
                if (propogation[j]->transformation == 4) {
                    qreal m11, m12, m21, m22, mdx, mdy;
                    mdx = imageList[i]->m.dx();
                    mdy = imageList[i]->m.dy();
                    m11 = imageList[i]->m.m11();
                    m22 = imageList[i]->m.m22();
                    m21 = imageList[i]->m.m21();
                    m12 = imageList[i]->m.m12();
                    mdy += propogation[j]->value;
                    imageList[i]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);
                }
            }
            //Finally draw and save new file
            paint.setWorldTransform(imageList[i]->m);
            QPointF leftCorner(0.0, 0.0);
            paint.drawImage(leftCorner, ApplyProp);
            paint.end();

            if (ApplyProp.format() == QImage::Format_Indexed8) {

                QImage tempToDraw(width, height, QImage::Format_Indexed8);

                QVector <QRgb> clut(256);
                for (int i = 0; i < 256; i++)
                    clut[i] = qRgb(i, i, i);

                int w4; //this is width rounded up to nearest multiple of 4 - trust me, you need this!
                if ((width % 4) == 0) w4 = width; // no problem with width
                else w4 = (width / 4) * 4 + 4; //round up
                tempToDraw.setColorTable(clut);
                //set up pointers to base of array - enables very fast access to data
                uchar *data1 = (uchar *) ToDraw.bits(); //image is the source (XRGB) image
                uchar *data2 = (uchar *) tempToDraw.bits();

                for (int x = 0; x < width; x++)
                    for (int y = 0; y < height; y++)
                        data2[y * w4 + x] = BLUE(data1, 4 * (y * width + x));


                ToDraw = tempToDraw;
            }

            QString savename = imageList[i]->fileName + ".xxx";
            if (imageList[i]->format == 0)ToDraw.save(savename, "BMP", 100);
            if (imageList[i]->format == 1)ToDraw.save(savename, "JPG", 100);
            if (imageList[i]->format == 2)ToDraw.save(savename, "PNG", 50);
        }
    } else QMessageBox::warning(0, "Error", "You should never see this - propagation failed, email me.", QMessageBox::Ok);

    //Disable propogaiton mode
    actionPropogate_Mode->setChecked(false);
    actionApply_Propogation->setEnabled(false);

    actionLock_Forward->setChecked(false);
    actionLock_Back->setChecked(false);
    actionLock_File->setChecked(false);


    actionLock_Forward->setEnabled(true);
    actionLock_Back->setEnabled(true);
    actionLock_File->setEnabled(true);

    showInfo(-1, -1);

    //Reset slider
    if (currentImage < 2) {
        horizontalSlider->setEnabled(true);
        spinBox->setEnabled(true);
        horizontalSlider->setEnabled(true);
        spinBox->setEnabled(true);
    }
    horizontalSlider->setMaximum(drectoryFileList.count());
    spinBox->setMaximum(drectoryFileList.count());
    horizontalSlider->setMinimum(1);
    spinBox->setMinimum(1);

    //Delete propogation list
    qDeleteAll(propogation.begin(), propogation.end());
    propogation.clear();
    redrawImage();

    //Clean statusbar
    statusbar->clearMessage();
    statusbar->removeWidget(&progress);
}


//Crop area setup:
void MainWindowImpl::on_actionCreate_Crop_Area_triggered(bool checked)
{
    if (checked == true) {

        autoAlign->setEnabled(false);
        if (setupFlag == 1)setupAlign->setChecked(false);
        cropArea = new QRect((width / 4), (height / 4), 200, 200);
        cropWidth->setValue(width / 2);
        cropHeight->setValue(height / 2);
        red2->setValue(redValue);
        green2->setValue(greenValue);
        blue2->setValue(blueValue);

        if (actionAdd_Markers->isChecked()) {
            if (autoMarkersUp == 1) {
                lockMarkers->toggle();
                lockMarkers->setEnabled(true);
                autoMarkersUp = 0;
                showInfo(-1, -1);
                delete gridOutline;
                delete autoMarkersGroup;
                autoMarkersGroup = NULL;
                markersLockToggled();
                linePointers.clear();
            }
            markersUp = 0;
            markersDialogue->hide();
            actionAdd_Markers->setChecked(false);
        }

        layoutWidgetTwo->setMaximumHeight(300);
        cropDock->show();
        actionCrop->setEnabled(true);
        cropUp = 1;
        redrawImage();
    }

    if (checked == false) {
        autoAlign->setEnabled(true);
        actionMove_Right->setEnabled(false);
        actionShrink_Right->setEnabled(false);
        actionMove_Left->setEnabled(false);
        actionShrink_Left->setEnabled(false);
        actionMove_Up->setEnabled(false);
        actionShrink_Up->setEnabled(false);
        actionMove_Down->setEnabled(false);
        actionShrink_Down->setEnabled(false);
        actionCrop->setEnabled(false);
        delete cropArea;
        cropArea = NULL;
        rectPointer = NULL;
        cropUp = 0;
        cropDock->hide();
        layoutWidgetTwo->setMaximumHeight(200);
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        redrawImage();
    }

}

//Crop!
void MainWindowImpl::on_actionCrop_triggered()
{

    if ((QMessageBox::question(0, "Crop", "Are you sure you want to crop your images?", QMessageBox::Ok, QMessageBox::Cancel)) == 4194304)return;
    else {
        cropping = 1;
        int count = imageList.count(), min = 0, max = 0, format;

        QStringList items;
        for (int i = 0; i < count; i++) items <<  imageList[i]->fileName;
        bool ok;
        QString minfile = QInputDialog::getItem(this, "Start file", "Please choose the file from which you want to crop", items, 0, false, &ok);
        if (!ok) return;
        for (int i = 0; i < count; i++)if (imageList[i]->fileName == minfile)min = i;

        QStringList items2;
        for (int i = (min + 1); i < count; i++) items2 <<  imageList[i]->fileName;
        QString maxfile = QInputDialog::getItem (this, "End file", "Please choose the file at which you want the crop to end", items2, items2.count() - 1, false, &ok);
        if (!ok) return;
        for (int i = 0; i < count; i++)if (imageList[i]->fileName == maxfile) max = i;

        QStringList formats;
        formats << "bmp" << "jpg" << "png" << "tiff";
        QString chosenFormat =  QInputDialog::getItem(this, "Choose file format", "Enter the file format you wish the cropped images to be saved as", formats, 0, false, &ok);
        if (!ok) return;

        if (chosenFormat == "bmp")format = 0;
        else if (chosenFormat == "jpg")format = 1;
        else if (chosenFormat == "png")format = 2;
        else format = 3;
        if (format == 3)if ((QMessageBox::question(0, "Tiff format chosen",
                                                       "Tiffs cannot be used with SPIERS edit; this option should only be chosen if the files are to be used in VG Studio or similar software", QMessageBox::Ok, QMessageBox::Cancel)) == 4194304)return;
        if (format != imageList[min]->format)if ((QMessageBox::question(0, "Change format?", "The format chosen is different to the original images do you wish to proceed?", QMessageBox::Ok,
                                                                            QMessageBox::Cancel)) == 4194304)return;

        //Create directory
        QString dirname = filesDirectory.absolutePath() + "/cut/";
        QDir cut;
        if (cut.mkpath(dirname) == false)  {
            QMessageBox::warning(0, "Error", "Can't create cut folder for images", QMessageBox::Ok);
            actionLock_Forward->setChecked(false);
            return;
        }


        int StartImage = currentImage;

        //Setup status & progress bar
        QProgressBar progress;
        progress.setRange (min, max);
        progress.setAlignment(Qt::AlignHCenter);
        statusbar->addPermanentWidget(&progress);

        //Normalise the crop rectangle to ensure it's valid...
        *cropArea = cropArea->normalized();
        count = 0;

        for (int i = (min); i < (max + 1); i++) {
            count++;
            progress.setValue(i);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

            QString output = "Cropping - ";
            QString output2;
            output2.sprintf("Processing (%d/%d)", count, (max - min));
            statusbar->showMessage(output + output2);

            QImage cropimage;
            QString loadname = imageList[i]->fileName + ".xxx";
            if (QFile::exists(loadname) == true)cropimage.load(loadname);
            else cropimage.load(imageList[i]->fileName);
            QString filename = dirname + drectoryFileList[i];
            QImage cropped = cropimage.copy(*cropArea);

            int dot = filename.lastIndexOf(".");

            filename = filename.left(dot);

            if (format == 0)cropped.save(filename + ".bmp", "BMP", 100);
            if (format == 1)cropped.save(filename + ".jpg", "JPG", 100);
            if (format == 2)cropped.save(filename + ".png", "PNG", 50);
            if (format == 3)cropped.save(filename + ".tif", "TIFF", 100);

            currentImage = i;
            redrawImage();
        }
        currentImage = StartImage;
        redrawImage();



        actionCreate_Crop_Area->trigger();
        actionAdd_Markers->trigger();

        //Clean statusbar
        statusbar->clearMessage();
        statusbar->removeWidget(&progress);

        QMessageBox::warning(0, "Crop completed", "Cropped images placed in cut folder of current directory.", QMessageBox::Ok);

    }
}

//Slots to resize crop area from Spin boxes in crop dialogue
void MainWindowImpl::resizeCropW(int value)
{
    cropArea->setWidth(value);
    redrawJustCropBox();
}

void MainWindowImpl::resizeCropH(int value)
{
    cropArea->setHeight(value);
    redrawJustCropBox();
}


//About
void MainWindowImpl::on_actionAbout_triggered()
{
    About adialogue;
    adialogue.exec();
}

//Lock file to changes - all done in the apply changes function
void MainWindowImpl::on_actionLock_File_triggered(bool checked)
{
    // No code necessary - used by propogation and file locking modes.
    Q_UNUSED(checked);

}

//Lock forward
void MainWindowImpl::on_actionLock_Forward_triggered(bool checked)
{
    if (checked == true)   {
        lockImage = currentImage;
        if (lockImage < 1) {
            QMessageBox::warning(0, "Error", "This is the beginning of the dataset, locking diabled.", QMessageBox::Ok);
            actionLock_Forward->setChecked(false);
            return;
        }
        if (!actionPropogate_Mode->isChecked())statusbar->showMessage("Forward File Locking mode");
        else statusbar->showMessage("Forward propagation mode");
        if (actionLock_Back->isChecked ())   {
            actionLock_Back->setChecked(false);
            actionLock_File->setChecked(false);
        }
        if (actionPropogate_Mode->isChecked())   {
            actionPropogate_Mode->setChecked(false);
            actionApply_Propogation->setEnabled(false);
        }
        horizontalSlider->setMaximum(lockImage + 1);
        spinBox->setMaximum(lockImage + 1);
        horizontalSlider->setMinimum(lockImage);
        spinBox->setMinimum(lockImage);
        actionMove_Forward_Back->setEnabled(true);
        actionLock_File->setEnabled(true);
    }

    if (checked == false)  {
        horizontalSlider->setMaximum(drectoryFileList.count());
        spinBox->setMaximum(drectoryFileList.count());
        horizontalSlider->setMinimum(1);
        spinBox->setMinimum(1);
        actionMove_Forward_Back->setEnabled(false);
        actionLock_File->setChecked(false);
        actionLock_File->setEnabled(false);

    }
    showInfo(-1, -1);
}

//Lock back
void MainWindowImpl::on_actionLock_Back_triggered(bool checked)
{
    if (checked == true)   {
        lockImage = currentImage;
        if (lockImage < 1) {
            QMessageBox::warning(0, "Error", "This is the beginning of the dataset, locking diabled.", QMessageBox::Ok);
            actionLock_Back->setChecked(false);
            return;
        }

        if (!actionPropogate_Mode->isChecked())statusbar->showMessage("Backward File Locking mode");
        else statusbar->showMessage("Backward propagation mode");
        lockImage = currentImage;
        if (actionLock_Forward->isChecked ())    {
            actionLock_Forward->setChecked(false);
            actionLock_File->setChecked(true);
        }
        if (actionPropogate_Mode->isChecked())   {
            actionPropogate_Mode->setChecked(false);
            actionApply_Propogation->setEnabled(false);
        }
        horizontalSlider->setMaximum(lockImage + 1);
        spinBox->setMaximum(lockImage + 1);
        horizontalSlider->setMinimum(lockImage);
        spinBox->setMinimum(lockImage);
        actionMove_Forward_Back->setEnabled(true);
        actionLock_File->setEnabled(true);
    }

    if (checked == false)  {
        horizontalSlider->setMaximum(drectoryFileList.count());
        spinBox->setMaximum(drectoryFileList.count());
        horizontalSlider->setMinimum(1);
        spinBox->setMinimum(1);
        actionMove_Forward_Back->setEnabled(false);
        actionLock_File->setChecked(false);
        actionLock_File->setEnabled(false);
    }
    showInfo(-1, -1);
}

//Move on or back in file locking mode
void MainWindowImpl::on_actionMove_Forward_Back_triggered()
{
    if (actionLock_Forward->isChecked()) {
        if (horizontalSlider->maximum() > (imageList.count() - 1)) {
            QMessageBox::warning(0, "Error", "This is the end of the dataset, locking diabled.", QMessageBox::Ok);
            return;
        }
        lockImage++;
        horizontalSlider->setMaximum(lockImage + 1);
        spinBox->setMaximum(lockImage + 1);
        horizontalSlider->setMinimum(lockImage);
        spinBox->setMinimum(lockImage);
    }
    if (actionLock_Back->isChecked())   {
        if (horizontalSlider->minimum() < 2) {
            QMessageBox::warning(0, "Error", "This is the beginning of the dataset, locking diabled.", QMessageBox::Ok);
            return;
        }
        lockImage--;
        horizontalSlider->setMaximum(lockImage + 1);
        spinBox->setMaximum(lockImage + 1);
        horizontalSlider->setMinimum(lockImage);
        spinBox->setMinimum(lockImage);
    }
}

//Slider
void MainWindowImpl::on_horizontalSlider_valueChanged(int value)
{
    QApplication::restoreOverrideCursor();
    if (autoMarkersUp == 1) aM.reset();
    currentImage = value - 1;
    fileList->setCurrentRow(currentImage);
    redrawImage();
}

//Save settings file
void MainWindowImpl::on_actionSave_triggered()
{
    int i;

    //Write to settings file
    QString filename = filesDirectory.absolutePath() + "/settings.txt";
    QFile settings(filename);
    settings.open(QFile::WriteOnly);
    QTextStream write(&settings);
    for (i = 0; i < imageList.count(); i++) {
        write << imageList[i]->fileName << "\t";
        write << imageList[i]->m.dx() << "\t";
        write << imageList[i]->m.dy() << "\t";
        write << imageList[i]->m.m11() << "\t";
        write << imageList[i]->m.m22() << "\t";
        write << imageList[i]->m.m21() << "\t";
        write << imageList[i]->m.m12() << "\t";
        write << imageList[i]->hidden << "\n";
    }

    write << markers.count() << "\t" << mSize->value() << "\t" << mThickness->value() << "\t" << redValue << "\t" << greenValue << "\t" << blueValue << "\t" << currentImage << "\t" <<
          lockMarkers->isChecked() << "\t" << actionConstant_autosave->isChecked() << "\n";

    for (i = 0; i < markers.count(); i++) {
        write << markers[i]->markerRect->x() << "\t" << markers[i]->markerRect->y() << "\t" <<  markers[i]->shape << "\t" <<  markers[i]->linePoint.x() << "\t" <<  markers[i]->linePoint.y() << "\n";
    }
    write << notes->toPlainText();

    settings.flush();
    settings.close();
}

//Save backup settings file
void MainWindowImpl::on_actionSave_Backup_triggered()
{
    QDateTime date;
    date = QDateTime::currentDateTime () ;
    int i;

    //Write to settings file
    QString filename = filesDirectory.absolutePath() + "/settings backup - " + date.toString("dd MMM yyyy - hh.mm.ss") + ".txt";
    QFile settings(filename);
    settings.open(QFile::WriteOnly);
    QTextStream write(&settings);
    for (i = 0; i < imageList.count(); i++) {
        write << imageList[i]->fileName << "\t";
        write << imageList[i]->m.dx() << "\t";
        write << imageList[i]->m.dy() << "\t";
        write << imageList[i]->m.m11() << "\t";
        write << imageList[i]->m.m22() << "\t";
        write << imageList[i]->m.m21() << "\t";
        write << imageList[i]->m.m12() << "\t";
        write << imageList[i]->hidden << "\n";
    }

    write << markers.count() << "\t" << mSize->value() << "\t" << mThickness->value() << "\t" << redValue << "\t" << greenValue << "\t" << blueValue << "\t" << currentImage << "\t" <<
          lockMarkers->isChecked() << "\t" << actionConstant_autosave->isChecked() << "\n";

    for (i = 0; i < markers.count(); i++) {
        write << markers[i]->markerRect->x() << "\t" << markers[i]->markerRect->y() << "\t" <<  markers[i]->shape << "\t" <<  markers[i]->linePoint.x() << "\t" <<  markers[i]->linePoint.y() << "\n";
    }
    write << notes->toPlainText();

    settings.flush();
    settings.close();
}

//Hide current image
void MainWindowImpl::on_actionHide_Image_triggered()
{
    if (currentImage == (imageList.count() - 1)) {
        QMessageBox::warning(0, "Error", "Cannot hide final Image.", QMessageBox::Ok);
        return;
    }
    if (currentImage == 0) {
        QMessageBox::warning(0, "Error", "Cannot hide first image.", QMessageBox::Ok);
        return;
    }

    imageList[currentImage]->hidden = true;
    statusbar->showMessage("This image is hidden.");
}

//Unhide current image
void MainWindowImpl::on_actionShow_Image_triggered()
{
    imageList[currentImage]->hidden = false;
    statusbar->showMessage("This image is no longer hidden.");
}

//Unhide all images
void MainWindowImpl::on_actionShow_All_triggered()
{
    for (int i = 0; i < imageList.count(); i++)imageList[i]->hidden = false;
    statusbar->showMessage("All images no longer hidden.");
}

//Advance ignoring hide
void MainWindowImpl::on_actionAdvance_To_Hidden_triggered()
{
    if (spinBox->isEnabled() == false)return;
    horizontalSlider->setValue(currentImage + 2);
    if (actionPropogate_Mode->isChecked() && currentImage == (propogateImage))actionLock_File->setChecked(false);
    if (actionLock_Forward->isChecked() && currentImage == lockImage)actionLock_File->setChecked(false);
    if (actionLock_Back->isChecked() && currentImage == lockImage)actionLock_File->setChecked(true);
}

//Retreat ignoring hide
void MainWindowImpl::on_actionRetreat_To_Hidden_triggered()
{
    if (spinBox->isEnabled() == false)return;
    horizontalSlider->setValue(currentImage);
    if (actionPropogate_Mode->isChecked() && currentImage == (propogateImage - 1))actionLock_File->setChecked(true);
    if (actionLock_Forward->isChecked() && currentImage == (lockImage - 1))actionLock_File->setChecked(true);
    if (actionLock_Back->isChecked() && currentImage == (lockImage - 1))actionLock_File->setChecked(false);
}

//Next
void MainWindowImpl::on_actionNext_Image_triggered()
{
    QApplication::restoreOverrideCursor();
    if (spinBox->isEnabled() == false)return;
    if (autoMarkersUp == 1 && !actionLock_Forward->isChecked() && !actionLock_Back->isChecked()) aM.reset();
    if (actionLock_Forward->isChecked() || actionLock_Back->isChecked())horizontalSlider->setValue(currentImage + 2);
    else {
        if (currentImage < (imageList.count() - 1))currentImage++;
        if (imageList[currentImage]->hidden == true)while (imageList[currentImage]->hidden == true && currentImage < (imageList.count() - 1))currentImage++;
        horizontalSlider->setValue(currentImage + 1);
        fileList->setCurrentRow(currentImage);
    }
    //if (actionPropogate_Mode->isChecked() && currentImage==(propogateImage))actionLock_File->setChecked(false);
    //Call redraw image again here so automarkers works in lock mode
    if (actionLock_Forward->isChecked() && currentImage == lockImage) {
        actionLock_File->setChecked(false);
        redrawImage();
    }
    if (actionLock_Back->isChecked() && currentImage == lockImage) {
        actionLock_File->setChecked(true);
        redrawImage();
    }

}

//Previous
void MainWindowImpl::on_actionPrevious_Image_triggered()
{
    QApplication::restoreOverrideCursor();
    if (spinBox->isEnabled() == false)return;
    if (autoMarkersUp == 1 && !actionLock_Forward->isChecked() && !actionLock_Back->isChecked()) aM.reset();
    if (actionLock_Forward->isChecked() || actionLock_Back->isChecked())horizontalSlider->setValue(currentImage);
    else {
        if (currentImage > 0)currentImage--;
        if (imageList[currentImage]->hidden == true)while (imageList[currentImage]->hidden == true && currentImage < (imageList.count() - 1))currentImage--;
        horizontalSlider->setValue(currentImage + 1);
        fileList->setCurrentRow(currentImage);
    }
    //if (actionPropogate_Mode->isChecked() && currentImage==(propogateImage-1))actionLock_File->setChecked(true);
    if (actionLock_Forward->isChecked() && currentImage == (lockImage - 1)) {
        actionLock_File->setChecked(true);
        redrawImage();
    }
    if (actionLock_Back->isChecked() && currentImage == (lockImage - 1)) {
        actionLock_File->setChecked(false);
        redrawImage();
    }

}


void MainWindowImpl::on_actionReset_Image_triggered()
{
    imageList[currentImage]->m.reset();
    QString loadname = imageList[currentImage]->fileName + ".xxx";
    if (QFile::exists(loadname) == true) {
        QFile deleteFile(loadname);
        deleteFile.remove();
    }
    redrawImage();
}

void MainWindowImpl::on_actionReset_Scene_triggered()
{
    QPixmap newimage;
    QString loadname = imageList[currentImage]->fileName + ".xxx";
    if (QFile::exists(loadname) == true)newimage.load(loadname);
    else newimage.load(imageList[currentImage]->fileName);
    width = newimage.width();
    height = newimage.height();

    redrawImage();
}

void MainWindowImpl::on_actionSwap_Image_With_Next_triggered()
{
    int flag = 0;
    if (imageList[currentImage + 1]->hidden == 1)if ((QMessageBox::question(0, "Next image hidden", "The next image is hidden, still swap?", QMessageBox::Ok, QMessageBox::Cancel)) == 4194304)return;

    QString swapName = imageList[currentImage]->fileName;
    QFile Current(swapName);
    Current.rename(imageList[currentImage]->fileName + ".swap");

    QString loadname = imageList[currentImage]->fileName + ".xxx";
    if (QFile::exists(loadname) == true) {
        flag = 1;
        QFile CurrentXXX(loadname);
        CurrentXXX.rename(loadname + ".swap");
    }

    QString newFile = imageList[currentImage + 1]->fileName;
    QFile newCurrent(newFile);
    newCurrent.rename(imageList[currentImage]->fileName);

    QString newloadname = imageList[currentImage + 1]->fileName + ".xxx";
    if (QFile::exists(newloadname) == true) {
        QFile newCurrentXXX(newloadname);
        newCurrentXXX.rename(loadname);
    }

    QFile previousCurrent(imageList[currentImage]->fileName + ".swap");
    previousCurrent.rename(imageList[currentImage + 1]->fileName);

    if (flag == 1) {
        QFile old(loadname + ".swap");
        old.rename(imageList[currentImage + 1]->fileName + ".xxx");
    }

    QTransform M = imageList[currentImage]->m;
    imageList[currentImage]->m = imageList[currentImage + 1]->m;
    imageList[currentImage + 1]->m = M;

    bool ishidden = imageList[currentImage]->hidden;
    imageList[currentImage]->hidden = imageList[currentImage + 1]->hidden;
    imageList[currentImage + 1]->hidden = ishidden;

    int fileformat = imageList[currentImage]->format;
    imageList[currentImage]->format = imageList[currentImage + 1]->format;
    imageList[currentImage + 1]->format = fileformat;

    redrawImage();
}

void MainWindowImpl::on_actionLoad_Settings_File_triggered()
{
    if (currentImage == -1) {
        QMessageBox::warning(0, "Error", "Please open the dataset you wish to apply a settings file to.", QMessageBox::Ok);
        return;
    }


    int i, j = 0;
    if ((QMessageBox::question(0, "Confirm", "Are you sure you want to load a settings file? This will overwrite the current settings and apply the new ones to the currently open dataset.",
                               QMessageBox::Ok, QMessageBox::Cancel)) == QMessageBox::Cancel)return;
    QString settingsFile = QFileDialog::getOpenFileName(this, tr("Select settings file"), "d:/");
    if (settingsFile == "") return;

    QFile settings(settingsFile);

    settings.open(QFile::ReadOnly);

    int numberMarkers = 0;

    QTextStream read(&settings);

    int x = imageList.count();
    currentImage = -1;
    i = -1;
    while (!read.atEnd()) {
        i++;
        QString line = read.readLine();
        QStringList list = line.split("\t");
        if (i < x) {
            //Check image list not modified - filenames should remain the same.
            if (!list[0].endsWith(imageList[i]->fileName, Qt::CaseInsensitive)) {
                QMessageBox::warning(0, "Error", "Image sequence has been modified. This will prevent the dataset loading correctly", QMessageBox::Ok);
                return;
            } else {
                qreal m11, m12, m21, m22, mdx, mdy;
                mdx = list[1].toDouble();
                mdy = list[2].toDouble();
                m11 = list[3].toDouble();
                m22 = list[4].toDouble();
                m21 = list[5].toDouble();
                m12 = list[6].toDouble();
                imageList[i]->m.setMatrix(m11, m12, 0., m21, m22, 0., mdx, mdy, 1.);
            }
        }
        if (i == x) {
            if (list.size() == 1)numberMarkers = line.toInt();
            else {
                numberMarkers = list[0].toInt();
                mSize->setValue(list[1].toInt());
                mThickness->setValue(list[2].toInt());
                red->setValue(list[3].toInt());
                green->setValue(list[4].toInt());
                blue ->setValue(list[5].toInt());
                if (list.size() > 7) {
                    if (list[7].toInt() > 0)lockMarkers->toggle();
                    markersLockToggled();
                    actionConstant_autosave->setChecked(list[8].toInt());
                }

            }
        }

        if (i > x && i <= (x + 5)) {
            markers[j]->markerRect->moveTo(list[0].toDouble(), list[1].toDouble());
            markers[j]->shape = list[2].toInt();
            markers[j]->linePoint.setX(list[3].toDouble());
            markers[j]->linePoint.setY(list[4].toDouble());
            j++;
        }

        if (i > (x + 5) && i <= (x + numberMarkers)) {
            MarkerData *append = new MarkerData(new QRectF(list[0].toDouble(), list[1].toDouble(), 5., 5.), list[2].toInt());
            markers.append(append);
            QString output;
            output.sprintf("Marker - %d", (j + 1));
            markerList->addItem(output);
            markers[j]->linePoint.setX(list[3].toDouble());
            markers[j]->linePoint.setY(list[4].toDouble());
            j++;
        }
    }

    settings.flush();
    settings.close();
    currentImage = 0;
    for (i = 0; i < imageList.count(); i++) {
        imageList[i]->format = -1;
        if (imageList[i]->fileName.endsWith(".png", Qt::CaseInsensitive))imageList[i]->format = 2;
        if (imageList[i]->fileName.endsWith(".jpg", Qt::CaseInsensitive) || imageList[i]->fileName.endsWith(".jpeg", Qt::CaseInsensitive))imageList[i]->format = 1;
        if (imageList[i]->fileName.endsWith(".bmp", Qt::CaseInsensitive))imageList[i]->format = 0;
        if (imageList[i]->format == -1) {
            QMessageBox::warning(0, "Error", "Please check extensions - should be either .jpg, .jpeg, .bmp or .png", QMessageBox::Ok);
            return;
        }
    }

    //Setup progress bar
    QProgressBar progress;
    progress.setRange (0, imageList.count());
    progress.setAlignment(Qt::AlignHCenter);
    statusbar->addPermanentWidget(&progress);

    int listLength = imageList.count();
    //Loop through images
    for (int i = 0; i < listLength; i++) {
        progress.setValue(i);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        QString output = "Applying settings - ";
        QString output2;
        output2.sprintf("Processing (%d/%d)", i + 1, listLength);
        statusbar->showMessage(output + output2);
        if (!imageList[i]->m.isIdentity()) {

            //Start painter
            QImage ApplySet(imageList[i]->fileName);
            QImage ToDraw(width, height, QImage::Format_RGB32);
            if (ApplySet.format() == QImage::Format_Indexed8)ToDraw.convertToFormat(QImage::Format_Indexed8);
            QPainter paint;
            paint.begin(&ToDraw);
            paint.setRenderHint(QPainter::SmoothPixmapTransform);

            //Draw and save new file
            paint.setWorldTransform(imageList[i]->m);
            QPointF leftCorner(0.0, 0.0);
            paint.drawImage(leftCorner, ApplySet);
            paint.end();


            if (ApplySet.format() == QImage::Format_Indexed8) {

                QImage tempToDraw(width, height, QImage::Format_Indexed8);

                QVector <QRgb> clut(256);
                for (int i = 0; i < 256; i++)
                    clut[i] = qRgb(i, i, i);

                int w4; //this is width rounded up to nearest multiple of 4 - trust me, you need this!
                if ((width % 4) == 0) w4 = width; // no problem with width
                else w4 = (width / 4) * 4 + 4; //round up
                tempToDraw.setColorTable(clut);
                //set up pointers to base of array - enables very fast access to data
                uchar *data1 = (uchar *) ToDraw.bits(); //image is the source (XRGB) image
                uchar *data2 = (uchar *) tempToDraw.bits();

                for (int x = 0; x < width; x++)
                    for (int y = 0; y < height; y++)
                        data2[y * w4 + x] = BLUE(data1, 4 * (y * width + x));


                ToDraw = tempToDraw;
            }

            QString savename = imageList[i]->fileName + ".xxx";
            if (imageList[i]->format == 0)ToDraw.save(savename, "BMP", 100);
            if (imageList[i]->format == 1)ToDraw.save(savename, "JPG", 100);
            if (imageList[i]->format == 2)ToDraw.save(savename, "PNG", 50);

            redrawImage();
        } else {
            QString loadname = imageList[i]->fileName + ".xxx";
            if (QFile::exists(loadname) == true)QFile::remove(loadname);
        }
    }

}

void MainWindowImpl::on_actionCompress_Dataset_triggered()
{
    if (currentImage == -1) {
        QMessageBox::warning(0, "Error", "Please open the dataset you wish to compress.", QMessageBox::Ok);
        return;
    }

    if ((QMessageBox::question(0, "Confirm",
                               "Are you sure you want to compress the dataset? It is recommended you only do this once you have finished working on it. All working .xxx files will be deleted, and the image files will be converted to PNGs. The settings file can later be reapplied to the dataset, but the converion to PNGs is permanent.",
                               QMessageBox::Ok, QMessageBox::Cancel)) == QMessageBox::Cancel)return;

    int x = imageList.count(), i, flag;

    //Setup progress bar
    QProgressBar progress;
    progress.setRange (0, x);
    progress.setAlignment(Qt::AlignHCenter);
    statusbar->addPermanentWidget(&progress);


    for (i = 0; i < x; i++) {

        progress.setValue(i);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        QString output = "Compressing - ";
        QString output2;
        output2.sprintf("Processing (%d/%d)", i + 1, x);
        statusbar->showMessage(output + output2);

        flag = 0;
        QString loadname = imageList[i]->fileName + ".xxx";
        if (QFile::exists(loadname) == true)QFile::remove(loadname);

        QString filename = imageList[i]->fileName;

        int dot = filename.lastIndexOf(".");

        if (filename.right(dot) != "png")flag = 1;

        filename = filename.left(dot);

        QImage Compress(imageList[i]->fileName);

        QString savename = filename + ".png";

        Compress.save(savename, "PNG", 50);

        if (flag == 1) {
            QFile::remove(imageList[i]->fileName);
            imageList[i]->fileName = savename;
        }
    }


}

void MainWindowImpl::on_actionManual_triggered()
{
    QDesktopServices::openUrl(QUrl("file:" + qApp->applicationDirPath() + "/SPIERSalign_Manual.pdf"));
}
