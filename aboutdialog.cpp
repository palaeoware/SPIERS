#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "../SPIERScommon/netmodule.h"
#include "SPIERSviewglobals.h"

aboutdialog::aboutdialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::aboutdialog)
{
    m_ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));
    QString html;
    html="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-\"";
    html+="html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style" ;
    html+="type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\"";
    html+="font-family:'Arial'; font-size:8.25pt; font-weight:400; font-style:normal;\">";
    QString v;
    v.sprintf("%3.2f",UPDATEVERSION);
    html+="<center><h2>SPIERSview " + v + "</h2></center>";
    html+="<p>This is freely distributed software - see licence.txt for details.</p>";
    html+="<p>Please contact m.sutton@ic.ac.uk with bug reports or suggestions.</p>";
    html+="<p>See www.spiers-software.org for more information.</p>";
    html+="</body></html>";
    m_ui->textBrowser->setHtml(html);

    if (voxml_mode) m_ui->HashLabel->setText("STL hash: " + STLHash);
}

aboutdialog::~aboutdialog()
{
    delete m_ui;
}

void aboutdialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
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


