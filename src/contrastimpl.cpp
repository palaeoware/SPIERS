#include "contrastimpl.h"
#include "globals.h"
#include <QPainter>
#include <QPicture>

//
ContrastImpl::ContrastImpl(int MaskNo, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    MaskNumber = MaskNo;
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    ContrastSlider->setValue(MasksSettings[MaskNo]->Contrast);
    RedrawColour();
}


void ContrastImpl::on_ContrastSlider_valueChanged(int value)
{
    Q_UNUSED(value);
    RedrawColour();
}

void ContrastImpl::RedrawColour()
{
    QPicture picture;
    QPainter painter;
    int c = ContrastSlider->value() + 1;
    if (c == 6) c = 20000; //big value, want 0's!
    painter.begin(&picture);           // paint in picture
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(QColor(MasksSettings[MaskNumber]->ForeColour[0] / c, MasksSettings[MaskNumber]->ForeColour[1] / c, MasksSettings[MaskNumber]->ForeColour[2] / c)));
    painter.drawRect(0, 0, 120, 24);    // draw a rect
    painter.setBrush(QBrush(QColor(MasksSettings[MaskNumber]->ForeColour[0], MasksSettings[MaskNumber]->ForeColour[1], MasksSettings[MaskNumber]->ForeColour[2])));
    painter.drawRect(30, 6, 60, 12);    // draw a rect
    painter.end();                     // painting done
    SampleLabel->setPicture(picture);
}

void ContrastImpl::on_buttonBox_accepted()
{
    int c = ContrastSlider->value() + 1;
    if (c == 6) c = 20000; //big value, want 0's!
    MasksSettings[MaskNumber]->BackColour[0] = MasksSettings[MaskNumber]->ForeColour[0] / c;
    MasksSettings[MaskNumber]->BackColour[1] = MasksSettings[MaskNumber]->ForeColour[1] / c;
    MasksSettings[MaskNumber]->BackColour[2] = MasksSettings[MaskNumber]->ForeColour[2] / c;
    MasksSettings[MaskNumber]->Contrast = ContrastSlider->value();
    Cancelled = false;
    close();
    // TODO
}

void ContrastImpl::on_buttonBox_rejected()
{
    Cancelled = true;
    close();
}



