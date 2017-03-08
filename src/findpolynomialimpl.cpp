#include "findpolynomialimpl.h"
#include "globals.h"
#include "Display.h"
#include "fileio.h"
#include <math.h>
#include <QDebug>
//

//these variables can be used for stashing /unstashing
double k; // the integer part of the thing
double RedConsts[10];
double GreenConsts[10];
double BlueConsts[10];

//These next are old code from the old DLL, minimally modified

//Next too are core of loop, need optimising
int findpolynomialImpl::MeasurePolyFitness()
{
	//measure fitness against the sample array
	int n;
	int f=0; //will be count of 'fit' results < or > 0 as appropriate
	double r;
	int max=Sample.count();
	Segment *seg=Segments[CurrentSegment];
	int inc=seg->PolySparse;

	for (n=0; n<max; n+=inc)
	{	
		SampleEntry *s= &(Sample[n]); //attempted optimisation
		r=CalcPoly(s->r,s->g, s->b,seg);

		//is segment array the one we are working with (+1 as 0-7 passed) and 
		if (s->seg==CurrentSegment) //correct segment
			{if (r>=0)
				f++;
			}
		else
			if (r<0) f++;
	}
	return f;
}

int rand255()
{
	// gives random number in short range (ignore the 255!)
	return (int) (qrand() * 2);
}

double findpolynomialImpl::PolyTweak(double val)
{
	//tweak value according to rules:
	// Get a %age for application

	int n;
	double perc;
	double rval;

	n=randn(4); //0,1,2, 3

	perc=0.01;

	switch (n)
	{
		case 0: 
			perc = 0.1; //10%
			break;
		case 1:
			perc = 0.5; //50%
			break;
		case 2:
			perc = 2; //200%
			break;
		case 3:
			perc = 10; //1000% - big change!
			break;
	}
	
	rval = (double)(rand255())/32768; //Should be +- 0-1
	
	return val + (val * perc * rval);
}

int  findpolynomialImpl::BreedPoly(int gens)
{

	Segment *seg=Segments[CurrentSegment];

	//test

	//Do 'gens' generations of selective breeding of the polynomial, returns a goodness of fit measure

	int n;
	int fits;
	int newfits;
        int ctweak=0, ctweak2=0;
        double otweak=0; //pret-tweak value

	if (Sample.count()==0) return -1;

	fits=MeasurePolyFitness(); //Get an initial measure

	for (n=0; n<gens; n++)
	{
		//choose a tweak
		ctweak=randn(4);

		switch (ctweak)
		{
		case 0:
            otweak=seg->PolyKall;
			seg->PolyKall=PolyTweak(seg->PolyKall);
			break;
		case 1:
			ctweak2 = randn(seg->PolyOrder);
            otweak=seg->PolyRedConsts[ctweak2];
			seg->PolyRedConsts[ctweak2]=PolyTweak(seg->PolyRedConsts[ctweak2]);
			break;
		case 2:
			ctweak2 = randn(seg->PolyOrder);
            otweak=seg->PolyGreenConsts[ctweak2];
			seg->PolyGreenConsts[ctweak2]=PolyTweak(seg->PolyGreenConsts[ctweak2]);
			break;
		case 3:
			ctweak2 = randn(seg->PolyOrder);
            otweak=seg->PolyBlueConsts[ctweak2];
			seg->PolyBlueConsts[ctweak2]=PolyTweak(seg->PolyBlueConsts[ctweak2]);
			break;
		}

		newfits = MeasurePolyFitness();

		if (newfits>=fits)
		{	//better or same - keep it
			fits=newfits;
		}
		else
		{	//worse or same - restore
			switch (ctweak)
			{
			case 0:
				seg->PolyKall=otweak;
				break;
			case 1:
				seg->PolyRedConsts[ctweak2]=otweak;
				break;
			case 2:
				seg->PolyGreenConsts[ctweak2]=otweak;
				break;
			case 3:
				seg->PolyBlueConsts[ctweak2]=otweak;
				break;
			}

		}
	}
	return fits;

}

double findpolynomialImpl::CalcScale()
{
	

	//work out scaling factor - find largest and / or smallest values from sample array
	int n;
	double maxval;
	double r;
	Segment *seg=Segments[CurrentSegment];

	maxval=0;

	for (n=0; n<Sample.count(); n+=seg->PolySparse)
	{
		r=CalcPoly(Sample[n].r,Sample[n].g,Sample[n].b, seg);
		if (r<0) r= 0-r; //do abs
		if (r>maxval) maxval=r;
	}
	//maxval is now biggest
	maxval /=128;

	return maxval;

}

//New QT stuff
findpolynomialImpl::findpolynomialImpl( QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

	label->setText("Status:");
	pleasestop=false;
	stopped=true;
}

void findpolynomialImpl::on_pushButton_pressed()
{
	pleasestop=true;
	if (stopped) close();
}

 void findpolynomialImpl::closeEvent(QCloseEvent *event)
 {
 	// in case close button hit
 	 pleasestop=true;
     if (!stopped) 
         event->ignore();
     else
         event->accept();
 }
 
void findpolynomialImpl::StashPoly()
{
	
	Segment *seg=Segments[CurrentSegment];
	k=seg->PolyKall; // the integer part of the thing
	for (int i=0; i<10; i++)
		RedConsts[i]=seg->PolyRedConsts[i];
	for (int i=0; i<10; i++)
		GreenConsts[i]=seg->PolyGreenConsts[i];
	for (int i=0; i<10; i++)
		BlueConsts[i]=seg->PolyBlueConsts[i];
}

void findpolynomialImpl::UnStashPoly()
{
	
	Segment *seg=Segments[CurrentSegment];
	seg->PolyKall=k; // the integer part of the thing
	for (int i=0; i<10; i++)
		seg->PolyRedConsts[i]=RedConsts[i];
	for (int i=0; i<10; i++)
		seg->PolyGreenConsts[i]=GreenConsts[i];
	for (int i=0; i<10; i++)
		seg->PolyBlueConsts[i]=BlueConsts[i];
}
 


//And a translated VB function
void findpolynomialImpl::Breed()
{
    int fit;
    QString LabelBaseString=label->text();
	QString MyOutString;
	QTextStream out(&MyOutString);

	Segment *seg = Segments[CurrentSegment];
	
    static int runsdone, lastfit, bestfit, unchangedcount, c;
    
	    
    //do more setup
    bestfit = 0;
    //cmdPolyStop.Enabled = True
    //cmdPolyBreed.Enabled = False
    runsdone = 0;
    
    //Do one 250 breed
    do 
    {
    
    	fit = BreedPoly(250);
        c +=250;
		MyOutString="";
        if (runsdone == 0)
            out<< "Run " << runsdone + 1 << " Gen " << c <<"\n Fitness: " << fit << " (" << (int)((double)(fit * 100) / (double)(Sample.count() / seg->PolySparse)) << "%)";
        else
            out<< "Run " <<runsdone + 1 <<" Gen " << c <<"\n Fitness: " << fit << " (" << (int)((double)(fit * 100) / (double)(Sample.count() / seg->PolySparse)) << "%)" << "\n" <<"  Best: " << bestfit << " (" << (int)((double)(bestfit * 100) / (double)(Sample.count() / seg->PolySparse)) << "%)";
		
		label->setText(LabelBaseString+MyOutString);
		qApp->processEvents(QEventLoop::AllEvents);
        
        if (pleasestop == true)
        {
            //stop hit - restore best if appropriate then getout
            if (runsdone == 0)
            {
            	bestfit = fit;
            	MyOutString="";
            	out<<"Polynomial fit: " << "\n"<< bestfit << " (" << (int)((double)(bestfit * 100) / (double)(Sample.count() / seg->PolySparse)) << "%)";
				label->setText(LabelBaseString+MyOutString);
				return;
            }
            else
            {
                //cmdPolyBreed.Enabled = True
                UnStashPoly();
            	MyOutString="";
                out<< "Polynomial fit: " << "\n"<< bestfit << " (" << (int)((double)(bestfit * 100) / (double)(Sample.count() / seg->PolySparse)) << "%)";
				label->setText(LabelBaseString+MyOutString);
				return;
            }
        }
        if (fit == lastfit) //no change - may be converged
         {
         	//qDebug()<<"maybe conv";
         	unchangedcount += 250;
         	
         	int v=1;
         	for (int i=0; i<seg->PolyConverge; i++) v*=2;
			v*=125;	

            if (unchangedcount >= v)
            {
            	//converged
                unchangedcount = 0;
                c = 0;
                if (fit > bestfit) //best yet
                {   
                	bestfit = fit;
                    StashPoly();
                }
                
                runsdone ++;
                if (runsdone == seg->PolyRetries)
                {   
                	//finished - reset controls, restore best net, get out
                    UnStashPoly();
	            	MyOutString="";
	                out<< "Polynomial fit: " << "\n"<< bestfit << " (" <<(int)((double)(bestfit * 100) / (double)(Sample.count() / seg->PolySparse)) << "%)";
					label->setText(LabelBaseString+MyOutString);
					return;
                }
                else
                {
                	//do
                    RandomizePoly(CurrentSegment, seg->PolyOrder);
                }
            }
        }
        else
        {
            unchangedcount = 0;
        }
        lastfit = fit;
    } while (true);	
}

double findpolynomialImpl::polyrand()
{
    double polyrand = pow(2.0,(((double)qrand()/(double)RAND_MAX * 16.0) - 8.0));
    if ((qrand()>(RAND_MAX/2))) return 0 - polyrand; else return polyrand;
}

void findpolynomialImpl::RandomizePoly(int s, int o)
{
    int n;
    
    //put into main array
    for (n = 0; n<o; n++)
    {
    	Segments[s]->PolyRedConsts[n] = polyrand();
    	Segments[s]->PolyGreenConsts[n] = polyrand();
    	Segments[s]->PolyBlueConsts[n] = polyrand();
    }
    Segments[s]->PolyOrder = o;
    Segments[s]->PolyKall = (qrand()/RAND_MAX)*200-100;
    Segments[s]->PolyScale = 10000000;
    
    //report
}

//The interface function
void findpolynomialImpl::find()
{
        int coount=0;
        for (int y=0; y<fheight; y++)
        for (int x=0; x<fwidth; x++)
        {
                int invertedpos=(fheight-1-y)*fwidth +x;
                if (Locks[invertedpos*2]) coount++;
        }

	QMutexLocker locker(&mutex);
	setModal(true);
	show();
	stopped=false;
	pleasestop=false;
	QString LabelString="Creating sample...";
	QTextStream out(&LabelString);
	
	label->setText(LabelString);
	qApp->processEvents(QEventLoop::AllEvents);
	
	//first step - set up the sample
	WriteAllData(CurrentFile);
	QTime t; t.start();  

	for (int i=0; i<FileCount; i++)
		if (mainwin->SliceSelectorList->item(i)->isSelected())
		{
			LoadAllData(i);
			uchar * colourdata=ColArray.bits();
			for (int y=0; y<fheight; y++)
                        for (int x=0; x<fwidth; x++)
			{
				int invertedpos=(fheight-1-y)*fwidth +x;
                                if (Locks[invertedpos*2])
				{
                                    //found a pixel
					if 	(MasksSettings[Masks[invertedpos]]->Show) //don't include hidden masks
					{	
						int r=0,g=0,b=0;
						
						if (GreyImage) //repeat same value 3 times - not ideal, but hardly going to be used for GS anyway!
						{
							for (int x2=x*ColMonoScale; x2<x*ColMonoScale+ColMonoScale; x2++)
							for (int y2=y*ColMonoScale; y2<y*ColMonoScale+ColMonoScale; y2++)
							{	
								r=colourdata[y2*cwidth4+x2];
							}
							r/=(ColMonoScale*ColMonoScale);
							Sample.append(SampleEntry((uchar)r,(uchar)r,(uchar)r,SegmentMap[y*fwidth+x]));
						}						
						else
						{
							for (int x2=x*ColMonoScale; x2<x*ColMonoScale+ColMonoScale; x2++)
							for (int y2=y*ColMonoScale; y2<y*ColMonoScale+ColMonoScale; y2++)
							{	
								r+=RED(colourdata,(y2*cwidth+x2)*4);
								g+=GREEN(colourdata,(y2*cwidth+x2)*4);
								b+=BLUE(colourdata,(y2*cwidth+x2)*4);
							}
							r/=(ColMonoScale*ColMonoScale);
							g/=(ColMonoScale*ColMonoScale);
							b/=(ColMonoScale*ColMonoScale);
                                                        Sample.append(SampleEntry((uchar)r,(uchar)g,(uchar)b,SegmentMap[y*fwidth+x]));
						}
					}
				}
			}
		}

	QList <int> segcounts;
	for (int i=0; i<SegmentCount; i++)
	{
		int count=0;
		for (int j=0; j<Sample.count(); j++)
			if (Sample[j].seg==i) count++;
		segcounts.append(count);
	}
		
	int noseg=Sample.count();
	if (noseg==0) {Message("Select some pixels using select/lock as a sample, select appropriate slices, then try again!"); return;}
	for (int i=0; i<SegmentCount; i++)
		noseg-=segcounts[i];
	out<<"done. Sample has "<<Sample.count()<<" pixels, consisting of:\n";
	out<<"No Segment: " << (noseg*100)/Sample.count()<<"%\n";
	for (int i=0; i<SegmentCount; i++)
	out<<Segments[i]->Name<<": "<<(segcounts[i]*100+50)/Sample.count()<<"%\n";
	
	label->setText(LabelString);
	qApp->processEvents(QEventLoop::AllEvents);

	LoadAllData(CurrentFile);
	Breed();
	pushButton->setText("Close");
	label->setText(label->text()+"\nPolynomial search complete");
	qApp->processEvents(QEventLoop::AllEvents);
	stopped=true;
	exec();
}

 
 SampleEntry::SampleEntry(uchar re, uchar gr,uchar bl, int se)
{
	seg=se; r=re; g=gr; b=bl;
}
