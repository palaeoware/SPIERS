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
#include "version.h"

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


void MainWindowImpl::SetUpDocks()
{
    //DOCK STUFF

    //current plan - delete ALL docks, first reparenting to save their contents widget
    //Create new ones, put widgets in. See if they work
    /*  qDebug()<<"parent of first dock"<<dockWidgetContents->parent()<<"dock is ";


        removeDockWidget (dockWidget_Generate);
        removeDockWidget (SliceSelector);
        removeDockWidget (DockMasksSettings);
        removeDockWidget (DockCurvesSettings);
        removeDockWidget (DockSegmentsSettings);
        removeDockWidget (DockOutputSettings);
        removeDockWidget (DockHist);
        removeDockWidget (DockInfo);

        dockWidgetContents->setParent(0); //unparent
        removeDockWidget(dockWidget_Main);
        delete dockWidget_Main;
        dockWidget_Main = new QDockWidget("Main Toolbox (F1)");
        dockWidget_Main->setWidget(dockWidgetContents);
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget_Main);

    */

    //to get round designer bug with parenting of docks
//  SliceSelector->setParent(this);
//  DockMasksSettings->setParent(this);
//  DockCurvesSettings->setParent(this);
//  DockOutputSettings->setParent(this);
//  DockSegmentsSettings->setParent(this);
//  DockInfo->setParent(this);
//  DockHist->setParent(this);

//  removeDockWidget (dockWidget_Main);

    addDockWidget (Qt::LeftDockWidgetArea, dockWidget_Main);
    addDockWidget (Qt::RightDockWidgetArea, dockWidget_Generate);
    addDockWidget (Qt::RightDockWidgetArea, SliceSelector);
    addDockWidget (Qt::RightDockWidgetArea, DockMasksSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockCurvesSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockSegmentsSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockOutputSettings);
    addDockWidget (Qt::RightDockWidgetArea, DockHist);
    addDockWidget (Qt::RightDockWidgetArea, DockInfo);

    dockWidget_Main->setVisible(true);
    SliceSelector->setVisible(false);
    DockMasksSettings->setVisible(true);
    DockCurvesSettings->setVisible(false);
    DockSegmentsSettings->setVisible(true);
    DockOutputSettings->setVisible(false);
    DockHist->setVisible(false);
    DockInfo->setVisible(false);
    dockWidget_Generate->setVisible(true);

//  dockWidget_Main->setFloating(false);
    SliceSelector->setFloating(false);
    DockMasksSettings->setFloating(false);
    DockCurvesSettings->setFloating(false);
    DockOutputSettings->setFloating(false);
    DockSegmentsSettings->setFloating(false);
    DockInfo->setFloating(false);
    DockHist->setFloating(false);
    dockWidget_Generate->setFloating(false);

    //QVBoxLayout *mylayout = new QVBoxLayout; //layout - any will do, box works fine
    //mylayout->addWidget(GVHist);
    //remove old layout - a bodge
    //DockHist->layout()->addWidget(GVHist);
    //DockHist->setLayout(mylayout);

    GVHist = new histgv;
    DockHist->setWidget(GVHist);

    //SliderPos->setVisible(false);
    //spinBox->setVisible(false);
    //graphicsView->setVisible(false);


    //dockWidget_Generate->resize(dockWidget_Generate->width(), 180 );
    //dock them

    //SliceSelector->setGeometry(9,230, 170, 100);
    //SliceSelector->setMinimumSize(170, 100);
    //SliceSelector->setMaximumSize(170, 1000);
    //dockWidget_Main->setGeometry(0,56, 170, 706);
    //dockWidget_Main->setMinimumSize(170, 600);
    //dockWidget_Main->setMaximumSize(170, 1200);

    //DockHist->setFloating(true);
    //DockHist->setAllowedAreas(

    //sort out toolbar too

    toolBar->addSeparator();
    QLabel *l1 = new QLabel("Brush Size:");
    toolBar->addWidget(l1);

    BrushSize = new KeysafeSpinBox;
    BrushSize->setKeyboardTracking(true);
    BrushSize->setMinimum(1);
    BrushSize->setMaximum(2000);
    BrushSize->setValue(1);
    toolBar->addWidget(BrushSize);

    /*  BrushSizeY = new QSpinBox;
        BrushSizeY->setKeyboardTracking(false);
        BrushSizeY->setMinimum(1);
        BrushSizeY->setMaximum(2000);
        BrushSizeY->setPrefix("Y: ");
        BrushSizeY->setValue(1);
        BrushSizeY->setEnabled(false);
        toolBar->addWidget(BrushSizeY);

        BrushSizeZ = new QSpinBox;
        BrushSizeZ->setKeyboardTracking(false);
        BrushSizeZ->setMinimum(1);
        BrushSizeZ->setMaximum(2000);
        BrushSizeZ->setPrefix("Z: ");
        BrushSizeZ->setValue(1);
        BrushSizeZ->setEnabled(false);
        toolBar->addWidget(BrushSizeZ);


        Yaw = new QDoubleSpinBox;
        Yaw->setKeyboardTracking(false);
        Yaw->setMinimum(-180);
        Yaw->setMaximum(180);
        Yaw->setPrefix("Yaw: ");
        Yaw->setValue(0);
        Yaw->setEnabled(false);
        toolBar->addWidget(Yaw);

        Pitch = new QDoubleSpinBox;
        Pitch->setKeyboardTracking(false);
        Pitch->setMinimum(-180);
        Pitch->setMaximum(180);
        Pitch->setPrefix("Pitch: ");
        Pitch->setValue(0);
        Pitch->setEnabled(false);
        toolBar->addWidget(Pitch);

        Roll = new QDoubleSpinBox;
        Roll->setKeyboardTracking(false);
        Roll->setMinimum(-180);
        Roll->setMaximum(180);
        Roll->setPrefix("Roll: ");
        Roll->setValue(0);
        Roll->setEnabled(false);
        toolBar->addWidget(Roll);

    */


}


void MainWindowImpl::on_actionRefresh_triggered()
{
    ShowImage(graphicsView);
    /*  QRect size =SliceSelector->geometry();
        size.setWidth(size.width()-2);
        SliceSelector->setGeometry(size);
    */
}



void MainWindowImpl::on_action3D_Brush_toggled(bool mode)
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

void MainWindowImpl::SetUpBrushEnabling()
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

void MainWindowImpl::on_actionManual_triggered()
{

    QDesktopServices::openUrl(QUrl("file:" + qApp->applicationDirPath() + "/SPIERSedit_Manual.pdf", QUrl::TolerantMode));

}


void MainWindowImpl::wheelEvent(QWheelEvent *event)
{
    ZoomSlider->setValue(ZoomSlider->value() + event->delta() / 12);
    event->ignore();
}


void MainWindowImpl::on_actionExport_Curves_as_CSV_triggered()
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
            out << endl;
        }
    }
    file.close();
}

void MainWindowImpl::on_actionImport_Curves_as_CSV_triggered()
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
                Curves[i]->SplinePoints[j]->X.append((items[pos++]).toFloat());
                Curves[i]->SplinePoints[j]->Y.append((items[pos++]).toFloat());
            }
        }
    }
    file.close();
}

void MainWindowImpl::on_actionOutput_visible_image_set_triggered()
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

    QString targetdir = QFileDialog::getExistingDirectory(0, "Select output directory", workingdir);

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

        QString formattedi;
        formattedi.sprintf("%05i", i);
        out << targetdir << "/" << formattedi << ".png";
        SaveMainImage(outstring);

        if (ExportingImages == false) break;

    }
    SliderPos->setValue(SaveCurrentFile);
    statusBar()->showMessage("Done.");
}

void MainWindowImpl::on_actionBugIssueFeatureRequest_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY) + QString(GITISSUE)));
}

void MainWindowImpl::on_actionCode_on_GitHub_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY)));
}
