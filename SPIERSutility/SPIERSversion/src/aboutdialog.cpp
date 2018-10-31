#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "globals.h"
#include "mainwindow.h"

aboutdialog::aboutdialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::aboutdialog)
{
    m_ui->setupUi(this);

    setWindowTitle("About");
    setWindowIcon(QIcon(":/ViewIcon.bmp"));

    QPixmap picture_1(":/img.png");
    m_ui->header->setPixmap(picture_1);
    m_ui->header->setAlignment(Qt::AlignCenter);

    m_ui->textLabel_1->setText(QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION));
    m_ui->textLabel_1->setObjectName("aboutTextLabel1");
    m_ui->textLabel_1->setAlignment(Qt::AlignCenter);

    m_ui->textLabel_2->setWordWrap(true);
    m_ui->textLabel_2->setText("This  software is " + QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) +
                               ". It was coded by"
                               " Mark Sutton (m.sutton@imperial.ac.uk). With additional code by Alan R.T. Spencer (alan.spencer@imperial.ac.uk) and Russell Garwood (russell.garwood@gmail.com)."
                               "<br><br>Reports are appreciated, and comments, suggestions, and feature requests are welcome.");
    m_ui->textLabel_2->setAlignment(Qt::AlignCenter);

    m_ui->textLabel_3->setWordWrap(true);
    m_ui->textLabel_3->setText("<b>Copyright and License:</b>"
                               "<br><br>" + QString(COPYRIGHT) +
                               "<br><br>" + QString(LICENCE) + " (see below).");
    m_ui->textLabel_3->setAlignment(Qt::AlignCenter);

    m_ui->textBrowser->setHtml(returnLicense());
    m_ui->textBrowser->setOpenLinks(true);
    m_ui->textBrowser->setOpenExternalLinks(true);

    QPixmap picture_2(":/resources/gplv3-127x51.png");
    m_ui->footer1->setPixmap(picture_2);
    m_ui->footer1->setAlignment(Qt::AlignCenter);

    QPixmap picture_3(":/resources/Built_with_Qt_RGB_logo.png");
    m_ui->footer2->setPixmap(picture_3);
    m_ui->footer2->setAlignment(Qt::AlignCenter);

    m_ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
}

aboutdialog::~aboutdialog()
{
    delete m_ui;
}

void aboutdialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void aboutdialog::on_closebutton_clicked()
{
    close();
}

QString aboutdialog::returnLicense()
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

