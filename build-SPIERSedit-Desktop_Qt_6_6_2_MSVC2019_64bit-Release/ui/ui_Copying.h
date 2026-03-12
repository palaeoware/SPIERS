/********************************************************************************
** Form generated from reading UI file 'Copying.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COPYING_H
#define UI_COPYING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>

QT_BEGIN_NAMESPACE

class Ui_Copying
{
public:
    QHBoxLayout *horizontalLayout;
    QProgressBar *progressBar;
    QLabel *label;

    void setupUi(QDialog *Copying)
    {
        if (Copying->objectName().isEmpty())
            Copying->setObjectName("Copying");
        Copying->setWindowModality(Qt::ApplicationModal);
        Copying->resize(400, 40);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Copying->sizePolicy().hasHeightForWidth());
        Copying->setSizePolicy(sizePolicy);
        Copying->setMinimumSize(QSize(400, 40));
        Copying->setMaximumSize(QSize(400, 40));
        Copying->setModal(true);
        horizontalLayout = new QHBoxLayout(Copying);
        horizontalLayout->setObjectName("horizontalLayout");
        progressBar = new QProgressBar(Copying);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(24);

        horizontalLayout->addWidget(progressBar);

        label = new QLabel(Copying);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);


        retranslateUi(Copying);

        QMetaObject::connectSlotsByName(Copying);
    } // setupUi

    void retranslateUi(QDialog *Copying)
    {
        Copying->setWindowTitle(QCoreApplication::translate("Copying", "Copying files...", nullptr));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Copying: public Ui_Copying {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COPYING_H
