/*********************************************

SPIERSedit 2: moreimpl.cpp

Implements the 'more' box under recent files
Just populates a list from superglobals
Returns selected one - won't return without one selected except as cancel

**********************************************/


#include "moreimpl.h"
#include "globals.h"
//
moreimpl::moreimpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(OK_Click()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(Cancel_Click()));

    //now set up the list
    foreach (RecentFiles rf, RecentFileList)
        listWidget->addItem(rf.File);
}

void moreimpl::OK_Click()
{
    QListWidgetItem *retitem;
    retitem = listWidget->currentItem();
    fname = retitem->text();
    Cancelled = false;
    close();
}


void moreimpl::Cancel_Click()
{
    Cancelled = true;
    close();
}

