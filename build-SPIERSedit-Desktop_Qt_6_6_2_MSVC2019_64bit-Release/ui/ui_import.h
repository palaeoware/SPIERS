/********************************************************************************
** Form generated from reading UI file 'import.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMPORT_H
#define UI_IMPORT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ImportDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLabel *label_3;
    QTextEdit *textEdit;
    QLabel *LabelDownsample;
    QSpinBox *spinBox;
    QSpinBox *spinBoxZ;
    QCheckBox *CheckMirrored;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ImportDialog)
    {
        if (ImportDialog->objectName().isEmpty())
            ImportDialog->setObjectName("ImportDialog");
        ImportDialog->setWindowModality(Qt::WindowModal);
        ImportDialog->resize(434, 192);
        ImportDialog->setModal(true);
        gridLayout = new QGridLayout(ImportDialog);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(ImportDialog);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit = new QLineEdit(ImportDialog);
        lineEdit->setObjectName("lineEdit");

        gridLayout->addWidget(lineEdit, 0, 1, 1, 4);

        label_2 = new QLabel(ImportDialog);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 5, 1, 1);

        label_3 = new QLabel(ImportDialog);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        textEdit = new QTextEdit(ImportDialog);
        textEdit->setObjectName("textEdit");
        textEdit->setTabChangesFocus(true);
        textEdit->setAcceptRichText(false);

        gridLayout->addWidget(textEdit, 1, 1, 1, 5);

        LabelDownsample = new QLabel(ImportDialog);
        LabelDownsample->setObjectName("LabelDownsample");

        gridLayout->addWidget(LabelDownsample, 2, 0, 1, 2);

        spinBox = new QSpinBox(ImportDialog);
        spinBox->setObjectName("spinBox");
        spinBox->setMinimum(1);
        spinBox->setMaximum(16);
        spinBox->setValue(1);

        gridLayout->addWidget(spinBox, 2, 2, 1, 1);

        spinBoxZ = new QSpinBox(ImportDialog);
        spinBoxZ->setObjectName("spinBoxZ");
        spinBoxZ->setMinimum(1);
        spinBoxZ->setMaximum(50);
        spinBoxZ->setValue(1);

        gridLayout->addWidget(spinBoxZ, 2, 3, 1, 1);

        CheckMirrored = new QCheckBox(ImportDialog);
        CheckMirrored->setObjectName("CheckMirrored");
        CheckMirrored->setLayoutDirection(Qt::RightToLeft);

        gridLayout->addWidget(CheckMirrored, 2, 4, 1, 2);

        buttonBox = new QDialogButtonBox(ImportDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 3, 1, 1, 5);

        QWidget::setTabOrder(lineEdit, textEdit);
        QWidget::setTabOrder(textEdit, buttonBox);

        retranslateUi(ImportDialog);

        QMetaObject::connectSlotsByName(ImportDialog);
    } // setupUi

    void retranslateUi(QDialog *ImportDialog)
    {
        ImportDialog->setWindowTitle(QCoreApplication::translate("ImportDialog", "File Name and Description", nullptr));
        label->setText(QCoreApplication::translate("ImportDialog", "File Name:", nullptr));
        label_2->setText(QCoreApplication::translate("ImportDialog", ".spe", nullptr));
        label_3->setText(QCoreApplication::translate("ImportDialog", "Description", nullptr));
        LabelDownsample->setText(QCoreApplication::translate("ImportDialog", "Downsamples:", nullptr));
        spinBox->setPrefix(QCoreApplication::translate("ImportDialog", "XY: ", nullptr));
        spinBoxZ->setPrefix(QCoreApplication::translate("ImportDialog", "Z: ", nullptr));
        CheckMirrored->setText(QCoreApplication::translate("ImportDialog", "Data is Front-to-Back ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImportDialog: public Ui_ImportDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMPORT_H
