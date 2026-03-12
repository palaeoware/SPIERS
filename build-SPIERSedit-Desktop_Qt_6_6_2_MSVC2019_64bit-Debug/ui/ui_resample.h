/********************************************************************************
** Form generated from reading UI file 'resample.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESAMPLE_H
#define UI_RESAMPLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_resample
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label2;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label;
    QVBoxLayout *verticalLayout;
    QSpinBox *SpinBoxZ;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *CheckBoxInterpolate;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *CheckBoxInterpolateCurves;
    QSpacerItem *horizontalSpacer_4;
    QSpinBox *spinBox;
    QProgressBar *progressBar;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *resample)
    {
        if (resample->objectName().isEmpty())
            resample->setObjectName("resample");
        resample->resize(277, 207);
        verticalLayout_3 = new QVBoxLayout(resample);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label2 = new QLabel(resample);
        label2->setObjectName("label2");

        verticalLayout_2->addWidget(label2);

        label_2 = new QLabel(resample);
        label_2->setObjectName("label_2");

        verticalLayout_2->addWidget(label_2);

        label_3 = new QLabel(resample);
        label_3->setObjectName("label_3");

        verticalLayout_2->addWidget(label_3);

        label = new QLabel(resample);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        SpinBoxZ = new QSpinBox(resample);
        SpinBoxZ->setObjectName("SpinBoxZ");
        SpinBoxZ->setMinimum(1);
        SpinBoxZ->setMaximum(50);

        verticalLayout->addWidget(SpinBoxZ);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        CheckBoxInterpolate = new QCheckBox(resample);
        CheckBoxInterpolate->setObjectName("CheckBoxInterpolate");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CheckBoxInterpolate->sizePolicy().hasHeightForWidth());
        CheckBoxInterpolate->setSizePolicy(sizePolicy);
        CheckBoxInterpolate->setMaximumSize(QSize(16, 16));
        CheckBoxInterpolate->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout->addWidget(CheckBoxInterpolate);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        CheckBoxInterpolateCurves = new QCheckBox(resample);
        CheckBoxInterpolateCurves->setObjectName("CheckBoxInterpolateCurves");
        CheckBoxInterpolateCurves->setMaximumSize(QSize(16, 16));
        CheckBoxInterpolateCurves->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_2->addWidget(CheckBoxInterpolateCurves);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_2);

        spinBox = new QSpinBox(resample);
        spinBox->setObjectName("spinBox");
        spinBox->setMinimum(1);
        spinBox->setMaximum(20);

        verticalLayout->addWidget(spinBox);


        horizontalLayout_3->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout_3);

        progressBar = new QProgressBar(resample);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);

        verticalLayout_3->addWidget(progressBar);

        buttonBox = new QDialogButtonBox(resample);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_3->addWidget(buttonBox);


        retranslateUi(resample);

        QMetaObject::connectSlotsByName(resample);
    } // setupUi

    void retranslateUi(QDialog *resample)
    {
        resample->setWindowTitle(QCoreApplication::translate("resample", "Change dataset downsampling", nullptr));
        label2->setText(QCoreApplication::translate("resample", "New Z sparsity (currently ", nullptr));
        label_2->setText(QCoreApplication::translate("resample", "Interpolate Lock/Select and Mask data", nullptr));
        label_3->setText(QCoreApplication::translate("resample", "Interpolate Curves data", nullptr));
        label->setText(QCoreApplication::translate("resample", "New XY downsample factor: (currently ", nullptr));
        CheckBoxInterpolate->setText(QString());
        CheckBoxInterpolateCurves->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class resample: public Ui_resample {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESAMPLE_H
