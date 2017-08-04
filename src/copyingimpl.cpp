/*********************************************

SPIERSedit 2: copyingimpl.cpp
A simple progress bar pop up - 
Originally just for file copying on import
Now used in several other circumstances too

**********************************************/

#include "globals.h"
#include "copyingimpl.h"
#include "fileio.h"
#include "Display.h"
#include "output.h"
#include "curves.h"
#include "brush.h"

#include <QProcess>
#include <QMutexLocker>
#include <QVarLengthArray>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QEventLoop>
#include <QCloseEvent>
#include <QtGlobal> 
#include <QListWidget>
#include <QFileDialog>
//#include <zlib.h>


CopyingImpl::CopyingImpl(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));
	CountMessage="";
	copying=false;

}

 void CopyingImpl::closeEvent(QCloseEvent *event)
 {
 	// in case close button hit
     if (copying) 
         event->ignore();
     else
         event->accept();
 } 
 
 
void CopyingImpl::Copy(QDir source,QDir dest)
{
	//this does file copying
	QStringList FilesToCopy=source.entryList();
	QString n1, n2,s1,s2;
	
	show();
	this->setWindowTitle("Copying Files...");
	
	copying=true;
	n2=dest.canonicalPath(); n1=source.canonicalPath();
        //int lastsep=n1.lastIndexOf("\\");  //this is last separator in path
	
	int n=0; //work out and set max on prog bar
	progressBar->setMaximum(FilesToCopy.count());
	
	int counter=0; //for curves
	Errors=0;
	foreach (QString str, FilesToCopy)
	{

		if (str.left(6)!="curves")
		{
			QFile::copy(n1+"/"+str,n2+"/"+str);
		}
		else //curve file - read into new data structure
		{
			QFile f(n1+"/"+str);
			if (f.size()==408518)
			{

				f.open(QIODevice::ReadOnly);
				QDataStream in(&f);    // Set up the datastream object
				in.setByteOrder(QDataStream::LittleEndian);  //Windows is LE, so will keep ALL our datafiles LE
				double dummy;
				in >> dummy; //should be -100000 - don't bother to check!
				//now read splinex (0 to 254, 0 to 99)
				//temp arrays first
				double SplineX[255][100];
				double SplineY[255][100];
				int SplineCount[255];
				

				for (int j=0; j<100; j++)
				for (int i=0; i<255; i++)
				{
					in>>SplineX[i][j];
				}
				
				for (int j=0; j<100; j++)
				for (int i=0; i<255; i++)
				{
					in>>SplineY[i][j];
				}
				
				for (int i=0; i<255; i++)
					SplineCount[i]=GetShort(&in);
				

				//OK, now we can try filling

				for (int i=0; i<CurveCount; i++)
				{


					Curves[i]->SplinePoints[counter]->Count=SplineCount[i];

					for (int j=0; j<SplineCount[i]; j++)
					{					
						Curves[i]->SplinePoints[counter]->X.append(SplineX[i][j]);
						Curves[i]->SplinePoints[counter]->Y.append(SplineY[i][j]);
					}
				}
				counter++;

			}
			else
			{

				//Curve files the wrong size - need to just blank all the curves
				for (int i=0; i<CurveCount; i++)
				{
                                    if (counter<Curves[i]->SplinePoints.count())
                                    {
					Curves[i]->SplinePoints[counter]->Count=0;
					Curves[i]->SplinePoints[counter]->X.clear();
					Curves[i]->SplinePoints[counter]->Y.clear();
                                     }
				}
				counter++;
			}
		}
		

		progressBar->setValue(n++);
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 

	}

	if (Errors!=0)
	{
		if (QMessageBox::question(this, "Import Curves", "Errors encountered with curve import - should I reset curves to avoid crashes?", 
			QMessageBox::Yes | QMessageBox::No)
		==QMessageBox::Yes)
		//clear all curves
		{
			qDeleteAll(Curves.begin(), Curves.end());Curves.clear();
		}
	}
	copying=false;
	hide();

}

void CopyingImpl::MakeNewSegFiles(int snum)
//create new files - snum is the new segment index (add 1 for filename index)
{
	show();
	this->setWindowTitle("Creating Files...");	
	copying=true;
	progressBar->setMaximum(Files.count());

	int lastsep, lastdot;

	QImage tempimage(fwidth, fheight,QImage::Format_Indexed8);
	
	QVector <QRgb> ctable;
	ctable.resize(256);
	for (int i=0; i<256; i++)
		ctable[i]=qRgb(i,i,i);
	
	tempimage.setColorTable(ctable);	
	tempimage.setColor(0,1);

	tempimage.fill(0);
	
	for (int i=0; i<Files.count(); i++)
	{
		QString Fname = Files.at(i);
		lastsep=qMax(Fname.lastIndexOf("\\"),Fname.lastIndexOf("/"));  //this is last separator in path
		lastdot=Fname.lastIndexOf(".");
		QString sfname=Fname.left(lastsep);
		QString actfn=Fname.mid(lastsep+1, lastdot-lastsep-1);
		QString temp = "/" + SettingsFileName + "/" + "s";
		QString t2; t2.sprintf("%d_",snum+1); 
		temp.append(t2); temp.append(actfn); temp.append(".bmp");
		sfname.append(temp);
		
		tempimage.save(sfname);
		progressBar->setValue(i);
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
	}
	copying=false;
	hide();
	return;	
}

void CopyingImpl::DeleteSegments(QList <int> list)
{
	//1. Assemble a before and after list
	//2. Rename all the files
	//3. delete Segment items
	//4. delete extra GAs
	show();
	copying=true;
	this->setWindowTitle("Deleting Segment Files...");	

	//First - work out a mask conversion table.
	QList <int> ToSegs;
	int ToSegPos=0;
	
	//get conversion table 
	for (int i=0; i<SegmentCount; i++)
	{
		if (list.indexOf(i)<0) //this is NOT one of the deleting ones
			 ToSegs.append(ToSegPos++);
		else
			ToSegs.append(-1);//means delete
	}
	
	
	//OK, should now have correct conversion list - now handle file changes
	progressBar->setMaximum(FullFiles.count()+1);

	for (int i=0; i<FullFiles.count(); i++)
	{
		for (int seg=0; seg<SegmentCount; seg++)
		{	//this is inneficient but won't matter				
			if (ToSegs[seg]!=seg)
			{
				int lastsep, lastdot;
				QString Fname = FullFiles.at(i);
				lastsep=qMax(Fname.lastIndexOf("\\"),Fname.lastIndexOf("/"));  //this is last separator in path
				lastdot=Fname.lastIndexOf(".");
				QString sfname=Fname.left(lastsep);
				QString actfn=Fname.mid(lastsep+1, lastdot-lastsep-1);
				QString temp = "/" + SettingsFileName + "/" + "s";
				QString t2; t2.sprintf("%d_",seg+1); 
				temp.append(t2); temp.append(actfn); 
				
				//temp.append(".bmp");
				sfname.append(temp); //sfname is now from name WITHOUT extension
				
				bool png=false;
				QFile oldfile(sfname+".bmp");
				if (!(oldfile.exists())) {oldfile.setFileName(sfname+".png"); png=true;}
				
				if ((oldfile.exists())) //if it doesn't exist then can't delete or copy - just ignore (will be for z-downsampling reasons I hope)
				{		 
					if (ToSegs[seg]==-1)
					{
						oldfile.remove(); //delete	
					}
					else
					{
						//rename
						int lastsep, lastdot;
						QString Fname = FullFiles.at(i);
						lastsep=qMax(Fname.lastIndexOf("\\"),Fname.lastIndexOf("/"));  //this is last separator in path
						lastdot=Fname.lastIndexOf(".");
						QString sfname2=Fname.left(lastsep);
						QString actfn=Fname.mid(lastsep+1, lastdot-lastsep-1);
						QString temp = "/" + SettingsFileName + "/" + "s";
						QString t2; t2.sprintf("%d_",ToSegs[seg]+1); 
						temp.append(t2); temp.append(actfn); // temp.append(".bmp");
						sfname2.append(temp); //sfname2 is now to name
						if (png) sfname2.append(".png"); else sfname2.append(".bmp");
						if (!oldfile.rename(sfname2)) Error("Failed to rename segment file from "+sfname+" to "+sfname2);			
					}				
				}
			}
			progressBar->setValue(i);
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
		}
	}

	//sort out the curve segment selection
	for (int i=0; i<CurveCount; i++)
	{
		int oldseg=Curves[i]->Segment; 
		if (oldseg>0) 
		{
			if (ToSegs[oldseg-1]==-1) Curves[i]->Segment=0; else Curves[i]->Segment=ToSegs[oldseg-1]+1;
		}
	}
	
	if (CurrentSegment>=0) if (ToSegs[CurrentSegment]==-1) CurrentSegment=0;
	if (CurrentRSegment>=0) if (ToSegs[CurrentRSegment]==-1) CurrentRSegment=0;
	//Now hack apart the actual masksdetails list - move pointers as above
	
	//delete all the dead ones
	for (int i=0; i<list.count(); i++) delete Segments[list[i]];

	//shuffle list back
	for (int i=0; i<SegmentCount; i++)
	{
		if (list.indexOf(i)<0) //this is NOT one of the deleting ones
			 Segments[ToSegs[i]]=Segments[i];
	}

	SegmentCount-=list.count();
	for (int i=0; i<list.count(); i++) {Segments.removeLast(); GA.removeLast();}
	
	//sort out output objects
	for (int i=0; i<OutputObjectsCount; i++)
		for (int j=0; j<OutputObjects[i]->ComponentSegments.count(); j++)
			OutputObjects[i]->ComponentSegments[j]=ToSegs[OutputObjects[i]->ComponentSegments[j]];

	close();
}

void CopyingImpl::GenerateLinear(QListWidget *SliceSelectorList)
{
	int c=SliceSelectorList->selectedItems().count();
	if (c>1) show();
	copying=true;
	this->setWindowTitle("Generating linear segment files...");
	WriteAllData(CurrentFile);
	if (c>1)  progressBar->setMaximum(c);
	for (int i=0; i<Files.count(); i++)
        {

		if ((SliceSelectorList->item(i))->isSelected()) MakeLinearGreyScale(CurrentSegment,i, false);
		if (c>1) progressBar->setValue(i);
		if (c>1) qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
	}
	LoadAllData(CurrentFile);
	copying=false;
	if (c>1) close();
}


void CopyingImpl::GeneratePoly(QListWidget *SliceSelectorList)
{
	int c=SliceSelectorList->selectedItems().count();
	if (c>1) show();
	copying=true;
	this->setWindowTitle("Generating polynomial segment files...");
	WriteAllData(CurrentFile);
	if (c>1)  progressBar->setMaximum(c);
	for (int i=0; i<Files.count(); i++)
	{
		if ((SliceSelectorList->item(i))->isSelected()) MakePolyGreyScale(CurrentSegment,i, false);
		if (c>1) progressBar->setValue(i);
		if (c>1) qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
	}
	LoadAllData(CurrentFile);
	copying=false;
	if (c>1) close();
}

void CopyingImpl::GenerateRange(QListWidget *SliceSelectorList)
{
	int c=SliceSelectorList->selectedItems().count();
	if (c>1) show();
	copying=true;
	this->setWindowTitle("Generating range segment files...");
	WriteAllData(CurrentFile);
	if (c>1)  progressBar->setMaximum(c);
	for (int i=0; i<Files.count(); i++)
	{
		if (RangeSelectedOnly) if ((SliceSelectorList->item(i))->isSelected()) MakeRangeGreyScale(CurrentSegment,i, false);
		if (!RangeSelectedOnly) if ((SliceSelectorList->item(i))->isSelected()) 
			for (int j=0; j<SegmentCount; j++) if (Segments[j]->Activated) MakeRangeGreyScale(j,i, false);

		if (c>1) progressBar->setValue(i);
		if (c>1) qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
	}
	LoadAllData(CurrentFile);
	copying=false;
	if (c>1) close();
}



void CopyingImpl::GenerateAllLinear()
{

	QMutexLocker locker(&mutex);	
	show();
	this->setWindowTitle("Creating initial segment files...");	
	copying=true;
	//WriteAllData(CurrentFile);
	progressBar->setMaximum(Files.count());
	for (int i=0; i<Files.count(); i++)
	{
		MakeLinearGreyScale(CurrentSegment,i, false);
		progressBar->setValue(i);
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
	}
	
	LoadAllData(CurrentFile);
	copying=false;
	close();
}

void CopyingImpl::GenerateAllBlank()
{

	QMutexLocker locker(&mutex);	
	show();
	this->setWindowTitle("Creating initial segment files...");	
	copying=true;
	//WriteAllData(CurrentFile);
	progressBar->setMaximum(Files.count());
	for (int i=0; i<Files.count(); i++)
	{
		MakeBlankGreyScale(CurrentSegment,i, false);
		progressBar->setValue(i);
		qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
	}
	
        LoadAllData(CurrentFile);
	copying=false;
	close();
}


void CopyingImpl::ReverseStretches(QList <double> *stretches, int Sstart, int Sstop)
{
    int n;

    QVector <double> thicks;
    thicks.resize(Sstop+1);
    //work out thicknesses
    for (n=Sstart + 2; n<=Sstop; n++)
    	{thicks[n]=(*stretches)[n] - (*stretches)[n - 1];}
    
    //now redo stretches
    for (n=Sstart + 2; n<=Sstop; n++)
    	{(*stretches)[n] = (*stretches)[n - 1] + thicks[Sstop - (n - 2)];}
}

bool CopyingImpl::DoIHaveChildren(int parent)
//Yes, I do!
{
	for (int i=0; i<OutputObjectsCount; i++)
		if (OutputObjects[i]->IsGroup==false && OutputObjects[i]->Parent==parent && OutputObjects[i]->Show) return true;
		else {if (OutputObjects[i]->IsGroup && OutputObjects[i]->Parent==parent && OutputObjects[i]->Show) if (DoIHaveChildren(i)) return true;}
	return false;
}

//OLD VERSION (QByteArray QList)
/*
void CopyingImpl::WriteSPVData(QList <QByteArray *> fullarray, QVector<double>*TrigArray, int TrigCount, QDataStream *out)
//output data to the file
{				
	
	qApp->processEvents(QEventLoop::ExcludeUserInputEvents); 
    
    //do a count of on pixels in fullarray

    (*out)<<fullarray.count(); //commence with number of fullarrays to decompress - though should know this already
    for (int i=0; i<fullarray.count(); i++)
   	{
		(*out)<<fullarray[i]->size()-4;			
		//Strip off 4 byte header and write
		out->writeRawData((fullarray[i]->constData())+4,fullarray[i]->size()-4);
	}

	//and output any triangles
    (*out)<<TrigCount;
    for (int z=0; z<TrigCount; z++) 
    	(*out) <<(*TrigArray)[z];
}
*/


void CopyingImpl::MaskCopy(int fromfile, MainWindowImpl *mw)
{
	bool IsShow=false;
	int count=0;
	if (mw->SliceSelectorList->selectedItems().count()>1) IsShow=true;
	QList <QTreeWidgetItem *> selitems = mw->MasksTreeWidget->selectedItems();

	if (selitems.count()==0) Message("No masks selected to copy");
	else
	{
		if (IsShow) 
		{	
			show();
			this->setWindowTitle("Copying Masks...");	
			copying=true;
			progressBar->setMaximum(mw->SliceSelectorList->selectedItems().count());
		}
		SaveMasks(CurrentFile);
		LoadMasks(fromfile);
		QByteArray FromMasks=Masks;
		int fsize=Masks.size();
		for (int k=0; k<Files.count(); k++)
		{
			if (mw->SliceSelectorList->item(k)->isSelected() && k!=fromfile)
			{	
				if (IsShow) {progressBar->setValue(++count); qApp->processEvents(QEventLoop::ExcludeUserInputEvents);} 
	
				bool dflag=false;
				LoadMasks(k); //get mask info in
				for (int i=0; i<selitems.count(); i++)
				for (int j=0;  j<=MaxUsedMask; j++)
				{
					if ((MasksSettings[j]->widgetitem)==selitems[i]) //found a selected mask
					{
						//OK, copy stored mask data to this mask array
						for (int n=0; n<fsize; n++)
                            if ((int)((quint8)FromMasks[n])==j && MasksSettings[(quint8)Masks[n]]->Lock==false) {Masks[n]=(uchar)j; dflag=true;}
					}
				}
				if (dflag) SaveMasks(k); 			
			}
		}
		if (IsShow) {copying=false; hide();}
		LoadMasks(CurrentFile);
		ShowImage(mw->graphicsView);
	}
}

void CopyingImpl::MaskCopy2(int fromfile, MainWindowImpl *mw) //this is copy from selected TO currentfile
{
	QList <QTreeWidgetItem *> selitems = mw->MasksTreeWidget->selectedItems();
	if (selitems.count()==0) Message("No masks selected to copy");
	else
	{
		SaveMasks(CurrentFile);
		QByteArray ToMasks=Masks; //store current masks
		int fsize=Masks.size();
	
		LoadMasks(fromfile); //get mask info in
		for (int i=0; i<selitems.count(); i++)
		for (int j=0;  j<=MaxUsedMask; j++)
		{
			if ((MasksSettings[j]->widgetitem)==selitems[i]) //found a selected mask
			{
				//OK, copy stored mask data to this mask array
				for (int n=0; n<fsize; n++)
                    if ((int)((quint8)Masks[n])==j && MasksSettings[(quint8)ToMasks[n]]->Lock==false) ToMasks[n]=(uchar)j;
			}
		}
		Masks=ToMasks;
		SaveMasks(CurrentFile);
		ShowImage(mw->graphicsView);
	}
}

void CopyingImpl::CurvesToMasks(MainWindowImpl *mw) //create a mask from a curve
{
	QList <QTreeWidgetItem *> selitems = mw->CurvesTreeWidget->selectedItems();
	if (selitems.count()==0) {Message("No curves selected!"); return;}
	bool IsShow=false;
	int count=0;
	QString slicestring;
	if (mw->SliceSelectorList->selectedItems().count()>1) { slicestring="all selected slices"; IsShow=true;} else slicestring="the selected slice";
	if (QMessageBox::question(this, "Mask from Curve", "This will add all pixels in the selected curves to the mask '" 
	+ MasksSettings[SelectedMask]->Name + "' for " + slicestring + ". Are you sure?", QMessageBox::Ok | QMessageBox::Cancel)
		==QMessageBox::Ok)
	{
		
		    QList <bool> UseMasks; for (int i=0; i<=MaxUsedMask; i++) UseMasks.append(true);  //include all masks

			if (IsShow) 
			{	
				show();
				this->setWindowTitle("Modifying Masks...");	
				copying=true;
				progressBar->setMaximum(mw->SliceSelectorList->selectedItems().count());
			}

			SaveMasks(CurrentFile);
			for (int i=0; i<FileCount; i++)
			if (mw->SliceSelectorList->item(i)->isSelected())
			{
				if (IsShow) {progressBar->setValue(++count); qApp->processEvents(QEventLoop::ExcludeUserInputEvents);} 

				//draw all the curves
				QByteArray array(fwidth*fheight,0); //initialise

	        	for (int sn=0; sn<CurveCount; sn++) 
	        	{
	        		if (Curves[sn]->widgetitem->isSelected())
                                        DrawCurveOutput(sn, i, (uchar *) array.data(), &UseMasks, false);
	        	}

				LoadMasks(i);
				for (int j=0; j<fheight; j++)
				for (int k=0; k<fwidth; k++)
                    if (array[(fheight-1-j)*fwidth+k]) if (MasksSettings[(quint8)Masks[j*fwidth+k]]->Lock==false) Masks[j*fwidth+k]=SelectedMask;
				SaveMasks(i); 
			}
			LoadMasks(CurrentFile);
			if (IsShow) {copying=false;hide();}
			ShowImage(mw->graphicsView);
	}
}


void CopyingImpl::CompressAllWorkingFiles(int level)
{
	int before=FileCompressionLevel;
	
	WriteAllData(CurrentFile);
	FileCompressionLevel=level;

	show();
	this->setWindowTitle("Recompressing Files...");	
	copying=true;
	progressBar->setMaximum(FileCount);

	for (int i=0; i<FileCount; i++)
	{
                //qDebug()<<"About to load "<<FullFiles[i];
		for (int n=0; n<SegmentCount; n++) LoadGreyData(i,n); 
		for (int n=0; n<SegmentCount; n++) SaveGreyData(i,n);
		LoadMasks(i); SaveMasks(i);
		LoadLocks(i); SaveLocks(i);
		
		progressBar->setValue(i); qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
	}
	FileCompressionLevel=before;

	LoadAllData(CurrentFile);
	copying=false;
}

void CopyingImpl::CompressAllSourceFiles(int level)
{
	show();
	this->setWindowTitle("Recompressing Files...");	
	copying=true;
	progressBar->setMaximum(FullFiles.count());
	QImage image; 

	//do GS check
	
	bool GSflag=false;
	image.load(FullFiles[0]);
	if (image.allGray()	&& image.format()!=QImage::Format_Indexed8)
		if (QMessageBox::question(this, "Convert to Greyscale?", "The first image at least appears to be a greyscale image saved as colour.\nDo you want to convert all images to greyscale? This slow the conversion process, but will save space and increase performance.\nClick 'Yes' if you want to convert all images to greyscale, or 'No' to proceed as normal.\n"
			, QMessageBox::Yes | QMessageBox::No)
		==QMessageBox::Yes) GSflag=true;

	QVector <QRgb> clut(256);
	
	for (int i=0; i<256; i++) 
		clut[i]=qRgb(i,i,i);

	for (int i=0; i<FullFiles.count(); i++)
	{
		image.load(FullFiles[i]);

		int lastdot;
		QString Fname = FullFiles.at(i);
		lastdot=Fname.lastIndexOf(".");
		QString sfname=Fname.left(lastdot);
		if (level==0) sfname+=".bmp";
		if (level==1) sfname+=".png";
		if (level==2) sfname+=".jpg";
		

		int w=image.width(); int h=image.height();
		int w4;
		if ((w % 4)==0) w4=w; // no problem with width
		else w4=(w/4)*4+4;	

		//convert to GS if necessary
		if (GSflag)
		{
			//Converting RGB to grey
			QImage imageGS(w, h, QImage::Format_Indexed8);
			imageGS.setColorTable(clut);
			uchar *data1=(uchar *) image.bits();
			uchar *data2=(uchar *) imageGS.bits();
			for (int x=0; x<w; x++)
			for (int y=0; y<h; y++)
				data2[y*w4+x]=RED(data1,4*(y*w+x));
	
			if (level==0) imageGS.save(sfname,"BMP",0);
			if (level==1) imageGS.save(sfname,"PNG",50);
			if (level==2) imageGS.save(sfname,"JPG",100);
//			image=image.convertToFormat(QImage::Format_Indexed8);
//			if (!(image.isGrayscale())) qDebug()<<"Ooops, conversion somehow didn't produce a greyscale";
		}
		else
		{
			//Just write image back in new format
			if (level==0) image.save(sfname,"BMP",0);
			if (level==1) image.save(sfname,"PNG",50);
			
			if (level==2) {image.save(sfname,"JPG",100);}
		}
		//remove old one if they are different
		if 	(FullFiles[i].toLower()!=sfname.toLower()) 
		{	
			QFile f(FullFiles[i]);
			f.remove();
		}
		FullFiles[i]=sfname;
				
		progressBar->setValue(i); qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
	}
 
   	if (zsparsity>1)
   	{
                //int count=0;
   		Files.clear();
   		for (int i=0; i<FullFiles.count(); i+=zsparsity)
   		{
   		 	Files.append(FullFiles[i]);
  		}
  		FileCount=Files.count();
  	}
  	else Files=FullFiles;

	if (GSflag) GreyImage=true;
	if (GSflag) ClearCache();
	copying=false;
	WriteSettings();	
}

void CopyingImpl::Apply3DBrush(int button)
{
	show();
	this->setWindowTitle("Applying 3D brush...");	
	copying=true;
	int count=0;
	//work out first and last files.
	//bodge for now - assume count is radius * 2 
	progressBar->setMaximum(Brush_Size*2);
	double BaseOffset=Stretches[CurrentFile];
	int wfile=CurrentFile;

	bool forwards=true;
	bool done=false;	
	do
	{
                //FilesDirty[wfile]=true;

        Brush.resize(Brush_Size, 2, qAbs(Stretches[wfile]-BaseOffset));
	
		//do the brushing
		switch (CurrentMode)
		{
			case 0: //brighten
				if (button==1) Brush.brighten(LastMouseX, LastMouseY, CurrentSegment, BrightUp);
				else Brush.brighten(LastMouseX, LastMouseY, CurrentSegment, 0-BrightDown);
				break;
			case 1: //masks
				if (button==1) Brush.mask(LastMouseX, LastMouseY, SelectedMask);
				else Brush.mask(LastMouseX, LastMouseY, SelectedRMask);
				break;
			case 3: //lock
				if (button==1) Brush.lock(LastMouseX, LastMouseY, 255);
				else Brush.lock(LastMouseX, LastMouseY,  0);
				break;
			case 4:
				if (button==1) Brush.segment(LastMouseX, LastMouseY, 255);
				else Brush.segment(LastMouseX, LastMouseY,  0);
				break;
			case 5:
				if (tabwidget->currentIndex()<2 || RangeSelectedOnly)			
					Brush.recalc(LastMouseX, LastMouseY, CurrentSegment);
				else
					//all segs
					for (int i=0; i<SegmentCount; i++) if (Segments[i]->Activated) Brush.recalc(LastMouseX, LastMouseY, i);
				break;
		}
		progressBar->setValue(count++); qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
		WriteAllData(wfile);
		//attempt to load next file in sequence
                if (Brush.PixelCount==0) if (forwards) {forwards=false; wfile=CurrentFile-1; goto past;} else {done=true;goto past;} //switch direction - or stop!
			
		if (forwards) wfile++; else wfile--;
		if (wfile>=Files.count() || wfile<0) 
		{
			if (forwards) {forwards=false; wfile=CurrentFile-1;} else done=true;	
		}
                past:
		if (done==false) //should be able to load next file
				LoadAllData(wfile);
	} while (done==false);

	copying=false;
	LoadAllData(CurrentFile);
	Brush.resize(Brush_Size, 2, 0);	
	
}
