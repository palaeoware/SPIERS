/*********************************************

SPIERSedit 2: mainwindowimpl2.cpp

Overspill from mainwindowimpl - which was getting big
and slow to compile! This contains 'service' functions
mostly
- setup/manipulation of mask and segment dockers
**********************************************/

#include <math.h>
#include "keysafespinbox.h"
#include "distributedialogimpl.h"
#include "exportdxf.h"
#include "mainwindowimpl.h"
#include "copyingimpl.h"
#include "selectsegmentimpl.h"
#include "globals.h"
#include "display.h"
#include "fileio.h"
#include "slicespacingdialogimpl.h"
#include "resizedialogimpl.h"
#include "curves.h"
#include "findpolynomialimpl.h"
#include "undo.h"
#include "myrangescene.h"

#include <QTimer>
#include <QTime>
#include <QPaintEvent>
#include <QPicture>
#include <QPainter>
#include <QInputDialog>
#include <QColorDialog>
#include <QImageReader>
#include <QImageWriter>


QTreeWidgetItem *LastItemClicked;
QTime LastTimeClicked;
int LastColumnClicked = -1;

void MainWindowImpl::RefreshOneMaskItem(QTreeWidgetItem *item, int i) //i is index of item in my array
{
    //if passed an item
    QLabel *test = new QLabel();

    QPicture picture;
    QPainter painter;
    painter.begin(&picture);           // paint in picture
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(QColor(MasksSettings[i]->BackColour[0], MasksSettings[i]->BackColour[1], MasksSettings[i]->BackColour[2])));
    painter.drawRect(0, 0, 20, 20);    // draw a rect

    painter.end();                     // painting done
    test->setPicture(picture);
    test->setAutoFillBackground(true);

    //now fg
    QLabel *test2 = new QLabel();

    QPicture picture2;
    QPainter painter2;
    painter2.begin(&picture2);           // paint in picture
    painter2.setPen(QPen(Qt::NoPen));


    painter2.setBrush(QBrush(QColor(MasksSettings[i]->ForeColour[0], MasksSettings[i]->ForeColour[1], MasksSettings[i]->ForeColour[2])));
    painter2.drawRect(0, 0, 20, 20);   // draw a rect
    painter2.end();                     // painting done
    test2->setPicture(picture2);
    test2->setAutoFillBackground(true);

    item->setText(0, " ");
    if (SelectedMask == i && SelectedRMask != i) item->setText(0, "L");
    if (SelectedRMask == i && SelectedMask != i) item->setText(0, "R");
    if (SelectedRMask == i && SelectedMask == i) item->setText(0, "B");

    MasksTreeWidget->setItemWidget (item, 2, test);
    MasksTreeWidget->setItemWidget (item, 3, test2);

    item->setText(1, MasksSettings[i]->Name);

    //now my write, show, lock widgets
    QLabel *show = new QLabel();
    if (MasksSettings[i]->Show)
    {
        QPixmap p = QPixmap(":/darkstyle/icon_eye_open.png");
        show->setPixmap(p.scaled(18, 18, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap p = QPixmap(":/darkstyle/icon_eye_closed.png");
        show->setPixmap(p.scaled(18, 18, Qt::KeepAspectRatio));
    }
    MasksTreeWidget->setItemWidget (item, 4, show);

    QLabel *lock = new QLabel();
    if (MasksSettings[i]->Lock)
    {
        QPixmap p = QPixmap(":/darkstyle/icon_padlock_closed.png");
        lock->setPixmap(p.scaled(14, 14, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap p = QPixmap(":/darkstyle/icon_padlock_open.png");
        lock->setPixmap(p.scaled(14, 14, Qt::KeepAspectRatio));
    }
    MasksTreeWidget->setItemWidget (item, 5, lock);
}

void MainWindowImpl::RefreshMasks()
{
    bodgeflag = true;
    QTime t;
    t.start();

    QList <bool> selflags;

    //MasksTreeWidget->setSortingEnabled(false);
    MasksTreeWidget->setUpdatesEnabled(false);
    //first record selected
    for (int i = 0; i <= MaxUsedMask; i++)
    {
        bool sf;
        sf = false;
        if (MasksSettings[i]->widgetitem)
        {
            if ((MasksSettings[i]->widgetitem)->isSelected()) sf = true;
        }
        selflags.append(sf);
    }

    MasksTreeWidget->clear();
    QTreeWidgetItem *item;

    //first find lowest listorder...


    QList <bool> usedflags;
    for (int i = 0; i <= MaxUsedMask; i++) usedflags.append(false);
    for (int kloop = 0; kloop <= MaxUsedMask; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j <= MaxUsedMask; j++)
        {
            if (MasksSettings[j]->ListOrder < lowestval && usedflags[j] == false)
            {
                lowestval = MasksSettings[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;
        usedflags[i] = true;

        QStringList strings;
        strings.append("");
        strings.append("");
        strings.append("");
        strings.append("");
        strings.append("");
        strings.append("");
        strings.append("");
        QString m;
        strings.append(m);
        item = new QTreeWidgetItem(strings);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        MasksTreeWidget->addTopLevelItem(item);

        RefreshOneMaskItem(item, i);
        //create colours, BG first


        if (selflags[i]) item->setSelected(true);
        MasksSettings[i]->widgetitem = item; //store pointer
    }
    MasksTreeWidget->resizeColumnToContents(1); //resize text column
    MasksTreeWidget->setUpdatesEnabled(true);
    bodgeflag = false;
    on_MasksTreeWidget_itemSelectionChanged();

}

void MainWindowImpl::RefreshOneSegmentItem(QTreeWidgetItem *item, int i) //i is index of item in my array
{
    //Modified from mask equivalent. Redraws item in list
    if (i < 0) return;
    QLabel *test = new QLabel();

    QPicture picture;
    QPainter painter;
    painter.begin(&picture);           // paint in picture
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(QColor(Segments[i]->Colour[0], Segments[i]->Colour[1], Segments[i]->Colour[2])));

    painter.drawRect(0, 0, 28, 20);     // draw a rect
    painter.end();                     // painting done
    test->setPicture(picture);
    test->setAutoFillBackground(true);

    item->setText(0, " ");
    if (CurrentSegment == i && CurrentRSegment != i) item->setText(0, "L");
    if (CurrentRSegment == i && CurrentSegment != i) item->setText(0, "R");
    if (CurrentRSegment == i && CurrentSegment == i) item->setText(0, "B");

    SegmentsTreeWidget->setItemWidget (item, 2, test);

    item->setText(1, Segments[i]->Name);

    //now my write, show, lock widgets
    QLabel *write = new QLabel();
    if (Segments[i]->Activated)
    {
        QPixmap p = QPixmap(":/darkstyle/icon_eye_open.png");
        write->setPixmap(p.scaled(18, 18, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap p = QPixmap(":/darkstyle/icon_eye_closed.png");
        write->setPixmap(p.scaled(18, 18, Qt::KeepAspectRatio));
    }
    SegmentsTreeWidget->setItemWidget (item, 3, write);

    QLabel *lock = new QLabel();
    if (Segments[i]->Locked)
    {
        QPixmap p = QPixmap(":/darkstyle/icon_padlock_closed.png");
        lock->setPixmap(p.scaled(14, 14, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap p = QPixmap(":/darkstyle/icon_padlock_open.png");
        lock->setPixmap(p.scaled(14, 14, Qt::KeepAspectRatio));
    }
    SegmentsTreeWidget->setItemWidget (item, 4, lock);
}


void MainWindowImpl::RefreshSegments()
{
    QList <bool> selflags;
    bodgeflag = true;

//stops selection chaining event cascades
    //MasksTreeWidget->setSortingEnabled(false);
    SegmentsTreeWidget->setUpdatesEnabled(false);
    //first record selected
    for (int i = 0; i < SegmentCount; i++)
    {
        bool sf;
        sf = false;
        if (Segments[i]->widgetitem)
        {
            if ((Segments[i]->widgetitem)->isSelected()) sf = true;
        }
        selflags.append(sf);
    }

    SegmentsTreeWidget->clear();
    QTreeWidgetItem *item;

    //first find lowest listorder...


    QList <bool> usedflags;
    for (int i = 0; i < SegmentCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < SegmentCount; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < SegmentCount; j++)
        {
            if (Segments[j]->ListOrder < lowestval && usedflags[j] == false)
            {
                lowestval = Segments[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;
        usedflags[i] = true;

        QStringList strings;
        strings.append("");
        strings.append("");
        strings.append("");
        strings.append("");
        strings.append("");
        QString m;
        strings.append(m);


        item = new QTreeWidgetItem(strings);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        SegmentsTreeWidget->addTopLevelItem(item);

        RefreshOneSegmentItem(item, i);
        //create colours, BG first



        Segments[i]->widgetitem = item; //store pointer
        if (selflags[i])
        {
            item->setSelected(true);
        }

    }

    SegmentsTreeWidget->resizeColumnToContents(1);
    SegmentsTreeWidget->setUpdatesEnabled(true);

    bodgeflag = false;
    on_SegmentsTreeWidget_itemSelectionChanged();

    rangescene->Refresh();
    return;
}


bool MainWindowImpl::AmIMerged(int i)
{
    if (OutputObjects[i]->Merge) return true;
    if (OutputObjects[i]->Parent == -1) return false;
    else return AmIMerged(OutputObjects[i]->Parent);
}

void MainWindowImpl::RefreshOneOOItem(QTreeWidgetItem *item, int i) //i is index of item in my array
{
    //Modified from mask equivalent. Redraws item in list
    if (i < 0) return;
    QLabel *test = new QLabel();

    QPicture picture;
    QPainter painter;
    painter.begin(&picture);           // paint in picture
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(QColor(OutputObjects[i]->Colour[0], OutputObjects[i]->Colour[1], OutputObjects[i]->Colour[2])));

    painter.drawRect(0, 0, 28, 20);     // draw a rect
    painter.end();                     // painting done
    test->setPicture(picture);
    test->setAutoFillBackground(true);

    OOTreeWidget->setItemWidget (item, 1, test);

    QString KeySt(OutputObjects[i]->Key);
    if (OutputObjects[i]->Key == 0) KeySt = "[-]";

    QString ResampleSt;
    QTextStream rs(&ResampleSt);
    //if (OutputObjects[i]->Resample==100) ResampleSt="None"; else
    rs << OutputObjects[i]->Resample << "%";

    item->setText(0, OutputObjects[i]->Name);
    item->setText(2, KeySt);
    item->setText(3, ResampleSt);

    QLabel *show = new QLabel();
    if (OutputObjects[i]->Show)
    {
        QPixmap p = QPixmap(":/darkstyle/icon_eye_open");
        show->setPixmap(p.scaled(18, 18, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap p = QPixmap(":/darkstyle/icon_eye_closed.png");
        show->setPixmap(p.scaled(18, 18, Qt::KeepAspectRatio));
    }
    OOTreeWidget->setItemWidget (item, 4, show);

    show = new QLabel();
    if (AmIMerged(i)) show->setPixmap(QPixmap(":/icons/merged.bmp"));
    else show->setPixmap(QPixmap(":/icons/not_merged.bmp"));
    OOTreeWidget->setItemWidget (item, 5, show);

    QString text = OutputObjects[i]->Name + " components:";
    for (int j = 0; j < OutputObjects[i]->ComponentMasks.count(); j++)
        text += "\nMask: " + MasksSettings[OutputObjects[i]->ComponentMasks[j]]->Name;
    for (int j = 0; j < OutputObjects[i]->ComponentSegments.count(); j++)
    {
        text += "\nSegment: " + Segments[OutputObjects[i]->ComponentSegments[j]]->Name;
    }
    for (int j = 0; j < OutputObjects[i]->CurveComponents.count(); j++)
        text += "\nCurve: " + Curves[OutputObjects[i]->CurveComponents[j]]->Name;

    item->setToolTip(0, text);

    if (OutputObjects[i]->IsGroup) item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
}


void MainWindowImpl::OODrawChildren( QList <bool> selflags, int parent)
{
    QTreeWidgetItem *item;


    //first find lowest listorder...
    QList <bool> usedflags;
    for (int i = 0; i < OutputObjectsCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < OutputObjectsCount; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < OutputObjectsCount; j++)
        {
            if (OutputObjects[j]->ListOrder < lowestval && usedflags[j] == false && OutputObjects[j]->Parent == parent)
            {
                lowestval = OutputObjects[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;

        if (lowestindex >= 0)
        {
            //qDebug()<<"Adding for index"<<i<<" Name "<<OutputObjects[i]->Name;
            usedflags[i] = true;


            QStringList strings;
            strings.append("");
            strings.append("");
            strings.append("");
            strings.append("");
            strings.append("");
            strings.append("");
            item = new QTreeWidgetItem(strings);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            if (parent == -1) OOTreeWidget->addTopLevelItem(item);
            else OutputObjects[parent]->widgetitem->addChild(item);

            RefreshOneOOItem(item, i);
            if (selflags[i]) item->setSelected(true);
            OutputObjects[i]->widgetitem = item; //store pointer
            if (OutputObjects[i]->IsGroup) OODrawChildren(selflags, i);
        }
    }
}

void MainWindowImpl::RefreshOO()
{

    if (OutputObjectsCount == 0)
    {
        OOTreeWidget->clear();
        return;
    }

    //Look for objects whose parent is not a group. They are spurious - delete them
    bool flag;

    do
    {
        flag = false;
        for (int i = 0; i < OutputObjectsCount; i++)
        {
            if (OutputObjects[i]->Parent != -1)
            {
                if (OutputObjects[i]->Parent < 0 || OutputObjects[i]->Parent >= OutputObjectsCount ) flag = true;
                else
                {
                    if (OutputObjects[OutputObjects[i]->Parent]->IsGroup == false) flag = true;
                }
            }
            //flag is true - delete object
            if (flag)
            {
                qDebug() << "Output Object " << i << " (" << OutputObjects[i]->Name << ") is incorrectly parented, and will be deleted";

                //will be deleting this one, so reduce all parents indices that will be affected by 1
                for (int j = 0; j < OutputObjectsCount; j++)
                {

                    if (OutputObjects[j]->Parent == i) OutputObjects[j]->Parent = -1; //if anything has THIS as parent, put in root
                    if (OutputObjects[j]->Parent > i) OutputObjects[j]->Parent--; //reduce parent ID by 1
                }

                delete OutputObjects.takeAt(i); //will be deleted
                OutputObjectsCount--;
                break;
            }
        }
    }
    while (flag == true);   //Restart loop if we found one!


    bodgeflag = true;
    QList <bool> selflags;

    OOTreeWidget->setUpdatesEnabled(false);
    //first record selected
    for (int i = 0; i < OutputObjectsCount; i++)
    {

        bool sf;
        sf = false;
        if (OutputObjects[i]->widgetitem)
        {

            if ((OutputObjects[i]->widgetitem)->isSelected()) sf = true;
            if ((OutputObjects[i]->widgetitem)->isExpanded()) OutputObjects[i]->Expanded = true;
            else OutputObjects[i]->Expanded = false;
        }
        selflags.append(sf);
    }
    OOTreeWidget->clear();
    OODrawChildren(selflags, -1); //start by drawing root children

    for (int i = 0; i < OutputObjectsCount; i++)
    {
        if (OutputObjects[i]->widgetitem)
        {
            if (OutputObjects[i]->Expanded) OutputObjects[i]->widgetitem->setExpanded(true);
            else OutputObjects[i]->widgetitem->setExpanded(false);
        }
    }

    OOTreeWidget->resizeColumnToContents(0);
    OOTreeWidget->setUpdatesEnabled(true);
    bodgeflag = false;
    on_OOTreeWidget_itemSelectionChanged();

    /*
     for (int i=0; i<OutputObjectsCount; i++)
    {   qDebug()<<"OO"<<i<<" WI="<<OutputObjects[i]->widgetitem<<" Name"<<OutputObjects[i]->Name;
        //if (OutputObjects[i]->widgetitem==0)
        {
            qDebug()<<OutputObjects[i]->Resample;
            qDebug()<<OutputObjects[i]->ListOrder;
            qDebug()<<OutputObjects[i]->IsGroup;
            qDebug()<<OutputObjects[i]->Parent;
            qDebug()<<OutputObjects[i]->Key;
            qDebug()<<OutputObjects[i]->Show;
            qDebug()<<OutputObjects[i]->Merge;
            qDebug()<<OutputObjects[i]->Expanded;
            qDebug()<<OutputObjects[i]->ComponentMasks.count();
            qDebug()<<OutputObjects[i]->ComponentSegments.count();
            qDebug()<<OutputObjects[i]->CurveComponents.count();
        }
    }
    */
    return;
}


void MainWindowImpl::RefreshOneCurveItem(QTreeWidgetItem *item, int i) //i is index of item in my array
{
    //Modified from mask equivalent. Redraws item in list
    if (i < 0) return;
    QLabel *test = new QLabel();

    QPicture picture;
    QPainter painter;
    painter.begin(&picture);           // paint in picture
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(QColor(Curves[i]->Colour[0], Curves[i]->Colour[1], Curves[i]->Colour[2])));

    painter.drawRect(0, 0, 28, 20);     // draw a rect
    painter.end();                     // painting done
    test->setPicture(picture);
    test->setAutoFillBackground(true);

    CurvesTreeWidget->setItemWidget (item, 1, test);

    item->setText(0, Curves[i]->Name);

    //Grey out name if appropriate
    if (actionGrey_out_curves_not_no_current_slice->isChecked())
        if (Curves[i]->SplinePoints[CurrentFile]->Count == 0)
            item->setForeground(0, QBrush(QColor(Qt::gray)));
        else
            item->setForeground(0, QBrush(QColor(Qt::black)));

    //now my tristate widget
    QLabel *write = new QLabel();
    if (Curves[i]->Closed == false) write->setPixmap(QPixmap(":/icons/curve_open.bmp"));
    else if (Curves[i]->Closed && Curves[i]->Filled == false) write->setPixmap(QPixmap(":/icons/curve_closed.bmp"));
    else write->setPixmap(QPixmap(":/icons/curve_filled.bmp"));
    CurvesTreeWidget->setItemWidget (item, 2, write);

    if (Curves[i]->Segment != 0) item->setText(3, Segments[(Curves[i]->Segment - 1)]->Name);
    else item->setText(3, "[None]");

    //Now do first and last curve items
    item->setText(4, "Not Used");
    QString str;
    QTextStream s(&str);
    int temp = -1;
    for (int j = 0; j < FileCount; j++)
    {
        if (Curves[i]->SplinePoints[j]->Count > 0)
        {
            s << j + 1;
            temp = j;
            break;
        }
    }
    for (int j = FileCount - 1; j > 0; j--)
    {
        if (Curves[i]->SplinePoints[j]->Count > 0)
        {
            if (temp != j) //if only one slice, just one number will do
                s << "-" << j + 1;
            item->setText(4, str); //should only get here if already found one the other way!
            break;
        }
    }
}

void MainWindowImpl::RefreshCurves()
{
    bodgeflag = true;
    bodgeflag = true;

    QList <bool> selflags;


    //qDebug()<<"RC1";
    CurvesTreeWidget->setUpdatesEnabled(false);

    //qDebug()<<"RC2";
    //first record selected
    for (int i = 0; i < CurveCount; i++)
    {
        bool sf = false;
        if (Curves[i]->widgetitem)
        {
            if ((Curves[i]->widgetitem)->isSelected()) sf = true;
        }
        selflags.append(sf);
    }

    //qDebug()<<"RC3";
    CurvesTreeWidget->clear();
    QTreeWidgetItem *item;

    //first find lowest listorder...

    //qDebug()<<"RC4";
    QList <bool> usedflags;
    for (int i = 0; i < CurveCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < CurveCount; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < CurveCount; j++)
        {
            if (Curves[j]->ListOrder < lowestval && usedflags[j] == false)
            {
                lowestval = Curves[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;
        usedflags[i] = true;

        QStringList strings;
        for (int j = 0; j < 5; j++) strings.append("");
        item = new QTreeWidgetItem(strings);

        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        CurvesTreeWidget->addTopLevelItem(item);

        RefreshOneCurveItem(item, i);
        //create colours, BG first

        if (selflags[i]) item->setSelected(true);
        Curves[i]->widgetitem = item; //store pointer

    }
    //qDebug()<<"RC5";
    CurvesTreeWidget->resizeColumnToContents(0);
    //qDebug()<<"RC5a";
    CurvesTreeWidget->resizeColumnToContents(3);
    //qDebug()<<"RC5b";
    CurvesTreeWidget->setUpdatesEnabled(true);
    //qDebug()<<"RC5c";

    //qDebug()<<"RC5d";
    bodgeflag = false;
    //qDebug()<<"RC5e";
    on_CurvesTreeWidget_itemSelectionChanged();
    //qDebug()<<"RC6f";

    return;
}


void MainWindowImpl::RefreshSegmentsBoxes()
{

    clearing = true; //flag to control maskbox change event and stop it triggering
    SegBoxLeft->clear();
    SegBoxRight->clear();
    SegBoxRight->addItem("[Delete All]", -2);

    QList <bool> usedflags;
    for (int i = 0; i < SegmentCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < SegmentCount; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < SegmentCount; j++)
        {
            if (Segments[j]->ListOrder < lowestval && usedflags[j] == false)
            {
                lowestval = Segments[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;
        usedflags[i] = true;

        SegBoxLeft->addItem(Segments[i]->Name, QVariant(i));
        SegBoxRight->addItem(Segments[i]->Name, QVariant(i));
    }

    int found = SegBoxLeft->findData(CurrentSegment);
    if (found == -1) SegBoxLeft->setCurrentIndex(0);
    else SegBoxLeft->setCurrentIndex(found);

    found = SegBoxRight->findData(CurrentRSegment);
    if (found == -1) SegBoxRight->setCurrentIndex(0);
    else SegBoxRight->setCurrentIndex(found);

    clearing = false;

}

void MainWindowImpl::RefreshMasksBoxes()
{
    clearing = true; //flag to control maskbox change event and stop it triggering
    MaskBoxLeft->clear();
    MaskBoxRight->clear();
    QList <bool> usedflags;
    for (int i = 0; i <= MaxUsedMask; i++) usedflags.append(false);
    for (int kloop = 0; kloop <= MaxUsedMask; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j <= MaxUsedMask; j++)
        {
            if (MasksSettings[j]->ListOrder < lowestval && usedflags[j] == false)
            {
                lowestval = MasksSettings[j]->ListOrder;
                lowestindex = j;
            }
        }
        int i = lowestindex;
        usedflags[i] = true;

        MaskBoxLeft->addItem(MasksSettings[i]->Name, QVariant(i));
        MaskBoxRight->addItem(MasksSettings[i]->Name, QVariant(i));
    }


    int found = MaskBoxLeft->findData(SelectedMask);
    if (found == -1) MaskBoxLeft->setCurrentIndex(0);
    else MaskBoxLeft->setCurrentIndex(found);

    found = MaskBoxLeft->findData(SelectedRMask);
    if (found == -1) MaskBoxRight->setCurrentIndex(0);
    else MaskBoxRight->setCurrentIndex(found);
    clearing = false;
}

QString MainWindowImpl::TextForSliceSelectorBox(int i)
{
    //qDebug()<<"in TTFSSB";
    int chars = 1;
    if (Files.count() > 9) chars = 2;
    if (Files.count() > 99) chars = 3;
    if (Files.count() > 999) chars = 4;
    if (Files.count() > 9999) chars = 5;
    if (Files.count() > 99999) chars = 6;


    //qDebug()<<"in TTFSSB1"<<i;
    QString Fname = Files.at(i);

    //qDebug()<<"TTFSSB-"<<Fname;
    int lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    //qDebug()<<"in TTFSSB1a"<<i;

    QString sfname = Fname.mid(lastsep + 1);
    //qDebug()<<"in TTFSSB1b"<<i;

    QString s = QString ("%1").arg(i + 1, chars, 10, QChar('0'));
    //qDebug()<<"in TTFSSB1c"<<i;


    //qDebug()<<"in TTFSSB2"<<Stretches[i]<<SlicePerMM;
    QString pos = QString ("%1").arg((double)Stretches[i] * ((double)1 / SlicePerMM), 0, 'f', 3);


    if (ShowSlicePosition)  s += " - " + pos + "mm";
    else s += " (" + sfname + ")";


    //qDebug()<<"out TTFSSB";
    return s;
}

void MainWindowImpl::SetUpGUIFromSettings()
{
    //qDebug()<<QImageReader::supportedImageFormats ();
    //qDebug()<<QImageWriter::supportedImageFormats ();
    ClearCache(); //make sure cache is empty
    bool t = Active;
    Active = false; //turn off updates while I do this

    plainTextEdit->setPlainText(Notes);
    //set up stretches
    if (FullStretches.count() != FullFiles.count())
    {
        FullStretches.clear();
        for (int i = 0; i < FullFiles.count(); i++)
            FullStretches.append(i + 1);
    }

    //do similar for stretches
    if (zsparsity > 1)
    {
        Stretches.clear();
        for (int i = 0; i < FullStretches.count(); i += zsparsity)
        {
            Stretches.append(FullStretches[i]);
        }
    }
    else Stretches = FullStretches;

    SliderPos->setMaximum(FileCount);
    PosSpinBox->setMaximum(FileCount);
    SliderPos->setMinimum(1);
    PosSpinBox->setMinimum(1);
    SliderPos->setValue(CurrentFile + 1);
    PosSpinBox->setValue(CurrentFile + 1);

    //SliderPos->setValue(CurrentFile);
    //work out slider etc settings from currentzoom

    DontRedoZoom = true; //avoid cascades on updating controls
    double t2 = log10(CurrentZoom * 100); //t=(slider/500)+1
    int slider = (t2 - 1) * 500;
    CurrentZoom = (pow(10.0, ((double)(slider)) / 500 + 1)) / 100; //fix CurrentZoom to nearest correct value
    ZoomSpinBox->setValue((int)(CurrentZoom * 100));
    ZoomSlider->setValue(slider);
    DontRedoZoom = false;

    MinSlider->setValue(CMin);
    MaxSlider->setValue(CMax);

    if (timer2) timer2->start(UndoTimerSetting * 1000);

    //Set from mode
    //0=bright, 1=masks, 2=curves, 3=lock, 4=segment, 5=recalc
    if (CurrentMode == 0) actionBright->setChecked(true);
    if (CurrentMode == 1) actionMask->setChecked(true);
    if (CurrentMode == 2) actionCurve->setChecked(true);
    if (CurrentMode == 3) actionLock->setChecked(true);
    if (CurrentMode == 4) actionSegment->setChecked(true);
    if (CurrentMode == 5) actionRecalc->setChecked(true);


    BrushSize->setValue(Brush_Size);

    SpinUp->setValue(BrightUp);
    SpinDown->setValue(BrightDown);
    SpinSoft->setValue(BrightSoft);
    TransSlider->setValue(Trans);

    actionThreshold->setChecked(ThreshFlag);
    actionShowMasks->setChecked(MasksFlag);
    actionShowSegs->setChecked(SegsFlag);
    actionSquare->setChecked(SquareBrush);

    actionHidden_masks_locked_for_generation->setChecked(HiddenMasksLockedForGeneration);
    actionSegment_brush_applies_masks->setChecked(SegmentBrushAppliesMasks);

    actionSquare->setChecked(SquareBrush);
    actionCurve_markers_as_crosses->setChecked(CurveMarkersAsCrosses);
    actionLock_curve_shape->setChecked(CurveShapeLocked);
    action3D_Brush->setChecked(ThreeDmode);
    RefreshMasksBoxes();
    RefreshSegmentsBoxes();
    RefreshMasks();
//  MasksTreeWidget->resizeColumnToContents(0); //do once only - user on their own after that
//  MasksTreeWidget->resizeColumnToContents(1); //do once only - user on their own after that
    MasksTreeWidget->setColumnWidth(0, 14);
    MasksTreeWidget->setColumnWidth(2, 10);
    MasksTreeWidget->setColumnWidth(3, 10);
    MasksTreeWidget->setColumnWidth(4, 16);
    MasksTreeWidget->setColumnWidth(5, 16);
    //MasksTreeWidget->resizeColumnToContents(4);
    //MasksTreeWidget->resizeColumnToContents(5);
    //MasksTreeWidget->resizeColumnToContents(6);
    MasksTreeWidget->setUniformRowHeights(true);
    //MasksTreeWidget->setUniformItemSizes(true);

    RefreshSegments();
    SegmentsTreeWidget->setColumnWidth(0, 14);
    SegmentsTreeWidget->setColumnWidth(2, 28);
    SegmentsTreeWidget->setColumnWidth(3, 20);
    SegmentsTreeWidget->setColumnWidth(4, 20);
    SegmentsTreeWidget->setUniformRowHeights(true);

    RefreshCurves();

    CurvesTreeWidget->setColumnWidth(1, 28);
    CurvesTreeWidget->setColumnWidth(2, 20);
    CurvesTreeWidget->setUniformRowHeights(true);
    CurvesTreeWidget->setColumnWidth(4, 30);


    RefreshOO();
    OOTreeWidget->setColumnWidth(1, 28);
    OOTreeWidget->resizeColumnToContents(0);
    OOTreeWidget->setColumnWidth(2, 32);
    OOTreeWidget->setColumnWidth(3, 56);
    OOTreeWidget->setColumnWidth(4, 20);
    OOTreeWidget->setColumnWidth(5, 64);
    OOTreeWidget->setUniformRowHeights(true);

    //GUI elements for generation toolbox
    Slices_Per_MM->setValue(SlicePerMM);
    Pixels_Per_MM->setValue(PixPerMM);
    Edge_Down_MM->setValue(SkewDown);
    Edge_Left_MM->setValue(SkewLeft);
    FirstFile->setMaximum(FileCount);
    LastFile->setMaximum(FileCount);
    FirstFile->setValue(FirstOutputFile + 1);
    LastFile->setValue(LastOutputFile + 1);
    PixelSensitivity->setValue(PixSens);
    DownsampleZ->setValue(ZDownsample);
    DownsampleXY->setValue(XYDownsample);

    //Enable Everything
    CheckMirror->setChecked(OutputMirroring);
    actionSave_Settings->setEnabled(true);
    actionChange_downsampling->setEnabled(true);
    actionSave_As->setEnabled(true);
    actionOutput_and_Render->setEnabled(true);
    menuMode->setEnabled(true);
    menuSlice_Spacing->setEnabled(true);
    menuOutput->setEnabled(true);
    menuUndo->setEnabled(true);
    menuBrush->setEnabled(true);
    menuMasks->setEnabled(true);
    menuSegments->setEnabled(true);
    menuCurves->setEnabled(true);
    menuWindow->setEnabled(true);
    dockWidget_Main->setEnabled(true);
    dockWidget_Generate->setEnabled(true);
    SliceSelector->setEnabled(true);
    DockMasksSettings->setEnabled(true);
    DockCurvesSettings->setEnabled(true);
    DockOutputSettings->setEnabled(true);
    DockSegmentsSettings->setEnabled(true);

    //qDebug()<<"MG3";
    SliceSelector->setVisible(MenuSliceSelectorChecked);
    actionSlice_Selector->setChecked(MenuSliceSelectorChecked);
    DockMasksSettings->setVisible(MenuMasksChecked);
    actionMasks->setChecked(MenuMasksChecked);
    DockCurvesSettings->setVisible(MenuCurvesChecked);
    actionCurves->setChecked(MenuCurvesChecked);
    DockSegmentsSettings->setVisible(MenuSegsChecked);
    actionSegments->setChecked(MenuSegsChecked);
    dockWidget_Generate->setVisible(MenuGenChecked);
    actionGeneration->setChecked(MenuGenChecked);
    DockOutputSettings->setVisible(MenuOutputChecked);
    actionOutput->setChecked(MenuOutputChecked);
    dockWidget_Main->setVisible(MenuToolboxChecked);
    actionMain_Toolbox->setChecked(MenuToolboxChecked);
    DockHist->setVisible(MenuHistChecked);
    actionHistorgram->setChecked(MenuHistChecked);
    DockInfo->setVisible(MenuInfoChecked);
    actionHistorgram->setChecked(MenuInfoChecked);
    actionHistogram_shows_selected->setChecked(MenuHistSelectedOnly);

    //Do the generation stuff
    SetUpGenerationToolbox(CurrentSegment);

    SliderPos->setEnabled(true);
    PosSpinBox->setEnabled(true);
    toolBar->setEnabled(true);

    //qDebug()<<"MG4";

    SliceSelectorList->clear();
    //Now populate the slice list
    //qDebug()<<"MG5"<<Files.count();
    for (int i = 0; i < Files.count(); i++)
    {

        QString Fname = TextForSliceSelectorBox(i);

        QListWidgetItem *item = new QListWidgetItem;

        QFont f;
        item->setText(Fname);

        f.setBold(true);
        if ((i + 1) % 10 == 0) item->setFont(f); // font.setBold(true);
        if ((i + 1) % 50 == 0)
        {
            item->setTextColor(QColor(100, 149, 237));
        }
        if ((i + 1) % 100 == 0)
        {
            item->setTextColor(QColor("blue"));
        }

        SliceSelectorList->addItem(item);


    }
    //qDebug()<<"MG6";

    SliceSelectorList->item(CurrentFile)->setSelected(true);
    //qDebug()<<"MG7";

    //set the selected curve (if there is one)
    //qDebug()<<"MG5";
    if (SelectedCurve >= 0)
    {
        Curves[SelectedCurve]->widgetitem->setSelected(true);
    }

    //qDebug()<<"MG6";
    Active = t;
    //work out cwidth and cheight and fwidth, fheight
    QImage f(Files[0]);
    cwidth = f.width();
    cheight = f.height();

    fwidth = cwidth / ColMonoScale;
    fheight = cheight / ColMonoScale;

    if ((fwidth % 4) == 0) fwidth4 = fwidth; // no problem with width
    else fwidth4 = (fwidth / 4) * 4 + 4;
    //qDebug()<<"MG7";

    cwidth4 = cwidth;
    if (GreyImage)
    {
        if ((cwidth % 4) != 0) cwidth4 = (cwidth / 4) * 4 + 4;
    }

    actionMask_Advance_slice_after_copy_operation->setChecked(MasksMoveForward);
    actionMaskGo_back_one_slice_after_copy->setChecked(MasksMoveBack);

    InfoLabel->setText("");

    actionShow_position_slice_selector->setChecked(ShowSlicePosition);

    CheckBoxRangeSelectedOnly->setChecked(RangeSelectedOnly);
    CheckBoxRangeHardFill->setChecked(RangeHardFill);
//  BrushSizeY->setValue(BrushY);
//  BrushSizeZ->setValue(BrushZ);
//  Yaw->setValue(yaw);
//  Pitch->setValue(pitch);
//  Roll->setValue(roll);
    SetUpBrushEnabling();

    //qDebug()<<"Done mgui";
}

void MainWindowImpl::on_SegmentAdd_pressed()
{
    //New Segment
    //Creates a new segment and adds it to the list
    //Will need to (a) Create segment structure and add it to segments
    //(b) redraw boxes and docker
    // (c) create files with appropriate indices - all with 0's for now
    if (QMessageBox::question(this, "Create Segments", "This will create a new segments and associated files - are you sure?", QMessageBox::Ok | QMessageBox::Cancel)
            != QMessageBox::Ok) return;

    QString s;
    QTextStream(&s) << "Segment " << SegmentCount + 1;
    Segment *m = new Segment(s);
    Segments.append(m);
    QStringList strings;
    strings.append("");
    strings.append("");
    strings.append("");
    strings.append("");
    strings.append("");

    QTreeWidgetItem *item = new QTreeWidgetItem(strings);
    item->setFlags(Qt::ItemIsSelectable |  Qt::ItemIsEnabled);
    SegmentsTreeWidget->addTopLevelItem(item);

    CopyingImpl cop;
    cop.MakeNewSegFiles(SegmentCount);

    GA.append(0);

    mutex.lock();
    ClearCache();
    SegmentCount++;
    LoadAllData(CurrentFile);

    Segments[SegmentCount - 1]->widgetitem = item; //store pointer
    RefreshOneSegmentItem(item, SegmentCount - 1);
    RefreshSegments();
    RefreshSegmentsBoxes();
    ShowImage(graphicsView);
    mutex.unlock();
    //no need to do a showimage - am creating these files empty
    //DumpSettings();
    ResetFilesDirty();
}

void MainWindowImpl::on_SegmentDelete_pressed()
{
    WriteAllData(CurrentFile);
    QList <int> list;
    for (int i = 0;  i < SegmentCount; i++)
    {
        if ((Segments[i]->widgetitem) > 0)
            if ((Segments[i]->widgetitem)->isSelected())
            {
                list.append(i);
            }
    }
    if (list.count() == SegmentCount)
    {
        Message("Can't delete the last segment");
        return;
    }
    if (list.count() > 0)
    {
        QString mess = "";
        if (zsparsity > 1) mess = ", even where excluded from the current dataset by your Z sparsity setting";
        if (QMessageBox::question(this, "Delete Segments", "This will remove these segments and delete all their associated files" + mess + " - are you sure?", QMessageBox::Ok | QMessageBox::Cancel)
                == QMessageBox::Ok)
        {
            ClearCache();
            CopyingImpl dialog;
            dialog.DeleteSegments(list);

            CleanseOO();
            RefreshSegments();
            RefreshCurves();
            RefreshSegmentsBoxes();
            InvalidateAllGreyCaches();
            LoadAllData(CurrentFile);
            ShowImage(graphicsView);
            ResetUndo();
        }
    }
    ResetFilesDirty();
}

void MainWindowImpl::SetUpGenerationToolbox(int s)
//takes segment and puts all values in
{
    LinearRedSpinBox->setValue(Segments[s]->LinPercent[0]);
    LinearGreenSpinBox->setValue(Segments[s]->LinPercent[1]);
    LinearBlueSpinBox->setValue(Segments[s]->LinPercent[2]);
    LinearGlobalSpinBox->setValue(Segments[s]->LinGlobal);
    GenInvert->setChecked(Segments[s]->LinInvert);

    SpinBoxSparsity->setValue(Segments[s]->PolySparse);
    SpinBoxOrder->setValue((int)Segments[s]->PolyOrder);
    SpinBoxRetries->setValue(Segments[s]->PolyRetries);
    SpinBoxConverge->setValue(Segments[s]->PolyConverge);
    SpinBoxContrast->setValue(Segments[s]->PolyContrast);
}

void MainWindowImpl::on_GenInvert_toggled(bool checked)
{
    Segments[CurrentSegment]->LinInvert = checked;
    if (GenerateAuto->checkState()) on_GenerateButton_clicked();
}

void MainWindowImpl::on_GenerateAuto_toggled(bool checked)
{
    Q_UNUSED(checked);
    //don't actually need to do anything here!
}

void MainWindowImpl::on_GenerateButton_clicked()
{
    //save all my data
    CopyingImpl dialog;
    if (tabwidget->currentIndex() == 0) dialog.GenerateLinear(SliceSelectorList);
    if (tabwidget->currentIndex() == 1) dialog.GeneratePoly(SliceSelectorList);
    if (tabwidget->currentIndex() == 2) dialog.GenerateRange(SliceSelectorList);

    ShowImage(graphicsView);
}

void MainWindowImpl::on_LinearRedSpinBox_valueChanged(int value)
{
    Segments[CurrentSegment]->LinPercent[0] = value;
    if (GenerateAuto->checkState()) on_GenerateButton_clicked();
}

void MainWindowImpl::GenButton()
{
    //public interface to private slot
    on_GenerateButton_clicked();
}
void MainWindowImpl::on_LinearGreenSpinBox_valueChanged(int value)
{
    Segments[CurrentSegment]->LinPercent[1] = value;
    if (GenerateAuto->checkState()) on_GenerateButton_clicked();
}

void MainWindowImpl::on_LinearGreenSpinBox_editingFinished()
{
}

void MainWindowImpl::on_LinearBlueSpinBox_valueChanged(int value)
{
    Segments[CurrentSegment]->LinPercent[2] = value;
    if (GenerateAuto->checkState()) on_GenerateButton_clicked();
}

void MainWindowImpl::on_LinearGlobalSpinBox_valueChanged(int value )
{
    Segments[CurrentSegment]->LinGlobal = value;
    if (GenerateAuto->checkState()) on_GenerateButton_clicked();
}

void MainWindowImpl::on_SpinBoxSparsity_valueChanged(int value)
{
    Segments[CurrentSegment]->PolySparse = value;

}

void MainWindowImpl::on_SpinBoxOrder_valueChanged(int value )
{
    Segments[CurrentSegment]->PolyOrder = value; //NO idea why this is a double
}

void MainWindowImpl::on_SpinBoxRetries_valueChanged(int value)
{
    Segments[CurrentSegment]->PolyRetries = value;
}

void MainWindowImpl::on_SpinBoxContrast_valueChanged(int value)
{
    Segments[CurrentSegment]->PolyContrast = value;
    if (GenerateAuto->checkState()) on_GenerateButton_clicked();
}

void MainWindowImpl::on_SpinBoxConverge_valueChanged(int value)
{
    QString s;
    int v;
    v = 1;
    for (int i = 0; i < value; i++) v *= 2;

    QTextStream(&s) << " (" << 125 * v << ")";
    SpinBoxConverge->setSuffix(s);
    Segments[CurrentSegment]->PolyConverge = value;
}



void MainWindowImpl::on_CurvesTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (DoubleClickTimer.elapsed() < 100) return; //avoid double calls
    for (int i = 0; i < CurveCount; i++)
        if (item == Curves[i]->widgetitem)   //found it
        {
            if (column == 2)   //widget
            {
                if (Curves[i]->Closed == false)
                {
                    Curves[i]->Closed = true;
                    Curves[i]->Filled = false;
                }
                else if (Curves[i]->Filled == false)
                {
                    Curves[i]->Filled = true;
                }
                else
                {
                    Curves[i]->Closed = false;
                    Curves[i]->Filled = false;
                }
                if (Curves[i]->Segment != 0) for (int j = 0; j < Files.count(); j++) if (Curves[i]->SplinePoints[j * zsparsity]->Count > 0) FilesDirty[j] = true;
            }

            if (column == 0)
            {
                QString temp =
                    QInputDialog::getText (this, "", "", QLineEdit::Normal, Curves[i]->Name);

                if (temp != "") Curves[i]->Name = temp;
                RefreshOO();
            }

            if (column == 1)   //colour
            {
                QColor newcol;

                newcol = QColorDialog::getColor(QColor(Curves[i]->Colour[0], Curves[i]->Colour[1], Curves[i]->Colour[2]));
                if (newcol.isValid())
                {
                    Curves[i]->Colour[0] = newcol.red();
                    Curves[i]->Colour[1] = newcol.green();
                    Curves[i]->Colour[2] = newcol.blue();
                }
            }

            if (column == 3)   //segment
            {
                SelectSegmentImpl dialog(Curves[i]->Segment);
                dialog.exec();
                if (dialog.Cancelled == false)
                {
                    Curves[i]->Segment = dialog.RetValue;
                    for (int j = 0; j < Files.count(); j++) if (Curves[i]->SplinePoints[j * zsparsity]->Count > 0) FilesDirty[j] = true;

                }
            }

            RefreshOneCurveItem(Curves[i]->widgetitem, i);
            CurvesTreeWidget->resizeColumnToContents(0);
            ShowImage(graphicsView);
        }
    DoubleClickTimer.restart();
}


void MainWindowImpl::on_CurvesTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    return; //now longer used
}

//Slice selector underlining
void MainWindowImpl::on_SliceSelectorList_itemSelectionChanged()
{
    //do the underlining trick
    if (SliceSelectorList->count() == FileCount) //if not it's just been cleared - avoids crash on reload!
        for (int i = 0; i < FileCount; i++)
        {
            QListWidgetItem *t =    SliceSelectorList->item(i);
            QFont f = t->font();
            if (t->isSelected())    f.setUnderline(true);
            else  f.setUnderline(false);
            t->setFont(f);
        }
}

void MainWindowImpl::on_CurvesTreeWidget_itemSelectionChanged()
{
    //qDebug()<<"ISC1";
    if (bodgeflag) return;
    //qDebug()<<"ISC2";
    QList <QTreeWidgetItem *> selitems = CurvesTreeWidget->selectedItems ();
    //qDebug()<<"ISC3";
    if (selitems.count() != 1) SelectedCurve = -1;
    else
    {
        for (int i = 0; i < CurveCount; i++)
            if (selitems[0] == Curves[i]->widgetitem)   //found it
            {
                SelectedCurve = i;
            }
    }

    //qDebug()<<"ISC4";

    //do the underlining trick
    for (int i = 0; i < CurvesTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *t =    CurvesTreeWidget->topLevelItem(i);
        QFont f = t->font(0);
        if (t->isSelected())    f.setUnderline(true);
        else  f.setUnderline(false);
        t->setFont(0, f);
    }
    //qDebug()<<"ISC5";
    ShowImage(graphicsView);
    //qDebug()<<"ISC6";
}

void MainWindowImpl::on_actionAdd_new_point_triggered()
{
    AddNode();
    ShowImage(graphicsView);
    RefreshCurves();
}

void MainWindowImpl::on_actionRemove_node_under_cursor_triggered()
{
    KillNode(this);
    ShowImage(graphicsView);
    RefreshCurves();
}


void MainWindowImpl::on_Curve_Add_pressed()
{
    //New Curve
    //Creates a new curve and adds it to the list
    //Will need to (a) Create  structure and add it to list
    //(b) redraw docker

    QString s;
    QTextStream(&s) << "Curve " << CurveCount + 1;
    Curve *c = new Curve(s);
    //qDebug()<<"H1";
    Curves.append(c);
    QStringList strings;
    strings.append("");
    strings.append("");
    strings.append("");
    strings.append("");
    //qDebug()<<"H2";

    QTreeWidgetItem *item = new QTreeWidgetItem(strings);
    item->setFlags(Qt::ItemIsSelectable |  Qt::ItemIsEnabled);
    CurvesTreeWidget->addTopLevelItem(item);
    //qDebug()<<"H3";

    CurveCount++;

    //qDebug()<<"H4";
    Curves[CurveCount - 1]->widgetitem = item; //store pointer
    RefreshOneCurveItem(item, CurveCount - 1);
    //qDebug()<<"H5";
    RefreshCurves();
    //qDebug()<<"H6";
    ShowImage(graphicsView);
    //qDebug()<<"H7";
}

void MainWindowImpl::on_CurveDelete_pressed()
{
    QList <int> list;
    for (int i = 0;  i < CurveCount; i++)
    {
        if ((Curves[i]->widgetitem) > 0)
            if ((Curves[i]->widgetitem)->isSelected())
            {
                list.append(i);
            }
    }
//        qDebug()<<"ist.count"<<list.count()<<"CurveCouunt"<<CurveCount;
    if (list.count() == CurveCount) SelectedCurve = -1;
    if (list.count() > 0)
    {
        QString mstring;
        if (list.count() == 1) mstring = "this curve";
        else mstring = "these curves";
        if (QMessageBox::question(this, "Delete Curves", "This will remove " + mstring + " and delete all associated points - are you sure?", QMessageBox::Ok | QMessageBox::Cancel)
                == QMessageBox::Ok)
        {
            //now do actual removal. Complex as need to keep indices working properly

            //First - work out a  conversion table.
            QList <int> ToCurves;
            int ToCurvePos = 0;

            //get conversion table
            for (int i = 0; i < CurveCount; i++)
            {
                if (list.indexOf(i) < 0) //this is NOT one of the deleting ones
                    ToCurves.append(ToCurvePos++);
                else
                    ToCurves.append(-1);//means delete
            }

            //OK, should now have correct conversion list

            //fix selectedcurve
            if (SelectedCurve >= 0) if (ToCurves[SelectedCurve] == -1) SelectedCurve = 0;

            //Now hack apart the actual list - move pointers as above

            //delete all the dead ones
            for (int i = 0; i < list.count(); i++) delete Curves[list[i]];

            //shuffle list back
            for (int i = 0; i < CurveCount; i++)
            {
                if (list.indexOf(i) < 0) //this is NOT one of the deleting ones
                    Curves[ToCurves[i]] = Curves[i];
            }

            CurveCount -= list.count();
            for (int i = 0; i < list.count(); i++)
            {
                Curves.removeLast();
            }

            //handle Undo's
            for (int i = 0; i < UndoEvents.count(); i++)
                for (int j = 0; j < UndoEvents[i]->DataObjects.count(); j++)
                {
                    //for every data object
                    if  (UndoEvents[i]->DataObjects[j]->Type == -1)
                    {
                        UndoEvents[i]->DataObjects[j]->CurveNumber = ToCurves[UndoEvents[i]->DataObjects[j]->CurveNumber];
                    }
                }

            for (int i = 0; i < RedoEvents.count(); i++)
                for (int j = 0; j < RedoEvents[i]->DataObjects.count(); j++)
                {
                    //for every data object
                    if  (RedoEvents[i]->DataObjects[j]->Type == -1)
                    {
                        RedoEvents[i]->DataObjects[j]->CurveNumber = ToCurves[RedoEvents[i]->DataObjects[j]->CurveNumber];
                    }
                }
            ShowImage(graphicsView);
            RefreshCurves();

            //sort out output objects
            for (int i = 0; i < OutputObjectsCount; i++)
                for (int j = 0; j < OutputObjects[i]->CurveComponents.count(); j++)
                    OutputObjects[i]->CurveComponents[j] = ToCurves[OutputObjects[i]->CurveComponents[j]];

            CleanseOO();
        }
    }
}

void MainWindowImpl::on_CurveMoveUp_pressed()
{
    //Move selected curve up
    CurvesTreeWidget->setUpdatesEnabled(false);
    if ((CurvesTreeWidget->selectedItems()).count() != 1)
        Message("Select a SINGLE curve");
    else
    {
        for (int i = 0;  i < CurveCount; i++)
        {
            if ((Curves[i]->widgetitem) > 0)
                if ((Curves[i]->widgetitem)->isSelected())
                {
                    //found selected
                    //swap with one above in list
                    QTreeWidgetItem *search = CurvesTreeWidget->itemAbove(Curves[i]->widgetitem);
                    if (search)
                        for (int j = 0;  j < CurveCount; j++)
                            if ((Curves[j]->widgetitem) == search)
                            {
                                //found it
                                int temp = Curves[i]->ListOrder;
                                QTreeWidgetItem *temp2 = Curves[i]->widgetitem;
                                Curves[i]->ListOrder = Curves[j]->ListOrder;
                                Curves[i]->widgetitem = Curves[j]->widgetitem;

                                Curves[j]->ListOrder = temp;
                                Curves[j]->widgetitem = temp2;

                                CurvesTreeWidget->setFocus(Qt::OtherFocusReason);
                                Curves[i]->widgetitem->setSelected(true);
                                Curves[j]->widgetitem->setSelected(false);
                                CurvesTreeWidget->scrollToItem(Curves[j]->widgetitem);
                                RefreshOneCurveItem(Curves[i]->widgetitem, i);
                                RefreshOneCurveItem(Curves[j]->widgetitem, j);
                                CurvesTreeWidget->setUpdatesEnabled(true);
                                return;

                            }
                }
        }
    }
    CurvesTreeWidget->setUpdatesEnabled(true);
    RefreshCurves(); //added later to get graying to work - possibly some of the above now not needed?
}

void MainWindowImpl::on_CurveMoveDown_pressed()
{
    CurvesTreeWidget->setUpdatesEnabled(false);

    if ((CurvesTreeWidget->selectedItems()).count() != 1)
        Message("Select a SINGLE curve");
    else
    {
        for (int i = 0;  i < CurveCount; i++)
        {
            if ((Curves[i]->widgetitem) > 0)
                if ((Curves[i]->widgetitem)->isSelected())
                {
                    //found selected
                    //swap with one above in list
                    QTreeWidgetItem *search = CurvesTreeWidget->itemBelow(Curves[i]->widgetitem);
                    if (search)
                        for (int j = 0;  j < CurveCount; j++)
                            if ((Curves[j]->widgetitem) == search)
                            {
                                //found it
                                int temp = Curves[i]->ListOrder;
                                QTreeWidgetItem *temp2 = Curves[i]->widgetitem;
                                Curves[i]->ListOrder = Curves[j]->ListOrder;
                                Curves[i]->widgetitem = Curves[j]->widgetitem;

                                Curves[j]->ListOrder = temp;
                                Curves[j]->widgetitem = temp2;

                                //now do it in the widget
                                CurvesTreeWidget->setFocus(Qt::OtherFocusReason);
                                Curves[i]->widgetitem->setSelected(true);
                                Curves[j]->widgetitem->setSelected(false);
                                CurvesTreeWidget->scrollToItem(Curves[j]->widgetitem);
                                RefreshOneCurveItem(Curves[i]->widgetitem, i);
                                RefreshOneCurveItem(Curves[j]->widgetitem, j);
                                CurvesTreeWidget->setUpdatesEnabled(true);
                                return;
                            }
                }
        }
    }
    CurvesTreeWidget->setUpdatesEnabled(true);
}


void MainWindowImpl::CurveCopy(int fromfile)
{
    QList <QTreeWidgetItem *> selitems = CurvesTreeWidget->selectedItems();
    if (selitems.count() == 0) Message("No curves selected to copy");
    else
    {
        MakeUndo("copy curves");
        for (int i = 0; i < selitems.count(); i++)
        {
            for (int j = 0;  j < CurveCount; j++)
            {
                if ((Curves[j]->widgetitem) == selitems[i])   //found a selected curve
                {
                    for (int k = 0; k < Files.count(); k++)
                        if (SliceSelectorList->item(k)->isSelected() && k != fromfile)
                        {
                            //k is slice, c is curve
                            Curves[j]->SplinePoints[k * zsparsity]->X.clear();
                            Curves[j]->SplinePoints[k * zsparsity]->Y.clear();
                            for (int l = 0; l < Curves[j]->SplinePoints[fromfile * zsparsity]->Count; l++)
                            {
                                Curves[j]->SplinePoints[k * zsparsity]->X.append(Curves[j]->SplinePoints[fromfile * zsparsity]->X[l]);
                                Curves[j]->SplinePoints[k * zsparsity]->Y.append(Curves[j]->SplinePoints[fromfile * zsparsity]->Y[l]);
                            }
                            Curves[j]->SplinePoints[k * zsparsity]->Count = Curves[j]->SplinePoints[k * zsparsity]->X.count();
                            if (Curves[j]->Segment != 0) FilesDirty[k] = true;

                        }
                }
            }
        }
        ShowImage(graphicsView);
    }
    CurvesDirty = true;

    RefreshCurves();
}

void MainWindowImpl::on_actionCopyCurvesFromPrevious_triggered()
{
    if (CurrentFile == 0) Message("This is the first slice image in the dataset");
    else CurveCopy(CurrentFile - 1);
    //ResetUndo();
    RefreshCurves();
}

void MainWindowImpl::on_actionCopyCurvesFromNext_triggered()
{
    if (CurrentFile == Files.count() - 1) Message("This is the last slice image in the dataset");
    else CurveCopy(CurrentFile + 1);
    //ResetUndo();
    RefreshCurves();
}

void MainWindowImpl::on_actionCopyCurvesToCurrent_triggered()
{
    QList <QTreeWidgetItem *> selitems = CurvesTreeWidget->selectedItems();
    QList <QListWidgetItem *> selslices = SliceSelectorList->selectedItems();
    if (selslices.count() != 1) Message("Select a single slice to copy curves from");
    else
    {
        //MakeUndo("copy curves");
        for (int i = 0; i < selitems.count(); i++)
        {
            for (int j = 0;  j < CurveCount; j++)
            {
                if ((Curves[j]->widgetitem) == selitems[i])   //found a selected curve
                {
                    for (int k = 0; k < Files.count(); k++)
                        if (SliceSelectorList->item(k)->isSelected() && k != CurrentFile)
                        {
                            //k is slice, c is curve
                            Curves[j]->SplinePoints[CurrentFile * zsparsity]->X.clear();
                            Curves[j]->SplinePoints[CurrentFile * zsparsity]->Y.clear();
                            for (int l = 0; l < Curves[j]->SplinePoints[k * zsparsity]->Count; l++)
                            {
                                Curves[j]->SplinePoints[CurrentFile * zsparsity]->X.append(Curves[j]->SplinePoints[k * zsparsity]->X[l]);
                                Curves[j]->SplinePoints[CurrentFile * zsparsity]->Y.append(Curves[j]->SplinePoints[k * zsparsity]->Y[l]);
                            }
                            Curves[j]->SplinePoints[CurrentFile * zsparsity]->Count = Curves[j]->SplinePoints[CurrentFile * zsparsity]->X.count();
                        }
                }
            }
        }
        ShowImage(graphicsView);
        CurvesDirty = true;
        CurvesUndoDirty = true;
        FilesDirty[CurrentFile] = true;
        RefreshCurves();
    }
}


void MainWindowImpl::on_actionCopy_from_current_slice_to_selected_triggered()
{
    CurveCopy(CurrentFile);
    //ResetUndo();
    RefreshCurves();
}

void MainWindowImpl::on_actionRemove_selected_curves_from_selected_slices_triggered()
{
    QList <QTreeWidgetItem *> selitems = CurvesTreeWidget->selectedItems();
    QList <QListWidgetItem *> selslices = SliceSelectorList->selectedItems();
    MakeUndo("delete curves");
    for (int i = 0; i < selitems.count(); i++)
    {
        for (int j = 0;  j < CurveCount; j++)
        {
            if ((Curves[j]->widgetitem) == selitems[i])   //found a selected curve
            {
                for (int k = 0; k < Files.count(); k++)
                    if (SliceSelectorList->item(k)->isSelected())   //and a selected slice
                    {
                        //k is slice, j is curve
                        Curves[j]->SplinePoints[k * zsparsity]->X.clear();
                        Curves[j]->SplinePoints[k * zsparsity]->Y.clear();
                        Curves[j]->SplinePoints[k * zsparsity]->Count = 0;
                        if (Curves[j]->Segment != 0) FilesDirty[k] = true;
                    }
            }
        }
    }
    //ResetUndo();
    ShowImage(graphicsView);
    RefreshCurves();
}

void MainWindowImpl::on_actionResize_keeping_curve_shape_triggered()
{
    //this works on all selected slices
    QList <QTreeWidgetItem *> selitems = CurvesTreeWidget->selectedItems();
    if (selitems.count() == 0) Message ("No curves selected");
    else
    {
        ResizeDialogImpl dialog;
        dialog.exec();
        if (dialog.Cancelled == false)
        {
            double percentsize = dialog.doubleSpinBox->value();
            MakeUndo("resize curves");
            for (int i = 0; i < selitems.count(); i++)
            {
                for (int j = 0;  j < CurveCount; j++)
                {
                    if ((Curves[j]->widgetitem) == selitems[i])   //found a selected curve
                    {
                        for (int k = 0; k < Files.count(); k++)
                            if (SliceSelectorList->item(k)->isSelected())   //and a selected slice
                            {
                                //k is slice, j is curve
                                //find centroid
                                double xpos = 0;
                                double ypos = 0;
                                for (int n = 0; n < Curves[j]->SplinePoints[k * zsparsity]->Count; n++)
                                {
                                    xpos += Curves[j]->SplinePoints[k * zsparsity]->X[n];
                                    ypos += Curves[j]->SplinePoints[k * zsparsity]->Y[n];
                                }
                                xpos /= Curves[j]->SplinePoints[k * zsparsity]->Count;
                                ypos /= Curves[j]->SplinePoints[k * zsparsity]->Count;

                                //now redo all positions in terms of percent to here!
                                for (int n = 0; n < Curves[j]->SplinePoints[k * zsparsity]->Count; n++)
                                {
                                    Curves[j]->SplinePoints[k * zsparsity]->X[n] = xpos + (Curves[j]->SplinePoints[k * zsparsity]->X[n] - xpos) * percentsize / 100;
                                    Curves[j]->SplinePoints[k * zsparsity]->Y[n] = ypos + (Curves[j]->SplinePoints[k * zsparsity]->Y[n] - ypos) * percentsize / 100;
                                }
                                if (Curves[j]->Segment != 0) FilesDirty[k] = true;
                            }
                    }
                }
            }
            ShowImage(graphicsView);
            //ResetUndo();
        }
    }
}

void MainWindowImpl::ResetUndo()
{
    //clear the undo buffer - done something that invalidates it!

    if (UndoEvents.count()) qDeleteAll(UndoEvents.begin(), UndoEvents.end());
    UndoEvents.clear();
    if (RedoEvents.count()) qDeleteAll(RedoEvents.begin(), RedoEvents.end());
    RedoEvents.clear();

    //make a 'start' event
    MasksUndoDirty = true;
    LocksUndoDirty = true;
    CurvesUndoDirty = true;
    for (int i = 0; i < SegmentCount; i++) Segments[i]->UndoDirty = true;
    UndoEvents.prepend(new UndoEvent(-1, -1));

    actionRedo->setEnabled(false);
    actionUndo->setEnabled(false);

    return;
}


void MainWindowImpl::on_OOTreeWidget_itemPressed(QTreeWidgetItem *item, int column)
{
    QTime now = QTime::currentTime();
    if (LastColumnClicked == column && LastItemClicked == item)
    {
        if (LastTimeClicked.msecsTo(now) < 500)   //double click
        {
            on_OOTreeWidget_itemDoubleClicked(item, column);
            return;
        }
    }

    LastItemClicked = item;
    LastTimeClicked = now;
    LastColumnClicked = column;

}

void MainWindowImpl::on_CurvesTreeWidget_itemPressed(QTreeWidgetItem *item, int column)
{
    QTime now = QTime::currentTime();
    if (LastColumnClicked == column && LastItemClicked == item)
    {
        if (LastTimeClicked.msecsTo(now) < 500)   //double click
        {
            on_CurvesTreeWidget_itemDoubleClicked(item, column);
            return;
        }
    }

    LastItemClicked = item;
    LastTimeClicked = now;
    LastColumnClicked = column;

}

void MainWindowImpl::on_MasksTreeWidget_itemPressed(QTreeWidgetItem *item, int column)
{
    //Double click bodge - not always being picked up for some reason, so manually look for close clicks
    QTime now = QTime::currentTime();
    if (LastColumnClicked == column && LastItemClicked == item)
    {
        if (LastTimeClicked.msecsTo(now) < 500)   //double click
        {
            on_MasksTreeWidget_itemDoubleClicked(item, column);
            return;
        }
    }

    LastItemClicked = item;
    LastTimeClicked = now;
    LastColumnClicked = column;

    if (column > 0) return;
    {
        if ((QApplication::mouseButtons().testFlag(Qt::RightButton)))
        {
            for (int i = 0;  i <= MaxUsedMask; i++)
            {
                if ((MasksSettings[i]->widgetitem) == item)
                {

                    int o = SelectedRMask;
                    SelectedRMask = i;
                    RefreshOneMaskItem(MasksSettings[o]->widgetitem, o);
                    RefreshOneMaskItem(item, i);
                    RefreshMasksBoxes();
                }
            }
        }
        if ((QApplication::mouseButtons().testFlag(Qt::LeftButton)))
        {
            for (int i = 0;  i <= MaxUsedMask; i++)
            {
                if ((MasksSettings[i]->widgetitem) == item)
                {
                    int o = SelectedMask;
                    SelectedMask = i;
                    RefreshOneMaskItem(item, i);
                    RefreshOneMaskItem(MasksSettings[o]->widgetitem, o);
                    RefreshMasksBoxes();
                }
            }
        }
    }
}

void MainWindowImpl::on_SegmentsTreeWidget_itemPressed(QTreeWidgetItem *item, int column)
{

    //Double click bodge - not always being picked up for some reason, so manually look for close clicks
    QTime now = QTime::currentTime();
    if (LastColumnClicked == column && LastItemClicked == item)
    {
        if (LastTimeClicked.msecsTo(now) < 500)   //double click
        {
            on_SegmentsTreeWidget_itemDoubleClicked(item, column);
            return;
        }
    }

    LastItemClicked = item;
    LastTimeClicked = now;
    LastColumnClicked = column;

    if (column != 0) return;
    {
        if ((QApplication::mouseButtons().testFlag(Qt::RightButton)))
        {
            for (int i = 0;  i < SegmentCount; i++)
            {
                if ((Segments[i]->widgetitem) == item)
                {
                    int o = CurrentRSegment;
                    CurrentRSegment = i;
                    if (o >= 0) RefreshOneSegmentItem(Segments[o]->widgetitem, o); //not if delete all!
                    RefreshOneSegmentItem(item, i);
                    RefreshSegmentsBoxes();
                }
            }
        }
        if ((QApplication::mouseButtons().testFlag(Qt::LeftButton)))
        {
            for (int i = 0;  i < SegmentCount; i++)
            {
                if ((Segments[i]->widgetitem) == item)
                {
                    int o = CurrentSegment;
                    CurrentSegment = i;
                    RefreshOneSegmentItem(item, i);
                    RefreshOneSegmentItem(Segments[o]->widgetitem, o);
                    RefreshSegmentsBoxes();
                    SetUpGenerationToolbox(CurrentSegment);
                    ShowImage(graphicsView);
                }
            }
        }
    }
}

void MainWindowImpl::on_SegmentsTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (DoubleClickTimer.elapsed() < 100) return; //avoid double calls
    //invert a yes/no etc - code largely from masks version
    for (int i = 0; i < SegmentCount; i++)
        if (item == Segments[i]->widgetitem)   //found it
        {
            if (column == 4)  Segments[i]->Locked = !(Segments[i]->Locked);
            if (column == 3)
            {
                Segments[i]->Activated = !(Segments[i]->Activated);
                ResetFilesDirty();
                rangescene->Refresh();
            }

            if (column == 1)
            {
                QString temp =
                    QInputDialog::getText (this, "", "", QLineEdit::Normal, Segments[i]->Name);

                if (temp != "") Segments[i]->Name = temp;
                RefreshOO();
                RefreshCurves();
            }

            if (column == 2)   //colour
            {
                QColor newcol = QColorDialog::getColor(QColor(Segments[i]->Colour[0], Segments[i]->Colour[1], Segments[i]->Colour[2]));
                if (newcol.isValid())
                {
                    Segments[i]->Colour[0] = newcol.red();
                    Segments[i]->Colour[1] = newcol.green();
                    Segments[i]->Colour[2] = newcol.blue();
                }
            }

            RefreshOneSegmentItem(Segments[i]->widgetitem, i);
            SegmentsTreeWidget->resizeColumnToContents(1);

            ShowImage(graphicsView);
        }
    DoubleClickTimer.restart();
}



void MainWindowImpl::on_OOTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (DoubleClickTimer.elapsed() < 100) return; //avoid double calls
    for (int i = 0; i < OutputObjectsCount; i++)
        if (item == OutputObjects[i]->widgetitem)   //found it
        {
            if (column == 0)   //Name
            {
                QString temp =
                    QInputDialog::getText (this, "", "", QLineEdit::Normal, OutputObjects[i]->Name);

                if (temp != "") OutputObjects[i]->Name = temp;
                RefreshOneOOItem(OutputObjects[i]->widgetitem, i);
                OOTreeWidget->resizeColumnToContents(0);
            }

            if (column == 1)   //Colour
            {
                QColor newcol;
                newcol = QColorDialog::getColor(QColor(OutputObjects[i]->Colour[0], OutputObjects[i]->Colour[1], OutputObjects[i]->Colour[2]));
                if (newcol.isValid())
                {
                    OutputObjects[i]->Colour[0] = newcol.red();
                    OutputObjects[i]->Colour[1] = newcol.green();
                    OutputObjects[i]->Colour[2] = newcol.blue();
                }
                RefreshOneOOItem(OutputObjects[i]->widgetitem, i);
            }

            if (column == 3)   //resample
            {
                bool ok;
                int newresample = QInputDialog::getInt(this, tr("Resample value"),
                                                       "", OutputObjects[i]->Resample, 0, 100, 1, &ok);
                if (ok) OutputObjects[i]->Resample = newresample;
                RefreshOneOOItem(OutputObjects[i]->widgetitem, i);
            }

            if (column == 2)   //Key
            {
                QStringList items;
                items << "[None]" << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" << "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";
                items << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9";

                bool ok;

                int asc = OutputObjects[i]->Key;
                if (asc >= 65 && asc <= 90) asc -= 64;
                else if (asc >= 48 && asc <= 57) asc -= 21;

                QString item = QInputDialog::getItem(this, tr("Shortcut Key"),
                                                     "", items, asc, false, &ok);
                if (ok && !item.isEmpty())
                {
                    if (item == "[None]") OutputObjects[i]->Key = 0;
                    else OutputObjects[i]->Key = (int) (item.toLatin1()[0]);
                }
                RefreshOneOOItem(OutputObjects[i]->widgetitem, i);

            }

            if (column == 4)   //show
            {
                OutputObjects[i]->Show = !OutputObjects[i]->Show;

                //Complex filedirty issue
                if (OutputObjects[i]->Show == true)
                    for (int x = 0; x < Files.count(); x++)
                        if (OutputObjects[i]->Outputarray.isEmpty()) FilesDirty[x] = true;

                RefreshOneOOItem(OutputObjects[i]->widgetitem, i);
            }

            if (column == 5)   //merge
            {
                if (OutputObjects[i]->IsGroup)
                {
                    OutputObjects[i]->Merge = !OutputObjects[i]->Merge;
                    ResetFilesDirty();
                }
                else
                {
                    if (OutputObjects[i]->Parent != -1)
                    {
                        QString message = "Merge this group together?";
                        if (OutputObjects[OutputObjects[i]->Parent]->Merge) message = "Un-merge this group?";
                        if (QMessageBox::question(this, "Merge Output Objects", message, QMessageBox::Yes | QMessageBox::No)
                                == QMessageBox::Yes)
                        {
                            OutputObjects[OutputObjects[i]->Parent]->Merge = !OutputObjects[OutputObjects[i]->Parent]->Merge;
                            ResetFilesDirty();
                        }
                    }
                    else Message("You can only merge a group");
                }
                RefreshOO(); //refresh the whole thing
            }
        }
    DoubleClickTimer.restart();
}

void MainWindowImpl::on_OOTreeWidget_expanded(QModelIndex index)
{
    OOTreeWidget->resizeColumnToContents(0);
}

void MainWindowImpl::on_OONew_clicked()
{
    //OK, add based on all the stuff selected

    //first - is this going in a group
    int parent = -1;

    QList <QTreeWidgetItem *> selitems = OOTreeWidget->selectedItems();
    if (selitems.count() == 1)
        for (int i = 0; i < OutputObjectsCount; i++)
            if (OutputObjects[i]->widgetitem == selitems[0])
                if (OutputObjects[i]->IsGroup) parent = i;

    OutputObject *o = new OutputObject("");
    int count = 0;
    QString qname = "";
    QString name;
    int cflag = false;
    // do the curves
    if (DirectCurves->isChecked())
    {
        for (int i = 0; i < CurveCount; i++)
        {
            if (Curves[i]->widgetitem->isSelected())
            {
                o->CurveComponents.append(i);
                qname = Curves[i]->Name + " ";
                count++;
                cflag = true;
            }
        }
    }

    QString sname = "";
    for (int i = 0; i < SegmentCount; i++)
        if (Segments[i]->widgetitem->isSelected() || (SegmentCount == 1 && cflag == false))
        {
            o->ComponentSegments.append(i);
            count++;
            if (sname == "") sname = " (" + Segments[i]->Name + ")";
            else sname = " (Multiple Segs)";
        }

    if (count == 0)
    {
        Message("Select one or more segments");
        delete o;
        return;
    }


    count = 0;
    for (int j = 0; j <= MaxUsedMask; j++)
    {
        if (MasksSettings[j]->widgetitem->isSelected())
        {
            //this combo is in
            o->ComponentMasks.append(j);
            if (SegmentCount == 1) name = MasksSettings[j]->Name;
            else name = MasksSettings[j]->Name + sname;
            count++;
        }
    }

    if (count == 0 && cflag == false)
    {
        Message("Select one or more masks");
        delete o;
        return;
    }

    if (count > 1)
    {
        name = "Object ";
        QTextStream t(&name);
        t << OutputObjects.count();
    }
    o->Name = qname + name;
    o->Key = (int) (NextKey->currentText().toLatin1()[0]);
    if (o->Key == 91) o->Key = 0;
    o->Resample = OOResample->value();
    o->Parent = parent;

    //work out a colour
    int r = 0, g = 0, b = 0, countc = 0;

    //average of all curves and masks
    for (int i = 0; i < o->ComponentMasks.count(); i++)
    {
        r += MasksSettings[o->ComponentMasks[i]]->ForeColour[0];
        g += MasksSettings[o->ComponentMasks[i]]->ForeColour[1];
        b += MasksSettings[o->ComponentMasks[i]]->ForeColour[2];
        countc++;
    }
    for (int i = 0; i < o->CurveComponents.count(); i++)
    {
        r += Curves[o->CurveComponents[i]]->Colour[0];
        g += Curves[o->CurveComponents[i]]->Colour[1];
        b += Curves[o->CurveComponents[i]]->Colour[2];
        countc++;
    }

    if (o->ComponentSegments.count() > 1)   //also include segs if more than one
    {
        for (int i = 0; i < o->ComponentSegments.count(); i++)
        {
            r += Segments[o->ComponentSegments[i]]->Colour[0];
            g += Segments[o->ComponentSegments[i]]->Colour[1];
            b += Segments[o->ComponentSegments[i]]->Colour[2];
            countc++;
        }
    }
    r /= countc;
    g /= countc;
    b /= countc;

    o->Colour[0] = r;
    o->Colour[1] = g;
    o->Colour[2] = b;

    int max = -1;
    for (int i = 0; i < OutputObjectsCount; i++)
        if (OutputObjects[i]->ListOrder > max) max = OutputObjects[i]->ListOrder;

    o->ListOrder = max + 1;
    OutputObjects.append(o);
    OutputObjectsCount++;

    ResetFilesDirty();

    RefreshOO();
    if (NextKey->currentIndex() != 0) NextKey->setCurrentIndex(NextKey->currentIndex() + 1);

}

void MainWindowImpl::on_OOGroup_clicked()
{
    QList <QTreeWidgetItem *> selected = OOTreeWidget->selectedItems();
    int oldparent = -2;
    bool flag = false;
    if (selected.count() == 0) Message("Select objects to group");
    else
    {
        //OK, do grouping
        for (int i = 0; i < selected.count(); i++)
            for (int j = 0; j < OutputObjectsCount; j++)
                if (OutputObjects[j]->widgetitem == selected[i])
                {
                    if (oldparent == -2) oldparent = OutputObjects[j]->Parent;
                    if (oldparent != OutputObjects[j]->Parent) flag = true; //this will have to be root level
                    OutputObjects[j]->Parent = OutputObjectsCount; //reparents if necessary of course
                }

        OutputObject *o = new OutputObject("Group");
        o->Key = (int) (NextKey->currentText().toLatin1()[0]);
        if (o->Key == 91) o->Key = 0;
        o->Resample = OOResample->value();
        o->IsGroup = true;
        if (flag) o->Parent = -1;
        else o->Parent = oldparent;
        int max = -1;
        for (int i = 0; i < OutputObjectsCount; i++)
            if (OutputObjects[i]->ListOrder > max) max = OutputObjects[i]->ListOrder;

        o->ListOrder = max + 1;
        OutputObjects.append(o);
        OutputObjectsCount++;

        ResetFilesDirty(); //need in case one of objects in group was in a merged group previously
        RefreshOO();
    }

}

void MainWindowImpl::on_OODelete_clicked()
{
    QList <int> list;
restart:
    for (int i = 0; i < OutputObjectsCount; i++)
    {
        if (OutputObjects[i]->widgetitem)
            if (OutputObjects[i]->widgetitem->isSelected())
            {
                bool flag = false;
                if (OutputObjects[i]->IsGroup == true && OutputObjects[i]->widgetitem->isExpanded() == false)
                {
                    //unexpanded group - if there are any unselected in here we want to ask if they should be deleted too
                    for (int j = 0; j < OutputObjectsCount; j++)
                        if (OutputObjects[j]->Parent == i)
                        {
                            if (OutputObjects[j]->widgetitem)
                                if (OutputObjects[j]->widgetitem->isSelected() == false)
                                    flag = true;
                        }
                }

                if (flag)
                {
                    if (QMessageBox::question(this, "Delete Output Objects", "Delete objects within group " + OutputObjects[i]->Name + "?", QMessageBox::Yes | QMessageBox::No)
                            == QMessageBox::Yes)
                        //Yes, delete them - simply select them
                        for (int j = 0; j < OutputObjectsCount; j++)
                            if (OutputObjects[j]->Parent == i)
                                if (OutputObjects[j]->widgetitem) OutputObjects[j]->widgetitem->setSelected(true);
                }

                //will be deleting this one, so reduce all parents indices that will be affected by 1
                for (int j = 0; j < OutputObjectsCount; j++)
                {

                    if (OutputObjects[j]->Parent == i) OutputObjects[j]->Parent = OutputObjects[i]->Parent;
                    if (OutputObjects[j]->Parent > i) OutputObjects[j]->Parent--;
                }

                delete OutputObjects.takeAt(i); //will be deleted
                OutputObjectsCount--;

                goto restart;
            }
    }

    RefreshOO();
}

void MainWindowImpl::on_OOUp_clicked()
{
    QList <QTreeWidgetItem *> selected = OOTreeWidget->selectedItems();
    if (selected.count() != 1) Message("Select a single item to move up");
    else
    {
        for (int i = 0; i < OutputObjectsCount; i++)
        {
            if (OutputObjects[i]->widgetitem == selected[0])
            {
                //i is now the correct index
                //find  one to swap with
                int highestval = -1;
                int highindex = -1;
                for (int j = 0; j < OutputObjectsCount; j++)
                {
                    if ((OutputObjects[j]->ListOrder) > highestval && (OutputObjects[j]->ListOrder) < OutputObjects[i]->ListOrder && (OutputObjects[j]->Parent) == (OutputObjects[i]->Parent))
                    {
                        highestval = OutputObjects[j]->ListOrder;
                        highindex = j;
                    }
                }

                if (highindex >= 0)
                {
                    //do the swap
                    int temp = OutputObjects[highindex]->ListOrder;
                    OutputObjects[highindex]->ListOrder = OutputObjects[i]->ListOrder;
                    OutputObjects[i]->ListOrder = temp;
                }
            }
        }
    }
    RefreshOO();
}

void MainWindowImpl::on_OODown_clicked()
{
    QList <QTreeWidgetItem *> selected = OOTreeWidget->selectedItems();
    if (selected.count() != 1) Message("Select a single item to move down");
    else
    {
        for (int i = 0; i < OutputObjectsCount; i++)
        {
            if (OutputObjects[i]->widgetitem == selected[0])
            {
                //i is now the correct index
                //find  one to swap with
                int lowestval = 9999999;
                int lowestindex = -1;
                for (int j = 0; j < OutputObjectsCount; j++)
                {
                    if ((OutputObjects[j]->ListOrder) < lowestval && (OutputObjects[j]->ListOrder) > OutputObjects[i]->ListOrder && (OutputObjects[j]->Parent) == (OutputObjects[i]->Parent))
                    {
                        lowestval = OutputObjects[j]->ListOrder;
                        lowestindex = j;
                    }
                }

                if (lowestindex >= 0)
                {
                    //do the swap
                    int temp = OutputObjects[lowestindex]->ListOrder;
                    OutputObjects[lowestindex]->ListOrder = OutputObjects[i]->ListOrder;
                    OutputObjects[i]->ListOrder = temp;
                }
            }
        }
    }
    RefreshOO();
}

void MainWindowImpl::on_MasksTreeWidget_itemSelectionChanged()
{
    if (bodgeflag) return;


    //do the underlining trick
    for (int i = 0; i < MasksTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *t =    MasksTreeWidget->topLevelItem(i);
        QFont f = t->font(1);
        if (t->isSelected())    f.setUnderline(true);
        else  f.setUnderline(false);
        t->setFont(1, f);
    }

    if (MasksTreeWidget->selectedItems().count() == 1 && NoUpdateSelectionFlag == false && (QApplication::mouseButtons().testFlag(Qt::RightButton)) == false)
    {
        for (int i = 0;  i <= MaxUsedMask; i++)
        {
            if (MasksSettings[i]->widgetitem->isSelected())
            {
                int o = SelectedMask;
                SelectedMask = i;
                RefreshOneMaskItem(MasksSettings[i]->widgetitem, i);
                RefreshOneMaskItem(MasksSettings[o]->widgetitem, o);
                RefreshMasksBoxes();
            }
        }
    }
}



void MainWindowImpl::on_SegmentsTreeWidget_itemSelectionChanged()
{
    if (bodgeflag) return; //do the underlining trick
    for (int i = 0; i < SegmentsTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *t = SegmentsTreeWidget->topLevelItem(i);
        QFont f = t->font(1);
        if (t->isSelected()) f.setUnderline(true);
        else     f.setUnderline(false);
        t->setFont(1, f);
    }

    //if only one item selected, change the primary selection!

    if (SegmentsTreeWidget->selectedItems().count() == 1 && NoUpdateSelectionFlag == false && (QApplication::mouseButtons().testFlag(Qt::RightButton)) == false)
    {
        for (int i = 0;  i < SegmentCount; i++)
        {
            if (Segments[i]->widgetitem->isSelected())
            {
                int o = CurrentSegment;
                CurrentSegment = i;
                RefreshOneSegmentItem(Segments[i]->widgetitem, i);
                RefreshOneSegmentItem(Segments[o]->widgetitem, o);
                RefreshSegmentsBoxes();
                SetUpGenerationToolbox(CurrentSegment);
                ShowImage(graphicsView);
                rangescene->Refresh();
            }
        }
    }
}

void MainWindowImpl::OOunderlineChildren(QTreeWidgetItem *par)
{
    //do the underlining trick
    for (int i = 0; i < par->childCount(); i++)
    {
        QTreeWidgetItem *t =    par->child(i);
        QFont f = t->font(0);
        if (t->isSelected())    f.setUnderline(true);
        else  f.setUnderline(false);
        t->setFont(0, f);
        OOunderlineChildren(t);
    }
}

void MainWindowImpl::on_OOTreeWidget_itemSelectionChanged()
{
    if (bodgeflag) return;
    //are we in the odd select mode?
    if (OutputRegroupMode)
    {
        int count = 0;
        for (int i = 0; i < OutputObjectsCount; i++)
        {
            if (OutputObjects[i]->widgetitem->isSelected() && OutputObjects[i]->IsGroup && !OutputObjects[i]->TempSelected)
            {
                //move all to this group
                for (int j = 0; j < OutputObjectsCount; j++)
                {
                    if  (OutputObjects[j]->TempSelected)
                    {
                        OutputObjects[j]->Parent = i;
                        count++;
                    }
                }

            }
        }
        if (count > 0)
        {
            OutputRegroupMode = false;
            OOTreeWidget->setCursor(Qt::ArrowCursor);
            RefreshOO();
        }
        else
        {
            if (QMessageBox::question(this, "Move Object to Group", "Select a group to move selected objects to, or click Cancel to abort", QMessageBox::Ok | QMessageBox::Cancel)
                    == QMessageBox::Cancel)
            {
                OutputRegroupMode = false;
                OOTreeWidget->setCursor(Qt::ArrowCursor);
            }
        }
    }

    //do the underlining trick
    for (int i = 0; i < OOTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *t =    OOTreeWidget->topLevelItem(i);
        QFont f = t->font(0);
        if (t->isSelected())    f.setUnderline(true);
        else  f.setUnderline(false);
        t->setFont(0, f);
        OOunderlineChildren(t);
    }

    //and the selection stuff
    QList <QTreeWidgetItem *> t = OOTreeWidget->selectedItems();
    if (t.count() == 1)
    {
        for (int i = 0; i < OutputObjectsCount; i++)
            if (OutputObjects[i]->widgetitem == t[0])
            {
                //single selection - alter selections of other stuff
                NoUpdateSelectionFlag = true; //stops selections changing the working mask

                for (int j = 0; j <= MaxUsedMask; j++) if (MasksSettings[j]->widgetitem) MasksSettings[j]->widgetitem->setSelected(false);
                for (int j = 0; j < OutputObjects[i]->ComponentMasks.count(); j++)
                    MasksSettings[OutputObjects[i]->ComponentMasks[j]]->widgetitem->setSelected(true);

                for (int j = 0; j < SegmentCount; j++) if (Segments[j]->widgetitem) Segments[j]->widgetitem->setSelected(false);
                for (int j = 0; j < OutputObjects[i]->ComponentSegments.count(); j++)
                    Segments[OutputObjects[i]->ComponentSegments[j]]->widgetitem->setSelected(true);


                for (int j = 0; j < CurveCount; j++)
                {
                    if (Curves[j]->widgetitem) Curves[j]->widgetitem->setSelected(false);
                }
                for (int j = 0; j < OutputObjects[i]->CurveComponents.count(); j++)
                    Curves[OutputObjects[i]->CurveComponents[j]]->widgetitem->setSelected(true);

                NoUpdateSelectionFlag = false;

            }
    }
}

void MainWindowImpl::on_actionUngroup_triggered()
{
    //Ungroup command
    //For all selected objects:
    //QMessageBox::information(this,"","About to try ungrouping");
    bool flag = true;
    for (int i = 0; i < OutputObjectsCount; i++)
    {
        if (OutputObjects[i]->widgetitem->isSelected())
        {
            if (OutputObjects[i]->Parent != -1)
            {
                //QMessageBox::information(this,"","About to reparent");

                OutputObjects[i]->Parent = OutputObjects[OutputObjects[i]->Parent]->Parent;
                //QMessageBox::information(this,"","reparented");
                flag = false;
            }
        }
    }
    //if (flag) QMessageBox::information(this,"","Flag is true"); else   QMessageBox::information(this,"","Flag is false");

    if (flag) Message ("No objects selected that can be removed from groups!");
    else
    {
        RefreshOO();
        ResetFilesDirty();
    }
    //QMessageBox::information(this,"","returning");

}


void MainWindowImpl::on_actionMove_item_to_Group_triggered()
{
    int count = 0;
    for (int i = 0; i < OutputObjectsCount; i++)
    {
        if (OutputObjects[i]->widgetitem->isSelected())
        {
            OutputObjects[i]->TempSelected = true;
            count++;
        }
        else  OutputObjects[i]->TempSelected = false;
        if (count != 0)
        {
            OutputRegroupMode = true;
            OOTreeWidget->setCursor(Qt::PointingHandCursor);
        }
    }
}

void MainWindowImpl::CleanseOO()
{
    //need to go through all the lists and remove anything that's -1

    for (int i = 0; i < OutputObjectsCount; i++)
    {
        if (!(OutputObjects[i]->IsGroup))
        {
            QList <int> NewList;

            NewList.clear();
            foreach (int v, OutputObjects[i]->ComponentMasks)
            {
                if (v != -1) if (NewList.indexOf(v) == -1) //if not already there
                        NewList.append(v);
            }
            OutputObjects[i]->ComponentMasks = NewList;

            NewList.clear();
            foreach (int v, OutputObjects[i]->ComponentSegments)
            {
                if (v != -1) if (NewList.indexOf(v) == -1) //if not already there
                        NewList.append(v);
            }
            OutputObjects[i]->ComponentSegments = NewList;

            NewList.clear();
            foreach (int v, OutputObjects[i]->CurveComponents)
            {
                if (v != -1) if (NewList.indexOf(v) == -1) //if not already there
                        NewList.append(v);
            }
            OutputObjects[i]->CurveComponents = NewList;

            if ((OutputObjects[i]->CurveComponents.count() +  OutputObjects[i]->ComponentSegments.count() +  OutputObjects[i]->ComponentMasks.count()) == 0)
            {
                //item now empty
                delete OutputObjects.takeAt(i);
                i--;
                OutputObjectsCount--;
            }
            else
            {
                if (OutputObjects[i]->ComponentMasks.count() == 0 || OutputObjects[i]->ComponentSegments.count() == 0)
                {
                    if (OutputObjects[i]->CurveComponents.count() == 0)
                    {
                        // if only one mask OR segment and no curves - can delete the whole object
                        delete OutputObjects.takeAt(i);
                        i--;
                        OutputObjectsCount--;
                    }
                    else     //there are curves - so delete the remaining seg/mask - just clear both lists - but leave object
                    {
                        OutputObjects[i]->ComponentMasks.clear();
                        OutputObjects[i]->ComponentSegments.clear();
                    }
                }
            }
        }
    }
    RefreshOO();
}


void MainWindowImpl::on_actionOutput_SPIERSview_triggered()
{
    CopyingImpl cop;
    if (actionUse_Old_Exporting_Code->isChecked()) cop.ExportSPV(0);
    else cop.ExportSPV_2(0);
}


void MainWindowImpl::on_actionExport_SPIERSview_and_Launch_triggered()
{
    CopyingImpl cop;
    if (actionUse_Old_Exporting_Code->isChecked()) cop.ExportSPV(1);
    else cop.ExportSPV_2(1);
}

void MainWindowImpl::on_actionView_in_SPIERSview_triggered()
{
    CopyingImpl cop;
    if (actionUse_Old_Exporting_Code->isChecked()) cop.ExportSPV(2);
    else cop.ExportSPV_2(2);
}

/*Now mask copying stuff*/

void MainWindowImpl::on_actionMaskCopy_selected_from_Previous_triggered()
{
    CopyingImpl cop;
    if (CurrentFile == 0) Message("This is the first slice image in the dataset");
    else
    {
        cop.MaskCopy(CurrentFile - 1, this);
        ResetUndo();
        //do advance/back
        if (MasksMoveForward)
            if (CurrentFile == FileCount - 1) Message ("This is the last file - can't move forward!");
            else SliderPos->setValue(CurrentFile + 2);
        else if (MasksMoveBack)
            if (CurrentFile == 0) Message ("This is the first file - can't move back!");
            else SliderPos->setValue(CurrentFile);
    }
}

void MainWindowImpl::on_actionMaskCopy_all_from_previous_triggered() //from current actually
{
    CopyingImpl cop;
    cop.MaskCopy(CurrentFile, this);
    ResetUndo();
    //do advance/back
    if (MasksMoveForward)
        if (CurrentFile == FileCount - 1) Message ("This is the last file - can't move forward!");
        else SliderPos->setValue(CurrentFile + 2);
    else if (MasksMoveBack)
        if (CurrentFile == 0) Message ("This is the first file - can't move back!");
        else SliderPos->setValue(CurrentFile);
}

void MainWindowImpl::on_actionMaskCopy_selected_from_next_triggered() //from next actually
{
    CopyingImpl cop;
    if (CurrentFile == FileCount - 1) Message("This is the last slice image in the dataset");
    else
    {
        cop.MaskCopy(CurrentFile + 1, this);

        //do advance/back
        if (MasksMoveForward)
            if (CurrentFile == FileCount - 1) Message ("This is the last file - can't move forward!");
            else SliderPos->setValue(CurrentFile + 2);
        else if (MasksMoveBack)
            if (CurrentFile == 0) Message ("This is the first file - can't move back!");
            else SliderPos->setValue(CurrentFile);

        ResetUndo();
    }
}

void MainWindowImpl::on_actionMaskCopy_all_from_next_triggered() //actually copy from selected to current
{
    CopyingImpl cop;
    if (SliceSelectorList->selectedItems().count() == 0 || SliceSelectorList->selectedItems().count() > 1 || (SliceSelectorList->selectedItems().count() == 1
                                                                                                              && SliceSelectorList->item(CurrentFile)->isSelected()))
        Message("Select ONE other slice to copy from");
    else
        for (int i = 0; i < FileCount; i++)
            if (SliceSelectorList->item(i)->isSelected())
            {
                cop.MaskCopy2(i, this);

                //do advance/back
                if (MasksMoveForward)
                    if (CurrentFile == FileCount - 1) Message ("This is the last file - can't move forward!");
                    else SliderPos->setValue(CurrentFile + 2);
                else if (MasksMoveBack)
                    if (CurrentFile == 0) Message ("This is the first file - can't move back!");
                    else SliderPos->setValue(CurrentFile);

                ResetUndo();
            }
}

void MainWindowImpl::on_actionMask_Advance_slice_after_copy_operation_toggled(bool )
{
    if (actionMask_Advance_slice_after_copy_operation->isChecked() )
    {
        MasksMoveBack = false;
        MasksMoveForward = true;
        actionMaskGo_back_one_slice_after_copy->setChecked(false);
    }
    else  MasksMoveForward = false;
}

void MainWindowImpl::on_actionMaskGo_back_one_slice_after_copy_toggled(bool )
{
    if (actionMaskGo_back_one_slice_after_copy->isChecked() )
    {
        MasksMoveForward = false;
        MasksMoveBack = true;
        actionMask_Advance_slice_after_copy_operation->setChecked(false);
    }
    else  MasksMoveBack = false;
}




void MainWindowImpl::on_actionCreate_mask_from_curve_triggered()
{
    CopyingImpl cop;
    cop.CurvesToMasks(this);
}



void MainWindowImpl::on_actionExport_DXF_triggered()
{
    ExportDXF dialog(this);
    dialog.exec();
}


extern bool temptestflag;

void MainWindowImpl::on_PosSpinBox_valueChanged(int val)
{
    SliderPos->setValue(val);
}

void MainWindowImpl::Moveimage(int imageno)
//scroll bar moved - change the current image
{

    if (!Active) return;
    if (CurrentFile == imageno - 1) return; //can happen if moved by direct call to Moved_Image

    PosSpinBox->setValue(imageno);
    temptestflag = true;
    WriteAllData(CurrentFile);  //store before we move
    UndoTimer(); //write if any changes

    if (HorribleBodgeFlagDontStoreUndo == false) //have I blocked undo storing? if not..
        UndoEvents.prepend(new UndoEvent(CurrentFile, imageno - 1)); //add the move event to undo list - store move FROM not to!
    CurrentFile = imageno - 1;
    LoadAllData(-10000 + CurrentFile);

    //set my 'blank' starting point for undo
    if (HorribleBodgeFlagDontStoreUndo == false)
    {
        MasksUndoDirty = true;
        LocksUndoDirty = true;
        CurvesUndoDirty = true;
        for (int i = 0; i < SegmentCount; i++) Segments[i]->UndoDirty = true;

        UndoEvents.prepend(new UndoEvent(-1, -1));
        qDeleteAll(RedoEvents.begin(), RedoEvents.end());
        RedoEvents.clear(); //wipe the redo
        if (RedoEvents.count() > 0) actionRedo->setEnabled(true);
        else actionRedo->setEnabled(false);
    }

    ShowImage(graphicsView);

    setWindowTitle("SPIERSEdit - " + SettingsFileName + " - " + Files[CurrentFile]);
    SliceSelectorList->clearSelection();
    SliceSelectorList->item(CurrentFile)->setSelected(true);
    SliceSelectorList->scrollToItem(SliceSelectorList->item(CurrentFile)); //make sure selected is visible

    graphicsView->viewport()->repaint();

    if (this->actionGrey_out_curves_not_no_current_slice->isChecked()) RefreshCurves();
    temptestflag = false;
}

void MainWindowImpl::on_actionGrey_out_curves_not_no_current_slice_triggered()
{
    RefreshCurves();
}

bool MainWindowImpl::event ( QEvent *event )
{
    //new event handler
    bool b;
    //if (temptestflag)
    //{
    //  if (event->type()==QEvent::Paint) {qDebug()<<"Paint event"<<((QPaintEvent*) event)->rect(); return false;}
    //}
    b = QWidget::event(event);
    return b;
};

void MainWindowImpl::on_actionShow_position_slice_selector_toggled(bool tog)
{
    Q_UNUSED(tog);
    ShowSlicePosition = actionShow_position_slice_selector->isChecked();
    for (int i = 0; i < FileCount; i++)
        SliceSelectorList->item(i)->setText(TextForSliceSelectorBox(i));
}


void MainWindowImpl::FixUpStretches()
{
    //check there IS an increasing sequence!
    int affectedslice = -1;
    for (int i = 1; i < FullFiles.count(); i++)
    {
        if (FullStretches[i] <= FullStretches[i - 1])
        {
            if (affectedslice == -1) affectedslice = i;
            do FullStretches[i]++;
            while (FullStretches[i] <= FullStretches[i - 1]);
        }
    }

    if (affectedslice != -1)
    {
        QString mess;
        QTextStream st(&mess);
        st << "This change will violate slice order (i.e. later slices will have a position value LOWER than earlier ones). To proceed SPIERS will have to modify position values for affected slices - is this OK?";
        if (QMessageBox::warning(this, "Change Slice Spacing", mess, QMessageBox::Ok | QMessageBox::Cancel)
                != QMessageBox::Ok)
        {
            FullStretches = OldStretches;
            return;
        }
    }

    //Redo Stretches from FullStretches
    if (zsparsity > 1)
    {
        Stretches.clear();
        for (int i = 0; i < FullStretches.count(); i += zsparsity)
        {
            Stretches.append(FullStretches[i]);
        }
    }
    else Stretches = FullStretches;


    //It's OK - modify the box
    for (int i = 0; i < FileCount; i++)
    {
        SliceSelectorList->item(i)->setText(TextForSliceSelectorBox(i));
    }

}


void MainWindowImpl::on_actionChange_slice_spacing_triggered()
{
    // This is relative slice spacing
    if (SliceSelectorList->selectedItems().count() == 0)
    {
        Message("No slices selected!");
        return;
    }

    OldStretches = FullStretches; // backup copy
    //TO DO - a warning if slice 0 is selected
    slicespacingdialogImpl dialog;
    dialog.exec();
    if (dialog.relative < 0) return;

    //OK, we have something to do!
    if (SliceSelectorList->item(0)->isSelected()) FullStretches[0] = dialog.relative;
    for (int i = 1; i < FileCount; i++)
    {
        if (SliceSelectorList->item(i)->isSelected()) FullStretches[i * zsparsity] = FullStretches[(i - 1) * zsparsity] + dialog.relative;
    }

    FixUpStretches();
}

void MainWindowImpl::on_actionSet_slice_position_triggered()
{
    double lowvalue, currentvalue;
    if (SliceSelectorList->selectedItems().count() == 0)
    {
        Message("No slices selected!");
        return;
    }

    OldStretches = FullStretches; // backup copy

    int firstsel = -1;
    for (int i = 0; i < FileCount; i++)
        if (SliceSelectorList->item(i)->isSelected())
        {
            firstsel = i;
            break;
        }

    if (firstsel == -1) Error ("Internal error - failed to find selected slice");
    if (firstsel == 0) lowvalue = 0;
    else lowvalue = FullStretches[(firstsel - 1) * zsparsity] * (1.0 / SlicePerMM);

    currentvalue = FullStretches[firstsel * zsparsity] * (1.0 / SlicePerMM);

    bool ok;
    double NewPos = QInputDialog::getDouble(this, tr("New position"),
                                            tr("New position for first selected slice (others selected will be moved relative to the first slice):"),
                                            currentvalue, lowvalue + .001, 1000000, 3, &ok);
    if (ok)
    {
        double offset = (NewPos - currentvalue) * SlicePerMM;
        for (int i = 0; i < FileCount; i++)
            if (SliceSelectorList->item(i)->isSelected()) FullStretches[i * zsparsity] += offset;
    }
    FixUpStretches();
}


void MainWindowImpl::on_FindPolynomial_pressed()
{
    WriteAllData(CurrentFile);
    findpolynomialImpl dialog;
    dialog.find();
}

void MainWindowImpl::on_SpinBoxRangeBase_valueChanged(int v )
{
    if (Active)
    {
        //if (v>=Segments[CurrentSegment]->RangeTop) v==Segments[CurrentSegment]->RangeTop-1;
        Segments[CurrentSegment]->RangeBase = v;
        if (v >= Segments[CurrentSegment]->RangeTop)
            SpinBoxRangeBase->setValue(Segments[CurrentSegment]->RangeTop - 1);
        else
            rangescene->Refresh();
        if (GenerateAuto->checkState()) GenButton();
    }
}

void MainWindowImpl::on_SpinBoxRangeTop_valueChanged(int v)
{
    if (Active)
    {
        Segments[CurrentSegment]->RangeTop = v;
        //SpinBoxRangeBase->setMaximum(v-1);
        //SpinBoxRangeBase->setMaximum(Segments[CurrentSegment]->RangeTop-1);
        if (v <= Segments[CurrentSegment]->RangeBase)
            SpinBoxRangeTop->setValue(Segments[CurrentSegment]->RangeBase + 1);
        else
            rangescene->Refresh();
        if (GenerateAuto->checkState()) GenButton();
    }
}


void MainWindowImpl::on_actionDistribute_over_range_triggered()
{
    int count = 0;
    for (int i = 0; i < SegmentCount; i++)
    {
        if (Segments[i]->Activated && Segments[i]->widgetitem != 0)
            if (Segments[i]->widgetitem->isSelected()) count++;
    }
    if (count < 2) Message("Select as least two segments to distribute");
    else
    {
        DistributeDialogImpl dialog;
        dialog.exec();
        rangescene->Refresh();
    }
}


void MainWindowImpl::on_actionMeasure_Volumes_triggered()
{
    CopyingImpl cop;
    cop.MeasureVols();
}

void MainWindowImpl::on_actionInterpolate_over_selected_slices_triggered()
{
    //Interpolate selected curve(s) from first to last selected slice
    QList <QTreeWidgetItem *> selitems = CurvesTreeWidget->selectedItems();

    if (selitems.count() != 1) Message ("Select a single curve");
    else
    {
        int c = -1;
        for (int i = 0; i < CurveCount; i++)
            if (Curves[i]->widgetitem == selitems[0]) c = i;

        if (c == -1) Error("Oops, didn't find the selected curve!");
        QList <QListWidgetItem *> selitems2 = SliceSelectorList->selectedItems();
        if (selitems2.count() < 2) Message ("Select at least two slices to interpolate between");
        else
        {
            //work out first and last slices

            int FirstFile = -1, LastFile = -1;
            for (int i = 0; i < Files.count(); i++)
            {
                if (SliceSelectorList->item(i)->isSelected())
                {
                    if (FirstFile == -1) FirstFile = i;
                    LastFile = i;
                }
            }
            FirstFile *= zsparsity;
            LastFile *= zsparsity;
            if (FirstFile + 1 >= LastFile)
            {
                Message("Nothing to interpolate!");
                return;
            }
            //loop through selected curves, check all
            if (Curves[c]->SplinePoints[FirstFile]->Count != Curves[c]->SplinePoints[LastFile]->Count) Message("Curve must have same number of points on first and last slices");
            else
            {
                if (QMessageBox::question(this, "Interpolate Curves",
                                          "This will interpolate selected curves between first and last selected slices - any data in intervening slices will be overwritten. Are you sure?", QMessageBox::Yes | QMessageBox::Cancel)
                        == QMessageBox::Yes)
                {
                    //OK, do the interpolation

                    //First set up undo point
                    MakeUndo("Interpolate");

                    //remove all intervening curves
                    for (int i = FirstFile + 1; i < LastFile; i++)
                    {
                        Curves[c]->SplinePoints[i]->Count = Curves[c]->SplinePoints[FirstFile]->Count;
                        Curves[c]->SplinePoints[i]->X.clear();
                        Curves[c]->SplinePoints[i]->Y.clear();
                        //qDebug()<<"Dirtying "<<i<<" Files(i) is "<<Files[i];
                        if (Curves[c]->Segment != 0) FilesDirty[i / zsparsity] = true;
                    }

                    for (int j = 0; j < Curves[c]->SplinePoints[FirstFile]->Count; j++)
                    {
                        double x = Curves[c]->SplinePoints[FirstFile]->X[j];
                        double y = Curves[c]->SplinePoints[FirstFile]->Y[j];
                        double xinc = (Curves[c]->SplinePoints[LastFile]->X[j] - x) / (double)(LastFile - FirstFile);
                        double yinc = (Curves[c]->SplinePoints[LastFile]->Y[j] - y) / (double)(LastFile - FirstFile);
                        for (int i = FirstFile + 1; i < LastFile; i++)
                        {
                            x += xinc;
                            y += yinc;
                            Curves[c]->SplinePoints[i]->X.append(x);
                            Curves[c]->SplinePoints[i]->Y.append(y);
                        }
                    }
                }
            }
        }
    }
    //ResetUndo();
    ShowImage(graphicsView);
    RefreshCurves();

}
