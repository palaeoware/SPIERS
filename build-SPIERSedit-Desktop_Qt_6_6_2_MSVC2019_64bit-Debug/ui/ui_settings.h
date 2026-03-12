/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *label_2;
    QSpinBox *SpinBoxUndo;
    QHBoxLayout *hboxLayout1;
    QLabel *label_3;
    QSpinBox *SpinBoxUndoTimer;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout2;
    QLabel *label;
    QSpinBox *SpinBoxCache;
    QCheckBox *BoxBackCache;
    QCheckBox *BoxRenderCache;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout3;
    QLabel *label_8;
    QSlider *SliderFileCompression;
    QLabel *LabelFileCompression;
    QVBoxLayout *vboxLayout1;
    QPushButton *RecompressSourceFiles;
    QPushButton *RecompressFiles;
    QHBoxLayout *hboxLayout4;
    QLabel *label_9;
    QSlider *SliderCacheCompression;
    QLabel *LabelCacheCompression;
    QFrame *frame;
    QHBoxLayout *hboxLayout5;
    QLabel *label_5;
    QSpinBox *AutoSave;
    QSpacerItem *spacerItem2;
    QPushButton *pushButton;
    QLabel *label_4;
    QSpacerItem *spacerItem3;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName("Settings");
        Settings->resize(249, 467);
        vboxLayout = new QVBoxLayout(Settings);
        vboxLayout->setObjectName("vboxLayout");
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName("hboxLayout");
        label_2 = new QLabel(Settings);
        label_2->setObjectName("label_2");

        hboxLayout->addWidget(label_2);

        SpinBoxUndo = new QSpinBox(Settings);
        SpinBoxUndo->setObjectName("SpinBoxUndo");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SpinBoxUndo->sizePolicy().hasHeightForWidth());
        SpinBoxUndo->setSizePolicy(sizePolicy);
        SpinBoxUndo->setMaximumSize(QSize(90, 16777215));
        SpinBoxUndo->setMaximum(100000000);
        SpinBoxUndo->setValue(512);

        hboxLayout->addWidget(SpinBoxUndo);


        vboxLayout->addLayout(hboxLayout);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName("hboxLayout1");
        label_3 = new QLabel(Settings);
        label_3->setObjectName("label_3");

        hboxLayout1->addWidget(label_3);

        SpinBoxUndoTimer = new QSpinBox(Settings);
        SpinBoxUndoTimer->setObjectName("SpinBoxUndoTimer");
        sizePolicy.setHeightForWidth(SpinBoxUndoTimer->sizePolicy().hasHeightForWidth());
        SpinBoxUndoTimer->setSizePolicy(sizePolicy);
        SpinBoxUndoTimer->setMaximumSize(QSize(90, 16777215));
        SpinBoxUndoTimer->setMinimum(1);
        SpinBoxUndoTimer->setMaximum(100);
        SpinBoxUndoTimer->setValue(4);

        hboxLayout1->addWidget(SpinBoxUndoTimer);


        vboxLayout->addLayout(hboxLayout1);

        spacerItem = new QSpacerItem(230, 16, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout->addItem(spacerItem);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setObjectName("hboxLayout2");
        label = new QLabel(Settings);
        label->setObjectName("label");

        hboxLayout2->addWidget(label);

        SpinBoxCache = new QSpinBox(Settings);
        SpinBoxCache->setObjectName("SpinBoxCache");
        sizePolicy.setHeightForWidth(SpinBoxCache->sizePolicy().hasHeightForWidth());
        SpinBoxCache->setSizePolicy(sizePolicy);
        SpinBoxCache->setMaximumSize(QSize(90, 16777215));
        SpinBoxCache->setMaximum(100000000);
        SpinBoxCache->setValue(512);

        hboxLayout2->addWidget(SpinBoxCache);


        vboxLayout->addLayout(hboxLayout2);

        BoxBackCache = new QCheckBox(Settings);
        BoxBackCache->setObjectName("BoxBackCache");
        BoxBackCache->setLayoutDirection(Qt::RightToLeft);

        vboxLayout->addWidget(BoxBackCache);

        BoxRenderCache = new QCheckBox(Settings);
        BoxRenderCache->setObjectName("BoxRenderCache");
        BoxRenderCache->setLayoutDirection(Qt::RightToLeft);

        vboxLayout->addWidget(BoxRenderCache);

        spacerItem1 = new QSpacerItem(230, 16, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout->addItem(spacerItem1);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setObjectName("hboxLayout3");
        label_8 = new QLabel(Settings);
        label_8->setObjectName("label_8");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy1);
        label_8->setMinimumSize(QSize(100, 0));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        hboxLayout3->addWidget(label_8);

        SliderFileCompression = new QSlider(Settings);
        SliderFileCompression->setObjectName("SliderFileCompression");
        SliderFileCompression->setMaximum(2);
        SliderFileCompression->setPageStep(1);
        SliderFileCompression->setOrientation(Qt::Horizontal);
        SliderFileCompression->setTickPosition(QSlider::TicksBothSides);
        SliderFileCompression->setTickInterval(1);

        hboxLayout3->addWidget(SliderFileCompression);

        LabelFileCompression = new QLabel(Settings);
        LabelFileCompression->setObjectName("LabelFileCompression");
        LabelFileCompression->setMinimumSize(QSize(40, 0));
        LabelFileCompression->setMaximumSize(QSize(40, 16777215));
        LabelFileCompression->setAlignment(Qt::AlignCenter);

        hboxLayout3->addWidget(LabelFileCompression);


        vboxLayout->addLayout(hboxLayout3);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName("vboxLayout1");
        RecompressSourceFiles = new QPushButton(Settings);
        RecompressSourceFiles->setObjectName("RecompressSourceFiles");

        vboxLayout1->addWidget(RecompressSourceFiles);

        RecompressFiles = new QPushButton(Settings);
        RecompressFiles->setObjectName("RecompressFiles");

        vboxLayout1->addWidget(RecompressFiles);


        vboxLayout->addLayout(vboxLayout1);

        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setObjectName("hboxLayout4");
        label_9 = new QLabel(Settings);
        label_9->setObjectName("label_9");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy2);
        label_9->setMinimumSize(QSize(100, 0));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        hboxLayout4->addWidget(label_9);

        SliderCacheCompression = new QSlider(Settings);
        SliderCacheCompression->setObjectName("SliderCacheCompression");
        SliderCacheCompression->setMaximum(2);
        SliderCacheCompression->setPageStep(1);
        SliderCacheCompression->setOrientation(Qt::Horizontal);
        SliderCacheCompression->setTickPosition(QSlider::TicksBothSides);
        SliderCacheCompression->setTickInterval(1);

        hboxLayout4->addWidget(SliderCacheCompression);

        LabelCacheCompression = new QLabel(Settings);
        LabelCacheCompression->setObjectName("LabelCacheCompression");
        LabelCacheCompression->setMinimumSize(QSize(40, 0));
        LabelCacheCompression->setAlignment(Qt::AlignCenter);

        hboxLayout4->addWidget(LabelCacheCompression);


        vboxLayout->addLayout(hboxLayout4);

        frame = new QFrame(Settings);
        frame->setObjectName("frame");
        frame->setMinimumSize(QSize(16, 24));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        hboxLayout5 = new QHBoxLayout(frame);
        hboxLayout5->setSpacing(0);
        hboxLayout5->setContentsMargins(0, 0, 0, 0);
        hboxLayout5->setObjectName("hboxLayout5");
        label_5 = new QLabel(frame);
        label_5->setObjectName("label_5");

        hboxLayout5->addWidget(label_5);

        AutoSave = new QSpinBox(frame);
        AutoSave->setObjectName("AutoSave");
        AutoSave->setMaximumSize(QSize(90, 16777215));
        AutoSave->setMinimum(1);
        AutoSave->setValue(5);

        hboxLayout5->addWidget(AutoSave);


        vboxLayout->addWidget(frame);

        spacerItem2 = new QSpacerItem(230, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout->addItem(spacerItem2);

        pushButton = new QPushButton(Settings);
        pushButton->setObjectName("pushButton");

        vboxLayout->addWidget(pushButton);

        label_4 = new QLabel(Settings);
        label_4->setObjectName("label_4");

        vboxLayout->addWidget(label_4);

        spacerItem3 = new QSpacerItem(230, 16, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout->addItem(spacerItem3);

        buttonBox = new QDialogButtonBox(Settings);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QDialog *Settings)
    {
        Settings->setWindowTitle(QCoreApplication::translate("Settings", "Global Settings", nullptr));
        label_2->setText(QCoreApplication::translate("Settings", "Maximum memory for undo", nullptr));
        SpinBoxUndo->setSuffix(QCoreApplication::translate("Settings", " Mb", nullptr));
        label_3->setText(QCoreApplication::translate("Settings", "Undo Timer Interval", nullptr));
        SpinBoxUndoTimer->setSuffix(QCoreApplication::translate("Settings", " seconds", nullptr));
        label->setText(QCoreApplication::translate("Settings", "Maximum memory for cache", nullptr));
        SpinBoxCache->setSuffix(QCoreApplication::translate("Settings", " Mb", nullptr));
        BoxBackCache->setText(QCoreApplication::translate("Settings", "Background caching", nullptr));
        BoxRenderCache->setText(QCoreApplication::translate("Settings", "Tune Cache for Rendering", nullptr));
        label_8->setText(QCoreApplication::translate("Settings", "File Compression", nullptr));
        LabelFileCompression->setText(QCoreApplication::translate("Settings", "Off", nullptr));
        RecompressSourceFiles->setText(QCoreApplication::translate("Settings", "(Re)compress source files now", nullptr));
        RecompressFiles->setText(QCoreApplication::translate("Settings", "(Re)compress working files now", nullptr));
        label_9->setText(QCoreApplication::translate("Settings", "Cache Compression:", nullptr));
        LabelCacheCompression->setText(QCoreApplication::translate("Settings", "Off", nullptr));
        label_5->setText(QCoreApplication::translate("Settings", "Autosave Frequency", nullptr));
        AutoSave->setSuffix(QCoreApplication::translate("Settings", " minutes", nullptr));
        pushButton->setText(QCoreApplication::translate("Settings", "Clear Recent Files List", nullptr));
        label_4->setText(QCoreApplication::translate("Settings", "Changes will take effect on opening a dataset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
