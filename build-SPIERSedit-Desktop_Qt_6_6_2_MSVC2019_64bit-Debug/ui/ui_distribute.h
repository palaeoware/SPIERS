/********************************************************************************
** Form generated from reading UI file 'distribute.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISTRIBUTE_H
#define UI_DISTRIBUTE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DistributeDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *SpinBoxFrom;
    QSpinBox *SpinBoxTo;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DistributeDialog)
    {
        if (DistributeDialog->objectName().isEmpty())
            DistributeDialog->setObjectName("DistributeDialog");
        DistributeDialog->resize(312, 85);
        verticalLayout = new QVBoxLayout(DistributeDialog);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(DistributeDialog);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        SpinBoxFrom = new QSpinBox(DistributeDialog);
        SpinBoxFrom->setObjectName("SpinBoxFrom");
        SpinBoxFrom->setMaximum(255);

        horizontalLayout->addWidget(SpinBoxFrom);

        SpinBoxTo = new QSpinBox(DistributeDialog);
        SpinBoxTo->setObjectName("SpinBoxTo");
        SpinBoxTo->setMinimum(1);
        SpinBoxTo->setMaximum(255);
        SpinBoxTo->setValue(255);

        horizontalLayout->addWidget(SpinBoxTo);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(DistributeDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DistributeDialog);

        QMetaObject::connectSlotsByName(DistributeDialog);
    } // setupUi

    void retranslateUi(QDialog *DistributeDialog)
    {
        DistributeDialog->setWindowTitle(QCoreApplication::translate("DistributeDialog", "Distrribute Segments", nullptr));
        label->setText(QCoreApplication::translate("DistributeDialog", "Distribute over range: ", nullptr));
        SpinBoxFrom->setPrefix(QCoreApplication::translate("DistributeDialog", "From: ", nullptr));
        SpinBoxTo->setPrefix(QCoreApplication::translate("DistributeDialog", "To: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DistributeDialog: public Ui_DistributeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISTRIBUTE_H
