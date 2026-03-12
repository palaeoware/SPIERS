/********************************************************************************
** Form generated from reading UI file 'curveresize.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CURVERESIZE_H
#define UI_CURVERESIZE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ResizeDialog
{
public:
    QHBoxLayout *hboxLayout;
    QDoubleSpinBox *doubleSpinBox;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ResizeDialog)
    {
        if (ResizeDialog->objectName().isEmpty())
            ResizeDialog->setObjectName("ResizeDialog");
        ResizeDialog->resize(256, 43);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ResizeDialog->sizePolicy().hasHeightForWidth());
        ResizeDialog->setSizePolicy(sizePolicy);
        ResizeDialog->setMinimumSize(QSize(256, 43));
        ResizeDialog->setMaximumSize(QSize(256, 43));
        hboxLayout = new QHBoxLayout(ResizeDialog);
        hboxLayout->setObjectName("hboxLayout");
        doubleSpinBox = new QDoubleSpinBox(ResizeDialog);
        doubleSpinBox->setObjectName("doubleSpinBox");
        doubleSpinBox->setMaximum(5000.000000000000000);
        doubleSpinBox->setValue(100.000000000000000);

        hboxLayout->addWidget(doubleSpinBox);

        buttonBox = new QDialogButtonBox(ResizeDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        hboxLayout->addWidget(buttonBox);


        retranslateUi(ResizeDialog);

        QMetaObject::connectSlotsByName(ResizeDialog);
    } // setupUi

    void retranslateUi(QDialog *ResizeDialog)
    {
        ResizeDialog->setWindowTitle(QCoreApplication::translate("ResizeDialog", "Resize curve by...", nullptr));
        doubleSpinBox->setSuffix(QCoreApplication::translate("ResizeDialog", "%", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ResizeDialog: public Ui_ResizeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CURVERESIZE_H
