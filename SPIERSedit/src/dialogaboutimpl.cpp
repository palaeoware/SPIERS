#include "dialogaboutimpl.h"
#include "mainwindowimpl.h"
#include "globals.h"

#include "../../SPIERScommon/src/netmodule.h"
#include <QString>

/**
 * @brief DialogAboutImpl::DialogAboutImpl
 * @param parent
 * @param f
 */
DialogAboutImpl::DialogAboutImpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowTitle("About");
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    header->setVisible(false);

    textLabel_1->setText(AppMainWindow->windowTitle());
    textLabel_1->setObjectName("aboutTextLabel1");
    textLabel_1->setAlignment(Qt::AlignCenter);

    textLabel_2->setWordWrap(true);
    textLabel_2->setText("This  software is " + QString(PRODUCTNAME) +
                         ". It was coded by"
                         " Mark Sutton (m.sutton@imperial.ac.uk). With additional code by Alan R.T. Spencer (alan.spencer@imperial.ac.uk) and Russell Garwood (russell.garwood@gmail.com)"
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

    QPixmap picture_2(":/license/gplV3Logo");
    footer1->setPixmap(picture_2);
    footer1->setAlignment(Qt::AlignCenter);

    QPixmap picture_3(":/license/builtWithQTLogo");
    footer2->setPixmap(picture_3);
    footer2->setAlignment(Qt::AlignCenter);

    buttonBox->setStandardButtons(QDialogButtonBox::Close);
}

/**
 * @brief DialogAboutImpl::returnLicense
 * @return
 */
QString DialogAboutImpl::returnLicense()
{
    QFile file(":/license/license");

    if (file.open(QIODevice::ReadOnly))
    {
        QString license = file.readAll();
        file.close();
        return license;
    }
    else return "License Missing!";
}
