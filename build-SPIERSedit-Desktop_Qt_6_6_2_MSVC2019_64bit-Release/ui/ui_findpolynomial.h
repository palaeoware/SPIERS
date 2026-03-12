/********************************************************************************
** Form generated from reading UI file 'findpolynomial.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDPOLYNOMIAL_H
#define UI_FINDPOLYNOMIAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_findpolynomial
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QPushButton *pushButton;

    void setupUi(QDialog *findpolynomial)
    {
        if (findpolynomial->objectName().isEmpty())
            findpolynomial->setObjectName("findpolynomial");
        findpolynomial->resize(235, 164);
        vboxLayout = new QVBoxLayout(findpolynomial);
        vboxLayout->setObjectName("vboxLayout");
        label = new QLabel(findpolynomial);
        label->setObjectName("label");

        vboxLayout->addWidget(label);

        pushButton = new QPushButton(findpolynomial);
        pushButton->setObjectName("pushButton");

        vboxLayout->addWidget(pushButton);


        retranslateUi(findpolynomial);

        QMetaObject::connectSlotsByName(findpolynomial);
    } // setupUi

    void retranslateUi(QDialog *findpolynomial)
    {
        findpolynomial->setWindowTitle(QCoreApplication::translate("findpolynomial", "Find Polynomial", nullptr));
        label->setText(QString());
        pushButton->setText(QCoreApplication::translate("findpolynomial", "Stop Search", nullptr));
    } // retranslateUi

};

namespace Ui {
    class findpolynomial: public Ui_findpolynomial {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDPOLYNOMIAL_H
