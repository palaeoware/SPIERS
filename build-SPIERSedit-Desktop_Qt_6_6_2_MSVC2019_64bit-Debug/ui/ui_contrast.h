/********************************************************************************
** Form generated from reading UI file 'contrast.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTRAST_H
#define UI_CONTRAST_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>

QT_BEGIN_NAMESPACE

class Ui_ContrastDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSlider *ContrastSlider;
    QLabel *SampleLabel;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ContrastDialog)
    {
        if (ContrastDialog->objectName().isEmpty())
            ContrastDialog->setObjectName("ContrastDialog");
        ContrastDialog->resize(505, 47);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ContrastDialog->sizePolicy().hasHeightForWidth());
        ContrastDialog->setSizePolicy(sizePolicy);
        ContrastDialog->setMinimumSize(QSize(505, 47));
        ContrastDialog->setMaximumSize(QSize(505, 47));
        QIcon icon;
        icon.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        ContrastDialog->setWindowIcon(icon);
        ContrastDialog->setModal(true);
        horizontalLayout = new QHBoxLayout(ContrastDialog);
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(ContrastDialog);
        label->setObjectName("label");
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label);

        ContrastSlider = new QSlider(ContrastDialog);
        ContrastSlider->setObjectName("ContrastSlider");
        ContrastSlider->setMinimum(1);
        ContrastSlider->setMaximum(5);
        ContrastSlider->setPageStep(1);
        ContrastSlider->setOrientation(Qt::Horizontal);
        ContrastSlider->setTickPosition(QSlider::TicksBothSides);
        ContrastSlider->setTickInterval(1);

        horizontalLayout->addWidget(ContrastSlider);

        SampleLabel = new QLabel(ContrastDialog);
        SampleLabel->setObjectName("SampleLabel");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(SampleLabel->sizePolicy().hasHeightForWidth());
        SampleLabel->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(SampleLabel);

        buttonBox = new QDialogButtonBox(ContrastDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(ContrastDialog);

        QMetaObject::connectSlotsByName(ContrastDialog);
    } // setupUi

    void retranslateUi(QDialog *ContrastDialog)
    {
        ContrastDialog->setWindowTitle(QCoreApplication::translate("ContrastDialog", "Contrast / background colour for mask", nullptr));
        label->setText(QCoreApplication::translate("ContrastDialog", "Contrast", nullptr));
        SampleLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ContrastDialog: public Ui_ContrastDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTRAST_H
