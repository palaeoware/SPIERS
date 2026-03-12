/********************************************************************************
** Form generated from reading UI file 'deletemask.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DELETEMASK_H
#define UI_DELETEMASK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DeleteMaskDialog
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QLabel *label_2;
    QListWidget *ListTargetMasks;
    QProgressBar *progressBar;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DeleteMaskDialog)
    {
        if (DeleteMaskDialog->objectName().isEmpty())
            DeleteMaskDialog->setObjectName("DeleteMaskDialog");
        DeleteMaskDialog->resize(394, 326);
        vboxLayout = new QVBoxLayout(DeleteMaskDialog);
        vboxLayout->setObjectName("vboxLayout");
        label = new QLabel(DeleteMaskDialog);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(7);
        label->setFont(font);
        label->setWordWrap(true);

        vboxLayout->addWidget(label);

        label_2 = new QLabel(DeleteMaskDialog);
        label_2->setObjectName("label_2");

        vboxLayout->addWidget(label_2);

        ListTargetMasks = new QListWidget(DeleteMaskDialog);
        ListTargetMasks->setObjectName("ListTargetMasks");

        vboxLayout->addWidget(ListTargetMasks);

        progressBar = new QProgressBar(DeleteMaskDialog);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);

        vboxLayout->addWidget(progressBar);

        buttonBox = new QDialogButtonBox(DeleteMaskDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(DeleteMaskDialog);

        QMetaObject::connectSlotsByName(DeleteMaskDialog);
    } // setupUi

    void retranslateUi(QDialog *DeleteMaskDialog)
    {
        DeleteMaskDialog->setWindowTitle(QCoreApplication::translate("DeleteMaskDialog", "Delete a Mask", nullptr));
        label->setText(QCoreApplication::translate("DeleteMaskDialog", "SPIERS requires that all pixels in all images are assigned to a mask, hence you MUST specify a target mask - all pixels assigned to the deleted mask will be reassigned to the target mask", nullptr));
        label_2->setText(QCoreApplication::translate("DeleteMaskDialog", "Target Mask:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeleteMaskDialog: public Ui_DeleteMaskDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DELETEMASK_H
