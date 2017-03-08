#include "dialogaboutimpl.h"
#include "../../SPIERScommon/netmodule.h"
//
DialogAboutImpl::DialogAboutImpl(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));
	QString html;
	html="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-\"";
	html+="html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style" ;
	html+="type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\"";
        html+="font-family:'Arial'; font-size:8.25pt; font-weight:400; font-style:normal;\">";
        QString v;
        v.sprintf("%3.2f",UPDATEVERSION);
        html+="<center><h2>SPIERSedit " + v + "</h2></center>";
        html+="<p>This is freely distributed software - see licence.txt for details.</p>";
        html+="<p>Please contact m.sutton@ic.ac.uk with bug reports or suggestions.</p>";
        html+="<p>See www.spiers-software.org for more information.</p>";
        html+="</body></html>";
	textBrowser->setHtml(html);
}


void DialogAboutImpl::on_pushButton_clicked()
{
	close();
}

