#include "exportdxf.h"
//
ExportDXF::ExportDXF( MainWindowImpl *mwp, QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

	mw=mwp;
}


//

