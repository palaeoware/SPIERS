#include "distributedialogimpl.h"
#include "globals.h"

DistributeDialogImpl::DistributeDialogImpl( QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

}


void DistributeDialogImpl::on_SpinBoxFrom_valueChanged(int v)
{
    SpinBoxTo->setMinimum(v + 1);
}

void DistributeDialogImpl::on_SpinBoxTo_valueChanged(int v)
{
    SpinBoxFrom->setMaximum(v - 1);
}

void DistributeDialogImpl::on_buttonBox_accepted()
{
    double range = static_cast<double>(SpinBoxTo->value() - SpinBoxFrom->value());

    int count = 0;
    for (int i = 0; i < SegmentCount; i++)
    {
        if (Segments[i]->Activated && Segments[i]->widgetitem != nullptr)
            if (Segments[i]->widgetitem->isSelected()) count++;
    }

    range /= static_cast<double>(count);
    double pos = static_cast<double>(SpinBoxFrom->value());
    QList <bool> usedflags;
    for (int i = 0; i < SegmentCount; i++) usedflags.append(false);
    for (int kloop = 0; kloop < SegmentCount; kloop++)
    {
        //find lowest
        int lowestval = 999999;
        int lowestindex = -1;
        for (int j = 0; j < SegmentCount; j++)
        {
            if (Segments[j]->ListOrder < lowestval && usedflags[j] == false && Segments[j]->Activated && Segments[j]->widgetitem->isSelected())
            {
                lowestval = Segments[j]->ListOrder;
                lowestindex = j;
            }
        }
        if (lowestindex == -1) //didn't find any- get out
        {
            close();
            return;
        }
        int i = lowestindex;
        usedflags[i] = true;

        //i is now next in list order
        Segments[i]->RangeBase = static_cast<int>(pos + .5);
        Segments[i]->RangeTop = static_cast<int>(pos + range + .5);
        pos += range;
    }
    close();
}

void DistributeDialogImpl::on_buttonBox_rejected()
{
    close();
}

