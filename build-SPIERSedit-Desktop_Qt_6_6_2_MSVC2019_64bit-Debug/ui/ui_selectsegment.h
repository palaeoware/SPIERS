/********************************************************************************
** Form generated from reading UI file 'selectsegment.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTSEGMENT_H
#define UI_SELECTSEGMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelectSegment
{
public:
    QVBoxLayout *vboxLayout;
    QComboBox *comboBox;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SelectSegment)
    {
        if (SelectSegment->objectName().isEmpty())
            SelectSegment->setObjectName("SelectSegment");
        SelectSegment->resize(204, 69);
        vboxLayout = new QVBoxLayout(SelectSegment);
        vboxLayout->setObjectName("vboxLayout");
        comboBox = new QComboBox(SelectSegment);
        comboBox->setObjectName("comboBox");

        vboxLayout->addWidget(comboBox);

        buttonBox = new QDialogButtonBox(SelectSegment);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(SelectSegment);

        QMetaObject::connectSlotsByName(SelectSegment);
    } // setupUi

    void retranslateUi(QDialog *SelectSegment)
    {
        SelectSegment->setWindowTitle(QCoreApplication::translate("SelectSegment", "Segment for Curve", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SelectSegment: public Ui_SelectSegment {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTSEGMENT_H
