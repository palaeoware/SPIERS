#include "dialogaboutimpl.h"
#include "mainwindowimpl.h"
#include "globals.h"

#include "../../SPIERScommon/netmodule.h"
#include <QString>

DialogAboutImpl::DialogAboutImpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowTitle("About");
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    QPixmap picture_1(":/img.png");
    header->setPixmap(picture_1);
    header->setAlignment(Qt::AlignCenter);

    textLabel_1->setText(AppMainWindow->windowTitle());
    textLabel_1->setObjectName("aboutTextLabel1");
    textLabel_1->setAlignment(Qt::AlignCenter);

    textLabel_2->setWordWrap(true);
    textLabel_2->setText("This  software is " + QString(PRODUCTNAME) +
                         ". It was coded by"
                         " Mark Sutton (m.sutton@imperial.ac.uk). It uses a GUI theme created/implemented by Alan R.T. Spencer (alan.spencer@imperial.ac.uk) and Russell Garwood (russell.garwood@gmail.com)"
                         "<br><br>Reports are appreciated, and comments, suggestions, and feature requests are welcome.");
    textLabel_2->setAlignment(Qt::AlignCenter);


    textLabel_3->setWordWrap(true);
    textLabel_3->setText("<b>Copyright and License:</b>"
                         "<br><br>" + QString(COPYRIGHT) +
                         "<br><br>" + QString(LICENCE) + " (see below).");
    textLabel_3->setAlignment(Qt::AlignCenter);

    textBrowser->setHtml(returnLicense());
    textBrowser->setOpenLinks(true);
    textBrowser->setOpenExternalLinks(true);

    QPixmap picture_2(":/resources/gplv3-127x51.png");
    footer1->setPixmap(picture_2);
    footer1->setAlignment(Qt::AlignCenter);

    QPixmap picture_3(":/resources/Built_with_Qt_RGB_logo.png");
    footer2->setPixmap(picture_3);
    footer2->setAlignment(Qt::AlignCenter);

    buttonBox->setStandardButtons(QDialogButtonBox::Close);
}

QString DialogAboutImpl::returnLicense()
{
    QFile file(":/license/license");

    if(file.open(QIODevice::ReadOnly))
    {
        QString license = file.readAll();
        file.close();
        return license;
    }
    else return "License Missing!";
}
