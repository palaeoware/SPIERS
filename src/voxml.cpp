#include "voxml.h"
#include "svobject.h"
#include "spv.h"
#include <QXmlStreamReader>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QVector3D>
#include <QBuffer>
#include "vtkSTLReader.h"
#include "vtkPLYReader.h"
#include "SPIERSviewglobals.h"
#include "ui_mainwindow.h"
#include "../SPIERScommon/netmodule.h"

QStringList  i_comments, i_reference, i_author, i_specimen, i_provenance, i_classification_name, i_classification_rank, i_title;
QMatrix4x4 globalmatrix;
bool voxml_mode;
float minx, maxx, miny, maxy, minz, maxz;
bool firstobject;

voxml_group::voxml_group()
{
    name="";
    key=0;
    visible=true;
    ingroup="";
    position=-1;
}

voxml_object::voxml_object()
{
    name="[Unnamed object]";
    key=0;
    visible=true;
    ingroup="";
    file="";
    url="";

    red=255; green=255; blue=255;
    matrix[0]=1.0;
    matrix[1]=0.0;
    matrix[2]=0.0;
    matrix[3]=0.0;
    matrix[4]=0.0;
    matrix[5]=1.0;
    matrix[6]=0.0;
    matrix[7]=0.0;
    matrix[8]=0.0;
    matrix[9]=0.0;
    matrix[10]=1.0;
    matrix[11]=0.0;
    matrix[12]=0.0;
    matrix[13]=0.0;
    matrix[14]=0.0;
    matrix[15]=1.0;
    transparency=0;
    position=-1;
};

voxml::voxml()
{
    //set defaults
    version=1; //default to v1, where export didn't even export a version (doh!)
    title="";
    scale=1.0;
    matrix[0]=1.0;
    matrix[1]=0.0;
    matrix[2]=0.0;
    matrix[3]=0.0;
    matrix[4]=0.0;
    matrix[5]=1.0;
    matrix[6]=0.0;
    matrix[7]=0.0;
    matrix[8]=0.0;
    matrix[9]=0.0;
    matrix[10]=1.0;
    matrix[11]=0.0;
    matrix[12]=0.0;
    matrix[13]=0.0;
    matrix[14]=0.0;
    matrix[15]=1.0;
}

voxml::~voxml()
{
     qDeleteAll(groups.begin(), groups.end());
     qDeleteAll(objects.begin(), objects.end());
}

bool voxml::read_spvf(QString fname)
{
    //open the file
    QFile *file = new QFile(fname);
    QFileInfo fi(fname);
    QString fpath=fi.canonicalPath();

//    qDebug()<<"H1";
    if (file->open(QIODevice::ReadOnly)==false)
    {
         QMessageBox::information(0,"test","Error - could not open SPVF file");
         return false;
    }

    QDataStream in(file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_5);

    QString xmlstring;

//    qDebug()<<"H2";
    //Set up string as a buffer so same code can read it!
    in>>xmlstring; //read the XML
    //qDebug()<<"XML:"<<xmlstring;
    QByteArray xmlbytearray;
    xmlbytearray.append(xmlstring);
    QBuffer buffer(&xmlbytearray);

    //setup XML reader
    buffer.open(QBuffer::ReadOnly);
    QXmlStreamReader xml;
    xml.setDevice(&buffer);

    bool flag;

    //qDebug()<<"H3";
    if (xml.readNextStartElement())
    {
             if (xml.name() == "vaxml")
            {
                 while (xml.readNextStartElement())
                 {
                     if (xml.name() == "header")
                     {
                         while (xml.readNextStartElement())
                         {

                             //qDebug()<<xml.name();
                             if (xml.name() == "version")
                             {
                                 QString text = xml.readElementText();
                                 int i=text.toInt(&flag);
                                 if (!flag) return xml_error("invalid version: " + text);
                                 //if (version>0) return xml_error("multiple version tags");
                                 version=i;
                             }

                             if (xml.name() == "title")
                             {
                                 QString text = xml.readElementText();
                                 if (text.length()==0) xml_error("invalid (empty) title");
                                 if (title.length()>0) return xml_error("multiple title tags");
                                 title=text;
                                 i_title.clear(); i_title.append(title);
                             }

                             if (xml.name() == "scale")
                             {
                                 QString text = xml.readElementText();
                                 float i=text.toFloat(&flag);
                                 if (!flag) return xml_error("invalid scale: " + text);
                                 scale=i;
                             }

                             if (xml.name() == "comments")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) comments << text; //append comments if not empty
                             }

                             if (xml.name() == "specimen")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) specimen << text; //append comments if not empty
                             }

                             if (xml.name() == "provenance")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) provenance << text; //append comments if not empty
                             }

                             if (version==1 && xml.name() == "classification")
                             {
                                 //OLD BEHAVIOUR - single block for classification
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0)
                                 {
                                     classification_name.append(text); //append comments if not empty
                                     classification_rank.append(""); //append a blank classification
                                 }
                             }

                             if (version>1 && xml.name() == "classification")
                             {
                                 QString rank="";
                                 QString nam="";
                                 while (xml.readNextStartElement())
                                 {
                                     if (xml.name() == "rank")
                                     {
                                         rank = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                     }

                                     if (xml.name() == "name")
                                     {
                                         nam = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                     }
                                 };
                                 if (nam=="" && rank=="")  return xml_error("invalid classification (no rank or name)");
                                 if (nam=="")  return xml_error("invalid classification (no name)");
                                 if (rank=="") rank="Unspecified";
                                 classification_rank.append(rank);
                                 classification_name.append(nam);
                             }

                             if (xml.name() == "author")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) author << text; //append comments if not empty
                             }
                             if (xml.name() == "reference")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) reference << text; //append comments if not empty
                             }

                         };
                     }
                     else if (xml.name() == "groups")
                     {
                         while (xml.readNextStartElement())
                         {
                             if (xml.name() == "group")
                             {  //create the group
                                 voxml_group *new_group = new(voxml_group);
                                 groups.append(new_group);

                                 while (xml.readNextStartElement())
                                 {
                                     if (xml.name() == "name")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) group name");
                                         if (new_group->name.length()>0) return xml_error("multiple names for a group");
                                         new_group->name=text;
                                     }


                                     if (xml.name() == "key")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()>1) xml_error("invalid key for group");
                                         if (new_group->key>0) return xml_error("multiple keys for a group");
                                         QByteArray b = text.toUpper().toLatin1();
                                         new_group->key=QChar(b.at(0));
                                     }

                                     if (xml.name() == "visible")
                                     {
                                         QString text = xml.readElementText();
                                         int i=text.toInt(&flag);
                                         if (!flag || i>1 || i<0) return xml_error("invalid visible (should be 1 or 0): " + text);
                                         new_group->visible=i;
                                     }

                                     if (xml.name() == "ingroup")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) group name");
                                         if (new_group->ingroup.length()>0) return xml_error("multiple ingroup entries for a group");
                                         new_group->ingroup=text;
                                     }
                                     if (xml.name() == "position")
                                     {
                                         QString text = xml.readElementText();
                                         int t=text.toInt(&flag);
                                         if (!flag) return xml_error("error in position for group");
                                         new_group->position=t;
                                     }
                                 };

                             }
                             else return xml_error("invalid element in <groups> section: " + xml.name().toString());

                         };
                     }
                     else if (xml.name() == "objects")
                     {
                         while (xml.readNextStartElement())
                         {
                             if (xml.name() == "object")
                             {  //create the object
                                 voxml_object *new_obj = new(voxml_object);
                                 objects.append(new_obj);

                                 while (xml.readNextStartElement())
                                 {

                                     if (xml.name() == "material")
                                     {
                                         while (xml.readNextStartElement())
                                         {
                                             if (xml.name() == "transparency")
                                             {
                                                 QString text = xml.readElementText();
                                                 float i=text.toFloat(&flag);
                                                 if (!flag) return xml_error("invalid transparency: " + text);
                                                 if (i<0 || i>1) return xml_error("invalid transparency: " + text);
                                                 new_obj->transparency=i;
                                             }

                                             if (xml.name() == "colour")
                                             {
                                                 while (xml.readNextStartElement())
                                                 {
                                                     if (xml.name() == "red")
                                                     {
                                                         QString text = xml.readElementText();
                                                         int i=text.toInt(&flag);
                                                         if (!flag) return xml_error("invalid red colour: " + text);
                                                         if (i<0 || i>255) return xml_error("invalid red colour: " + text);
                                                         new_obj->red=i;
                                                     }

                                                     if (xml.name() == "green")
                                                     {
                                                         QString text = xml.readElementText();
                                                         int i=text.toInt(&flag);
                                                         if (!flag) return xml_error("invalid green colour: " + text);
                                                         if (i<0 || i>255) return xml_error("invalid green colour: " + text);
                                                         new_obj->green=i;
                                                     }

                                                     if (xml.name() == "blue")
                                                     {
                                                         QString text = xml.readElementText();
                                                         int i=text.toInt(&flag);
                                                         if (!flag) return xml_error("invalid blule colour: " + text);
                                                         if (i<0 || i>255) return xml_error("invalid blue colour: " + text);
                                                         new_obj->blue=i;
                                                     }
                                                 };
                                             }
                                         };
                                     }

                                     if (xml.name() == "file")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) object file");
                                         if (new_obj->file.length()>0) return xml_error("multiple files for an object");
                                         new_obj->file=text;
                                     }

                                     if (xml.name() == "url") //will ignore in practice
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) object URL");
                                         if (new_obj->url.length()>0) return xml_error("multiple URLs for an object");
                                         new_obj->url=text;
                                     }

                                     if (xml.name() == "name")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) object name");
                                         if (new_obj->name!="[Unnamed object]") return xml_error("multiple names for an object");
                                         new_obj->name=text;

                                     }

                                     if (xml.name() == "key")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()>1) xml_error("invalid key for object");
                                         if (new_obj->key>0) return xml_error("multiple keys for a object");
                                         QByteArray b = text.toUpper().toLatin1();
                                         new_obj->key=QChar(b.at(0));
                                     }

                                     if (xml.name() == "visible")
                                     {
                                         QString text = xml.readElementText();
                                         int i=text.toInt(&flag);
                                         if (!flag || i>1 || i<0) return xml_error("invalid visible (should be 1 or 0): " + text);
                                         new_obj->visible=i;
                                     }

                                     if (xml.name() == "ingroup")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) group name");
                                         if (new_obj->ingroup.length()>0) return xml_error("multiple ingroup entries for an object");
                                         new_obj->ingroup=text;
                                     }

                                     if (xml.name() == "position")
                                     {
                                         QString text = xml.readElementText();
                                         int t=text.toInt(&flag);
                                         if (!flag) return xml_error("error in position for object");
                                         new_obj->position=t;
                                     }

                                     if (xml.name() == "matrix")
                                     {
                                         for (int i=0; i<16; i++)
                                         {
                                             QString n;
                                             QTextStream t(&n);
                                             t<<"m"<<i;
                                             xml.readNextStartElement();
                                             if (xml.name()!=n) return xml_error("error in object matrix item: " + n );
                                             QString text = xml.readElementText();
                                             float v=text.toFloat(&flag);
                                             if (!flag) return xml_error("error in object matrix item: " + n );
                                             new_obj->matrix[i]=v;
                                         }
                                         xml.skipCurrentElement(); //required to skip to end of matrix
                                     }

                                 };
                             }
                         };
                     }
                         //else return xml_error("invalid element in <objects> section: " + xml.name().toString());
                 };
             }
    }
    //qDebug()<<"H4";

    //Now attempt to read all the STLs.
    QList <vtkPolyData *> PolyData;

    MainWin->setSpecificLabel("Setting up objects");
    qApp->processEvents();

    float f=0.0;
    //Now read all the STL stuff
    for (int i=0; i<objects.count(); i++)
    {
        QByteArray b1, b2;
        in>>b1;
        b2=qUncompress(b1);
        //b2 now holds the STL-style data
        //qDebug()<<"Original "<<b1.count()<<"Uncompressed to "<<b2.count();
        //buffer to act as streamer for this
        QBuffer stlbuffer(&b2);       stlbuffer.open(QIODevice::ReadOnly);

        QDataStream stl_in(&stlbuffer);


        vtkPolyData *pd;
        vtkPoints *verts;
        vtkCellArray *cellarray;
        vtkIdTypeArray *actualarray;

        verts = vtkPoints::New();
        actualarray = vtkIdTypeArray::New();
        cellarray=vtkCellArray::New();
        pd=vtkPolyData::New();
        pd->Initialize();
        verts->Initialize();
        cellarray->Initialize();
        actualarray->Initialize();

        int vcount, tcount;
        stl_in>>vcount;
        //qDebug()<<"Read "<<vcount<<"vertices";
        verts->SetNumberOfPoints(vcount);
        for (int i=0; i<vcount; i++)
        {
            double x,y,z;

            stl_in>>x;
            stl_in>>y;
            stl_in>>z;
            verts->InsertPoint(i, x,y,z);
            if ((i%1000)==0)
            {
//                qDebug()<<"Point"<<i<<"xyz:"<<x<<y<<z;
            }
        }

//        qDebug()<<"H5";
        stl_in>>tcount;
        actualarray->SetNumberOfValues(tcount*4);
        int pos=0;
        for (int i=0; i<tcount; i++)
        {
            int t1,t2,t3;
            stl_in>>t1;
            stl_in>>t2;
            stl_in>>t3;
            actualarray->SetValue(pos++,3);
            actualarray->SetValue(pos++,t1);
            actualarray->SetValue(pos++,t2);
            actualarray->SetValue(pos++,t3);
        }
//        qDebug()<<"Read "<<tcount<<"triangles";

        cellarray->SetCells(tcount,actualarray);
        pd->SetPolys(cellarray);
        pd->SetPoints(verts);
        //Do actual reading


        //done - code from here on is as in normal voxml reader

        PolyData.append(pd);
        f+=(100.0/objects.count());
        //MainWin->setSpecificProgress((int)f);
        MainWin->setSpecificProgress((int)(f/2.0));
        qApp->processEvents();
    }
    voxml_mode=true;
    //qDebug()<<"H6";

    //All OK - create the local objects
    //Create a 'dummy' spv
    SPV *spv= new SPV(0,0,0,0,0);
    SPVs.append(spv);
    spv->filename=title;
    i_classification_name=classification_name;
    i_classification_rank=classification_rank;
    i_provenance=provenance;
    i_specimen=specimen;
    i_author=author;
    i_comments=comments;
    i_reference=reference;

    mm_per_unit=scale;


    for (int i=0; i<groups.count(); i++)
    {
        SVObject *svo = new SVObject(i);
        spv->ComponentObjects.append(svo);
        SVObjects.append(svo);
        svo->IsGroup=true;
        svo->Visible=groups[i]->visible;
        svo->Name=groups[i]->name;
        if (groups[i]->position!=-1) svo->Position=groups[i]->position;
        svo->Key=groups[i]->key;
    }
    //second pass to set up ingroups
    for (int i=0; i<groups.count(); i++)
    {   SVObject *svo=spv->ComponentObjects[i];
        svo->InGroup=find_group_from_name(spv,groups[i]->ingroup); //works for empty ingroup - will miss all and return -1
    }

    int firstrealobj=groups.count();

    f=0.0;

    //set em up
    for (int i=0; i<objects.count(); i++)
    {
        SVObject *svo = new SVObject(i+firstrealobj);
        spv->ComponentObjects.append(svo);
        SVObjects.append(svo);
         svo->IsGroup=false;
        svo->Visible=objects[i]->visible;
        svo->Name=objects[i]->name;
        if (objects[i]->position!=-1) svo->Position=objects[i]->position;
        svo->Key=objects[i]->key;
        svo->InGroup=find_group_from_name(spv,objects[i]->ingroup); //works for empty ingroup - will miss all and return -1
        svo->Colour[0]=objects[i]->red;
        svo->Colour[1]=objects[i]->green;
        svo->Colour[2]=objects[i]->blue;
        svo->Transparency=conv_trans(objects[i]->transparency);
        for (int j=0; j<16; j++) svo->matrix[j]=objects[i]->matrix[j];
        svo->spv=spv;
        svo->pd=PolyData[i];
    }

    //surface them. Two loops needed as matrices must be in place before I start buggering with this!
    for (int i=0; i<objects.count(); i++)
    {
        SVObject *svo = SVObjects[(i+firstrealobj)];
        svo->MakeDlists();
        MainWin->UpdateGL();
        f+=(100.0/objects.count());
        MainWin->ui->ProgBarOverall->setValue(50+(int)(f/2.0));
    }

    return true;


}

bool voxml::read_voxml(QString fname)
{

    qDebug()<<"Here";
    QByteArray shasharray;

    //open the file
    QFile *file = new QFile(fname);
    QFileInfo fi(fname);
    QString fpath=fi.canonicalPath();

    //qDebug()<<"About to try to open"<<fname<<file;
    if (file->open(QIODevice::ReadOnly)==false)
    {
         QMessageBox::information(0,"test","Error - could not open file VAXML file");
         return false;
    }


    //setup XML reader
    QXmlStreamReader xml;
    xml.setDevice(file);
    //qDebug()<<"Error 1"<<xml.errorString();

    bool flag;

    if (xml.readNextStartElement())
    {
             if (xml.name() == "vaxml")
            {
                 while (xml.readNextStartElement())
                 {
                     if (xml.name() == "header")
                     {
                         while (xml.readNextStartElement())
                         {

                             //qDebug()<<xml.name();
/*                             if (xml.name() == "matrix")
                             {
                                 for (int i=0; i<16; i++)
                                 {
                                     QString n;
                                     QTextStream t(&n);
                                     t<<"m"<<i;
                                     xml.readNextStartElement();
//                                     qDebug()<<xml.name()<<n;
                                     if (xml.name()!=n) return xml_error("error in header matrix item: " + n );
                                     QString text = xml.readElementText();
                                     float v=text.toFloat(&flag);
                                     if (!flag) return xml_error("error in header matrix item: " + n );
                                     matrix[i]=v;
                                 }
                                 xml.skipCurrentElement(); //required to skip to end of matrix
                             }
*/
                             if (xml.name() == "version")
                             {
                                 QString text = xml.readElementText();
                                 int i=text.toInt(&flag);
                                 if (!flag) return xml_error("invalid version: " + text);
                                 //if (version>0) return xml_error("multiple version tags");
                                 version=i;
                             }

                             if (xml.name() == "title")
                             {
                                 QString text = xml.readElementText();
                                 if (text.length()==0) xml_error("invalid (empty) title");
                                 if (title.length()>0) return xml_error("multiple title tags");
                                 title=text;
                                 i_title.clear(); i_title.append(title);
                             }

                             if (xml.name() == "scale")
                             {
                                 QString text = xml.readElementText();
                                 float i=text.toFloat(&flag);
                                 if (!flag) return xml_error("invalid scale: " + text);
                                 scale=i;
                             }

                             if (xml.name() == "comments")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) comments << text; //append comments if not empty
                             }

                             if (xml.name() == "specimen")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) specimen << text; //append comments if not empty
                             }

                             if (xml.name() == "provenance")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) provenance << text; //append comments if not empty
                             }

                             if (version==1 && xml.name() == "classification")
                             {
                                 //OLD BEHAVIOUR - single block for classification
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0)
                                 {
                                     classification_name.append(text); //append comments if not empty
                                     classification_rank.append(""); //append a blank classification
                                 }
                             }

                             if (version>1 && xml.name() == "classification")
                             {
                                 QString rank="";
                                 QString nam="";
                                 while (xml.readNextStartElement())
                                 {
                                     if (xml.name() == "rank")
                                     {
                                         rank = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                     }

                                     if (xml.name() == "name")
                                     {
                                         nam = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                     }
                                 };
                                 if (nam=="" && rank=="")  return xml_error("invalid classification (no rank or name)");
                                 if (nam=="")  return xml_error("invalid classification (no name)");
                                 if (rank=="") rank="Unspecified";
                                 classification_rank.append(rank);
                                 classification_name.append(nam);
                             }

                             if (xml.name() == "author")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) author << text; //append comments if not empty
                             }
                             if (xml.name() == "reference")
                             {
                                 QString text = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                                 if (text.length()>0) reference << text; //append comments if not empty
                             }

                         };
                     }
                     else if (xml.name() == "groups")
                     {
                         while (xml.readNextStartElement())
                         {
                             if (xml.name() == "group")
                             {  //create the group
                                 voxml_group *new_group = new(voxml_group);
                                 groups.append(new_group);

                                 while (xml.readNextStartElement())
                                 {
                                     if (xml.name() == "name")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) group name");
                                         if (new_group->name.length()>0) return xml_error("multiple names for a group");
                                         new_group->name=text;
                                     }


                                     if (xml.name() == "key")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()>1) xml_error("invalid key for group");
                                         if (new_group->key>0) return xml_error("multiple keys for a group");
                                         QByteArray b = text.toUpper().toLatin1();
                                         new_group->key=QChar(b.at(0));
                                     }

                                     if (xml.name() == "visible")
                                     {
                                         QString text = xml.readElementText();
                                         int i=text.toInt(&flag);
                                         if (!flag || i>1 || i<0) return xml_error("invalid visible (should be 1 or 0): " + text);
                                         new_group->visible=i;
                                     }

                                     if (xml.name() == "ingroup")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) group name");
                                         if (new_group->ingroup.length()>0) return xml_error("multiple ingroup entries for a group");
                                         new_group->ingroup=text;
                                     }
                                     if (xml.name() == "position")
                                     {
                                         QString text = xml.readElementText();
                                         int t=text.toInt(&flag);
                                         if (!flag) return xml_error("error in position for group");
                                         new_group->position=t;
                                     }
                                 };

                             }
                             else return xml_error("invalid element in <groups> section: " + xml.name().toString());

                         };
                     }
                     else if (xml.name() == "objects")
                     {
                         while (xml.readNextStartElement())
                         {
                             if (xml.name() == "object")
                             {  //create the object
                                 voxml_object *new_obj = new(voxml_object);
                                 objects.append(new_obj);

                                 while (xml.readNextStartElement())
                                 {

                                     if (xml.name() == "material")
                                     {
                                         while (xml.readNextStartElement())
                                         {
                                             if (xml.name() == "transparency")
                                             {
                                                 QString text = xml.readElementText();
                                                 float i=text.toFloat(&flag);
                                                 if (!flag) return xml_error("invalid transparency: " + text);
                                                 if (i<0 || i>1) return xml_error("invalid transparency: " + text);
                                                 new_obj->transparency=i;
                                             }

                                             if (xml.name() == "colour")
                                             {
                                                 while (xml.readNextStartElement())
                                                 {
                                                     if (xml.name() == "red")
                                                     {
                                                         QString text = xml.readElementText();
                                                         int i=text.toInt(&flag);
                                                         if (!flag) return xml_error("invalid red colour: " + text);
                                                         if (i<0 || i>255) return xml_error("invalid red colour: " + text);
                                                         new_obj->red=i;
                                                     }

                                                     if (xml.name() == "green")
                                                     {
                                                         QString text = xml.readElementText();
                                                         int i=text.toInt(&flag);
                                                         if (!flag) return xml_error("invalid green colour: " + text);
                                                         if (i<0 || i>255) return xml_error("invalid green colour: " + text);
                                                         new_obj->green=i;
                                                     }

                                                     if (xml.name() == "blue")
                                                     {
                                                         QString text = xml.readElementText();
                                                         int i=text.toInt(&flag);
                                                         if (!flag) return xml_error("invalid blule colour: " + text);
                                                         if (i<0 || i>255) return xml_error("invalid blue colour: " + text);
                                                         new_obj->blue=i;
                                                     }
                                                 };
                                             }
                                         };
                                     }

                                     if (xml.name() == "file")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) object file");
                                         if (new_obj->file.length()>0) return xml_error("multiple files for an object");
                                         new_obj->file=text;
                                     }

                                     if (xml.name() == "url")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) object URL");
                                         if (new_obj->url.length()>0) return xml_error("multiple URLs for an object");
                                         new_obj->url=text;
                                     }

                                     if (xml.name() == "name")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) object name");
                                         if (new_obj->name!="[Unnamed object]") return xml_error("multiple names for an object");
                                         new_obj->name=text;

                                     }

                                     if (xml.name() == "key")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()>1) xml_error("invalid key for object");
                                         if (new_obj->key>0) return xml_error("multiple keys for a object");
                                         QByteArray b = text.toUpper().toLatin1();
                                         new_obj->key=QChar(b.at(0));
                                     }

                                     if (xml.name() == "visible")
                                     {
                                         QString text = xml.readElementText();
                                         int i=text.toInt(&flag);
                                         if (!flag || i>1 || i<0) return xml_error("invalid visible (should be 1 or 0): " + text);
                                         new_obj->visible=i;
                                     }

                                     if (xml.name() == "ingroup")
                                     {
                                         QString text = xml.readElementText();
                                         if (text.length()==0) xml_error("invalid (empty) group name");
                                         if (new_obj->ingroup.length()>0) return xml_error("multiple ingroup entries for an object");
                                         new_obj->ingroup=text;
                                     }

                                     if (xml.name() == "position")
                                     {
                                         QString text = xml.readElementText();
                                         int t=text.toInt(&flag);
                                         if (!flag) return xml_error("error in position for object");
                                         new_obj->position=t;
                                     }

                                     if (xml.name() == "matrix")
                                     {
                                         for (int i=0; i<16; i++)
                                         {
                                             QString n;
                                             QTextStream t(&n);
                                             t<<"m"<<i;
                                             xml.readNextStartElement();
                                             if (xml.name()!=n) return xml_error("error in object matrix item: " + n );
                                             QString text = xml.readElementText();
                                             float v=text.toFloat(&flag);
                                             if (!flag) return xml_error("error in object matrix item: " + n );
                                             new_obj->matrix[i]=v;
                                         }
                                         xml.skipCurrentElement(); //required to skip to end of matrix
                                     }

                                 };
                             }
                         };
                     }
                         else return xml_error("invalid element in <objects> section: " + xml.name().toString());
                 };
             }
    }
    //qDebug()<<"Error 2"<<xml.errorString();
    //qDebug()<<"Past";
    //some more error checking
    //1. Do all groups referred to by ingroup exist?
//    qDebug()<<"Groups"<<groups.count();
    foreach(voxml_group *g, groups)
    {
        if (g->ingroup!="") //if it's in a group
        {
            bool flag=false;
            foreach(voxml_group *g2, groups)
            {
                if (g->ingroup==g2->name && g2!=g) flag=true;
            }
            if (flag==false) return xml_error("missing group name: " + g->ingroup);
        }
    }

    foreach(voxml_object *o, objects)
    {
        if (o->ingroup!="") //if it's in a group
        {
            bool flag=false;
            foreach(voxml_group *g2, groups)
            {
                if (o->ingroup==g2->name) flag=true;
            }
            if (flag==false) return xml_error("missing group name: " + o->ingroup);
        }
    }

    //2. Do all groups have unique names?
    foreach(voxml_group *g, groups)
    {
        foreach(voxml_group *g2, groups)
        {
            if (g->name==g2->name && g2!=g) return xml_error("repeated group name: " + g->name);
        }
    }

    //3. Do all files exist?
    foreach(voxml_object *o, objects)
    {
    }

    //Got and checked voxml stuff - now try and load the STL/PLYs and set everything up!

    //Now attempt to read all the STL/PLYs.
    QList <vtkPolyData *> PolyData;

    MainWin->ui->OutputLabelOverall->setText("Importing objects");
    qApp->processEvents();

    float f=0.0;
    foreach(voxml_object *o, objects)
    {

        QFile file(fpath + "/" + o->file);
        if (!(file.exists()))
        {
            if (o->url.length()==0)
                return xml_error("referenced file '" + file.fileName() + "' does not exist.");
            else
            {
                //File not there - let's download it!
                MainWin->setSpecificLabel("Downloading " + o->file);

                //make path if needbe
                QDir d;
                QFileInfo fi(fpath + "/" + o->file);
                d.mkpath(fi.absolutePath());


                NetModule n;
                n.doDownload(o->url,fpath + "/" + o->file,MainWin->ui->ProgBarSpecific);
                do
                {
                    qApp->processEvents();
                } while(n.DownloadDone==false && n.DownloadError==false);
                if (n.DownloadError) return xml_error("Problem downloading referenced file '" + file.fileName() + "' from URL " + o->url);
            }

        }

        // File is available now
        MainWin->setSpecificLabel("");
        qApp->processEvents();

        //Is this stl or ply?
        if (QString(o->file.right(4)).toUpper()==QString(".stl").toUpper())
        {
            vtkSTLReader *reader = vtkSTLReader::New();
            reader->SetFileName(QString(fpath + "/" + o->file).toLatin1());
            reader->Update();
            vtkPolyData *pd;
            pd=vtkPolyData::New();
            pd=reader->GetOutput();
            PolyData.append(pd);
            f+=(100.0/objects.count());
            MainWin->ui->ProgBarOverall->setValue((int)(f/2.0));
            qApp->processEvents();

            //now add some bytes from the STL to the hash - pick 50 bytes scattered through file.
            QFile stlfile((fpath + "/" + o->file).toLatin1());
            QFileInfo stlfi((fpath + "/" + o->file).toLatin1());
            stlfile.open(QIODevice::ReadOnly);
            int inc=stlfi.size()/50;
            for (int i=1; i<50; i++)
            {
                stlfile.seek(i*inc);
                char buffer;
                stlfile.read(&buffer,1);
                shasharray.append(buffer);
            }

        }
        else if (QString(o->file.right(4)).toUpper()==QString(".ply").toUpper())
        {
            vtkPLYReader *reader = vtkPLYReader::New();
            reader->SetFileName(QString(fpath + "/" + o->file).toLatin1());
            reader->Update();
            vtkPolyData *pd;
            pd=vtkPolyData::New();
            pd=reader->GetOutput();
            PolyData.append(pd);
            f+=(100.0/objects.count());
            MainWin->ui->ProgBarOverall->setValue((int)(f/2.0));
            qApp->processEvents();

            //now add some bytes from the PLY to the hash - pick 50 bytes scattered through file.
            QFile plyfile((fpath + "/" + o->file).toLatin1());
            QFileInfo plyfi((fpath + "/" + o->file).toLatin1());
            plyfile.open(QIODevice::ReadOnly);
            int inc=plyfi.size()/50;
            for (int i=1; i<50; i++)
            {
                plyfile.seek(i*inc);
                char buffer;
                plyfile.read(&buffer,1);
                shasharray.append(buffer);
            }

        }
        else xml_error("File '" + file.fileName() + "' is not STL or PLY format");

    }


    voxml_mode=true;

    //All OK - create the local objects
    //Create a 'dummy' spv
    SPV *spv= new SPV(0,0,0,0,0);
    SPVs.append(spv);
    spv->filename=title;
    i_classification_name=classification_name;
    i_classification_rank=classification_rank;
    i_provenance=provenance;
    i_specimen=specimen;
    i_author=author;
    i_comments=comments;
    i_reference=reference;

    NetModule n;
    QCryptographicHash h1(QCryptographicHash::Md5);
    h1.addData(shasharray);
    n.CheckHash(h1.result(),&i_comments);

    //store for messaging
    QString t(h1.result().toHex());
    STLHash=t;

    mm_per_unit=scale;


    for (int i=0; i<groups.count(); i++)
    {
        SVObject *svo = new SVObject(i);
        spv->ComponentObjects.append(svo);
        SVObjects.append(svo);
        svo->IsGroup=true;
        svo->Visible=groups[i]->visible;
        svo->Name=groups[i]->name;
        if (groups[i]->position!=-1) svo->Position=groups[i]->position;
        svo->Key=groups[i]->key;
    }
    //second pass to set up ingroups
    for (int i=0; i<groups.count(); i++)
    {   SVObject *svo=spv->ComponentObjects[i];
        svo->InGroup=find_group_from_name(spv,groups[i]->ingroup); //works for empty ingroup - will miss all and return -1
    }

    int firstrealobj=groups.count();

    f=0.0;

    //set em up
    for (int i=0; i<objects.count(); i++)
    {
        SVObject *svo = new SVObject(i+firstrealobj);
        spv->ComponentObjects.append(svo);
        SVObjects.append(svo);
         svo->IsGroup=false;
        svo->Visible=objects[i]->visible;
        svo->Name=objects[i]->name;
        if (objects[i]->position!=-1) svo->Position=objects[i]->position;
        svo->Key=objects[i]->key;
        svo->InGroup=find_group_from_name(spv,objects[i]->ingroup); //works for empty ingroup - will miss all and return -1
        svo->Colour[0]=objects[i]->red;
        svo->Colour[1]=objects[i]->green;
        svo->Colour[2]=objects[i]->blue;
        svo->Transparency=conv_trans(objects[i]->transparency);
        for (int j=0; j<16; j++) svo->matrix[j]=objects[i]->matrix[j];
        svo->spv=spv;
        svo->pd=PolyData[i];
    }

    //surface them. Two loops needed as matrices must be in place before I start buggering with this!
    for (int i=0; i<objects.count(); i++)
    {
        if (i==0) firstobject=true; else firstobject=false;
        SVObject *svo = SVObjects[(i+firstrealobj)];
        svo->MakeDlists();
        MainWin->UpdateGL();
        f+=(100.0/objects.count());
        MainWin->ui->ProgBarOverall->setValue(50+(int)(f/2.0));
    }

    //qDebug()<<"Final ` box is "<<minx<<maxx<<miny<<maxy<<minz<<maxz;

    //Do some clever stuff to determine shift/rescale etc for this

    globalmatrix.setToIdentity();



    //work out rescale
    qreal rescale=1;
    float biggestsize = qMax(maxx-minx,qMax(maxy-miny, maxz-minz));
    if (biggestsize>3 || biggestsize<1 ) rescale=3/((qreal)(biggestsize));


    //work out translate
    float midx=rescale*(maxx+minx)/2;
    float midy=rescale*(maxy+miny)/2;
    float midz=rescale*(maxz+minz)/2;

    globalmatrix.translate(0-midx,0-midy, 0.3-midz);
    globalmatrix.scale(rescale);
    globalrescale=rescale;
    //In theory now have a transform matrix to apply to EVERYTHING

   //qDebug()<<"Global matrix is "<<globalmatrix;
    return true;
}


int voxml::conv_trans(float t)
// do an approx convert from float trans to SPIERSview's 0-4 codes
{
    //
    t=1-t;
    if (t>.9) return 0;
    if (t>.5) return 1;
    if (t>.3) return 2;
    if (t>.1) return 3;
    return 4;
}

float voxml::conv_trans_export(int t)
{
    //do it the other way
    if (t==1) return .3;
    if (t==2) return .6;
    if (t==3) return .8;
    if (t==4) return .95;
}

int voxml::find_group_from_name(SPV *spv, QString name)
{
    for (int i=0; i<spv->ComponentObjects.count(); i++)
    {
        if (spv->ComponentObjects[i]->Name==name) return i;
    }
    return -1;
}

bool voxml::xml_error(QString ErrorText)
{
        QMessageBox::critical(0,
                              "VAXML reader",
                              "Error reading VAXML file: " + ErrorText,
                              QMessageBox::Ok);
        return false;
}

bool voxml::read_element(QXmlStreamReader *xml)
{
    if  (xml->hasError()) return false;

    QXmlStreamReader::TokenType token;
    do
    {
        token = xml->readNext();
    } while (token == QXmlStreamReader::StartDocument);

    if  (xml->hasError()) return false;

    return true;
}

QString voxml::encode(QString text)
{
    text=text.replace(QChar('&'),"&amp;");
    text=text.replace(QChar(39),"&apos;");
    text=text.replace(QChar(34),"&quot;");
    text=text.replace(QChar('<'),"&lt;");
    text=text.replace(QChar('>'),"&gt;");
    return text;
}

bool voxml::write_voxml(QString fname, bool mode) //mode true means this is part of finalilsed mode
{

   file.setFileName(fname);

   QFileInfo f(fname);
    QDir d;
   if (mode==false)
        d.mkpath(f.dir().absolutePath()+"/"+f.baseName()+"_stl");

    //check no repeated group names
    bool groups=false;
    foreach(SVObject *o, SVObjects)
    {
        if (o->IsGroup )
        {
            groups=true;
            foreach(SVObject *o2, SVObjects)
            {
                if (o->Name==o2->Name && o2!=o && o2->IsGroup)
                {
                    if (mode==true) QMessageBox::critical(0,
                                          "SPV save",
                                          "Finalised mode requires that all groups have unique names: please fix your group names before saving",
                                          QMessageBox::Ok);
                    else QMessageBox::critical(0,
                                                              "VAXML writer",
                                                              "VAXML requires that all groups have unique names: please fix your group names before exporting",
                                                              QMessageBox::Ok);
                    return false;
                }
            }

        }
    }

    foreach(SVObject *o, SVObjects)
    {
        if (o->Name.length()<1)
        {
            if (mode==true) QMessageBox::critical(0,
                                  "SPV save",
                                  "Finalised mode requires that all objects and groups have names: please name all items before saving",
                                  QMessageBox::Ok);
            else QMessageBox::critical(0,
                                  "VAXML writer",
                                  "VAXML requires that all objects and groups have names: please name all items before exporting",
                                  QMessageBox::Ok);
            return false;
        }
    }


    QString outstring;

    QTextStream out(&outstring);

    out<<"<?xml version='1.0' encoding=\"ISO-8859-1\" ?>\n";
    out<<"<vaxml>\n";
    out<<"<header>\n";
    out<<"<version>2</version>\n";

    if (i_title.count()==0) out<<"<title>" + SPVs[0]->filenamenopath + "</title>\n";
    else out <<"<title>" + encode(i_title[0]) + "</title>\n";

    out<<"<scale>"<<mm_per_unit<<"</scale>\n";

    foreach(QString comment, i_comments) out<<"<comments>"<<encode(comment)<<"</comments>\n";
    foreach(QString comment, i_reference) out<<"<reference>"<<encode(comment)<<"</reference>\n";
    foreach(QString comment, i_author) out<<"<author>"<<encode(comment)<<"</author>\n";
    foreach(QString comment, i_provenance) out<<"<provenance>"<<encode(comment)<<"</provenance>\n";
    foreach(QString comment, i_specimen) out<<"<specimen>"<<encode(comment)<<"</specimen>\n";
    for (int i=0; i<i_classification_name.count(); i++)
    {
        out<<"<classification><rank>"<<encode(i_classification_rank[i])<<"</rank><name>"<<encode(i_classification_name[i])<<"</name></classification>\n";
    }

    out<<"</header>\n";

    //do groups
    if (groups) out<<"<groups>\n";
    foreach(SVObject *o, SVObjects)
    {
        if (o->IsGroup)
        {
            out<<"<group>\n";
            out<<"<name>"<<encode(o->Name)<<"</name>\n";
            if (o->Key!=0) out<<"<key>"<<o->Key<<"</key>\n";
            if (o->Visible) out<<"<visible>1</visible>\n"; else out<<"<visible>0</visible>\n";
            if (o->InGroup!=-1) out<<"<ingroup>" + encode(SVObjects[o->Parent()]->Name) + "</ingroup>\n";
            out<<"<position>"<<o->Position<<"</position>\n";
            out <<"</group>\n";
        }
    }
    if (groups) out<<"</groups>\n";

    //now objects
    out<<"<objects>\n";
    foreach(SVObject *o, SVObjects)
    {
        if (!(o->IsGroup) && (o->Visible || MainWin->ui->actionExport_Hidden_Objects->isChecked()))
        {
            out<<"<object>\n";
            out<<"<name>"<<encode(o->Name)<<"</name>\n";
            if (o->Key!=0) out<<"<key>"<<o->Key<<"</key>\n";
            if (o->Visible) out<<"<visible>1</visible>\n"; else out<<"<visible>0</visible>\n";

            if (o->InGroup!=-1) out<<"<ingroup>" + encode(SVObjects[o->Parent()]->Name) + "</ingroup>\n";
            out<<"<position>"<<o->Position<<"</position>\n";


            QString fname2;
            fname2.sprintf("%d",o->Index+1);
            if (!(o->Name.isEmpty())) {fname2.append("-"); fname2.append(o->Name);}
            fname2.append(".stl");
            out<<("<file>"+encode(f.baseName() + "_stl/"+fname2)+"</file>\n");
            //DON'T write a matrix - should all be identity now, as export to STL matrixes them...
            //DON'T write a URL (obviously)
            //out<<"<matrix>";
            //for (int i=0; i<16; i++) out<<"<m"<<i<<">"<<o->matrix[i]<<"</m"<<i<<">";
            //out<<"</matrix>\n";

            out<<"<material>\n";
            out<<"<colour>";
            out<<"<red>"<<(int)(o->Colour[0])<<"</red>";
            out<<"<green>"<<(int)(o->Colour[1])<<"</green>";
            out<<"<blue>"<<(int)(o->Colour[2])<<"</blue>";
            out<<"</colour>\n";
            if (o->Transparency!=0)
            {
                float t=conv_trans_export(o->Transparency);
                out<<"<transparency>"<<conv_trans_export(o->Transparency)<<"</transparency>";
            }
            out<<"</material>\n";
            out <<"</object>\n";
        }
    }
    out<<"</objects>\n";

    out<<"</vaxml>\n";

    if (mode)
    {
        if (file.open(QIODevice::WriteOnly)==false)
        {
            QMessageBox::critical(0,
                                  "SPVF save",
                                  "Cannot open SPVF file for writing. Perhaps it exists and is write protected?",
                                  QMessageBox::Ok);
            return false;
        }
    }
    else
    {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)==false)
        {
            QMessageBox::critical(0,
                              "VAXML writer",
                              "Cannot open VAXML file for writing. Perhaps it exists and is write protected?",
                              QMessageBox::Ok);
            return false;
        }
    }

    if (mode)
    {
        dataout.setDevice(&file);
        dataout.setByteOrder(QDataStream::LittleEndian);
        dataout.setVersion(QDataStream::Qt_4_5);
        dataout<<outstring;
    }
    else
    {
        QTextStream t(&file);
        t<<outstring;
        file.close();
    }

    return true;
}
