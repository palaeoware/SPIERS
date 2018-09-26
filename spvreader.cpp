#include "spvreader.h"
#include <stdio.h>
#include <string.h>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QTime>

#include "SPIERSviewglobals.h"
#include <QDebug>
#include <QMatrix4x4>

//RJG update case for ubuntu
#include "spv.h"
#include "mc.h"
#include "zlib.h"


//RJG update case for ubuntu
#include "svobject.h"
#include "compressedslice.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vtkProperty2D.h"
#include "voxml.h"

SPVreader::SPVreader()
{
    //Constructor does nothing
    ReplaceIndex=-1;
}

int SPVreader::ProcessFileReplacement(QString filename, int SPVindex)
{
    FileName=filename;

    ReplaceIndex=SPVindex;
    InternalProcessFile(filename);
    MainWin->EnableRenderCommands();
    return ReplaceIndex; //may well be an error code -2 = multipart, -3 = too old
}

void SPVreader::TransposeMatrix(float *m)
{
      QMatrix4x4 thematrix(m);
      thematrix=thematrix.transposed();
      thematrix.copyDataTo(m);
}

void SPVreader::ProcessFile(QString filename)
{
    //The old processfile stuff. This is the only external API function to the class

    //set the class-level globals
    FileName=filename;
    //qDebug()<<FileName;
    //FileName="mm";

    //call internal version with messier interface
    InternalProcessFile(filename);

    MainWin->EnableRenderCommands();
}

void SPVreader::FixUpData()
//Bodge to fix up any problems with key codes
{
    //Fix any odd characters in keys
    for (int i=0; i<svobjects.count(); i++)
    {
        int key=(int)(svobjects[i]->Key.toLatin1());
        if (key>=97 && key<=122) svobjects[i]->Key=(QChar(key-64)); //to Upper
        if (key<48 || key>90) svobjects[i]->Key=0;
        if (key>=58 && key<=64) svobjects[i]->Key=0;
    }
}

void SPVreader::WriteFile(bool withpd)
{
    WriteSPV(withpd);
}

void WriteFinalised()
{



    QFile OutputFile(fname);
    if (OutputFile.open(QIODevice::WriteOnly)==false)
    {
        QMessageBox::warning(MainWin,"File Error","Can't open SPV file for writing - is it set to read-only?");
        return;
    }

    voxml v;
    if (v.write_voxml(fname,true)==false) return;

    //Add the STLs

        //2. Write VAXML in text mode
        //3. Write compressed STLs
}

void SPVreader::WriteSPV(bool withpd)
{
    //Write new version of SPV out
//    qDebug()<<"File is"<<fname;
    QFile OutputFile(fname);
    if (OutputFile.open(QIODevice::WriteOnly)==false)
    {
        QMessageBox::warning(MainWin,"File Error","Can't open SPV file for writing - is it set to read-only?");
        return;
    }

    QDataStream out(&OutputFile);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_4_4);

    out<<(double)-1; //my initial 'not a v1 bodge
    out<<(int)VERSION;

    int c=0;
    out<<SPVs.count();
    for (int i=0; i<SPVs.count(); i++) c+=SPVs[i]->ComponentObjects.count();
    out<<c; //NEW - total count of objects (for progress bar)
    out<<MainWin->ui->actionSave_Memory->isChecked();

    //some global settings... maybe just matrix for now
    for (int i=0; i<SPVs.count(); i++)  //NEW - do each SPV
    {
        SPV *s=SPVs[i];
        out<<s->filenamenopath; //NEW
        out<<s->ComponentObjects.count();
        out<<s->PixPerMM;
        out<<s->SlicePerMM;
        out<<s->SkewDown/s->PixPerMM;;
        out<<s->SkewLeft/s->SlicePerMM;
        out<<s->iDim;
        out<<s->jDim;
        out<<s->kDim;
        out<<s->MirrorFlag;
//        qDebug()<<"SPV "<<i<<"Mirror flag is "<<s->MirrorFlag;
        for (int i=0; i<s->kDim+1; i++)
            out<<s->stretches[i];

        //NEW - removed matrix for SPV
        //Now the component object details
        for (int i=0; i<s->ComponentObjects.count(); i++)
        {
            svobject *o = s->ComponentObjects[i];
            out<<o->Name;
            out<<o->Key;
            out<<o->Colour[0];
            out<<o->Colour[1];
            out<<o->Colour[2];
            if (o->Transparency<0) out<<(int)0; else out<<o->Transparency; //bodge for backward compatibility
            out<<o->IsGroup;
            out<<o->InGroup;
            out<<o->Position;
            out<<o->Visible;
            out<<o->Resample;
            if (o->IslandRemoval<0) out<<(int)0; else out<<o->IslandRemoval; //bodge for backward compatibility
            if (o->Smoothing<0) out<<(int)0; else out<<o->Smoothing; //bodge for backward compatibility

            if (o->BuggedData) out<<o->ResampleType-10000; else out<<o->ResampleType;

            //transpose it to write
            TransposeMatrix(o->matrix);
            for (int i=0; i<16; i++)
                out<<o->matrix[i];
            //and Transpose it back so we can keep using it
            TransposeMatrix(o->matrix);
//            qDebug()<<"Object"<<i<<o->Name<<"Matrix is: ";
//            for (int i=0; i<16; i++)
//                qDebug()<<i<<":"<<o->matrix[i];

            out<<o->Index;
            out<<o->scale;
        }

        //Now the data
        for (int i=0; i<s->ComponentObjects.count(); i++)
        {
            svobject *o = s->ComponentObjects[i];
            if (!(o->IsGroup))
            {
                if (o->AllSlicesCompressed) //Not done by slice
                {
                    out<<-3;
                    out<<o->AllSlicesSize;
                    OutputFile.write((char *)o->AllSlicesCompressed,o->AllSlicesSize);
                }
                else
                {
                    out<<(s->kDim-2); //this is the number of pieces thing - always kdim -2 on a writeback
                    //we know how many slice there will be - it's Kdim

                    for (int j=0; j<s->kDim-2; j++) //allow for two end caps
                    {
                        if (o->compressedslices.count()==0) //<v5, no comp slices
                            out<<(int)-1;
                        else
                        {
                            CompressedSlice *sl = o->compressedslices[j+1];

                            if (sl->empty)
                            {
                                //no data, just write a -1
                                out<<(int)-1;
                            }
                            else
                            {
                                out<<(int)0; //code for there is a grid and then data
                                //write the grid
                                out.writeRawData((char *)sl->grid,s->GridSize);
                                //now write the compressed data
                                out<<sl->datasize;
                                out.writeRawData((char *)sl->data,sl->datasize);
                            }
                        }

                    }
                }
                //and now optionally write the polydata object too
                out<<withpd;
                if (withpd) o->WritePD(&OutputFile);
            }
         }

    }

    //Now the orphan objects - which can only be groups
    QList <svobject *> AllObs;
    AllObs=svobjects;
    for (int i=0; i<SPVs.count(); i++)
    for (int j=0; j<SPVs[i]->ComponentObjects.count(); j++)
    {
        int p=AllObs.indexOf(SPVs[i]->ComponentObjects[j]);
        if (p==-1) exit(20);
        AllObs.removeAt(p);
    }

    //AllObs should now just list orphan groups
    out<<AllObs.count();
    for (int i=0; i<AllObs.count(); i++)
    {
            svobject *o = AllObs[i];
            out<<o->Name;
            out<<o->Key;
            out<<o->IsGroup;
            out<<o->InGroup;
            out<<o->Position;
            out<<o->Visible;
            out<<o->Index;
    }
    out<<QString("InfoLists");
    out<<i_comments<< i_reference<< i_author<< i_specimen<< i_provenance<< i_classification_name<< i_classification_rank<< i_title;
    //finally tag on the info lists. No need for a new version for this

    //Now scale stuff and a few interface things
    out<< ScaleBallColour[0];
    out<< ScaleBallColour[1];
    out<< ScaleBallColour[2];
    out<< ScaleBallScale; //resize applied to ball - for scale calc
    for (int i=0; i<16; i++) out<<ScaleMatrix[i];

    bool tempbool;
    tempbool=MainWin->ui->actionQuadric_Fidelity_Reduction->isChecked();
    out<< tempbool;
    tempbool=MainWin->ui->actionShow_Ball_2->isChecked();
    out<< tempbool;

    //background colour
    out<<back_red<<back_green<<back_blue;

    //grid colour
    out<<grid_red<<grid_green<<grid_blue;
    out<<grid_minor_red<<grid_minor_green<<grid_minor_blue;

    //This tagged on the end to keep some sort of file compatibility
    for (int i=0; i<SPVs.count(); i++)  //NEW - do each SPV
    {
        SPV *s=SPVs[i];
        for (int i=0; i<s->ComponentObjects.count(); i++)
        {
            svobject *o = s->ComponentObjects[i];
            out<<o->Shininess;
            out<<o->Transparency; //these might now be <0
            out<<o->IslandRemoval; //these might now be <0
            out<<o->Smoothing; //these might now be <0
        }
    }
    MainWin->ui->statusBar->showMessage("Save Complete");
}

void SPVreader::ReadSPV6(QString Filename)
{
    int BaseIndex;
    //actually reads 6 and up
    double dummy;
    //Read in new version of SPV out
    QFile InputFile(Filename);
    if (InputFile.open(QIODevice::ReadOnly)==false)
    {
        QMessageBox::warning(MainWin,"File Error","Fatal - Can't open SPV file for reading");
        exit(0);
    }


    BaseIndex=-1;
    for (int i=0; i<svobjects.count(); i++) //non 0 for imports
        if (svobjects[i]->Index>BaseIndex) BaseIndex=svobjects[i]->Index;

    BaseIndex++; //will now be first free index number

    QDataStream in(&InputFile);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_4);

    //temp variables - we don't create SPV quite yet
    int version;
    int objectcount;
    double PixPerMM, SlicePerMM, SkewDown, SkewLeft;
    int iDim, jDim, kDim;

    in>>dummy; //will be -1
    in>>version;

    int spvcount=1;
    int totalobjcount;
    if (version>6) //single SPV version
    {
        in>>spvcount;
        if (spvcount>1 && ReplaceIndex>=0) {ReplaceIndex=-2; return;} //error - can't replace one piece with a multipiece!
        in>>totalobjcount;
        bool t;
        in>>t;
        MainWin->ui->actionSave_Memory->setChecked(t);
    }

    MainWin->setSpecificLabel("Reading file...");
    MainWin->setSpecificProgress(0);
    qApp->processEvents();

    for (int spv_i=0; spv_i<spvcount; spv_i++) //for each spv
    {

        QString filenamenopath;
        if (version>6) in>>filenamenopath; //NEW
        in>>objectcount;
        if (version==6) totalobjcount=objectcount;

        in>>PixPerMM;
        in>>SlicePerMM;
        in>>SkewDown;
        in>>SkewLeft;
        in>>iDim;
        in>>jDim;
        in>>kDim;

        //Now make the SPV
         SPV *thisspv=new SPV(SPVs.count(),version,iDim, jDim,kDim);
        SPVs.append(thisspv);
        thisspv->filename=Filename;
        thisspv->filenamenopath=filenamenopath;

        //work out proper scale - from FIRST Spv (maybe this one, maybe not)


        //store these in the SPV object
        thisspv->PixPerMM=PixPerMM;
        thisspv->SlicePerMM=SlicePerMM;
        thisspv->SkewDown=-SkewDown*PixPerMM;
        thisspv->SkewLeft=-SkewLeft*PixPerMM;
        mm_per_unit = ((float)(SPVs[0]->iDim)/(float)SCALE)/SPVs[0]->PixPerMM;

        //create and append all the svobjects
        for (int i=0; i<objectcount; i++)
        {
          svobject *newobj=new svobject(svobjects.count());
          newobj->spv = thisspv; //pointer in object to SPV
          thisspv->ComponentObjects.append(newobj); //pointer in SPV to object
          svobjects.append(newobj); //put it in my general list
          newobj->Index=svobjects.count()-1;
        }

        in>>thisspv->MirrorFlag;
//        qDebug()<<"Just mirror"<<thisspv->MirrorFlag;

        float Mat[16];
        if (version==6) //matrix now
        {
            for (int i=0; i<16; i++) in>>Mat[i];
            for (int i=0; i<objectcount; i++)
            {
            for (int j=0; j<16;j++)
                thisspv->ComponentObjects[i]->matrix[j]=Mat[j];
              //NEW BODGE - transpose this matrix
            TransposeMatrix(thisspv->ComponentObjects[i]->matrix);
              for (int j=0; j<16; j++) thisspv->ComponentObjects[i]->defaultmatrix[j]=thisspv->ComponentObjects[i]->matrix[j];
              thisspv->ComponentObjects[i]->gotdefaultmatrix=true;

            }
        }

        thisspv->stretches=(double *)malloc((kDim+1) * sizeof(double));
        for (int i=0; i<thisspv->kDim+1; i++)
            in>>thisspv->stretches[i];

        //Now the component object details
        for (int i=0; i<objectcount; i++)
        {
            svobject *o = thisspv->ComponentObjects[i];
            in>>o->Name;
            in>>o->Key;
            in>>o->Colour[0];
            in>>o->Colour[1];
            in>>o->Colour[2];
            in>>o->Transparency;
            in>>o->IsGroup;
            in>>o->InGroup; //this may cause trouble...
                if (o->InGroup>=0) o->InGroup+=BaseIndex; //offset for import
            in>>o->Position;
                o->Position+=BaseIndex;
            in>>o->Visible;
            in>>o->Resample;
            in>>o->IslandRemoval;
            in>>o->Smoothing;
            in>>o->ResampleType;

            //And a loveley bodge - if this is negative it means data is old (pre v4).
            //Merged in here to avoid a new file version
            if (o->ResampleType<0) {o->ResampleType=10000+o->ResampleType; o->BuggedData=true;}

            if (version>=7) for (int i=0; i<16; i++) in>>o->matrix[i];
            TransposeMatrix(o->matrix);
            for (int j=0; j<16; j++) thisspv->ComponentObjects[i]->defaultmatrix[j]=thisspv->ComponentObjects[i]->matrix[j];
            thisspv->ComponentObjects[i]->gotdefaultmatrix=true;

            if (version>=7) {in>>o->Index; o->Index+=BaseIndex;}
            if (version>=7) {in>>o->scale;}
        }


        if (ReplaceIndex>=0) //we are replacing something
        {
            for (int i=0; i<objectcount; i++)
            {
                svobject *o = thisspv->ComponentObjects[i];
                //Find matching part in old SPV
                int match=-1;
                for (int j=0; j<SPVs[ReplaceIndex]->ComponentObjects.count(); j++)
                {
                    if (SPVs[ReplaceIndex]->ComponentObjects[j]->Name==o->Name) match=j;
                }
                if (match>=0)
                {
                   //put it into the correct group
                   o->InGroup=SPVs[ReplaceIndex]->ComponentObjects[match]->InGroup;
                   o->Transparency=SPVs[ReplaceIndex]->ComponentObjects[match]->Transparency;
                   o->Position=SPVs[ReplaceIndex]->ComponentObjects[match]->Position;
                   o->Visible=SPVs[ReplaceIndex]->ComponentObjects[match]->Visible;
                   o->Resample=SPVs[ReplaceIndex]->ComponentObjects[match]->Resample;
                   o->Smoothing=SPVs[ReplaceIndex]->ComponentObjects[match]->Smoothing;
                   o->IslandRemoval=SPVs[ReplaceIndex]->ComponentObjects[match]->IslandRemoval;

                   for (int j=0; j<16; j++) o->matrix[j]=SPVs[ReplaceIndex]->ComponentObjects[match]->matrix[j];
                }
                else //take matrix from first object and cross fingers
                {
                   for (int j=0; j<16; j++) o->matrix[j]=SPVs[ReplaceIndex]->ComponentObjects[0]->matrix[j];
                }
            }
        }

        //Now the data
        QTime t;
        t.start();
        for (int i=0; i<objectcount; i++)
        {
            svobject *o = thisspv->ComponentObjects[i];

            MainWin->setSpecificLabel("Preprocessing Data");
            qApp->processEvents();
            if (!(o->IsGroup))
            {

                int pieces;

                in>>pieces; //this is the number of pieces thing - always kdim -2 on a writeback
                             //but for a merged object it could be more on first read from edit-generated SPV

                if (pieces==-3) //Code for 'single compressed thing
                {
                    in>>o->AllSlicesSize;
                    o->AllSlicesCompressed=(unsigned char *)malloc(o->AllSlicesSize);
                    InputFile.read((char *)o->AllSlicesCompressed,o->AllSlicesSize);
                }
                else
                {

                    CompressedSlice *BlankSlice =new CompressedSlice(o,true);
                    (o->compressedslices).append(BlankSlice);
                    int ThisSlice=1;
                    //qDebug()<<"pieces"<<pieces;
                    for (int j=0; j<pieces; j++)
                    {

                        MainWin->setSpecificProgress((j*100)/pieces);
                        if (j%10==0) qApp->processEvents();


                        if (j % (thisspv->kDim-2) == 0) ThisSlice= 1;// reached a restart from merge

                        int EmptyFlag;
                        in>>EmptyFlag;
                        if (EmptyFlag==-1) //empty
                        {
                            if (j<(thisspv->kDim-2)) //no merge complications
                            {
                                CompressedSlice *blankslice=new CompressedSlice(o,true);
                                o->compressedslices.append(blankslice);
                            }
                            //do nothing if it's a merge job
                            ThisSlice++;

                        }
                        else //not empty
                        {
                            //read the grid
                            t.start();
                            CompressedSlice *newslice=new CompressedSlice(o,false);
                            newslice->grid=(unsigned char *)malloc(thisspv->GridSize);
                            int temp;
                            temp=in.readRawData((char *)(newslice->grid),thisspv->GridSize);

                            in>>newslice->datasize;

                            newslice->data =(unsigned char *)malloc(newslice->datasize);

                            temp=in.readRawData((char *)newslice->data,newslice->datasize);
                            //qDebug()<<"1."<<t.elapsed();
                            if (j<(thisspv->kDim-2)) //no merge complications
                            {
                                  (o->compressedslices).append(newslice);
                                  //qDebug()<<"h1";
                            }
                            else
                            {
                                  o->compressedslices[ThisSlice]->Merge(newslice,"c:/test.bmp");
                                  delete newslice;
                                  //qDebug()<<"h2";
                            }
                            ThisSlice++;
                            //qDebug()<<"2."<<t.elapsed();
                        }
                    }

                     BlankSlice =new CompressedSlice(o,true);
                    (o->compressedslices).append(BlankSlice);
                }
                bool withpd;
                in>>withpd;
               if (withpd)
                {
                   ContainsPresurfaced=true;
                   o->ReadPD(&InputFile);
                   o->Dirty=true;
                   o->SurfaceMe=false;

               }
                else
               {
                    ContainsNonPresurfaced=true;
                    o->SurfaceMe=true;

                }
            }
            //qDebug()<<"3."<<t.elapsed();
        }


    }

    //Now the orphan objects - which can only be groups
    if (version>=8)
    {
        int newobs;
        in>>newobs;
        for (int i=0; i<newobs; i++)
        {

            svobject *o = new svobject(0);
            in>>o->Name;
            in>>o->Key;
            in>>o->IsGroup;
            in>>o->InGroup;
            in>>o->Position;
            in>>o->Visible;
            in>>o->Index;
            svobjects.append(o);
        }
    }

    if (!(in.atEnd()))
    {
        QString check;
        in>>check;
        if (check!="InfoLists")
            qDebug()<<"Error!";
        else
        in>>i_comments>> i_reference>> i_author>> i_specimen>> i_provenance>> i_classification_name>> i_classification_rank>> i_title;
    }

    //Now scale stuff and a few interface things
    if (!(in.atEnd()))
    {
        in >> ScaleBallColour[0];
        in >> ScaleBallColour[1];
        in >> ScaleBallColour[2];
        in >> ScaleBallScale; //resize applied to ball - for scale calc
        for (int i=0; i<16; i++) in>>ScaleMatrix[i];

        bool tempbool;
        in >> tempbool;
        MainWin->ui->actionQuadric_Fidelity_Reduction->setChecked(tempbool);
        in >> tempbool;
        MainWin->ui->actionShow_Ball_2->setChecked(tempbool);
    }


    if (!(in.atEnd()))
    {
        in >> back_red;
        in >> back_green;
        in >> back_blue;
    }

    if (!(in.atEnd()))
    {
        in >> grid_red;
        in >> grid_green;
        in >> grid_blue;
        in >> grid_minor_red;
        in >> grid_minor_green;
        in >> grid_minor_blue;
    }

    if (!(in.atEnd()))
    for (int i=0; i<SPVs.count(); i++)  //NEW - do each SPV
    {
        SPV *s=SPVs[i];
        for (int i=0; i<s->ComponentObjects.count(); i++)
        {
            svobject *o = s->ComponentObjects[i];
            in>>o->Shininess;
            in>>o->Transparency; //these might now be <0
            in>>o->IslandRemoval; //these might now be <0
            in>>o->Smoothing; //these might now be <0
        }
    }

    FixUpData();
    MainWin->RefreshObjects();
    MainWin->RefreshInfo();


    int items=0;
    for (int z=BaseIndex; z<svobjects.count(); z++) if (!(svobjects[z]->IsGroup)) items++;

    int icount=1;
    //Now do all the processing



    for (int i=BaseIndex; i<svobjects.count(); i++)
    {
       if (!(svobjects[i]->IsGroup))
       {
            QString status;
            QTextStream ts(&status);
            ts<<"Processing '"<<svobjects[i]->Name<<"', "<<icount<<" of "<<items;
            MainWin->ui->OutputLabelOverall->setText(status);
            MainWin->ui->ProgBarOverall->setValue((icount++*100)/items);
           if (svobjects[i]->SurfaceMe)
           {
               mc surfacer(svobjects[i]); //create surfacer object

               unsigned char *fullarray=0;
               if (svobjects[i]->AllSlicesCompressed)
               {

                    int size=svobjects[i]->spv->size;
                    if ((fullarray=(unsigned char *)malloc(size * svobjects[i]->spv->kDim))==NULL)
                      {
                            QMessageBox::warning((QWidget *)MainWin,"Memory Error", "Fatal Error - could not obtain enough memory to reconstruct volume.\nTry exporting from a newer version of SPIERSview");
                            exit(0);
                      }

                      //ensure top and bottom are blank
                      for (int z=0; z<size; z++)
                        fullarray[z]=0;

                      unsigned char *endfullarray = fullarray + (svobjects[i]->spv->size) * (svobjects[i]->spv->kDim-1);

                      for (int z=0; z<size; z++)
                        endfullarray[z]=0;

                    svobjects[i]->spv->fullarray=fullarray;
                    uLongf s=size*svobjects[i]->spv->kDim;
                    uncompress(fullarray, &s, svobjects[i]->AllSlicesCompressed, svobjects[i]->AllSlicesSize);

                    int count=0;
                    for (int iii=0; iii<size * svobjects[i]->spv->kDim; iii++)
                        if (fullarray[iii]) count++;

                }
               surfacer.SurfaceObject();
               if (fullarray) {free(fullarray);svobjects[i]->spv->fullarray=0;}
               svobjects[i]->MakePolyData();
           }
            //Next job - do isosurface stretching and convert into VTK format
            svobjects[i]->ForceUpdates(-1,-1);
            MainWin->UpdateGL();
       }
    }
  int ttrig=0;
  for (int i=0; i<svobjects.count(); i++) ttrig+=svobjects[i]->Triangles;
  QString status;
  status.sprintf("Completed",ttrig/1000);
  MainWin->ui->OutputLabelOverall->setText(status);
  MainWin->ui->ProgBarOverall->setValue(100);
  MainWin->UpdateGL();
  //MainWin->UpdateScaleEnabling();
}

bool SPVreader::is_sp2(char *fn)
{
  if ((fn!=NULL) && ((fn[strlen(fn)-1])=='2'))
    return true;
  else //either spv or null name
    return false;
}

void SPVreader::FileFailed(QString fname, bool write, int n)
{
    QString Message;
    if (write)
        Message.sprintf
                ("Error code % d - could not open file %s for writing - it may be write-protected",n,fname);
    else
        Message.sprintf("Error code % d - could not open file %s - does it exist?",n,fname);

    QMessageBox::warning((QWidget *)MainWin,"File Error",Message);
    exit(0);
}

void SPVreader::InternalProcessFile(QString filename)
{
  QFileInfo fi(filename);

  QString path=fi.absolutePath();
  QString fname=fi.fileName();

  IsSP2=(fi.suffix()=="sp2");
  if (!IsSP2)
  {
      ProcessSPV(filename, 0, (float *)0);
      return;
  }
  else
  {
      char buffer[1024];
      SP2_lock=true; //lock all interactions
      QFile in(filename);
      if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
          {FileFailed(filename, false, 13); return;}

      int filecount=0;
     //Read main array of SPVs, brights and matrices
     do
     {
        do
        {
             if (in.readLine(buffer,200)<=0) return; //get line - error if nothing returned
        }
        while (buffer[0]=='\n' || buffer[0]==' ' || buffer[0]=='\r'); //if it's just a return try again

        while (buffer[strlen(buffer)-1]=='\n' || buffer[strlen(buffer)-1]=='\r')
                        buffer[strlen(buffer)-1] = 0; //lose the newline character

        char namebuff[200];
        strcpy(namebuff,buffer);

       if (strcmp("END",buffer)==0 && filecount == 0) return; //Error - sp2 file does not refer to any spv files
       if (strcmp("END",buffer)!=0)
       {
         float bright;
         float matrix[16];
         QString d(in.readLine());
         bright=d.toFloat();
         for (int m=0; m<16; m++)
         {
             QString d(in.readLine());
             matrix[m]=d.toFloat();
         }

         QString spvname=path+"/"+QString(namebuff);

         ProcessSPV(spvname, filecount, matrix);
       }
       filecount++;
     }
     while (strcmp(buffer,"END"));
     //work out proper scale - from FIRST Spv (maybe this one, maybe not)
     mm_per_unit = ((float)(SPVs[0]->iDim)/(float)SCALE)/SPVs[0]->PixPerMM;

     SP2_lock=false;
   }
}


void SPVreader::invert_endian(unsigned char *data, int count)
{
        unsigned char newdata[8]; //max size is double
        int n;

#ifdef _BIG_ENDIAN
        for (n=0; n<count; n++) newdata[count-1-n]=data[n];
        for (n=0; n<count; n++) data[n]=newdata[n];
#endif
        return;
}

void SPVreader::WriteWithEndian(FILE *file, unsigned char *pointer, int size, int count)
{
    int n;
    for (n=0; n<count; n++)
        invert_endian(&(pointer[size*n]),size);

    fwrite(pointer,size*count,1,file);

    for (n=0; n<count; n++)
        invert_endian(&(pointer[size*n]),size);
}


int SPVreader::ProcessSPV(QString filename, unsigned int index, float *PassedMatrix=0)
{
  double p1,p2,p3,p4,dummydouble;
  int pos, flen, n, m, version, ii, dummyint;
  int fwidth, fheight, filesused, items, bsize; //these need to be 32 bit ints
  int SPVnumber;
  unsigned char *tarray;
  unsigned char *fullarray; //this is legacy
  uLongf size;
  double temp;
  char dummy[4096];
  int slen, temp2;
  FILE *file;
  short OutKeys[201], OutResamples[201], OutColours[201*3]; // these must be 16 bit ints
  int BaseIndex, firstgroup;

  int errnum = 0;

  QFile f(filename);
  f.open(QIODevice::ReadOnly);
  //qDebug()<<f.handle();
  file = fdopen(f.handle(), "rb");
  if (file==NULL)
  //if (!f.open(file,QIODevice::ReadOnly,QFile::AutoCloseHandle))
    {FileFailed(filename, false, errnum); return 1;}

  //read all the parameters in
  fread(&p1,8,1,file);   invert_endian((unsigned char *)(&p1),8);

  version=1; //default
  if (p1<0) // V1 SPV files lacked a versioning system...
                        // Work around is for first param to be negative in
                        // v2 and up. For V2 and up, next param is int version
                        // followed by real p1.
  {
          fread(&version,sizeof(int),1,file);   invert_endian((unsigned char *)(&version),4);
          fread(&p1,8,1,file);   invert_endian((unsigned char *)(&p1),8);
  }

  if (version>5 && version<1000)
  {
      //New style file
      fclose(file);
      ReadSPV6(filename);
  }
  else
  {
      if (ReplaceIndex>=0) {ReplaceIndex=-3; return 0;} //can't replace with a pre-v6 spv
      //Legacy SPV - continue with old code
      fread(&p2,8,1,file);   invert_endian((unsigned char *)(&p2),8);
      fread(&p3,8,1,file);   invert_endian((unsigned char *)(&p3),8);
      fread(&p4,8,1,file);   invert_endian((unsigned char *)(&p4),8);
      fread(&fwidth,4,1,file);   invert_endian((unsigned char *)(&fwidth),4);
      fread(&fheight,4,1,file);   invert_endian((unsigned char *)(&fheight),4);
      fread(&filesused,4,1,file);   invert_endian((unsigned char *)(&filesused),4);
      fread(&items,4,1,file);   invert_endian((unsigned char *)(&items),4);

      SPV *thisspv=new SPV(SPVs.count(),version,fwidth, fheight,filesused);
      SPVs.append(thisspv);
      thisspv->filename=filename;
      //get filename without path
      QString fn = filename;
      fn=fn.mid(qMax(fn.lastIndexOf("\\"),fn.lastIndexOf("/"))+1);
      thisspv->filenamenopath=fn;

      //store these in the SPV object
      thisspv->PixPerMM=p1;
      thisspv->SlicePerMM=p2;
      thisspv->SkewDown=-p3*p1;
      thisspv->SkewLeft=-p4*p1;

      mm_per_unit = ((float)(fwidth)/(float)SCALE)/thisspv->PixPerMM;

      //create and append all the svobjects
      for (int i=0; i<items; i++)
      {
          svobject *newobj=new svobject(svobjects.count());
          newobj->spv = thisspv; //pointer in object to SPV
          thisspv->ComponentObjects.append(newobj); //pointer in SPV to object
          svobjects.append(newobj); //put it in my general list
          newobj->Index=svobjects.count()-1;
      }

      //Keys array - currently there are 201
      fread(OutKeys,sizeof(OutKeys),1,file);
            for (n=0; n<201; n++) invert_endian((unsigned char *)(&OutKeys[n]),2);
      fread(OutColours,sizeof(OutColours),1,file);
            for (n=0; n<201*3; n++) invert_endian((unsigned char *)(&OutColours[n]),2);
      fread(OutResamples,sizeof(OutResamples),1,file);
            for (n=0; n<201; n++) invert_endian((unsigned char *)(&OutResamples[n]),2);

      //put these in my new structures
      for (int i=0; i<items; i++)
      {
          thisspv->ComponentObjects[i]->Key = (QChar) OutKeys[i];
          thisspv->ComponentObjects[i]->Colour[0] = (uchar) OutColours[i];
          thisspv->ComponentObjects[i]->Colour[1] = (uchar) OutColours[i+201];
          thisspv->ComponentObjects[i]->Colour[2] = (uchar) OutColours[i+201*2];
          thisspv->ComponentObjects[i]->Resample = (int) OutResamples[i];
          if (PassedMatrix)
          for (int j=0; j<16; j++)
          {
              thisspv->ComponentObjects[i]->matrix[j]=PassedMatrix[j];
          }
          TransposeMatrix(thisspv->ComponentObjects[i]->matrix);
          for (int j=0; j<16; j++) thisspv->ComponentObjects[i]->defaultmatrix[j]=thisspv->ComponentObjects[i]->matrix[j];
          thisspv->ComponentObjects[i]->gotdefaultmatrix=true;
      }

      //Handle stretch array (v2 only)
      double *stretches=(double *)malloc((filesused+1) * sizeof(double));
      thisspv->stretches=stretches;
      if (version>1)
      {
            //Read stretches array
             fread(stretches, filesused*sizeof(double), 1, file);
             for (n=0; n<filesused; n++)
                     invert_endian((unsigned char *)(&stretches[n]),sizeof(double));
                     temp=stretches[n];
      }
      else //V1.0 - must construct a stretcharray with no changes
      {
                      temp=0; //construct a no-stretch array
                      for (n=0; n<filesused; n++) stretches[n]=temp++;
      }

      //extra stretches item
      stretches[filesused]=stretches[filesused-1]+1;
      // v3 and up: model mirrored in y to correct for BMP inversion.
      if (version>2) thisspv->MirrorFlag=true; else thisspv->MirrorFlag=false;

      pos = 0;
      flen = fwidth * fheight - 1;


      if (version<5) //old file, need old style large array
      {
            //alloc the main array
          if ((fullarray=(unsigned char *)malloc(filesused * fwidth * fheight))==NULL)
          {
                QMessageBox::warning((QWidget *)MainWin,"Memory Error", "Fatal Error - could not obtain enough memory to reconstruct volume.\nTry exporting from a newer version of SPIERSview");
                exit(0);
          }

          //ensure top and bottom are blank
          for (int i=0; i<fwidth * fheight; i++)
            fullarray[i]=0;

          unsigned char *endfullarray = fullarray + (fwidth * fheight) * (filesused-1);

          for (int i=0; i<fwidth * fheight; i++)
            endfullarray[i]=0;
      }
      else fullarray=0; //no fullarrray
      //Previously made blank slices here- now we just have a blank flag in the compressedslices object

      for (int m=0; m<items; m++) //for each item in the file
      {

          QString status;
          status.sprintf("Processing object %d of %d",m+1,items);
          MainWin->ui->OutputLabelOverall->setText(status);
          MainWin->ui->ProgBarOverall->setValue((m*100)/items);

          MainWin->setSpecificLabel("Preprocessing Data");
          qApp->processEvents();

          svobject *thisobj=thisspv->ComponentObjects[m];

          if (version<4) thisobj->BuggedData=true;
          if (version>4)
          {    //Chunked compression - build into fullarray

              int pieces;

              //read the number of pieces of the file - ought to be same as file count, but no chances!
              fread(&pieces,4,1,file);   invert_endian((unsigned char *)(&pieces),4);

              CompressedSlice *blankslice=new CompressedSlice(thisobj,true);
              (thisobj->compressedslices).append(blankslice);
              int SlicePointer=1;

              for (int p=0; p<pieces; p++)
              {
                 MainWin->setSpecificProgress((p*100)/pieces);
                 if (p%10==0) qApp->processEvents();


                if (p % (filesused-2) == 0) SlicePointer= 1;// reached a restart from merge

                    //read the size
                  fread(&bsize,4,1,file);  invert_endian((unsigned char *)(&bsize),4);

                  CompressedSlice *s=new CompressedSlice(thisobj, false);

                  if (bsize==-1) //No data - flag as empty
                      s->empty=true;
                  else
                  {
                      tarray=(unsigned char *)malloc(bsize);
                      fread(tarray,bsize,1,file);
                      s->datasize=bsize;  s->data = tarray;

                      s->grid=(unsigned char *)malloc(thisspv->GridSize);
                      if (version>=1000) //read the grid
                       fread(s->grid,thisspv->GridSize,1,file);
                      //if no grid all grid squares are on
                      else for (int i=0; i<thisspv->GridSize; i++) s->grid[i]=(uchar)255;
//                      for (int i=0; i<thisspv->GridSize; i++) qDebug()<<"Grid "<<i<<"="<< s->grid[i]<<s->grid;
                  }



                  if (p<(filesused-2)) //no merge complications
                  {

                        (thisobj->compressedslices).append(s);
                  }
                  else
                  {
                        QString fname;
                        fname=QString("C:/Documents and Settings/mdsutton/Desktop/Test/cdump_%1_%2_a")
                              .arg(p,3,10,QChar('0')).arg(SlicePointer,3,10,QChar('0'));
                        thisobj->compressedslices[SlicePointer]->Dump(fname);
                        fname=QString("C:/Documents and Settings/mdsutton/Desktop/Test/cdump_%1_%2_b")
                              .arg(p,3,10,QChar('0')).arg(SlicePointer,3,10,QChar('0'));
                        s->Dump(fname);
                        fname=QString("C:/Documents and Settings/mdsutton/Desktop/Test/cdump_%1_%2_d")
                              .arg(p,3,10,QChar('0')).arg(SlicePointer,3,10,QChar('0'));
                        thisobj->compressedslices[SlicePointer]->Merge(s,fname);
                        fname=QString("C:/Documents and Settings/mdsutton/Desktop/Test/cdump_%1_%2_c")
                          .arg(p,3,10,QChar('0')).arg(SlicePointer,3,10,QChar('0'));
                        thisobj->compressedslices[SlicePointer]->Dump(fname);
                        delete s;
                  }
                   SlicePointer++;
                }

                blankslice =new CompressedSlice(thisobj,true);
                (thisobj->compressedslices).append(blankslice);
            }
            else
            { //old style - one chunk
                  //get the compressed file size
                  //sprintf(dummy, "Old-style (non-chunked), fullarray is %d\n",fullarray); DebugPrint(dummy);

                  fread(&bsize,4,1,file);   invert_endian((unsigned char *)(&bsize),4);

                  //alloc and read in
                  tarray=(unsigned char *)malloc(bsize);
                  fread(tarray,bsize,1,file);

                  //uncompress it
                  size=filesused * fwidth * fheight;
                  temp2=uncompress(fullarray, &size, tarray, bsize);
                  thisobj->AllSlicesCompressed=tarray;
                  thisobj->AllSlicesSize=bsize;
                  thisspv->fullarray=fullarray;
             }


          int TrigCount;
          double *TrigArray;
          if (version>3) //contains spline info
          {
                     fread(&TrigCount,4,1,file);   invert_endian((unsigned char *)(&TrigCount),4);
                     if (TrigCount>0)
                     {
                             TrigArray=(double *)malloc(6*TrigCount*sizeof(double));
                             fread(TrigArray,6*TrigCount*sizeof(double),1,file);

                             //Now worry about endianism - note loop is skipped on Win32 for speed
                             //#ifdef _BIG_ENDIAN
                             //for (ii=0; ii<TrigCount*6; ii++)
                             //invert_endian((unsigned char *)(&(TrigArray[TrigCount])),sizeof(double));
                             //#endif
                     }


          } else TrigCount=0;


          //Make isosurface
          mc surfacer(thisobj); //create surfacer object
          surfacer.SurfaceObject();
          //qDebug()<<"Surfaced";
          //Next job - do isosurface stretching and convert into VTK format
          thisobj->MakePolyData();
          thisobj->ForceUpdates(-1,-1);
          MainWin->UpdateGL();
          FixUpData();
          MainWin->RefreshObjects();
          if (TrigCount>0) free(TrigArray);

          //qDebug()<<"Freed";

        }
          //qDebug()<<"HERE";
          if (fullarray) free(fullarray);
          thisspv->fullarray=0; //and reset in the object


          //now ready to read panel arrays
          int TableStart=ftell(file);

          int ttrig=0;
          for (int i=0; i<svobjects.count(); i++) ttrig+=svobjects[i]->Triangles;
          QString status;
          status.sprintf("Completed");
          MainWin->ui->OutputLabelOverall->setText(status);
          MainWin->ui->ProgBarOverall->setValue(100);
          qApp->processEvents();


          //now back at end with all data stored for later writing.
          //attempt to read arrays - expect errors - these will indicate that they weren't there!

          //Set a default ListCount to flag failure
          int ListCount=-1;

          if (fread(&dummyint,sizeof(int),1,file)!=1) goto out;
          invert_endian((unsigned char *)&dummyint,sizeof(int));

          if (dummyint!=10000) goto out; //10000 is code for listcount stuff follows...

          if (fread(&ListCount,sizeof(ListCount),1,file)!=1) goto out;
          invert_endian((unsigned char *)&ListCount,sizeof(ListCount));


          //If Listcount > count of objects in the SPV create new blank group objects
          //Convert so reads these into the objects in SPV (thisspv->Objects[i] etc.

          firstgroup=thisspv->ComponentObjects.count();
          if (ListCount>=(thisspv->ComponentObjects.count()))
          {
              //Extra objects (must be groups)
              for (int i=thisspv->ComponentObjects.count(); i<ListCount; i++)
              {
                  svobject *group = new svobject(svobjects.count());
                  group->IsGroup=true;
                  svobjects.append(group);
                  thisspv->ComponentObjects.append(group);
              }

          }

          int ObjNumbers[201];
          bool IsGroup[201];
          int InGroup[201];
          bool ShowGroups[201];
          unsigned char ListKeys[201];
          //For now just read these as fixed size arrays. Need an alternative to read arbitrary numbers of them in
          if (fread(&(ObjNumbers[0]),sizeof(int)*ListCount,1,file)!=1) goto out;
                for (n=0; n<ListCount; n++) invert_endian((unsigned char *)(&ObjNumbers[n]),sizeof(int));
          if (fread(&(InGroup[0]),sizeof(int)*ListCount,1,file)!=1) goto out;
                for (n=0; n<ListCount; n++) invert_endian((unsigned char *)(&InGroup[n]),sizeof(int));
          if (fread(&(IsGroup[0]),sizeof(bool)*ListCount,1,file)!=1) goto out;
                for (n=0; n<ListCount; n++) invert_endian((unsigned char *)(&IsGroup[n]),sizeof(bool));
          if (fread(&(ShowGroups[0]),sizeof(bool)*ListCount,1,file)!=1) goto out;
                for (n=0; n<ListCount; n++) invert_endian((unsigned char *)(&ShowGroups[n]),sizeof(bool));
          if (fread(&(ListKeys[0]),ListCount,1,file)!=1) goto out;

          //OK, old style had funny order - groups may not be at end - I think it's actually display order
          //ObjNumbers gives the read-in number for these objects... for non-groups anyway

          BaseIndex=thisspv->ComponentObjects[0]->Index; //first item - might not be 0 if this is not first spv
          for (int i=0; i<ListCount; i++)
          {
              //Loop round all of these old nasty things
              //is it a group? If so fix up it's ObjNumber, currently 0
              if (IsGroup[i]) ObjNumbers[i]=BaseIndex + firstgroup++; else ObjNumbers[i]+=BaseIndex;
          }

          for (int i=0; i<ListCount; i++)
          {
               //OK, this time round can assign all the ingroups etc properly
              int realindex=ObjNumbers[i]-BaseIndex;
              if ((InGroup[i])==-1)
                 thisspv->ComponentObjects[realindex]->InGroup=-1;
              else
                 thisspv->ComponentObjects[realindex]->InGroup = ObjNumbers[InGroup[i]];

              thisspv->ComponentObjects[realindex]->Key = (QChar) ListKeys[i];
          }

          //now read strings
          for (n=0; n<ListCount; n++)
          {
                if (fread(&slen,sizeof(int),1,file)!=1) goto out;
                //read this many characters
                if (fread(dummy,slen,1,file)!=1) goto out;
                dummy[slen]='\0'; //terminate it
                QString readinname=dummy;

                //strip the key off the front if there
                if (readinname.mid(1,3)==" - ")
                    readinname=readinname.mid(4);
                thisspv->ComponentObjects[ObjNumbers[n]-BaseIndex]->Name=readinname;
          }

          //Finally, there may be some re-ordering to do. Write positions in.
          for (int i=0; i<ListCount; i++)
          {
              thisspv->ComponentObjects[ObjNumbers[i]-BaseIndex]->Position=i+BaseIndex;
          }

    out:
               FixUpData();
               MainWin->RefreshObjects();
              fclose(file);


      return 0;
  }
}
