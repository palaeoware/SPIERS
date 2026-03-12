/********************************************************************************
** Form generated from reading UI file 'slicespacing.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLICESPACING_H
#define UI_SLICESPACING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_slicespacingdialog
{
public:
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QDoubleSpinBox *Relative;
    QLabel *label_2;
    QDoubleSpinBox *Absolute;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *slicespacingdialog)
    {
        if (slicespacingdialog->objectName().isEmpty())
            slicespacingdialog->setObjectName("slicespacingdialog");
        slicespacingdialog->resize(215, 97);
        vboxLayout = new QVBoxLayout(slicespacingdialog);
        vboxLayout->setObjectName("vboxLayout");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(slicespacingdialog);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        Relative = new QDoubleSpinBox(slicespacingdialog);
        Relative->setObjectName("Relative");
        Relative->setDecimals(1);
        Relative->setMaximum(100000.000000000000000);
        Relative->setValue(100.000000000000000);

        gridLayout->addWidget(Relative, 0, 1, 1, 1);

        label_2 = new QLabel(slicespacingdialog);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        Absolute = new QDoubleSpinBox(slicespacingdialog);
        Absolute->setObjectName("Absolute");
        Absolute->setDecimals(3);
        Absolute->setMaximum(100.000000000000000);
        Absolute->setSingleStep(0.010000000000000);
        Absolute->setValue(1.000000000000000);

        gridLayout->addWidget(Absolute, 1, 1, 1, 1);


        vboxLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(slicespacingdialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(slicespacingdialog);

        QMetaObject::connectSlotsByName(slicespacingdialog);
    } // setupUi

    void retranslateUi(QDialog *slicespacingdialog)
    {
        slicespacingdialog->setWindowTitle(QCoreApplication::translate("slicespacingdialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("slicespacingdialog", "Relative Spacing", nullptr));
        Relative->setSuffix(QCoreApplication::translate("slicespacingdialog", "%", nullptr));
        label_2->setText(QCoreApplication::translate("slicespacingdialog", "Absolute Spacing", nullptr));
        Absolute->setSuffix(QCoreApplication::translate("slicespacingdialog", "mm", nullptr));
    } // retranslateUi

};

namespace Ui {
    class slicespacingdialog: public Ui_slicespacingdialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLICESPACING_H
