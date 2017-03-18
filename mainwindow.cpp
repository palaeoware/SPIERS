#include <QtWidgets/QApplication>
#include <QtWidgets/QActionGroup>

//RJG update case for ubuntu
#include <QFileDialog>
#include <QMenuBar>

#include <QDebug>
#include <QtWidgets/QHBoxLayout>
#include <QTime>
#include <QtWidgets/QMessageBox>
#include <QTextStream>
#include <QtWidgets/QFileDialog>
#include <QImage>
#include <QList>
#include <QPainter>
#include <QPicture>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QColorDialog>
#include <QDir>
#include <QPalette>
#include <QMutableListIterator>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QGLFormat>
#include <QtWidgets/QShortcut>


#include "mainwindow.h"
#include "spv.h"
#include "ui_mainwindow.h"

//RJG update case
#include "svobject.h"
#include "voxml.h"
#include "quickhelpbox.h"

#include "aboutdialog.h"
#include "SPIERSviewglobals.h"
#include "spvreader.h"
#include "../SPIERScommon/netmodule.h"
#include <vtkProperty2D.h>
#include "movetogroup.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
//Constructor sets up VTK widget and one-shot timer to do load. Nothing else.
{
  specificprogress=0;
  ui->setupUi(this);
  MainWin=this; //set global pointer to this window

  FilterKeys=true; //set to true to turn off interception of keys needed for type-in boxes

  //set up action group
  QActionGroup *StereoGroup = new QActionGroup(this);
  StereoGroup->addAction(ui->actionNo_Stereo);
  StereoGroup->addAction(ui->actionAnaglyph_Stereo);
  StereoGroup->addAction(ui->actionSplit_Stereo);
  StereoGroup->addAction(ui->actionQuadBuffer_Stereo);
  StereoGroup->addAction(ui->actionOrthographic_View);
  ui->actionNo_Stereo->setChecked(true);

  QActionGroup *OutputFormatGroup = new QActionGroup(this);

  OutputFormatGroup->addAction(ui->actionBitmap_BMP);
  OutputFormatGroup->addAction(ui->actionJPEG_High_Quality_JPG);
  OutputFormatGroup->addAction(ui->actionJPEG_Medium_Quality_JPG);
  OutputFormatGroup->addAction(ui->actionJPEG_Low_Quality_JPG);
  OutputFormatGroup->addAction(ui->actionPortable_Network_Graphic_PNG);
  OutputFormatGroup->addAction(ui->actionTagged_Image_File_Format_TIFF);
  ui->actionBitmap_BMP->setChecked(true);

  StereoGroup->addAction(ui->actionNo_Stereo);
  StereoGroup->addAction(ui->actionAnaglyph_Stereo);
  StereoGroup->addAction(ui->actionSplit_Stereo);
  StereoGroup->addAction(ui->actionQuadBuffer_Stereo);
  StereoGroup->addAction(ui->actionOrthographic_View);
  ui->actionNo_Stereo->setChecked(true);


   QGLFormat f;
   f.setVersion(GL_MAJOR,GL_MINOR);
   f.setSampleBuffers(true);
   f.setSampleBuffers(true);
   f.setSamples(1);

   //qDebug()<<"Here";
  gl3widget=new GlWidget(ui->frameVTK); //,f);
  gllayout = new QHBoxLayout;
  gllayout->addWidget(gl3widget);
  gllayout->setSpacing(2);
  gllayout->setMargin(2);
  ui->frameVTK->setLayout(gllayout);

  QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close())); //quit

  StartTimer=new QTimer(this);
  StartTimer->setSingleShot(true);
  StartTimer->setInterval(0);//as soon as may be
  QObject::connect(StartTimer,SIGNAL(timeout()),this,SLOT(StartTimer_fired()));
  StartTimer->start();

  SpinTimer=new QTimer(this);
  SpinTimer->setSingleShot(true);
  SpinTimer->setInterval(SPINTIMERINTERVAL);
  QObject::connect(SpinTimer,SIGNAL(timeout()),this,SLOT(SpinTimer_fired()));
  SpinTimer->start();

  PBtimer=new QTimer(this);
  PBtimer->setSingleShot(false);
  PBtimer->setInterval(100);//10 times per second at most
  QObject::connect(PBtimer,SIGNAL(timeout()),this,SLOT(showSpecificProgress()));
  PBtimer->start();

  time = new QTime(); //used by spin timer
  time->start();

  setWindowIcon(QIcon(qApp->applicationDirPath()+"/" + "ViewIcon.bmp"));
  ui->dockWidgetPieces->setVisible(false);
  ui->InfoDock->setVisible(false);
  ui->ProgressDock->setVisible(true);
  ui->ClipDock->setVisible(false);
  ui->AnimDock->setVisible(false);

  QPalette p = ui->PiecesList->palette();
  p.setColor(QPalette::Highlight, Qt::blue);
  ui->PiecesList->setPalette(p);

  p = ui->treeWidget->palette();
  p.setColor(QPalette::Highlight, Qt::blue);
  ui->treeWidget->setPalette(p);

  gl3widget->SetClip(ui->ClipStart->value(),ui->ClipDepth->value(),ui->ClipAngle->value());

  voxml_mode=false;
  ScaleMatrix[0]=1.0;
  ScaleMatrix[1]=0.0;
  ScaleMatrix[2]=0.0;
  ScaleMatrix[3]=0.0;
  ScaleMatrix[4]=0.0;
  ScaleMatrix[5]=1.0;
  ScaleMatrix[6]=0.0;
  ScaleMatrix[7]=0.0;
  ScaleMatrix[8]=0.0;
  ScaleMatrix[9]=0.0;
  ScaleMatrix[10]=1.0;
  ScaleMatrix[11]=0.0;
  ScaleMatrix[12]=0.0;
  ScaleMatrix[13]=0.0;
  ScaleMatrix[14]=0.0;
  ScaleMatrix[15]=1.0;

//  ScaleBallColour[0]=255;
//  ScaleBallColour[1]=128;
//  ScaleBallColour[2]=128;

//  ScaleBallScale=1.0;

  mm_per_unit=1.0;
  globalrescale=1.0;

  scalelabel=new QLabel(this);
  ui->statusBar->addPermanentWidget(scalelabel);

  ktrlabel=new QLabel(this);
  ui->statusBar->addPermanentWidget(ktrlabel);

  model_ktr=0;

  ui->ZoomSlider->setVisible(false);
  ui->label_7->setVisible(false);

  ui->treeWidget->setColumnWidth(0,100);
  ui->treeWidget->setColumnWidth(1,20);
  ui->treeWidget->setColumnWidth(2,20);
  ui->treeWidget->setColumnWidth(3,28);
  ui->treeWidget->setColumnWidth(4,44);
  ui->treeWidget->setColumnWidth(5,40);
  ui->treeWidget->setColumnWidth(6,90);
  ui->treeWidget->setColumnWidth(7,90);


  back_red=0;back_green=0;back_blue=0;
  grid_red=0; grid_green=255; grid_blue=0;
  grid_minor_red=255; grid_minor_green=0; grid_minor_blue=255;
  globalmatrix.setToIdentity();

  ContainsPresurfaced=false;
  ContainsNonPresurfaced=false;

  //qDebug()<<"Here2";
  AnimOutputDir=QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}


void MainWindow::ActionKey(QChar C)
{
    for (int i=0; i<SVObjects.count(); i++)
         if (SVObjects[i]->Key.toLatin1()==C.toLatin1()) SVObjects[i]->Visible = !SVObjects[i]->Visible;
    UpdateGL();
    RefreshObjects();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateGL()
{
    //qDebug()<<"In udgl ";

    gl3widget->update();
    //do the info at bottom too


}

//Timer handlers
void MainWindow::StartTimer_fired()
//start up timer should fire this only once - starts SPV load process
{ QMessageBox::warning(this,"Got here","Reached here");
    //Some General initialisation
    NextActualDlist=1;

    //Get Filename

    DisableRenderCommands();

    if (fname=="") //no filename provided
    {
		/*
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setNameFilter("SPIERSview files (*.spv *.sp2)");
		dialog.setViewMode(QFileDialog::Detail);

		QStringList files;
		if (dialog.exec())
			 files = dialog.selectedFiles();
		else return;
		
		QString ifname=files[0];
		*/
agin:
        FilterKeys=false;
        fname = QFileDialog::getOpenFileName(
                         this,
                         "Select SPIERSview file",
                         "",
                         "SPIERSview files (*.spv *.sp2 *spvf *.xml *.vaxml)");

        FilterKeys=true;

        if (MacBodgeClickedNoForUpdateDownload==true && fname.isNull()) {	MacBodgeClickedNoForUpdateDownload=false; goto agin;}
        if (fname.isNull()) exit(0); //if nothing there, cancel
    }

    if (fname.right(3)=="xml") //catches xml or voxml
    {
        voxml v;
        if (v.read_voxml(fname))
        {
            QString shortfname="SPIERSview - " + fname.mid(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/"))+1);
            this->setWindowTitle(shortfname);
            EnableRenderCommands();
            RefreshObjects();
            StripDownForVoxml(); //reduce interface to view-only level
            RefreshInfo();
            UpdateGL();
        }
        else exit(0);
    }
    else if (fname.right(4)=="spvf") //finalised files
    {
        voxml v;
        if (v.read_spvf(fname))
        {
            QString shortfname="SPIERSview - " + fname.mid(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/"))+1);
            this->setWindowTitle(shortfname);
            EnableRenderCommands();
            RefreshObjects();
            StripDownForVoxml(); //reduce interface to view-only level
            RefreshInfo();
            UpdateGL();
        }
        else exit(0);
    }
    else
    {
            QString shortfname="SPIERSview - " + fname.mid(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/"))+1);
            this->setWindowTitle(shortfname);
            SPVreader r;
            r.ProcessFile(fname);
            //widget->ResizeScaleBall(1.0);
            RefreshInfo();


    }

}

void MainWindow::StripDownForVoxml()
{
    ui->actionSave_Changes->setVisible(false);
    ui->actionSave_Changes->setEnabled(false);
    ui->actionSave_As->setVisible(false);
    ui->actionSave_As->setEnabled(false);
    ui->actionSave_Presurfaced->setVisible(false);
    ui->actionSave_Presurfaced->setEnabled(false);
    ui->actionSave_Finalised_As->setVisible(false);
    ui->actionSave_Finalised_As->setEnabled(false);
    ui->actionImport_SPV->setVisible(false);
    ui->actionImport_SPV->setEnabled(false);
    ui->actionImport_Replacement->setVisible(false);
    ui->actionImport_Replacement->setEnabled(false);
    ui->actionRemove_Piece->setVisible(false);
    ui->actionRemove_Piece->setEnabled(false);
    ui->actionExport_Hidden_Objects->setVisible(false);
    ui->actionExport_Hidden_Objects->setEnabled(false);
//    ui->menuExport->setVisible(false);
//    ui->menuExport->setEnabled(false);
    //ui->actionSTL->setVisible(false);
    //ui->actionSTL->setEnabled(false);
    //ui->actionDXF->setVisible(false);
    //ui->actionDXF->setEnabled(false);
    ui->actionAuto_Resurface->setVisible(false);
    ui->actionAuto_Resurface->setEnabled(false);
    ui->actionResurface_Now->setVisible(false);
    ui->actionResurface_Now->setEnabled(false);
    ui->actionSave_Memory->setVisible(false);
    ui->actionSave_Memory->setEnabled(false);
    ui->actionReposition_Selected->setVisible(false);
    ui->actionReposition_Selected->setEnabled(false);
    ui->actionPieces_Panel->setVisible(false);
    ui->actionPieces_Panel->setEnabled(false);
//    ui->actionSelect_All->setVisible(false);
//    ui->actionSelect_All->setEnabled(false);
//    ui->actionSelect_None->setVisible(false);
//    ui->actionSelect_None->setEnabled(false);
    ui->actionGroup->setVisible(false);
    ui->actionGroup->setEnabled(false);
    ui->actionUngroup->setVisible(false);
    ui->actionUngroup->setEnabled(false);
    ui->actionMove_to_group->setVisible(false);
    ui->actionMove_to_group->setEnabled(false);
    ui->action_Move_Up->setVisible(false);
    ui->action_Move_Up->setEnabled(false);
    ui->actionMove_Down->setVisible(false);
    ui->actionMove_Down->setEnabled(false);
    ui->actionSet_Resampling->setVisible(false);
    ui->actionSet_Resampling->setEnabled(false);
    ui->actionQuadric_Fidelity_Reduction->setVisible(false);
    ui->actionQuadric_Fidelity_Reduction->setEnabled(false);

    ui->menuSmoothing->setVisible(false);
    ui->menuSmoothing->setEnabled(false);
    ui->menuIsland_Removal->setVisible(false);
    ui->menuIsland_Removal->setEnabled(false);

    /*
    ui->actionOff->setVisible(false);
    ui->actionOff->setEnabled(false);
    ui->actionVery_Weak->setVisible(false);
    ui->actionVery_Weak->setEnabled(false);
    ui->actionWeak->setVisible(false);
    ui->actionWeak->setEnabled(false);
    ui->actionMedium_2->setVisible(false);
    ui->actionMedium_2->setEnabled(false);
    ui->actionStrongish->setVisible(false);
    ui->actionStrongish->setEnabled(false);
    ui->actionStrong->setVisible(false);
    ui->actionStrong->setEnabled(false);
    ui->actionStrongest->setVisible(false);
    ui->actionStrongest->setEnabled(false);
    ui->actionOff_2->setVisible(false);
    ui->actionOff_2->setEnabled(false);
    ui->actionRemove_Tiny->setVisible(false);
    ui->actionRemove_Tiny->setEnabled(false);
    ui->actionRemove_Small->setVisible(false);
    ui->actionRemove_Small->setEnabled(false);
    ui->actionRemove_Medium->setVisible(false);
    ui->actionRemove_Medium->setEnabled(false);
    ui->actionRemove_Large->setVisible(false);
    ui->actionRemove_Large->setEnabled(false);
    ui->actionRemove_All->setVisible(false);
    ui->actionRemove_All->setEnabled(false);
    */
    ui->actionIncrease_Size->setEnabled(false);
    ui->actionDecrease_Size->setEnabled(false);
    ui->actionReset_Size->setVisible(false);
    ui->actionReset_Size->setEnabled(false);

    ui->actionProgress_Bars->setChecked(false);
    on_actionProgress_Bars_triggered();
}

QString MainWindow::DegConvert(float angle)
{
    if (angle>360)
        angle = fmod(angle,360.0);
    while (angle<0)
        angle +=360;

    QString retval;
    retval.sprintf("%05.1f",angle);
    return retval;
}

QString MainWindow::TransConvert(float trans)
{
    QString retval;
    retval.sprintf("%04.1f",trans);
    return retval;
}

void MainWindow::SpinTimer_fired()
{ 
/*    if (QApplication::focusWidget()==0)
    {   qDebug()<<"Refocussing";
        ui->treeWidget->setFocus();
    }
    qDebug()<<"Focus with "<<QApplication::focusWidget();
*/
    int millisecs=time->elapsed();
    time->restart();

    Framenumbs.append(millisecs);
    if (Framenumbs.count()>10)
    {

          double tot=0;
          for (int i=0; i<10; i++)
          {
              tot+=(double)Framenumbs[i];
          }
          tot/=10; //tot is average ms
          tot+=1;
          int framerate=(int)(1000.0/tot);
          QString m;
          QTextStream s(&m);

          QString XrotS=DegConvert(Xrot);
          QString YrotS=DegConvert(Yrot);
          QString ZrotS=DegConvert(Zrot);

          QString XtransS=TransConvert(Xtrans);
          QString YtransS=TransConvert(Ytrans);
          QString ZtransS=TransConvert(Ztrans);

          s<<"Frame rate: "<<framerate<<"  Rotations: X:"<<XrotS<<" Y:"<<YrotS<<" Z:"<<ZrotS<<"  Translations: X:"<<XtransS<<" Y:"<<YtransS<<" Z:"<<ZtransS;

          ui->statusBar->showMessage(m);//should be m
          Framenumbs.clear();
    }

    //also a good place to recalc the width of output

    int wheight = gl3widget->height();
    int wwidth = gl3widget->width();

    int aheight = int((double)wheight / (((double) wwidth) / ((double) ui->AnimRescaleX->value())));
    QString s;
    s.sprintf("%d px", aheight);
    ui->LabelAnimHeight->setText(s);

  if (ui->actionAuto_Spin->isChecked())
  {
      gl3widget->YRotate((float)millisecs * SPINRATE);
      UpdateGL();
  }

  SpinTimer->setInterval(SPINTIMERINTERVAL);
  SpinTimer->start();

  QString mess;

  int TotalTriangles=0;
  double Volume=0.0; //in mm cubed
  int ObjCount=0;

  for (int j=0; j<SVObjects.count(); j++)
      if (SVObjects[j]->widgetitem && SVObjects[j]->spv )
      if (SVObjects[j]->widgetitem->isSelected())
      {
          //qDebug()<<"j,SVJ"<<j<<SVObjects[j];
          //qDebug()<<"SVJ"<<SVObjects[j]->spv;
          //qDebug()<<"SVJ"<<SVObjects[j]->spv->PixPerMM;
          if (!voxml_mode)
          {
              double spvscale=(1.0/SVObjects[j]->spv->PixPerMM)*SVObjects[j]->scale;
              spvscale = spvscale*spvscale*(1.0/SVObjects[j]->spv->SlicePerMM)*SVObjects[j]->scale; //square it, multiply by slice spacing
              Volume+=((double)SVObjects[j]->Voxels)*spvscale;
          }
          TotalTriangles+=SVObjects[j]->Triangles;
          ObjCount++;
      }

  if (ObjCount==0)
      mess.sprintf("Whole Model: %d KTr  ", model_ktr/1000);
  else
  {
      QString oc;
      if (ObjCount==1) oc="object"; else oc="objects";
      if (voxml_mode)
      {
          mess = QString("%1 %3: %2 KTr ").arg(ObjCount).arg(TotalTriangles/1000).arg(oc);
      }
      else
      {
          int dp = (int)(log10(1.0/Volume)+5);
          if (dp<0) dp=0;

          if (dp>10)
              mess = QString("%1 %4: %2 KTr, %3 cubic mm ").arg(ObjCount).arg(TotalTriangles/1000).arg(Volume,0,'e').arg(oc);
              else
              mess = QString("%1 %4: %2 KTr, %3 cubic mm ").arg(ObjCount).arg(TotalTriangles/1000).arg(Volume,0,'f',dp).arg(oc);
      }
      //mess.sprintf(QString(QString("%d objects: %d KTr, %.") + dp + QString("f cubic mm")).toLatin1(), ObjCount,TotalTriangles/1000, Volume);
  }
  ktrlabel->setText(mess);

}

void MainWindow::on_actionShow_Scale_Grid_triggered()
{
    UpdateGL();
}


void MainWindow::on_ClipStart_valueChanged(int clip)
{
  gl3widget->SetClip(ui->ClipStart->value(),ui->ClipDepth->value(),ui->ClipAngle->value());
  UpdateGL();
}

void MainWindow::on_ClipDepth_valueChanged(int clip)
{
    //call the above
    on_ClipStart_valueChanged(clip);
}

void MainWindow::on_ClipAngle_valueChanged(int clip)
{
    //call the above
    on_ClipStart_valueChanged(clip);
}


void MainWindow::on_actionZoom_In_triggered()
{
    ui->ClipAngle->setValue(ui->ClipAngle->value()-10);
}
void MainWindow::on_actionZoom_Out_triggered()
{
    ui->ClipAngle->setValue(ui->ClipAngle->value()+10);
}


void MainWindow::on_actionAnaglyph_Stereo_triggered()
{
    if (gl3widget->context()->format().stereo()) on_actionQuadBuffer_Stereo_triggered();
    else
    UpdateGL();
}

void MainWindow::on_actionSplit_Stereo_triggered()
{
    if (gl3widget->context()->format().stereo()) on_actionQuadBuffer_Stereo_triggered();
    else
    UpdateGL();
}

void MainWindow::on_actionNo_Stereo_triggered()
{
    if (gl3widget->context()->format().stereo()) on_actionQuadBuffer_Stereo_triggered();
    else
    UpdateGL();
}

void MainWindow::on_actionQuadBuffer_Stereo_triggered()
{
    setSamples(gl3widget->context()->format().samples());

    for (int i=0; i<SVObjects.count(); i++)
        if (!SVObjects[i]->IsGroup)
            SVObjects[i]->Dirty=true;
    on_actionResurface_Now_triggered();
    UpdateGL();
}


void MainWindow::on_actionMute_Colours_triggered()
{
    UpdateGL();
}


void MainWindow::on_actionRotate_Clockwise_triggered()
{
    gl3widget->Rotate(-1);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionRotate_Anticlockwise_triggered()
{
    gl3widget->Rotate(1);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionLarge_Rotate_Clockwise_triggered()
{
    gl3widget->Rotate(-30);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionLarge_Rotate_Anticlockwise_triggered()
{
    gl3widget->Rotate(30);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionMove_away_from_viewer_triggered()
{
    gl3widget->MoveAway(.02);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionMove_towards_viewer_triggered()
{
    gl3widget->MoveAway(-.02);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionLarge_Move_Closer_triggered()
{
    gl3widget->MoveAway(-.5);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionLarge_Move_Away_triggered()
{
    gl3widget->MoveAway(0.5);
    UpdateGL();
    FileDirty=true;
}

void MainWindow::on_actionScreen_Capture_triggered()
{
    UpdateGL();
    QImage ScreenCapture = gl3widget->grabFramebuffer();
    FilterKeys=false;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Current View"),
            "",tr("JPEG Image (*.jpg);;Windows Bitmap Image (*.bmp);;TIFF image (*.tif);;PNG Image (*.png)"));
    FilterKeys=true;

    ScreenCapture.save(fileName);
}

void MainWindow::UnsetAllStereo()
{
    ui->actionVery_Low->setChecked(false);
    ui->actionLow->setChecked(false);
    ui->actionMedium->setChecked(false);
    ui->actionHigh->setChecked(false);
    ui->actionVery_High->setChecked(false);
}


void MainWindow::on_actionVery_Low_triggered() {gl3widget->SetStereoSeparation(.005);  UnsetAllStereo(); ui->actionVery_Low->setChecked(true);  UpdateGL(); }
void MainWindow::on_actionLow_triggered() {gl3widget->SetStereoSeparation(.015);   UnsetAllStereo();  ui->actionLow->setChecked(true);   UpdateGL();}
void MainWindow::on_actionMedium_triggered() {gl3widget->SetStereoSeparation(.04);   UnsetAllStereo(); ui->actionMedium->setChecked(true);   UpdateGL();}
void MainWindow::on_actionHigh_triggered() {gl3widget->SetStereoSeparation(.07);  UnsetAllStereo();  ui->actionHigh->setChecked(true);   UpdateGL();}
void MainWindow::on_actionVery_High_triggered() {gl3widget->SetStereoSeparation(.10);  UnsetAllStereo();   ui->actionVery_High->setChecked(true);  UpdateGL();}


//Functions for drawing object tree
void MainWindow::RefreshOneItem(QTreeWidgetItem *item,int i)
{
    if (i<0) return;
    //We need
    //0 - Name
    //1 - Show
    //2 - Key
    //3 - Colour
    //4 - Fidelity
    //5 - Transparency
    //6 - Island Removal
    //7 - Smoothing
    //8 - Shininess

    //Sort out colour box (2)
    QLabel *test=new QLabel();

    QPicture picture;
    QPainter painter;
    painter.begin(&picture);           // paint in picture
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(QColor(SVObjects[i]->Colour[0],SVObjects[i]->Colour[1],SVObjects[i]->Colour[2])));

    painter.drawRect(0,0,28,20); 		// draw a rect
    painter.end();                     // painting done
    test->setPicture(picture);
    test->setAutoFillBackground(true);

    if (SVObjects[i]->IsGroup)
    {
        QLabel *l = new QLabel("");
        ui->treeWidget->setItemWidget (item, 3, l);
    }
    else
    ui->treeWidget->setItemWidget (item, 3, test);

    QString KeySt(SVObjects[i]->Key);
    if (SVObjects[i]->Key==0) KeySt="[-]";

    QString ResampleSt;
    QTextStream rs(&ResampleSt);
    rs<<SVObjects[i]->Resample<<"%";

    item->setText(0,SVObjects[i]->Name);
    item->setText(2,KeySt);

    QLabel *show=new QLabel();
    if (SVObjects[i]->Visible) show->setPixmap(QPixmap(":/eye.bmp"));
    else show->setPixmap(QPixmap(":/eye_off.bmp"));
    ui->treeWidget->setItemWidget (item, 1, show);

    if (SVObjects[i]->IsGroup)
    {
        item->setText(4,"");
        item->setText(5,"");
        item->setText(6,"");
        item->setText(7,"");
        item->setText(8,"");
        item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
    else
    {
        item->setText(4,ResampleSt);

        //Transparency
        QString t;
        if (SVObjects[i]->Transparency==0) t="Off";
        if (SVObjects[i]->Transparency==1) t="Low";
        if (SVObjects[i]->Transparency==2) t="Lowish";
        if (SVObjects[i]->Transparency==3) t="Med";
        if (SVObjects[i]->Transparency==4) t="High";
        if (SVObjects[i]->Transparency<0) t.sprintf("Custom (%d%%)",0-SVObjects[i]->Transparency);
        item->setText(5,t);

        if (SVObjects[i]->IslandRemoval==0) t="Off";
        if (SVObjects[i]->IslandRemoval==1) t="Remove Tiny";
        if (SVObjects[i]->IslandRemoval==2) t="Remove Small";
        if (SVObjects[i]->IslandRemoval==3) t="Remove Medium";
        if (SVObjects[i]->IslandRemoval==4) t="Remove Large";
        if (SVObjects[i]->IslandRemoval==5) t="Remove All";
        if (SVObjects[i]->IslandRemoval<0) t.sprintf("Custom (%d)",0-SVObjects[i]->IslandRemoval);
        item->setText(6,t);

        if (SVObjects[i]->Smoothing==0) t="Off";
        if (SVObjects[i]->Smoothing==1) t="Very weak";
        if (SVObjects[i]->Smoothing==2) t="Weak";
        if (SVObjects[i]->Smoothing==3) t="Medium";
        if (SVObjects[i]->Smoothing==4) t="Strongish";
        if (SVObjects[i]->Smoothing==5) t="Strong";
        if (SVObjects[i]->Smoothing==6) t="Strongest";
        if (SVObjects[i]->Smoothing<0) t.sprintf("Custom (%d)",0-SVObjects[i]->Smoothing);
        item->setText(7,t);

        if (SVObjects[i]->Shininess==0) t="Off";
        if (SVObjects[i]->Shininess==1) t="Less";
        if (SVObjects[i]->Shininess==2) t="Default";
        if (SVObjects[i]->Shininess==3) t="Full";
        if (SVObjects[i]->Shininess<0) t.sprintf("Custom (%d%%)",0-SVObjects[i]->Shininess);
        item->setText(8,t);
    }
}


void MainWindow::DrawChildObjects(QList <bool> selflags, int parent)
{
    QTreeWidgetItem *item;

    //first find lowest Position in list...
    QList <bool> usedflags; for (int i=0; i<SVObjects.count(); i++) usedflags.append(false);
    for (int kloop=0; kloop<SVObjects.count(); kloop++)
    {
    //find lowest
            int lowestval=999999;
            int lowestindex=-1;
            for (int j=0; j<SVObjects.count(); j++)
            {
                    if (SVObjects[j]->Position<lowestval && usedflags[j]==false && SVObjects[j]->Parent()==parent)
                            {lowestval=SVObjects[j]->Position; lowestindex=j;}
            }
            int i=lowestindex;

            if (lowestindex>=0)
            {
                    usedflags[i]=true;

                    QStringList strings;
                    for (int ii=0; ii<8; ii++) strings.append("");
                    item=new QTreeWidgetItem(strings);
                    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    if (parent==-1) ui->treeWidget->addTopLevelItem(item);
                    else SVObjects[parent]->widgetitem->addChild(item);

                    RefreshOneItem(item,i);
                    if (selflags[i]) item->setSelected(true);
                    SVObjects[i]->widgetitem=item; //store pointer
                    if (SVObjects[i]->IsGroup) DrawChildObjects(selflags, i);
            }
    }    return;
}


void MainWindow::RefreshObjects()
{
     if (SVObjects.count()==0) {ui->treeWidget->clear(); return;}

     //set columwidths

     ui->treeWidget->setUniformRowHeights(true);

     if (voxml_mode)
     {
         ui->treeWidget->setColumnHidden(6,true);
         ui->treeWidget->setColumnHidden(7,true);
         ui->treeWidget->setColumnHidden(4,true);
     }

     QList <bool> selflags;

      ui->treeWidget->setUpdatesEnabled(false);

      for (int i=0; i<SVObjects.count(); i++)
       {
                bool sf;
                sf=false;
                if (SVObjects[i]->widgetitem)
                {
                    if ((SVObjects[i]->widgetitem)->isSelected()) sf=true;
                    if ((SVObjects[i]->widgetitem)->isExpanded()) SVObjects[i]->Expanded=true; else SVObjects[i]->Expanded=false;
                }
                selflags.append(sf);
        }

        ui->treeWidget->clear();
        DrawChildObjects(selflags, -1); //start by drawing root children

        for (int i=0; i<SVObjects.count(); i++)
        {
                if (SVObjects[i]->widgetitem && SVObjects[i]->IsGroup)
                {
                        if (SVObjects[i]->Expanded) SVObjects[i]->widgetitem->setExpanded(true);
                                else SVObjects[i]->widgetitem->setExpanded(false);
                }
        }

        //OOTreeWidget->resizeColumnToContents(0);
        ui->treeWidget->setUpdatesEnabled(true);

        //on_OOTreeWidget_itemSelectionChanged();

        RefreshPieces();
        return;
}

void MainWindow::RefreshInfo()
{
    //redo the info window
    if (SPVs.count()==0) return;


    //First - if there is a title use it for window title

    if (i_title.count()==1) setWindowTitle("SPIERSview: " + i_title[0]);
    else
    {
           QString shortfname="SPIERSview - " + fname.mid(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/"))+1);
           setWindowTitle(shortfname);
    }
    ui->infoTreeWidget->clear();

    QTreeWidgetItem * title = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    title->setText(0,"Title");
    QString rootdata="";
    for (int i=0; i<i_title.count(); i++)
    {
        QTreeWidgetItem * titleitem = new QTreeWidgetItem();
        titleitem->setText(0,i_title[i]);
        titleitem->setData(0,Qt::UserRole,i_title[i]);
        title->addChild(titleitem);
        rootdata += i_title[i] + "\n";
    }
    title->setData(0,Qt::UserRole,rootdata);
    ui->infoTreeWidget->addTopLevelItem(title);

    rootdata="";
    QTreeWidgetItem * classification = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    classification->setText(0,"Classification");
    for (int i=0; i<i_classification_name.count(); i++)
    {
        QTreeWidgetItem * commentitem = new QTreeWidgetItem;
        commentitem->setText(0,i_classification_rank[i]+ ": " + i_classification_name[i]);
        commentitem->setData(0,Qt::UserRole,+ ": " + i_classification_name[i]);
        classification->addChild(commentitem);
        rootdata += i_classification_rank[i]+ ": " + i_classification_name[i] + "\n";
    }
    classification->setData(0,Qt::UserRole,rootdata);
    ui->infoTreeWidget->addTopLevelItem(classification);

    rootdata="";
    QTreeWidgetItem * author = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    author->setText(0,"Authors");
    for (int i=0; i<i_author.count(); i++)
    {
        QTreeWidgetItem * commentitem = new QTreeWidgetItem;
        commentitem->setText(0,i_author[i]);
        commentitem->setData(0,Qt::UserRole,i_author[i]);
        author->addChild(commentitem);
        rootdata += i_author[i] + "\n";
    }
    author->setData(0,Qt::UserRole,rootdata);
    ui->infoTreeWidget->addTopLevelItem(author);

    rootdata="";
    QTreeWidgetItem * reference = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    reference->setText(0,"References");
    for (int i=0; i<i_reference.count(); i++)
    {
        QTreeWidgetItem * commentitem = new QTreeWidgetItem;
        commentitem->setText(0,i_reference[i]);
        commentitem->setData(0,Qt::UserRole,i_reference[i]);
        reference->addChild(commentitem);
        rootdata += i_reference[i] + "\n";
    }
    reference->setData(0,Qt::UserRole,rootdata);
    ui->infoTreeWidget->addTopLevelItem(reference);


    rootdata="";
    QTreeWidgetItem * specimen = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    specimen->setText(0,"Specimen");
    for (int i=0; i<i_specimen.count(); i++)
    {
        QTreeWidgetItem * commentitem = new QTreeWidgetItem;
        commentitem->setText(0,i_specimen[i]);
        commentitem->setData(0,Qt::UserRole,i_specimen[i]);
        specimen->addChild(commentitem);
        rootdata += i_specimen[i] + "\n";
    }
    specimen->setData(0,Qt::UserRole,rootdata);
    ui->infoTreeWidget->addTopLevelItem(specimen);

    rootdata="";
    QTreeWidgetItem * provenance = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    provenance->setText(0,"Provenance");
    for (int i=0; i<i_provenance.count(); i++)
    {
        QTreeWidgetItem * commentitem = new QTreeWidgetItem;
        commentitem->setText(0,i_provenance[i]);
        commentitem->setData(0,Qt::UserRole,i_provenance[i]);
        provenance->addChild(commentitem);
        rootdata += i_provenance[i] + "\n";
    }
    provenance->setData(0,Qt::UserRole,rootdata);
    ui->infoTreeWidget->addTopLevelItem(provenance);

    QTreeWidgetItem * comments = new QTreeWidgetItem(QTreeWidgetItem::UserType);
    comments->setText(0,"Comments");
    rootdata="";
    for (int i=0; i<i_comments.count(); i++)
    {
        QTreeWidgetItem * commentitem = new QTreeWidgetItem();
        commentitem->setText(0,i_comments[i]);
        commentitem->setData(0,Qt::UserRole,i_comments[i]);
        comments->addChild(commentitem);
        rootdata += i_comments[i] + "\n";
    }
    comments->setData(0,Qt::UserRole,rootdata);
    ui->infoTreeWidget->addTopLevelItem(comments);

    RedoInfoText();
}

void MainWindow::RedoInfoText()
{
    ui->infoText->setPlainText("");
    QString text;
    if (ui->infoTreeWidget->selectedItems().count()!=1) return;
    text=ui->infoTreeWidget->selectedItems().at(0)->data(0,Qt::UserRole).toString();
    ui->infoText->setPlainText(text);
}

void MainWindow::on_infoTreeWidget_itemSelectionChanged()
{
    RedoInfoText();
}

void MainWindow::on_infoTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (voxml_mode) return; //disabled in VAXML mode
    FileDirty=true;
    //work out if this is a root or child item
    if (item->type()==QTreeWidgetItem::UserType)
    { //a category
        QString title;
        if (item->text(0)=="Title") title="title";
       if (item->text(0)=="Comments") title="comment";
       if (item->text(0)=="Authors") title="author";
       if (item->text(0)=="References") title="reference";
       if (item->text(0)=="Provenance") title="provenance note";
       if (item->text(0)=="Specimen") title="specimen note";
       if (item->text(0)=="Classification") title="classification";
        //it's an add item
       if (title=="title" && i_title.count()!=0)
       { QMessageBox::warning(this,"Only one title!","Only one title is allowed",QMessageBox::Ok); return;}

       if (title=="classification")
       {
           FilterKeys=false;
           QString text1 = QInputDialog::getText(this,"Classification item","Rank:", QLineEdit::Normal,"");
           QString text2 = QInputDialog::getText(this,"Classification item","Name:",QLineEdit::Normal,"");
           FilterKeys=true;
            i_classification_rank.append(text1); i_classification_name.append(text2);

           RefreshInfo();
       }
       else
       {
           FilterKeys=false;
        QString text = QInputDialog::getText(this,"New " + title,"Enter text for new " +title);
           FilterKeys=true;
        if (text.length()>0)
        {
            if (item->text(0)=="Title") i_title.append(text);
            if (item->text(0)=="Comments") i_comments.append(text);
            if (item->text(0)=="Authors") i_author.append(text);
            if (item->text(0)=="References") i_reference.append(text);
            if (item->text(0)=="Provenance") i_provenance.append(text);
            if (item->text(0)=="Specimen") i_specimen.append(text);
            RefreshInfo();
        }
      }
    }
    else //A data item
    {
        QString oldtext=item->text(0);
        QString title;
        if (item->parent()->text(0)=="Title") title="title";
       if (item->parent()->text(0)=="Comments") title="comment";
       if (item->parent()->text(0)=="Authors") title="author";
       if (item->parent()->text(0)=="References") title="reference";
       if (item->parent()->text(0)=="Specimen") title="specimen note";
       if (item->parent()->text(0)=="Provenance") title="provenance note";
       if (item->parent()->text(0)=="Classification") title="classification";
        //it's an add item
       if (title=="classification")
       {
           FilterKeys=false;
           QString text1 = QInputDialog::getText(this,"Edit classification item","Rank:", QLineEdit::Normal,"");
           QString text2 = QInputDialog::getText(this,"Edit classification item","Name:",QLineEdit::Normal,"");
           FilterKeys=true;

           for (int i=0; i<i_classification_rank.count(); i++) if (i_classification_rank[i] + ": " +i_classification_name[i] ==oldtext)
           { i_classification_rank[i]=text1; i_classification_name[i]=text2;}

           RefreshInfo();
       }
       else
       {
           FilterKeys=false;
           QString text = QInputDialog::getText(this,"Edit " + title,"Modify text for " +title,QLineEdit::Normal,item->text(0));
           FilterKeys=true;

           if (text.length()>0)
           {
               if (item->parent()->text(0)=="Title")
                    for (int i=0; i<i_title.count(); i++) if (i_title[i]==oldtext) i_title[i]=text;
               if (item->parent()->text(0)=="Comments")
                    for (int i=0; i<i_comments.count(); i++) if (i_comments[i]==oldtext) i_comments[i]=text;
               if (item->parent()->text(0)=="Authors")
                    for (int i=0; i<i_author.count(); i++) if (i_author[i]==oldtext) i_author[i]=text;
               if (item->parent()->text(0)=="Specimen")
                    for (int i=0; i<i_specimen.count(); i++) if (i_specimen[i]==oldtext) i_specimen[i]=text;
               if (item->parent()->text(0)=="Provenance")
                    for (int i=0; i<i_provenance.count(); i++) if (i_provenance[i]==oldtext) i_provenance[i]=text;
               if (item->parent()->text(0)=="References")
                    for (int i=0; i<i_reference.count(); i++) if (i_reference[i]==oldtext) i_reference[i]=text;

               RefreshInfo();
           }
       }
        //edit item
    }

}

void MainWindow::deleteinfo()
{
    if (ui->infoTreeWidget->isVisible()==false) return;
    if (ui->infoTreeWidget->selectedItems().count()!=1) return;
    if (voxml_mode) return;
    QTreeWidgetItem *item = ui->infoTreeWidget->selectedItems().at(0);

    QString oldtext=item->text(0);
    QString title;
   if (item->text(0)=="Title") title="title";
   if (item->text(0)=="Comments") title="comment";
   if (item->text(0)=="Authors") title="author";
   if (item->text(0)=="Provenance") title="provenance note";
   if (item->text(0)=="Specimen") title="specimen note";
   if (item->text(0)=="References") title="reference";
   if (item->text(0)=="Classification") title="classification item";

    if (item->type()==QTreeWidgetItem::UserType)
    //all of something
    {
        if (item->text(0)=="Comments") title="comment";
        if (item->text(0)=="Title") title="title";
        if (item->text(0)=="Authors") title="author";
        if (item->text(0)=="Provenance") title="provenance note";
        if (item->text(0)=="Specimen") title="specimen note";
        if (item->text(0)=="References") title="reference";
        if (item->text(0)=="Classification") title="classification item";

        if (QMessageBox::question(this,"Delete","Delete ALL " + title + "s?",QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
        {
            if (item->text(0)=="Title") i_title.clear();
            if (item->text(0)=="Comments") i_comments.clear();
            if (item->text(0)=="Authors")  i_author.clear();
            if (item->text(0)=="Specimen")  i_specimen.clear();
            if (item->text(0)=="Provenance")  i_provenance.clear();
            if (item->text(0)=="References") i_reference.clear();
            if (item->text(0)=="Classification") {i_classification_name.clear();i_classification_rank.clear();}
            RefreshInfo();
        }
    }
    else
    //single item
    {
        if (item->parent()->text(0)=="Title") title="title";
        if (item->parent()->text(0)=="Comments") title="comment";
        if (item->parent()->text(0)=="Authors") title="author";
        if (item->parent()->text(0)=="References") title="reference";
        if (item->text(0)=="Provenance") title="provenance note";
        if (item->text(0)=="Specimen") title="specimen note";
        if (item->parent()->text(0)=="Classification") title="classification item";

        if (QMessageBox::question(this,"Delete","Really delete " + title + "?",QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
        {
            if (item->parent()->text(0)=="Title") i_title.clear();

            if (item->parent()->text(0)=="Comments")
                for (int i=0; i<i_comments.count(); i++) if (i_comments[i]==oldtext) {i_comments.removeAt(i); break;}
            if (item->parent()->text(0)=="Authors")
                for (int i=0; i<i_author.count(); i++) if (i_author[i]==oldtext) {i_author.removeAt(i); break;}
            if (item->parent()->text(0)=="Specimen")
                for (int i=0; i<i_specimen.count(); i++) if (i_specimen[i]==oldtext) {i_specimen.removeAt(i); break;}
            if (item->parent()->text(0)=="Provenance")
                for (int i=0; i<i_provenance.count(); i++) if (i_provenance[i]==oldtext) {i_provenance.removeAt(i); break;}
            if (item->parent()->text(0)=="References")
                for (int i=0; i<i_reference.count(); i++) if (i_reference[i]==oldtext) {i_reference.removeAt(i); break;}
            if (item->parent()->text(0)=="Classification")
                for (int i=0; i<i_classification_rank.count(); i++) if (i_classification_rank[i] + ": " +i_classification_name[i]==oldtext) {i_classification_rank.removeAt(i); i_classification_name.removeAt(i); break;}
            RefreshInfo();
        }
    }

}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int column)
//Handler for double clicking in my list
{
    //We need
    //0 - Name
    //1 - Show
    //2 - Key
    //3 - Colour
    //4 - Resample/Simplify

    //Currently not handled for everything else

    //First find index of item
    for (int i=0; i<SVObjects.count(); i++)
    if (item==SVObjects[i]->widgetitem) //found it
    {
        if (column==0)
        {
            FilterKeys=false;
                QString temp=
                QInputDialog::getText (this,"", "", QLineEdit::Normal, SVObjects[i]->Name);
                FilterKeys=true;

                if (temp!="") {SVObjects[i]->Name=temp;    FileDirty=true;}

                RefreshObjects();
        }

        if (column==1)
        {
            SVObjects[i]->Visible=!(SVObjects[i]->Visible);
            FileDirty=true;
            RefreshOneItem(item,i);
            UpdateGL();
        }

        if (column==2) //Key
        {
             QStringList items;
             items << "[None]"<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T"<<"U"<<"V"<<"W"<<"X"<<"Y"<<"Z";
             items <<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9";

             bool ok;

             int asc=(char) (SVObjects[i]->Key).toLatin1(); if (asc>=65 && asc<=90) asc-=64; else if (asc>=48 && asc<=57) asc-=21;
             FilterKeys=false;

             QString qitem = QInputDialog::getItem(this, tr("Shortcut Key"),
                                                  "", items, asc, false, &ok);
             FilterKeys=true;

             if (ok && !qitem.isEmpty())
             {
                     FileDirty=true;
                     if (qitem=="[None]") SVObjects[i]->Key=0; else SVObjects[i]->Key=(int) (qitem.toLatin1()[0]);
             }
             RefreshOneItem(item,i);
        }

        //Is this a group?
        if (SVObjects[i]->IsGroup) return; //don't check colour or resample
        if (column==3) //colour
        {
            QColor newcol;
            FilterKeys=false;

            newcol=QColorDialog::getColor(QColor(SVObjects[i]->Colour[0],SVObjects[i]->Colour[1],SVObjects[i]->Colour[2]));
            FilterKeys=true;

            if (newcol.isValid())
            {
                FileDirty=true;
                SVObjects[i]->Colour[0]=newcol.red();
                SVObjects[i]->Colour[1]=newcol.green();
                SVObjects[i]->Colour[2]=newcol.blue();
                RefreshOneItem(item,i);
                UpdateGL();
            }
        }

        if (column==4)
        {
                SetResample();
        }

    }
}

void MainWindow::SetResample()
//For all selected objects set new resample %age
{
    bool flag;
    FilterKeys=false;

    int temp= QInputDialog::getInt (this,"Resample Percentage", "Enter new resample value for all selected objects",100,0,100,1,&flag);;
    FilterKeys=true;
    if (flag)
    {
        for (int i=0; i<SVObjects.count(); i++)
        {
            if (SVObjects[i]->widgetitem->isSelected())
            {
                SVObjects[i]->Resample=temp;
                SVObjects[i]->Dirty=true;
                FileDirty=true;
            }
        }
        RefreshObjects();
        if (ui->actionAuto_Resurface->isChecked() && ui->actionResurface_Now->isEnabled()) on_actionResurface_Now_triggered();
    }
}

void MainWindow::on_actionSet_Resampling_triggered()
{
    SetResample();
}

void MainWindow::on_actionOff_triggered() {SetSmoothing(0);}
void MainWindow::on_actionVery_Weak_triggered()  {SetSmoothing(1);}
void MainWindow::on_actionWeak_triggered() {SetSmoothing(2);}
void MainWindow::on_actionMedium_2_triggered() {SetSmoothing(3);}
void MainWindow::on_actionStrongish_triggered() {SetSmoothing(4);}
void MainWindow::on_actionStrong_triggered() {SetSmoothing(5);}
void MainWindow::on_actionStrongest_triggered() {SetSmoothing(6);}
void MainWindow::on_actionSmoothing_Custom_triggered()
{
    bool flag;
    FilterKeys=false;
    int temp= QInputDialog::getInt (this,"Smoothing", "Enter custom number of smoothing iterations. Normal range is 0 to 100, but up to 500 are supported.",20,0,500,1,&flag);
    FilterKeys=true;
    if (flag) SetSmoothing(0-temp);
}

void MainWindow::on_actionOff_2_triggered() {SetIslands(0);}
void MainWindow::on_actionRemove_Tiny_triggered() {SetIslands(1);}
void MainWindow::on_actionRemove_Small_triggered() {SetIslands(2);}
void MainWindow::on_actionRemove_Medium_triggered() {SetIslands(3);}
void MainWindow::on_actionRemove_Large_triggered() {SetIslands(4);}
void MainWindow::on_actionRemove_All_triggered() {SetIslands(5);}
void MainWindow::on_actionIsland_Removal_Custom_triggered()
{
    bool flag;
    FilterKeys=false;
    int temp= QInputDialog::getInt (this,"Island Removal", "Enter cut-off size; islands with fewer triangles with this will be removed.",20,0,999999999,1,&flag);
    FilterKeys=true;

    if (flag) SetIslands(0-temp);
}

void MainWindow::SetSmoothing(int v)
{
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->widgetitem->isSelected())
        {
            SVObjects[i]->Smoothing=v;
            FileDirty=true;
            SVObjects[i]->Dirty=true;
        }
    }
    RefreshObjects();
    if (ui->actionAuto_Resurface->isChecked() && ui->actionResurface_Now->isEnabled()) on_actionResurface_Now_triggered();
}

void MainWindow::SetIslands(int v)
{
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->widgetitem->isSelected())
        {
            SVObjects[i]->IslandRemoval=v;
            FileDirty=true;
            SVObjects[i]->Dirty=true;
        }
    }
    RefreshObjects();
    if (ui->actionAuto_Resurface->isChecked() && ui->actionResurface_Now->isEnabled()) on_actionResurface_Now_triggered();
}

void MainWindow::SetTrans(int v)
{
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->widgetitem->isSelected())
        {
            FileDirty=true;
            SVObjects[i]->Transparency=v;
        }
    }
    RefreshObjects();
    UpdateGL();
}


void MainWindow::SetShininess(int s)
{
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->widgetitem->isSelected())
        {
            FileDirty=true;
            SVObjects[i]->Shininess=s;
        }
    }
    RefreshObjects();
    UpdateGL();
}

void MainWindow::on_actionResurface_Now_triggered()
{
    //show prog bars if needbe
    if (ui->actionProgress_Bars->isChecked()==false && voxml_mode==true) {ui->actionProgress_Bars->setEnabled(false); ui->ProgressDock->setVisible(true);}
    if (voxml_mode==false) MainWin->DisableRenderCommands();
    //How many to change?
    int ObjectsRedoing=0;
    for (int i=0; i<SVObjects.count(); i++) if (SVObjects[i]->Dirty && SVObjects[i]->IsGroup==false) ObjectsRedoing++;

    int ThisObject=0;

    for (int i=0; i<SVObjects.count(); i++) //have to clear VBOs first or there are problems
    if (SVObjects[i]->Dirty)
    {
        qDeleteAll(SVObjects[i]->VertexBuffers);
        qDeleteAll(SVObjects[i]->ColourBuffers);
        SVObjects[i]->BoundingBoxBuffer.destroy();
        SVObjects[i]->VertexBuffers.clear();
        SVObjects[i]->ColourBuffers.clear();

    }

    for (int i=0; i<SVObjects.count(); i++)
    if (SVObjects[i]->Dirty)
    {
        SVObjects[i]->ForceUpdates(ThisObject++, ObjectsRedoing);
        UpdateGL();
    }

    ui->OutputLabelOverall->setText("Complete");
    ui->ProgBarOverall->setValue(100);

    if (voxml_mode==false) MainWin->EnableRenderCommands();
    if (ui->actionProgress_Bars->isChecked()==false && voxml_mode==true) {ui->actionProgress_Bars->setEnabled(true); ui->ProgressDock->setVisible(false);}
}


void  MainWindow::on_actionOpaque_2_triggered() {SetTrans(0);}
void  MainWindow::on_action75_Opaque_2_triggered() {SetTrans(1);}
void  MainWindow::on_action62_Opaque_triggered(){SetTrans(2);}
void  MainWindow::on_action50_Opaque_triggered(){SetTrans(3);}
void  MainWindow::on_action25_Opaque_triggered(){SetTrans(4);}

void  MainWindow::on_actionTransparency_Custom_triggered()
{
    bool flag;
    FilterKeys=false;
    int temp= QInputDialog::getInt (this,"Transparency", "Enter custom transparency percentage from 0 (opaque) to 99 for all selected objects",50,0,99,1,&flag);
    FilterKeys=true;
    if (flag) SetTrans(0-temp);
}

void  MainWindow::on_actionMatte_triggered() {SetShininess(0);}
void  MainWindow::on_actionSlightly_Shiny_triggered() {SetShininess(1);}
void  MainWindow::on_actionFairly_Shiny_triggered() {SetShininess(2);}
void  MainWindow::on_actionShiny_triggered() {SetShininess(3);}
void  MainWindow::on_actionShininessCustom_triggered()
{
    bool flag;
    FilterKeys=false;
    int temp= QInputDialog::getInt (this,"Shininess Percentage", "Enter custom shininess percentage from 0 to 100 for all selected objects",50,0,100,1,&flag);
    FilterKeys=true;
    if (flag) SetShininess(0-temp);
}




void MainWindow::EnableRenderCommands()
{
   //qDebug()<<"Here";
    ui->actionAuto_Resurface->setEnabled(true);
   ui->actionResurface_Now->setEnabled(true);
   ui->actionOff->setEnabled(true);
   //ui->actionLow->setEnabled(true);
   //ui->actionMedium->setEnabled(true);
   ui->actionMedium_2->setEnabled(true);
   ui->actionOff_2->setEnabled(true);
   ui->actionRemove_All->setEnabled(true);
   ui->actionRemove_Large->setEnabled(true);
   ui->actionRemove_Medium->setEnabled(true);
   ui->actionRemove_Small->setEnabled(true);
   ui->actionRemove_Tiny->setEnabled(true);
   ui->actionSet_Resampling->setEnabled(true);
   ui->actionQuadric_Fidelity_Reduction->setEnabled(true);
    ui->actionRemove_Piece->setEnabled(true);
    ui->actionImport_Replacement->setEnabled(true);
    ui->actionSmoothing_Custom->setEnabled(true);
    ui->actionIsland_Removal_Custom->setEnabled(true);
   ui->actionStrong->setEnabled(true);
   ui->actionStrongest->setEnabled(true);
   ui->actionStrongish->setEnabled(true);
   ui->actionVery_Weak->setEnabled(true);
   ui->actionWeak->setEnabled(true);
   ui->actionSave_Presurfaced->setEnabled(true);
   ui->actionSave_Finalised_As->setEnabled(true);

   if (IsSP2) ui->actionSave_Changes->setEnabled(false);  else ui->actionSave_Changes->setEnabled(true);
   ui->actionSave_As->setEnabled(true);
   ui->actionSTL->setEnabled(true);
   ui->actionDXF->setEnabled(true);
   ui->actionImport_SPV->setEnabled(true);

   ui->actionAANone->setEnabled(true);
   ui->action2x->setEnabled(true);
   ui->action4x->setEnabled(true);
   ui->action8x->setEnabled(true);
   ui->action16x->setEnabled(true);

   qApp->processEvents();
   //qDebug()<<"Out";
}

void MainWindow::DisableRenderCommands()
{
    ui->actionAuto_Resurface->setEnabled(false);
    ui->actionResurface_Now->setEnabled(false);
    ui->actionOff->setEnabled(false);
   //ui->actionLow->setEnabled(false);
   //ui->actionMedium->setEnabled(false);
   ui->actionMedium_2->setEnabled(false);
   ui->actionOff_2->setEnabled(false);
   ui->actionRemove_All->setEnabled(false);
   ui->actionRemove_Large->setEnabled(false);
   ui->actionRemove_Medium->setEnabled(false);
   ui->actionRemove_Small->setEnabled(false);
   ui->actionRemove_Tiny->setEnabled(false);
   ui->actionSet_Resampling->setEnabled(false);
   ui->actionQuadric_Fidelity_Reduction->setEnabled(false);
   ui->actionRemove_Piece->setEnabled(false);
   ui->actionImport_Replacement->setEnabled(false);

   ui->actionStrong->setEnabled(false);
   ui->actionStrongest->setEnabled(false);
   ui->actionStrongish->setEnabled(false);
   ui->actionVery_Weak->setEnabled(false);
   ui->actionWeak->setEnabled(false);
   ui->actionSave_Presurfaced->setEnabled(false);
   ui->actionSave_Finalised_As->setEnabled(false);
   ui->actionSave_Changes->setEnabled(false);
   ui->actionSave_As->setEnabled(false);
   ui->actionSTL->setEnabled(false);
   ui->actionDXF->setEnabled(false);
   ui->actionImport_SPV->setEnabled(false);
   ui->actionAuto_Spin->setChecked(false);

   ui->actionAANone->setEnabled(false);
   ui->action2x->setEnabled(false);
   ui->action4x->setEnabled(false);
   ui->action8x->setEnabled(false);
   ui->action16x->setEnabled(false);

   ui->actionIsland_Removal_Custom->setEnabled(false);
   ui->actionSmoothing_Custom->setEnabled(false);
   qApp->processEvents();
}

void MainWindow::on_actionObject_Panel_triggered()
{
    if (ui->actionObject_Panel->isChecked())
    {
        ui->dockWidget->setVisible(true);
    }
    else
    {
        ui->dockWidget->setVisible(false);
    }
}

void MainWindow::on_actionOrthographic_View_triggered()
{
    if (gl3widget->context()->format().stereo()) on_actionQuadBuffer_Stereo_triggered();
    else
    UpdateGL();
}

void  MainWindow::on_actionPieces_Panel_triggered()
{
    if (ui->actionPieces_Panel->isChecked())
    {
        ui->dockWidgetPieces->setVisible(true);
    }
    else
    {
        ui->dockWidgetPieces->setVisible(false);
    }
}

void MainWindow::on_actionAbout_triggered()
{
        aboutdialog d;
        FilterKeys=false;
        d.exec();
        FilterKeys=true;

}

void MainWindow::on_actionQuick_Guide_triggered()
{
        QuickHelpBox d;
        d.exec();
}

void MainWindow::on_actionInfo_triggered()
{
    if (ui->actionInfo->isChecked())
    {
        ui->InfoDock->setVisible(true);
    }
    else
    {
        ui->InfoDock->setVisible(false);
    }
}

void MainWindow::on_actionProgress_Bars_triggered()
{
    if (ui->actionProgress_Bars->isChecked())
    {
        ui->ProgressDock->setVisible(true);
    }
    else
    {
        ui->ProgressDock->setVisible(false);
    }
}

void MainWindow::on_actionAnimation_Panel_triggered()
{
    if (ui->actionAnimation_Panel->isChecked())
    {
        ui->AnimDock->setVisible(true);
    }
    else
    {
        ui->AnimDock->setVisible(false);
    }
}

void MainWindow::on_actionClip_Controls_triggered()
{
    if (ui->actionClip_Controls->isChecked())
    {
        ui->ClipDock->setVisible(true);
    }
    else
    {
        ui->ClipDock->setVisible(false);
    }
}

void MainWindow::on_actionSave_Changes_triggered()
{
   bool flag=false;
   if (ContainsPresurfaced && !ContainsNonPresurfaced) flag=true;
   if (!ContainsPresurfaced && ContainsNonPresurfaced) flag=false;
   if (ContainsPresurfaced && ContainsNonPresurfaced)
   {
       if (QMessageBox::question(this,"Ambiguous save", "This file contains items with both presurfaced and compact data. Proceeding will save as compact - is this OK?", QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes) return;
   }

   SPVreader r;
   r.WriteFile(flag);
   FileDirty=false;
}

void MainWindow::on_actionSave_As_triggered()
{
    //First - check no pre v5 files in here

    QString cpath;
    cpath=fname;
    cpath=fname.left(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/")));

	/*QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter("SPV files (*.spv)");
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory(cpath);

	QStringList files;
	if (dialog.exec())
		 files = dialog.selectedFiles();
	else return;

	QString f=files[0];

	*/
    FilterKeys=false;

        QString f=QFileDialog::getSaveFileName(this, tr("Save File (Compact Mode)"),
                            cpath,
                            tr("SPV files (*.spv)"));
        FilterKeys=true;

	if (f.isEmpty()) return;
    fname=f;
    QString shortfname="SPIERSview - " + fname.mid(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/"))+1);
    this->setWindowTitle(shortfname);
    SPVreader r;
    r.WriteFile(false);
    FileDirty=false;

}


void MainWindow::on_actionSave_Presurfaced_triggered()
{
    QString cpath;
    cpath=fname;
    cpath=fname.left(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/")));
    FilterKeys=false;

    QString f=QFileDialog::getSaveFileName(this, tr("Save File (Presurfaced Mode)"),
                            cpath,
                            tr("SPV files (*.spv)"));
    FilterKeys=true;

	if (f.isEmpty()) return;
    fname=f;
    QString shortfname="SPIERSview - " + fname.mid(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/"))+1);
    this->setWindowTitle(shortfname);

    SPVreader r;
    r.WriteFile(true); //with polydata structures tagged on
    ContainsPresurfaced=true;
    ContainsNonPresurfaced=false;
    FileDirty=false;
}

void MainWindow::on_actionDXF_triggered()
{

/*	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter("DXF files (*.dxf)");
	dialog.setViewMode(QFileDialog::Detail);

	QStringList files;
	if (dialog.exec())
		 files = dialog.selectedFiles();
	else return;

	QString filename=files[0];

*/
    FilterKeys=false;

    QString filename = QFileDialog::getSaveFileName(
                     this,
                     "Filename for DXF export",
                     "",
                     "DXF files (*.dxf)");
    FilterKeys=true;

    //Now we do a whole load of initialisation!
    if (filename.isNull()) return; //if nothing there, cancel

    DisableRenderCommands();

    QFile dxffile(filename);
    if (dxffile.open(QIODevice::WriteOnly | QIODevice::Text)==false)
        QMessageBox::warning(this,"Can't open file for writing","Error - I can't open the file. Is it write-protected?");
    else
    {
        QTextStream dxf(&dxffile);
        //basic header
        dxf<<"999\nCreated by SPIERSview 2\n0\n";
        dxf<<"SECTION\n2\nHEADER\n9\n$ACADVER\n1\nAC1006\n9\n$INSBASE\n10\n0.0\n20\n0.0\n30\n0.0\n9\n$EXTMIN\n10\n0.0\n20\n0.0\n9\n$EXTMAX\n10\n1000.0\n20\n1000.0\n0\nENDSEC\n";
        dxf<<"0\nSECTION\n2\nTABLES\n0\nTABLE\n2\nLTYPE\n70\n1\n0\nLTYPE\n2\nCONTINUOUS\n70\n64\n3\nSolid line\n72\n65\n73\n0\n40\n0.000000\n0\nENDTAB\n0\n";

        //now layer definitions

        //layer header
        dxf<<"TABLE\n2\nLAYER\n70\n6\n0\n";
        int counto=1;
            for (int i=0; i<SVObjects.count(); i++)
            if (!(SVObjects[i]->IsGroup))
            {
                counto++;
                QString name;
                if (SVObjects[i]->Name.isEmpty()) name.sprintf("%d",counto); else name=SVObjects[i]->Name;
                dxf<<"LAYER\n2\n"<<name.toLatin1()<<"\n70\n64\n62\n7\n6\nCONTINUOUS\n0\n";
            }

        //last few bits
        dxf<<"ENDTAB\n0\nSECTION\n2\nBLOCKS\n0\nENDSEC\n";

        dxf<<"0\nSECTION\n2\nTABLES\n0\nTABLE\n2\nLTYPE\n70\n1\n0\nLTYPE\n2\nCONTINUOUS\n70\n64\n3\nSolid line\n72\n65\n73\n0";
        dxf<<"\n40\n0.0\n0\nENDTAB\n0\nTABLE\n2\nLAYER\n70\n1\n0\nLAYER\n2\nIso\n70\n64\n62\n1\n6\nCONTINUOUS\n0\nENDTAB\n0\nENDSEC\n";

        //setup entities section
        dxf<<"0\nSECTION\n2\nENTITIES\n";

        dxf.flush();
        //now do all the objects
        long count=0;
        for (int i=0; i<SVObjects.count(); i++)
        if (!(SVObjects[i]->IsGroup) && (SVObjects[i]->Visible || ui->actionExport_Hidden_Objects->isChecked()))
        {
            QString status;
            status.sprintf("Exporting object %d of %d", i+1, SVObjects.count());
            ui->OutputLabelOverall->setText(status);
            ui->ProgBarOverall->setValue((i*100)/SVObjects.count());
            //find name
            count+=(long)SVObjects[i]->WriteDXFfaces(&dxffile);
        }

        ui->OutputLabelOverall->setText("DXF export complete");
        ui->ProgBarOverall->setValue(100);
        dxf<<"0\nENDSEC\n0\nEOF\n";
     }
    EnableRenderCommands();
}

void MainWindow::on_actionSave_Finalised_As_triggered()
{

    QString cpath, oldfname;
    oldfname=cpath=fname;
    cpath=fname.left(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/")));
    FilterKeys=false;

        QString f=QFileDialog::getSaveFileName(this, tr("Save File (Finalised Mode)"),
                            cpath,
                            tr("SPVF files (*.spvf)"));
        FilterKeys=true;

        if (f.isEmpty()) return;
    fname=f;


    DisableRenderCommands();

    int objcount=0;
    for (int i=0; i<SVObjects.count(); i++)
    if (!(SVObjects[i]->IsGroup && (SVObjects[i]->Visible || ui->actionExport_Hidden_Objects->isChecked()))) objcount++;

    voxml v;
    if (v.write_voxml(fname,true)==false)
    {  EnableRenderCommands(); return;}

    long count=0;
    for (int i=0; i<SVObjects.count(); i++)
    if (!(SVObjects[i]->IsGroup && (SVObjects[i]->Visible || ui->actionExport_Hidden_Objects->isChecked())))
    {
        QString status;
        status.sprintf("Saving object %d of %d", i+1, objcount);
        ui->OutputLabelOverall->setText(status);
        ui->ProgBarOverall->setValue((i*100)/objcount);

        //find name
        QString fname2;
        fname2.sprintf("%d",SVObjects[i]->Index+1);
        if (!(SVObjects[i]->Name.isEmpty())) {fname2.append("-"); fname2.append(SVObjects[i]->Name);}
        fname2.append(".stl");

        count+=(long)SVObjects[i]->AppendCompressedFaces(fname, fname2, &(v.dataout));
    }

    ui->OutputLabelOverall->setText("SPVF finalised export complete");
    ui->ProgBarOverall->setValue(100);

    EnableRenderCommands();
    QString shortfname="SPIERSview - " + fname.mid(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/"))+1);
    this->setWindowTitle(shortfname);
    fname=oldfname; //restore fname for use in save as
}

void MainWindow::on_actionSTL_triggered()
{
    QString cpath, oldfname;
    oldfname=cpath=fname;
    cpath=fname.left(qMax(fname.lastIndexOf("\\"),fname.lastIndexOf("/")));
    FilterKeys=false;

        QString f=QFileDialog::getSaveFileName(this, tr("Save VAXML File"),
                            cpath,
                            tr("VAXML files (*.vaxml)"));
        FilterKeys=true;
        if (f.isEmpty()) return;
    fname=f;


    DisableRenderCommands();


    int objcount=0;
    for (int i=0; i<SVObjects.count(); i++)
    if (!(SVObjects[i]->IsGroup)) objcount++;

//    qDebug()<<fname;
    //Now write the voxml file - use current file name - do first to avoid finding problems after long STL export!
    voxml v;
    if (v.write_voxml(fname,false)==false)
    { ui->OutputLabelOverall->setText("VAXML export failed"); EnableRenderCommands(); return;}
    else
            ui->OutputLabelOverall->setText("VAXML export complete, exporting STL component objects");

    long count=0;
    for (int i=0; i<SVObjects.count(); i++)
    if (!(SVObjects[i]->IsGroup))
    {
        QString status;
        status.sprintf("Exporting object %d of %d", i+1, objcount);
        ui->OutputLabelOverall->setText(status);
        ui->ProgBarOverall->setValue((i*100)/objcount);
        //find name

        QString fname2;
        fname2.sprintf("%d",SVObjects[i]->Index+1);
        if (!(SVObjects[i]->Name.isEmpty())) {fname2.append("-"); fname2.append(SVObjects[i]->Name);}
        fname2.append(".stl");

        QFileInfo fi(fname);

        fname2=fi.dir().absolutePath() + "/" + fi.baseName() + "_stl/" + fname2;
//        qDebug()<<"Outputting"<<fname2<<fi.dir().absolutePath() + "/" + fi.baseName() + "_stl";
        count+=(long)SVObjects[i]->WriteSTLfaces(fi.dir().absolutePath() + "/" + fi.baseName() + "_stl", fname2);
    }

    ui->OutputLabelOverall->setText("VAXML / STL export complete");
    ui->ProgBarOverall->setValue(100);

    EnableRenderCommands();
    fname=oldfname; //restore fname for use in save as
}

void MainWindow::on_actionAuto_Resurface_triggered()
{
     if (ui->actionAuto_Resurface->isChecked()) on_actionResurface_Now_triggered();
}

void MainWindow::on_actionShow_All_triggered()
{
    for (int i=0; i<SVObjects.count(); i++)   SVObjects[i]->Visible=true;
    FileDirty=true;
    RefreshObjects();
    UpdateGL();
}

void MainWindow::on_actionHide_All_triggered()
{
    //hide everything except groups
    for (int i=0; i<SVObjects.count(); i++)  if (SVObjects[i]->IsGroup==false) SVObjects[i]->Visible=false;
    FileDirty=true;
    RefreshObjects();
    UpdateGL();
}

void MainWindow::on_actionInvert_Show_triggered()
{
    //First - note what is visible
    QList <bool> visible;
    for (int i=0; i<SVObjects.count(); i++) if (SVObjects[i]->IsGroup) visible.append(false); else visible.append(gl3widget->CanISee(i));

    //then go through - turn ON all groups, everything else as in visible list
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->IsGroup) SVObjects[i]->Visible=true;
        else SVObjects[i]->Visible=!visible[i];
    }
    FileDirty=true;
    RefreshObjects();
    UpdateGL();
}

void MainWindow::on_actionSave_Memory_triggered()
{
     FileDirty=true;
     if (ui->actionSave_Memory->isChecked())
         for (int i=0; i<SVObjects.count(); i++) SVObjects[i]->CompressPolyData(false);
}

void MainWindow::on_actionRemove_Piece_triggered()
{
    if (ui->PiecesList->selectedItems().count()==0)
    {
        QMessageBox::warning(this,"Can't Delete","Select a piece to delete first!");
    }
    else if (ui->PiecesList->count()==1)
    {
        QMessageBox::warning(this,"Can't Delete","Can't delete the final piece");
    }
    else
    {
        int i=ui->PiecesList->currentRow();
        QString piecename=SPVs[i]->filenamenopath;

        QMessageBox msgBox;
       msgBox.setText("Delete a Piece");
       msgBox.setInformativeText("Do you really delete the piece '" + piecename + "?");
       msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
       msgBox.setDefaultButton(QMessageBox::No);
       int ret = msgBox.exec();
       if (ret==QMessageBox::No) return;

       //OK, we are go!

       KillSPV(i);
   }
}

void MainWindow::on_actionImport_SPV_triggered()
{
/*	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter("SPV files (*.spv)");
	dialog.setViewMode(QFileDialog::Detail);

	QStringList files;
	if (dialog.exec())
		 files = dialog.selectedFiles();
	else return;

	QString ifname=files[0];
*/
    FilterKeys=false;

    QString ifname = QFileDialog::getOpenFileName(
                     this,
                     "Select SPIERSview file to import",
                     "",
                     "SPIERSview files (*.spv)");
    FilterKeys=true;

    //Now we do a whole load of initialisation!
    if (ifname.isNull()) return; //if nothing there, cancel

    //OK, we have a file. Try
    SPVreader r;
    r.ProcessFile(ifname);
    FileDirty=true;
    RefreshInfo();

}

void MainWindow::KillSPV(int retcode)
{
    SPV *s=SPVs[retcode];
       //OK. Now a horrible job. Need to remove all objects first. Perhaps go through all non-groups in this SPV and flag them for removal
    for (int i=0; i<s->ComponentObjects.count(); i++)
        if (!(s->ComponentObjects[i]->IsGroup)) s->ComponentObjects[i]->killme=true;

    //Second, go through the main object list and remove anything flagged(use mutable iterator), deleting it too.
    //Don't have to bother removing stuff from SPVs list, as entire SPV is going to be deleted
    QMutableListIterator<SVObject *> it(SVObjects);
    while (it.hasNext())
    {
        SVObject *o = it.next();
        if (o->killme) {delete o; it.remove();}
    };

    //Then remove the SPV from the SPVs list and delete that - Put a dealloc in the destructor  for stretches at least
    delete SPVs.takeAt(retcode);

    //remove any empty groups (for each group do a trawl for children. Mark any with none.
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->IsGroup)
        {
            int children=0; //now count children
            for (int j=0; j<SVObjects.count(); j++)
            {
                if (SVObjects[j]->InGroup == SVObjects[i]->Index) children++;
            }
            if (children==0) SVObjects[i]->killme=true;
        }
    }

    //Another delete run  - remove any empty groups found
    //first from SPV lists
    for (int i=0; i<SPVs.count(); i++)
    {
        QMutableListIterator<SVObject *> it2(SPVs[i]->ComponentObjects);
        while (it2.hasNext())
        {
            SVObject *o = it2.next();
            if (o->killme) {it2.remove();}
        };
    }

    //Now from overall item list, this time deleting object
    QMutableListIterator<SVObject *> it2(SVObjects);
    while (it2.hasNext())
    {
        SVObject *o = it2.next();
        if (o->killme) {delete o; it2.remove();}
    };


    //Newer code to sort out all the indexes:
    //go through entire list of SVobjects
    //Create conversion table of old index to new index � just loop through each, put oldindex in oldindexfor[n]
    //Now go through and fix old indices
    //Then go through and fix parents using same lookup table (called InGroup)

    int newindex=0;
    QList <int> indices;

    //work out my lookuptable
    for (int i=0; i<SVObjects.count(); i++)
    {
        int ind=SVObjects[i]->Index;
        //does it exist?
        if (indices.indexOf(ind)==-1) //didn't find item
        indices.append(ind);
    }

    //go through and correct
    for (int i=0; i<SVObjects.count(); i++)
        SVObjects[i]->Index=indices.indexOf(SVObjects[i]->Index);

    //now go through and correct the InGroups
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->InGroup!=-1)
            SVObjects[i]->InGroup=indices.indexOf(SVObjects[i]->InGroup);
    }

    FileDirty=true;
    RefreshObjects();

}
void MainWindow::on_actionImport_Replacement_triggered()
{
    //import, but check we have a piece selected first

    if (ui->PiecesList->selectedItems().count()==0)
    {
        QMessageBox::warning(this,"Can't Replace","Select a piece to replace first!");
    }
    else
    {
/*
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::ExistingFile);
		dialog.setNameFilter("SPV files (*.spv)");
		dialog.setViewMode(QFileDialog::Detail);

		QStringList files;
		if (dialog.exec())
			 files = dialog.selectedFiles();
		else return;

		QString ifname=files[0];
*/
        FilterKeys=false;

        QString ifname = QFileDialog::getOpenFileName(
                         this,
                         "Select SPIERSview file to import",
                         "",
                         "SPIERSview files (*.spv)");
        FilterKeys=true;

        //Now we do a whole load of initialisation!
        if (ifname.isNull()) return; //if nothing there, cancel

        //OK, we have a file. Try
        SPVreader r;
        int retcode=r.ProcessFileReplacement(ifname,ui->PiecesList->currentRow());
        if (retcode==-2) QMessageBox::warning(this,"Can't Replace","Replacement file must contain a single piece only");
        if (retcode==-3) QMessageBox::warning(this,"Can't Replace","Replacement file format too old");

        if (retcode<0) return;

        KillSPV(retcode);
    }
}

void MainWindow::RefreshPieces()
{
    ui->PiecesList->clear();
    for (int i=0; i<SPVs.count(); i++)
    {
        QString name;
        name.sprintf("%d: ",i+1);
        name.append(SPVs[i]->filenamenopath);
        ui->PiecesList->addItem(name);
    }
}

void MainWindow::on_PiecesList_itemSelectionChanged()
{
    if (ui->PiecesList->count()<SPVs.count()) return;
    for (int i=0; i<SPVs.count(); i++)
    {
        if (ui->PiecesList->item(i)->isSelected())
        {
            for (int j=0; j<SVObjects.count(); j++)
               SVObjects[j]->widgetitem->setSelected(false);

            for (int j=0; j<SPVs[i]->ComponentObjects.count(); j++)
            {
                SPVs[i]->ComponentObjects[j]->widgetitem->setSelected(true);
            }
        }
    }

}

void MainWindow::on_actionSelect_All_triggered()
{
        for (int j=0; j<SVObjects.count(); j++)
           SVObjects[j]->widgetitem->setSelected(true);
}

void MainWindow::on_actionSelect_None_triggered()
{
        for (int j=0; j<SVObjects.count(); j++)
           SVObjects[j]->widgetitem->setSelected(false);
}

void MainWindow::on_PiecesList_itemDoubleClicked(QListWidgetItem *item)
{
    int i=ui->PiecesList->currentRow();
    FilterKeys=false;
    QString temp=
    QInputDialog::getText (this,"", "", QLineEdit::Normal, SPVs[i]->filenamenopath);
    FilterKeys=true;

    if (temp!="") SPVs[i]->filenamenopath=temp;
    RefreshPieces();
}

void MainWindow::on_actionIncrease_Size_triggered()
{
    gl3widget->Resize(1.003);
    FileDirty=true;
    UpdateGL();
}

void MainWindow::on_actionDecrease_Size_triggered()
{
    gl3widget->Resize(.997);
    FileDirty=true;
    UpdateGL();
}

void MainWindow::on_actionReset_Size_triggered()
{
    gl3widget->ResetSize();
    FileDirty=true;
    UpdateGL();
}

 void MainWindow::closeEvent(QCloseEvent *event)
 {
     if (voxml_mode==true) {event->accept(); return;}

     if (!(ui->actionSave_As->isEnabled()))
     {
         //quitting during a surfacing action - confirm
           QMessageBox msgBox;
           msgBox.setText("Processing underway");
           msgBox.setInformativeText("Do you really want to quit while SPIERSview is working? You will not be able to save any changes.");
           msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
           msgBox.setDefaultButton(QMessageBox::Cancel);
           int ret = msgBox.exec();
           if (ret==QMessageBox::Yes) exit(0);
           event->ignore();
           return;
     }
     if (FileDirty)
     {
       QMessageBox msgBox;
       msgBox.setText("Data has beeen modified");
       msgBox.setInformativeText("Do you want to save changes?");
       msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
       msgBox.setDefaultButton(QMessageBox::Save);
       int ret = msgBox.exec();
       if (ret==QMessageBox::Save)
       {
           on_actionSave_Changes_triggered();
           event->accept();
           exit(0);
       }
       else if (ret==QMessageBox::Cancel) event->ignore();
       else if (ret==QMessageBox::Discard) {event->accept();   exit(0);}
   }

 }

void MainWindow::on_action_Move_Up_triggered()
{
        QList <QTreeWidgetItem *> selected=ui->treeWidget->selectedItems();
        if (selected.count()!=1) QMessageBox::information(this,"","Select a single item to move up");
        else
        {
                for (int i=0; i<SVObjects.count(); i++)
                {
                        if (SVObjects[i]->widgetitem==selected[0])
                        {
                                //i is now the correct index
                                //find 	one to swap with
                                int highestval=-1;
                                int highindex=-1;
                                for (int j=0; j<SVObjects.count(); j++)
                                {
                                        if ((SVObjects[j]->Position)>highestval && (SVObjects[j]->Position)< SVObjects[i]->Position && (SVObjects[j]->InGroup)==(SVObjects[i]->InGroup))
                                                {
                                                        highestval=SVObjects[j]->Position;
                                                        highindex=j;
                                                }
                                }

                                if (highindex>=0)
                                {
                                        //do the swap
                                        int temp=SVObjects[highindex]->Position;
                                        SVObjects[highindex]->Position=SVObjects[i]->Position;
                                        SVObjects[i]->Position=temp;
                                        FileDirty=true;
                                }
                        }
                }
        }
        RefreshObjects();
}

void MainWindow::on_actionMove_Down_triggered()
{
        QList <QTreeWidgetItem *> selected=ui->treeWidget->selectedItems();
        if (selected.count()!=1) QMessageBox::information(this,"","Select a single item to move down");
        else
        {
                for (int i=0; i<SVObjects.count(); i++)
                {
                        if (SVObjects[i]->widgetitem==selected[0])
                        {
                                //i is now the correct index
                                //find 	one to swap with
                                int lowestval=9999999;
                                int lowestindex=-1;
                                for (int j=0; j<SVObjects.count(); j++)
                                {
                                        if ((SVObjects[j]->Position)<lowestval && (SVObjects[j]->Position)> SVObjects[i]->Position && (SVObjects[j]->InGroup)==(SVObjects[i]->InGroup))
                                                {
                                                        lowestval=SVObjects[j]->Position;
                                                        lowestindex=j;
                                                }
                                }

                                if (lowestindex>=0)
                                {
                                        //do the swap
                                        int temp=SVObjects[lowestindex]->Position;
                                        SVObjects[lowestindex]->Position=SVObjects[i]->Position;
                                        SVObjects[i]->Position=temp;
                                        FileDirty=true;
                                }
                        }
                }
        }
        RefreshObjects();
}

void MainWindow::on_actionGroup_triggered()
{
    QList <QTreeWidgetItem *> selected=ui->treeWidget->selectedItems();
    int oldparent=-2;
    bool flag=false;
    int nextindex=-1;
    for (int i=0; i<SVObjects.count(); i++)
        if (SVObjects[i]->Index>nextindex) nextindex=SVObjects[i]->Index;
    nextindex++;
    if (selected.count()==0) QMessageBox::warning(this,"","Select objects to group");
    else
    {
            //OK, do grouping
            for (int i=0; i<selected.count(); i++)
                    for (int j=0; j<SVObjects.count(); j++)
                            if (SVObjects[j]->widgetitem==selected[i])
                            {
                                    if (oldparent==-2) oldparent=SVObjects[j]->InGroup;
                                    if (oldparent!=SVObjects[j]->InGroup) flag=true; //this will have to be root level
                                    SVObjects[j]->InGroup=nextindex; //reparents if necessary of course
                            }

            SVObject *o=new SVObject(nextindex);
            o->Key=0; //no key
            o->Visible=true;
            o->Name="Group";
            o->IsGroup=true;
            if (flag) o->InGroup=-1; else o->InGroup=oldparent;
            int max=-1;
            for (int i=0; i<SVObjects.count(); i++)
                    if (SVObjects[i]->Position>max) max=SVObjects[i]->Position;

            o->Position=max+1;
            SVObjects.append(o);
            FileDirty=true;
            RefreshObjects();
    }
}

void MainWindow::on_actionUngroup_triggered()
{
    //Ungroup command
    //For all selected objects:
    bool flag=true;
    for (int i=0; i<SVObjects.count(); i++)
    {
            if (SVObjects[i]->widgetitem->isSelected())
            {
                    if (SVObjects[i]->InGroup!=-1)
                    {
                            SVObjects[i]->InGroup = SVObjects[SVObjects[i]->Parent()]->InGroup;
                            flag=false;
                    }
            }
    }

    //Also - any groups selected should simply reparent all their offspring (might already be done)
     for (int i=0; i<SVObjects.count(); i++)
    {
            if (SVObjects[i]->widgetitem->isSelected() && SVObjects[i]->IsGroup)
            {
                for (int j=0; j<SVObjects.count(); j++)
                {
                    if (SVObjects[j]->InGroup == SVObjects[i]->Index) SVObjects[j]->InGroup=SVObjects[i]->InGroup;
                }
            }
    }

    //Now - are there any emtpty groups, and if so are they selected? If so we can delete them


    //remove any empty groups (for each group do a trawl for children. Mark any with none.
    for (int i=0; i<SVObjects.count(); i++)
    {
        if (SVObjects[i]->IsGroup)
        {
            int children=0; //now count children
            for (int j=0; j<SVObjects.count(); j++)
            {
                if (SVObjects[j]->InGroup == SVObjects[i]->Index) children++;
            }
            if (children==0 && SVObjects[i]->widgetitem->isSelected()) SVObjects[i]->killme=true;
        }
    }

    //Another delete run  - remove any empty groups found

        //first from SPV lists
    for (int i=0; i<SPVs.count(); i++)
    {
        QMutableListIterator<SVObject *> it2(SPVs[i]->ComponentObjects);
        while (it2.hasNext())
        {
            SVObject *o = it2.next();
            if (o->killme) {it2.remove();}
        };
    }

    //Now from overall item list, this time deleting object
    QMutableListIterator<SVObject *> it2(SVObjects);
    while (it2.hasNext())
    {
        SVObject *o = it2.next();
        if (o->killme) {delete o; it2.remove();}
    };


    FileDirty=true;
    RefreshObjects();

    //if (flag) QMessageBox::warning(this,"","No objects selected that can be removed from groups!");
    //else RefreshObjects();
}

void MainWindow::on_actionMove_to_group_triggered()
{
    bool flag=false;
    MoveToGroup mov(this);
    if (mov.valid)
    {
        flag=false;
        for (int i=0; i<SVObjects.count(); i++)
            if (SVObjects[i]->widgetitem->isSelected()) flag=true;
        if (flag)
        {
            int parent=-2;
            flag=true;
            for (int i=0; i<SVObjects.count(); i++)
            if (SVObjects[i]->widgetitem->isSelected())
            {
                if (parent==-2)
                    parent=SVObjects[i]->InGroup;
                else if (parent!=SVObjects[i]->InGroup) flag=false;
            }

            if (flag)
            {
                    MoveToGroup mov(this);
                    mov.exec();
                    int Group;
                    if (mov.Group==-2) return;
                    if (mov.Group==-1) Group=-1;
                    else Group=SVObjects[mov.Group]->Index;

                    for (int i=0; i<SVObjects.count(); i++)
                    if (SVObjects[i]->widgetitem->isSelected())
                        SVObjects[i]->InGroup=Group;

                    RefreshObjects();
                    FileDirty=true;
             }
            else
            {
                QMessageBox::warning(this,"", "Select objects/groups with same current parent to move");
            }

            //Catch - can't move to a child object. Also can't move self

        }
        else
        {
            QMessageBox::warning(this,"", "Select at least one object to move to a group");
        }
     }
    else
    {
        QMessageBox::warning(this,"", "No eligible groups exist");
    }
}

void MainWindow::UnsetAllAA()
{
    ui->actionAANone->setChecked(false);
    ui->action2x->setChecked(false);
    ui->action4x->setChecked(false);
    ui->action8x->setChecked(false);
    ui->action16x->setChecked(false);
}

void MainWindow::setSamples(int i)
{
    //qDebug()<<"InSS";
    //have to delete widget and recreate

    //first clear all VBOs or they try to redraw during widget creation
    for (int i=0; i<SVObjects.count(); i++) //have to clear VBOs first or there are problems
    {
        qDeleteAll(SVObjects[i]->VertexBuffers);
        qDeleteAll(SVObjects[i]->ColourBuffers);
        SVObjects[i]->VertexBuffers.clear();
        SVObjects[i]->ColourBuffers.clear();
        SVObjects[i]->BoundingBoxBuffer.destroy();
    }

    QGLFormat fmt;
    fmt.setVersion(GL_MAJOR,GL_MINOR);
    fmt.setSampleBuffers(true);
    fmt.setSamples(i);
    fmt.setStereo(ui->actionQuadBuffer_Stereo->isChecked());
    GlWidget *gl3widget2= new GlWidget(ui->frameVTK); //,fmt);

    gl3widget2->ClipStart=gl3widget->ClipStart;
    gl3widget2->ClipDepth=gl3widget->ClipDepth;
    gl3widget2->ClipAngle=gl3widget->ClipAngle;
    gl3widget2->DefaultClipAngle=gl3widget->DefaultClipAngle;
    gl3widget2->LastMouseXpos=gl3widget->LastMouseXpos;
    gl3widget2->LastMouseYpos=gl3widget->LastMouseYpos;
    gl3widget2->xdim=gl3widget->xdim;
    gl3widget2->ydim=gl3widget->ydim;
    gl3widget2->campos=gl3widget->campos;
    gl3widget2->StereoSeparation=gl3widget->StereoSeparation;

    gllayout->removeWidget(gl3widget);
    delete gl3widget;
    gllayout->addWidget(gl3widget2);
    gl3widget=gl3widget2;
}

void MainWindow::on_actionAANone_triggered()
{
    setSamples(1);
    for (int i=0; i<SVObjects.count(); i++)
        if (!SVObjects[i]->IsGroup)
            SVObjects[i]->Dirty=true;

    UnsetAllAA();
    ui->actionAANone->setChecked(true);
    on_actionResurface_Now_triggered();
}

void MainWindow::on_action2x_triggered()
{
    setSamples(2);
    for (int i=0; i<SVObjects.count(); i++)
        if (!SVObjects[i]->IsGroup)
            SVObjects[i]->Dirty=true;
    UnsetAllAA();
    ui->action2x->setChecked(true);
    on_actionResurface_Now_triggered();
}

void MainWindow::on_action4x_triggered()
{
    setSamples(4);
    for (int i=0; i<SVObjects.count(); i++)
        if (!SVObjects[i]->IsGroup)
            SVObjects[i]->Dirty=true;

    UnsetAllAA();
    ui->action4x->setChecked(true);
    on_actionResurface_Now_triggered();
}

void MainWindow::on_action8x_triggered()
{
    setSamples(8);
    for (int i=0; i<SVObjects.count(); i++)
        if (!SVObjects[i]->IsGroup)
            SVObjects[i]->Dirty=true;
    UnsetAllAA();
    ui->action8x->setChecked(true);
    on_actionResurface_Now_triggered();
}

void MainWindow::on_action16x_triggered()
{
    setSamples(16);
    for (int i=0; i<SVObjects.count(); i++)
        if (!SVObjects[i]->IsGroup)
            SVObjects[i]->Dirty=true;
    UnsetAllAA();
    ui->action16x->setChecked(true);
    on_actionResurface_Now_triggered();
}

void MainWindow::on_actionRescale_by_triggered()
{
    bool ok;
    FilterKeys=false;

    double d = QInputDialog::getDouble(this, "Rescale object",
                                       tr("Amount (1 = no rescale):"), 1, 0.00001, 1000, 4, &ok);
    FilterKeys=true;
    if (ok)
    {
        gl3widget->Resize(d);
        FileDirty=true;
        UpdateGL();
    }
    else
    return;
}

/*void MainWindow::on_actionSet_Scale_Ball_Size_triggered()
{
    //set scale ball to a real size

    bool ok;
    float oldsize=(ScaleBallScale/globalrescale)*mm_per_unit;
    double d = QInputDialog::getDouble(this, "Scale ball size",
                                       tr("New size for scale ball in mm:"), oldsize, 0.001, 10000, 3, &ok);
    if (ok)
    {
        widget->ResizeScaleBall((float)d);
        UpdateGL();
    }
    else
    return;

}
*/

void MainWindow::on_actionBackground_Colour_triggered()
{
    FilterKeys=false;

    QColor newcolour = QColorDialog::getColor(QColor(back_red, back_green, back_blue));
    FilterKeys=true;
    back_red=newcolour.red();
    back_green=newcolour.green();
    back_blue=newcolour.blue();
}

void MainWindow::on_actionManual_triggered()
{
    //qDebug()<<qApp->applicationDirPath() + "/SPIERSview_Manual.pdf";
    QDesktopServices::openUrl(QUrl("file:" + qApp->applicationDirPath() + "/SPIERSview_Manual.pdf",QUrl::TolerantMode));

}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (ui->actionInvert_Mouse_Wheel->isChecked())
        ui->ClipAngle->setValue(ui->ClipAngle->value()-event->delta()/12);
    else
        ui->ClipAngle->setValue(ui->ClipAngle->value()+event->delta()/12);
}

void MainWindow::on_actionScale_Grid_Colour_triggered()
{
    FilterKeys=false;

    QColor newcolour = QColorDialog::getColor(QColor(grid_red, grid_green, grid_blue));
    FilterKeys=true;
    grid_red=newcolour.red();
    grid_green=newcolour.green();
    grid_blue=newcolour.blue();
    FileDirty=true;
    UpdateGL();
}

void MainWindow::on_actionMinor_Grid_Colour_triggered()
{
    FilterKeys=false;
    QColor newcolour = QColorDialog::getColor(QColor(grid_minor_red, grid_minor_green, grid_minor_blue));
    FilterKeys=true;
    grid_minor_red=newcolour.red();
    grid_minor_green=newcolour.green();
    grid_minor_blue=newcolour.blue();
    FileDirty=true;
    UpdateGL();
}


void MainWindow::on_actionSet_Image_Folder_triggered()
{
    //Select output directory
    FilterKeys=false;

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    AnimOutputDir,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    FilterKeys=true;

    if (dir!="")
    {
        AnimOutputDir=dir;

        //do enables of a bunch of stuff
        ui->AnimSpinFileNum->setEnabled(true);
        ui->AnimFilenameStub->setEnabled(true);
        ui->AnimRescaleX->setEnabled(true);
        ui->actionSave_Image_and_Apply_Step->setEnabled(true);
        ui->actionApply_Multiple_Steps_Saving_Images->setEnabled(true);
        ui->menuSet_Image_Output_Format->setEnabled(true);
        ui->actionRescale_Output->setEnabled(true);
        ui->CheckSave->setEnabled(true);
    }
}

void MainWindow::ApplyAnimStep()
{
    gl3widget->YRotate(ui->SpinYInc->value());
    gl3widget->XRotate(ui->SpinXInc->value());
    gl3widget->ZRotate(ui->SpinZInc->value());
    gl3widget->Translate(ui->XTrans->value(),ui->YTrans->value(),ui->ZTrans->value());
    int before = ui->ClipAngle->value();
    ui->ClipAngle->setValue(ui->ClipAngle->value()-ui->AnimSpinZoom->value());
    if (before==ui->ClipAngle->value()) UpdateGL();  //Not needed otherwise - clipangle update should do it
}

void MainWindow::AnimSaveImage()
{
    QImage ScreenCapture = gl3widget->grabFramebuffer();//false);

    if (ui->actionRescale_Output->isChecked())
    {
        int wheight = gl3widget->height();
        int wwidth = gl3widget->width();

        int aheight = int((double)wheight / (((double) wwidth) / ((double) ui->AnimRescaleX->value())));
        ScreenCapture = ScreenCapture.scaled(QSize(ui->AnimRescaleX->value(),aheight),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }

    QString fileName;
    QTextStream s(&fileName);

    QString num;
    num.sprintf("%.5d",ui->AnimSpinFileNum->value());

    QString formatstring=".bmp";
    int qual=100;

    if (ui->actionBitmap_BMP->isChecked()) formatstring=".bmp";
    if (ui->actionJPEG_High_Quality_JPG->isChecked()) formatstring=".jpg";
    if (ui->actionJPEG_Medium_Quality_JPG->isChecked()) {formatstring=".jpg"; qual=80;}
    if (ui->actionJPEG_Low_Quality_JPG->isChecked()) {formatstring=".jpg"; qual=40;}
    if (ui->actionPortable_Network_Graphic_PNG->isChecked()) {formatstring=".png"; qual=0;}
    if (ui->actionTagged_Image_File_Format_TIFF->isChecked()) formatstring=".tiff";

    s<<AnimOutputDir <<"/" << ui->AnimFilenameStub->text() << num << formatstring;

    ScreenCapture.save(fileName,0,qual);

    ui->AnimSpinFileNum->setValue(ui->AnimSpinFileNum->value()+1);
}


void MainWindow::on_actionSave_Image_and_Apply_Step_triggered()
{
    AnimSaveImage();
    ApplyAnimStep();
}

void MainWindow::on_actionApply_Step_triggered()
{
    ApplyAnimStep();
}

void MainWindow::on_actionApply_Steps_triggered()
{
    bool ok;
    FilterKeys=false;

    int numsteps = QInputDialog::getInt(0,"Number of Steps","Enter number of animation steps required",1,1,100000,1,&ok);
    FilterKeys=true;
    if (!ok) return;
    UpdateGL();
    ui->OutputLabelOverall->setText("Applying Steps");
    ui->ProgBarOverall->setValue(0);
    ui->OutputLabelSpecific->setText("");
    setSpecificProgress(0);
    for (int i=0; i<numsteps; i++)
    {
        ui->ProgBarOverall->setValue((i*100)/numsteps);
        ApplyAnimStep();
        UpdateGL();
        QApplication::processEvents();
    }
    ui->OutputLabelOverall->setText("Animation Complete");
    ui->ProgBarOverall->setValue(100);
}

void MainWindow::on_actionApply_Multiple_Steps_Saving_Images_triggered()
{
    bool ok;
    FilterKeys=false;

    int numsteps = QInputDialog::getInt(0,"Number of Steps","Enter number of animation steps required",1,1,100000,1,&ok);
    FilterKeys=true;
    if (!ok) return;
    UpdateGL();
    ui->OutputLabelOverall->setText("Creating Files");
    ui->ProgBarOverall->setValue(0);
    ui->OutputLabelSpecific->setText("");
    setSpecificProgress(0);
    for (int i=0; i<numsteps; i++)
    {
        ui->ProgBarOverall->setValue((i*100)/numsteps);
        AnimSaveImage();
        ApplyAnimStep();
        UpdateGL();
        QApplication::processEvents();
    }
    ui->OutputLabelOverall->setText("Animation Complete");
    ui->ProgBarOverall->setValue(100);
}

void MainWindow::on_SingleStepButton_pressed()
{
    if (ui->CheckSave->isChecked()) AnimSaveImage();
    ApplyAnimStep();
}

void MainWindow::on_SingleStepSaveButton_pressed()
{
    AnimSaveImage();
    ApplyAnimStep();
}

void MainWindow::on_MultipleStepButton_pressed()
{
    if (ui->CheckSave->isChecked()) on_actionApply_Multiple_Steps_Saving_Images_triggered();
    else on_actionApply_Steps_triggered();
}

void MainWindow::on_actionReset_to_default_position_triggered()
{
    gl3widget->ResetToDefault();
    UpdateGL();
}

void MainWindow::on_actionSet_new_default_position_triggered()
{
    gl3widget->NewDefault();
    UpdateGL();
}

void MainWindow::on_actionBounding_Box_triggered()
{
    UpdateGL();
}


//Event filter receiver - to deal with glitchy accelerator keys
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (FilterKeys==false) return false;
    bool flag=false;
    //is focus in a spin box?
    if (qApp->focusWidget())
    {
        QString name=qApp->focusWidget()->metaObject()->className();
        //qDebug()<<name;
        if (name=="QSpinBox" || name=="QDoubleSpinBox") return false;
        if (name=="QTreeWidget") flag=true;

    }
    if (event->type()==QEvent::KeyPress)
    {

        QKeyEvent *kevent = (QKeyEvent*)event;

        //info box delete key
        if (kevent->key()==Qt::Key_Delete) {deleteinfo(); return true;}

        //Simple trigger filters
        if (kevent->key()==  Qt::Key_F5  && kevent->modifiers()== Qt::NoModifier && ui->actionResurface_Now->isEnabled())   {  ui->actionResurface_Now->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_V  && kevent->modifiers()== Qt::ControlModifier && ui->actionSave_Changes->isEnabled())   {  ui->actionSave_Changes->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_X  && kevent->modifiers()== Qt::ControlModifier && ui->actionExit->isEnabled())   {  ui->actionExit->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_N  && kevent->modifiers()== Qt::ControlModifier && ui->actionApply_Step->isEnabled())   {  ui->actionApply_Step->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_N  && kevent->modifiers()== Qt::ControlModifier+Qt::ShiftModifier && ui->actionSave_Image_and_Apply_Step->isEnabled())   {  ui->actionSave_Image_and_Apply_Step->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_O  && kevent->modifiers()== Qt::ControlModifier && ui->actionShow_All->isEnabled())   {  ui->actionShow_All->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_H  && kevent->modifiers()== Qt::ControlModifier && ui->actionHide_All->isEnabled())   {  ui->actionHide_All->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_I  && kevent->modifiers()== Qt::ControlModifier && ui->actionInvert_Show->isEnabled())   {  ui->actionInvert_Show->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_G  && kevent->modifiers()== Qt::ControlModifier && ui->actionGroup->isEnabled())   {  ui->actionGroup->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_E  && kevent->modifiers()== Qt::ControlModifier && ui->actionMove_to_group->isEnabled())   {  ui->actionMove_to_group->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F12  && kevent->modifiers()== Qt::NoModifier && ui->action_Move_Up->isEnabled())   {  ui->action_Move_Up->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F11  && kevent->modifiers()== Qt::NoModifier && ui->actionMove_Down->isEnabled())   {  ui->actionMove_Down->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_1  && kevent->modifiers()== Qt::ControlModifier && ui->actionOpaque_2->isEnabled())   {  ui->actionOpaque_2->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_2  && kevent->modifiers()== Qt::ControlModifier && ui->action75_Opaque_2->isEnabled())   {  ui->action75_Opaque_2->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_3  && kevent->modifiers()== Qt::ControlModifier && ui->action62_Opaque->isEnabled())   {  ui->action62_Opaque->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_4  && kevent->modifiers()== Qt::ControlModifier && ui->action50_Opaque->isEnabled())   {  ui->action50_Opaque->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_5  && kevent->modifiers()== Qt::ControlModifier && ui->action25_Opaque->isEnabled())   {  ui->action25_Opaque->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_PageUp  && kevent->modifiers()== Qt::NoModifier && ui->actionZoom_In->isEnabled())   {  ui->actionZoom_In->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_PageDown  && kevent->modifiers()== Qt::NoModifier && ui->actionZoom_Out->isEnabled())   {  ui->actionZoom_Out->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_Right  && kevent->modifiers()== Qt::NoModifier && ui->actionRotate_Clockwise->isEnabled())   {  ui->actionRotate_Clockwise->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_Left  && kevent->modifiers()== Qt::NoModifier && ui->actionRotate_Anticlockwise->isEnabled())   {  ui->actionRotate_Anticlockwise->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_1  && kevent->modifiers()== Qt::AltModifier && ui->actionVery_Low->isEnabled())   {  ui->actionVery_Low->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_2  && kevent->modifiers()== Qt::AltModifier && ui->actionLow->isEnabled())   {  ui->actionLow->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_3  && kevent->modifiers()== Qt::AltModifier && ui->actionMedium->isEnabled())   {  ui->actionMedium->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_4  && kevent->modifiers()== Qt::AltModifier && ui->actionHigh->isEnabled())   {  ui->actionHigh->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_5  && kevent->modifiers()== Qt::AltModifier && ui->actionVery_High->isEnabled())   {  ui->actionVery_High->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F5  && kevent->modifiers()== Qt::NoModifier && ui->actionResurface_Now->isEnabled())   {  ui->actionResurface_Now->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_Equal  && kevent->modifiers()== Qt::NoModifier && ui->actionIncrease_Size->isEnabled())   {  ui->actionIncrease_Size->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_Minus  && kevent->modifiers()== Qt::NoModifier && ui->actionDecrease_Size->isEnabled())   {  ui->actionDecrease_Size->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_BracketLeft  && kevent->modifiers()== Qt::NoModifier && ui->actionMove_towards_viewer->isEnabled())   {  ui->actionMove_towards_viewer->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_BracketRight  && kevent->modifiers()== Qt::NoModifier && ui->actionMove_away_from_viewer->isEnabled())   {  ui->actionMove_away_from_viewer->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_BraceLeft  && kevent->modifiers()== Qt::NoModifier && ui->actionLarge_Move_Closer->isEnabled())   {  ui->actionLarge_Move_Closer->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_BraceRight  && kevent->modifiers()== Qt::NoModifier && ui->actionLarge_Move_Away->isEnabled())   {  ui->actionLarge_Move_Away->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_Right  && kevent->modifiers()== Qt::ShiftModifier && ui->actionLarge_Rotate_Clockwise->isEnabled())   {  ui->actionLarge_Rotate_Clockwise->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_Left  && kevent->modifiers()== Qt::ShiftModifier && ui->actionLarge_Rotate_Anticlockwise->isEnabled())   {  ui->actionLarge_Rotate_Anticlockwise->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_D  && kevent->modifiers()== Qt::ControlModifier+Qt::AltModifier && ui->actionSet_new_default_position->isEnabled())   {  ui->actionSet_new_default_position->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_R  && kevent->modifiers()== Qt::ControlModifier+Qt::AltModifier && ui->actionReset_to_default_position->isEnabled())   {  ui->actionReset_to_default_position->activate(QAction::Trigger);     return true;}


        //More complex ones - same code works fine
        if (kevent->key()==  Qt::Key_A  && kevent->modifiers()== Qt::ControlModifier && ui->actionAnaglyph_Stereo->isEnabled())   {  ui->actionAnaglyph_Stereo->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_S  && kevent->modifiers()== Qt::ControlModifier && ui->actionSplit_Stereo->isEnabled())   {  ui->actionSplit_Stereo->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_M  && kevent->modifiers()== Qt::ControlModifier && ui->actionMute_Colours->isEnabled())   {  ui->actionMute_Colours->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_U  && kevent->modifiers()== Qt::ControlModifier && ui->actionAuto_Spin->isEnabled())   {  ui->actionAuto_Spin->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_B  && kevent->modifiers()== Qt::ControlModifier && ui->actionRotate_Lock->isEnabled())   {  ui->actionRotate_Lock->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F1  && kevent->modifiers()== Qt::NoModifier && ui->actionObject_Panel->isEnabled())   {  ui->actionObject_Panel->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F5  && kevent->modifiers()== Qt::ShiftModifier && ui->actionAuto_Resurface->isEnabled())   {  ui->actionAuto_Resurface->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F3  && kevent->modifiers()== Qt::NoModifier && ui->actionProgress_Bars->isEnabled())   {  ui->actionProgress_Bars->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F2  && kevent->modifiers()== Qt::NoModifier && ui->actionPieces_Panel->isEnabled())   {  ui->actionPieces_Panel->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F6  && kevent->modifiers()== Qt::NoModifier && ui->actionClip_Controls->isEnabled())   {  ui->actionClip_Controls->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F4  && kevent->modifiers()== Qt::NoModifier && ui->actionInfo->isEnabled())   {  ui->actionInfo->activate(QAction::Trigger);     return true;}
        if (kevent->key()==  Qt::Key_F7  && kevent->modifiers()== Qt::NoModifier && ui->actionAnimation_Panel->isEnabled())   {  ui->actionAnimation_Panel->activate(QAction::Trigger);     return true;}


       //Now the object keys
        if ( kevent->modifiers()== Qt::NoModifier)
        {
            qDebug()<<"Key"<<kevent->key();
            if (kevent->key()>=Qt::Key_0 && kevent->key()<=Qt::Key_9) ActionKey(QChar(kevent->text().at(0)));
            if (kevent->key()>=Qt::Key_A && kevent->key()<=Qt::Key_Z) ActionKey(QChar(kevent->text().toUpper().at(0)));
            return true; //DO pass it through
        }
    }

    return false;
}
 void MainWindow::setSpecificProgress(int p)
 {
     if (p<0) p=0;
     if (p>100) p=100;
     specificprogress=p;
 }


 void MainWindow::setSpecificLabel(QString t)
 {
     specificlabel=t;
 }

 void MainWindow::showSpecificProgress()
 {
     //qDebug()<<"Timer called";
     ui->ProgBarSpecific->setValue(specificprogress);
     ui->OutputLabelSpecific->setText(specificlabel);
    //if (specificprogress!=0) this->setWindowState(Qt::WindowMaximized);
     qApp->processEvents();
 }

