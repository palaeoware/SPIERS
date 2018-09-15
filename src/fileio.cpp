/*********************************************

SPIERSedit 2: fileio.cpp

Contents:
Functions to load/save data from image files
Functions to load/save settings files
Function to apply default settings to all global variables
Also caching functions for file IO

**********************************************/

#include "fileio.h"
#include "globals.h"
#include "undo.h"
#include <QImage>
#include <QRgb>
#include <QFile>
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QThread>
#include <QBuffer>

QList <Cache *> Caches;

CacheGreyData::CacheGreyData()
{
    Data = 0;
    CompressedData = 0;
}

CacheGreyData::~CacheGreyData()
{
    //delete things - but DO check that greyarray isn't in use!
    if (CompressedData) delete CompressedData;
    if (Data)
    {
        bool flag = true;
        for (int i = 0; i < SegmentCount; i++)
            if (GA[i] == Data) flag = false;
        if (flag) delete Data;
    }
}

Cache::~Cache()
{
    //remove all my GA data, if any
    if (GreyData.count()) qDeleteAll(GreyData.begin(), GreyData.end()); //all internal deletion handled by destructor
}

Cache::Cache()
{
    Filenum = -1;
    ColData = 0;
    Locks = 0;
    Masks = 0;
    GreyData.clear();
    ColDataCompressed = 0;
    for (int i = 0; i < SegmentCount; i++)
        GreyData.append (new CacheGreyData); //create all my segment data structures
}

void Cache::CalcMySize()
{
    Size = 0;
    int csize, cmsize;
    Q_UNUSED(csize);
    Q_UNUSED(cmsize);
    if (ColData) Size += ColData->byteCount();
    if (ColDataCompressed) Size += ColDataCompressed->size();
    csize = Size;
    if (Locks) Size += Locks->size();
    if (Masks) Size += Masks->size();
    cmsize = Size;
    for (int i = 0; i < GreyData.count(); i++)
    {
        if (GreyData[i]->CompressedData) Size += (GreyData[i]->CompressedData->size());
        if (GreyData[i]->Data) Size += (GreyData[i]->Data->byteCount());
    }


}

void Cache::Blank(int fnum)
{
    if (ColData != 0) delete ColData;
    if (ColDataCompressed != 0) delete ColDataCompressed;
    if (Locks != 0) delete Locks;
    if (Masks != 0) delete Masks;
    qDeleteAll(GreyData.begin(), GreyData.end()); //all internal deletion handled by destructor

    GreyData.clear();

    for (int i = 0; i < SegmentCount; i++)
        GreyData.append (new CacheGreyData); //create all my segment data structures
    //recreate new GDs
    ColData = 0;
    Locks = 0;
    Masks = 0;
    ColDataCompressed = 0;
    Filenum = fnum;
    LastUsed = QDateTime::currentDateTime();
    Size = 0;
}

void InvalidateAllGreyCaches() //use after deleting a segment!
{
    mutex.lock();
    for (int i = 0; i < Caches.count(); i++)
    {
        qDeleteAll(Caches[i]->GreyData.begin(), Caches[i]->GreyData.end());
        Caches[i]->GreyData.clear();
        Caches[i]->CalcMySize();
    }
    mutex.unlock();
}

void ClearCache() //use after deleting a segment and similar circs
{
    mutex.lock();
    for (int i = 0; i < Caches.count(); i++) Caches[i]->Blank(-1);
    qDeleteAll(Caches.begin(), Caches.end());
    Caches.clear();
    mutex.unlock();
}

int CacheIndex(int fnum)
{
    //find cache index for this file
    for (int i = 0; i < Caches.count(); i++)
    {
        if (Caches[i]->Filenum == fnum)
        {
            Caches[i]->LastUsed = QDateTime::currentDateTime();
            return i;
        }
    }
    return -1; //not found
}


void CacheDebug(int i, QString message)
{
    qDebug() << message << " cache#" << i << " fnum" << Caches[i]->Filenum << "=" << Files[Caches[i]->Filenum] << "Size" << Caches[i]->Size;
    qDebug() << "  ColCompressed " << Caches[i]->ColDataCompressed << " ColData" << Caches[i]->ColData;
    qDebug() << "  Locks" << Caches[i]->Locks << "  Masks" << Caches[i]->Masks;
    qDebug() << "  ColData" << Caches[i]->ColDataCompressed;
    for (int j = 0; j < Caches[i]->GreyData.count(); j++)
        qDebug() << "    GD " << j << " Comp " << Caches[i]->GreyData[j]->CompressedData << " Norm" << Caches[i]->GreyData[j]->Data;
}

int GetCacheIndex(int fnum)
{
    //First - does this exist?

    int i = CacheIndex(fnum);

    if (i >= 0)
    {
        return i;
    }

    //if not, let's create a new entry if space, or use oldest otherwise
    long long int SizeTotal = 0;
    for (int j = 0; j < Caches.count(); j++)
        SizeTotal += (long long int) Caches[j]->Size;

    if (SizeTotal < ((long long int)CacheMem * (long long int)(1024 * 1024)))
    {
        Caches.append(new Cache);
        Caches[Caches.count() - 1]->Blank(fnum);

        //CacheDebug(Caches.count()-1,"Made a new cache entry");
        return Caches.count() - 1;
    }
    else //use an old one
    {
        QDateTime oldest = QDateTime::currentDateTime();
        int bestindex = -1;
        //qDebug()<<Caches.count();
        for (i = 0; i < Caches.count(); i++)
        {
            if (Caches[i]->Filenum == -1) //empty
            {
                Caches[i]->Blank(fnum);
                return i;
            }
            if (Caches[i]->LastUsed <= oldest)
            {
                oldest = Caches[i]->LastUsed;
                bestindex = i;
            }
        }
        if (bestindex == -1) Error ("Didn't find a cache item to reuse - should never see this message");
        Caches[bestindex]->Blank(fnum);
        return bestindex;
    }
}

int Counter;
int Errors;


void LoadAllData(int fnum)
//Load all info for currentfile into arrays
{
    QTime t;
    t.start();
    int n;
    if (fnum < 0)
    {
        fnum += 10000;
        LoadColourData(fnum);

        for (n = 0; n < SegmentCount; n++) LoadGreyData(fnum, n);

        LoadMasks(fnum);

        LoadLocks(fnum);
        return;
    }


    LoadColourData(fnum);

    for (n = 0; n < SegmentCount; n++) LoadGreyData(fnum, n);

    LoadMasks(fnum);

    LoadLocks(fnum);

    return;
}

bool SimpleLoadColourData(QString fname)
{

    QImage Data(fname); //load
    if (Data.isNull()) Error("Couldn't load source data file in SimpleLoadColourData " + fname);
    if (Data.width() != cwidth || Data.height() != cheight)
    {
        Error ("Source file has wrong dimensions - all source images dimensions need to be the same ");
    }

    int d = Data.format();
    if (d != QImage::Format_RGB32 && d != QImage::Format_ARGB32 && d != QImage::Format_Indexed8)
        Error("File is not in a valid format");
    else ColArray = Data;
    return true;
}

void LoadColourData(int fnum)
//Get info from colour file
{
    mutex.lock();
    //check cache
    int i;
    if (RenderCache) goto past;

    i = CacheIndex(fnum);
    if (i >= 0)
    {
        //Use cache copy
        if (CacheCompressionLevel > 0)
        {
            if (Caches[i]->ColDataCompressed != 0)
            {
                ColArray.loadFromData(*(Caches[i]->ColDataCompressed));
            }
            else goto past;
        }
        else
        {
            if (Caches[i]->ColData != 0)
            {
                ColArray = *(Caches[i]->ColData);
            }
            else goto past;
        }
    }
    else
    {
past:  //so can get here with a valid cache entry but no colour file
//      qDebug()<<"Didn't find cache entry, loading from disk";

        QImage Data(Files.at(fnum)); //load
        if (Data.isNull())
            Error("Couldn't load source data file in LoadColourData: " + Files.at(fnum));
        if (Data.width() != cwidth || Data.height() != cheight)
        {
            Error ("Source file has wrong dimensions - all source images dimensions need to be the same ");
        }



        int d = Data.format();
        if (d != QImage::Format_RGB32 && d != QImage::Format_ARGB32 && d != QImage::Format_Indexed8)
            Error("File is not in a valid format");
        else ColArray = Data;

        if (!RenderCache)
        {
            //Store a copy in cache
            int newcache = GetCacheIndex(fnum); //gets index (may be new) for this to be cached in
            //store data
            if (CacheCompressionLevel == 0) //no compression -stash data
            {
                if (Caches[newcache]->ColData == 0) Caches[newcache]->ColData = new QImage(Data); //if existing will be correct!
                if (Caches[newcache]->ColDataCompressed) delete Caches[newcache]->ColDataCompressed;
            }
            else
            {
                if (Caches[newcache]->ColData) delete Caches[newcache]->ColData; //delete any full sized version
                if (Caches[newcache]->ColDataCompressed == 0)
                {
                    Caches[newcache]->ColDataCompressed = new QByteArray;
                    QBuffer buffer(Caches[newcache]->ColDataCompressed);
                    buffer.open(QIODevice::WriteOnly);
                    if (CacheCompressionLevel == 1)
                        ColArray.save(&buffer, "PNG", 99); // writes image into ba in PNG format
                    else
                        ColArray.save(&buffer, "JPG", 100); //store as jpeg if on high compression!
                }
            }
            Caches[newcache]->CalcMySize();
        }
    }

    if (ColArray.format() == QImage::Format_Indexed8) GreyImage = true;
    else GreyImage = false;
    cwidth4 = cwidth;
    if (GreyImage) if ((cwidth % 4) != 0) cwidth4 = (cwidth / 4) * 4 + 4;

    mutex.unlock();
}

bool SimpleLoadGreyData(int fnum, int seg, QImage *greydata)
{

    int lastsep, lastdot;
    QString Fname = FullFiles.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp = "/" + SettingsFileName + "/" + "s";
    QString t2;
    t2.sprintf("%d_", seg + 1);
    temp.append(t2);
    temp.append(actfn);
    sfname.append(temp);

    if (QFile(sfname + ".bmp").exists()) sfname += ".bmp";
    else if (QFile(sfname + ".png").exists()) sfname += ".png";
    //Load it from disk

    QFile f(sfname);
    if ((f.exists()))
    {
        greydata->load(sfname);
        return true;
    }
    else return false;

}

void LoadGreyData(int fnum, int seg)
//Get info from segment file
{
    //qDebug()<<"In LGD, file"<<fnum<<"seg"<<seg<<"GA count"<<GA.count();

    while (GA.count() <= seg) GA.append((QImage *)0); //put in a blank if we don't have enough!
    //qDebug()<<"In LGD, file"<<fnum<<"seg"<<seg<<"GA count"<<GA.count();
    //check cache
    mutex.lock();
    int i = CacheIndex(fnum);

    if (i >= 0)
    {
        //qDebug()<<"Using cache copy";
        //Use cache copy
        if (CacheCompressionLevel > 0)
        {
            //use PNG version
            //qDebug()<<"PNG";
            if (Caches[i]->GreyData[seg]->CompressedData)
            {
                if (GA[seg] == 0) GA[seg] = new QImage;
                GA[seg]->loadFromData(*(Caches[i]->GreyData[seg]->CompressedData));
                mutex.unlock();
                //qDebug()<<"ret from PNG";
                return;
            }
        }
        else //uncompressed
        {
            //qDebug()<<"uncom";
            if (Caches[i]->GreyData[seg]->Data) //there IS data
            {
                if (GA[seg] == 0) GA[seg] = new QImage;
                GA[seg] = Caches[i]->GreyData[seg]->Data;
                mutex.unlock();
                //qDebug()<<"Got cached copy, returning";
                //qDebug()<<GA[seg];
                //qDebug()<<GA[seg]->size();

                return;
            }
        }
        //qDebug()<<"Shouldn't see this"; Error("Cache error");
    }

    //qDebug()<<"Past cache";
    int lastsep, lastdot;
    QString Fname = Files.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp = "/" + SettingsFileName + "/" + "s";
    QString t2;
    t2.sprintf("%d_", seg + 1);
    temp.append(t2);
    temp.append(actfn);
    sfname.append(temp);

    char tbuff[5];
    Q_UNUSED(tbuff);
    if (QFile(sfname + ".bmp").exists())
    {
        sfname += ".bmp";
        tbuff[0] = 'B';
        tbuff[1] = 'M';
        tbuff[2] = 'P';
        tbuff[3] = 0;
    }
    else if (QFile(sfname + ".png").exists())
    {
        sfname += ".png";
        tbuff[0] = 'B';
        tbuff[1] = 'M';
        tbuff[2] = 'P';
        tbuff[3] = 0;
    }
    //Load it from disk

    //qDebug()<<"Loading a file - "<<sfname;
    QFile f(sfname);
    //qDebug()<<"sfname"<<f.exists();
    if ((f.exists()))
    {
        GA[seg] = new QImage;

        bool r = GA[seg]->load(sfname);

//                qDebug()<<"Loaded "<<sfname<<"- success is "<<r;
        //if it fails - just try again!
        while (r == false || GA[seg]->isNull())
        {
            //qDebug()<<"In warning";
            QMessageBox::warning(mainwin, "Problem opening file", "Couldn't load segment image, oh dear!  " + sfname + "\nWill try again - perhaps you can fix the file?");
            r = GA[seg]->load(sfname);
            //qDebug()<<"Tried again, now OK. tbuff is "<<tbuff;
        }
    }
    else
    {
        //qDebug()<<"It didn't exist - creating";
        GA[seg] = new QImage(fwidth, fheight, QImage::Format_Indexed8);
        //set up the CLUT
        QVector <QRgb> clut(256);

        for (int i = 0; i < 256; i++)
            clut[i] = qRgb(i, i, i);
        GA[seg]->setColorTable(clut);
    }


    int newcache = GetCacheIndex(fnum); //gets index (may be new) for this to be cached in
    //add new entries for segments

    if (CacheCompressionLevel == 0)
    {
//                qDebug()<<"Putting into cache... item "<<newcache;
        Caches[newcache]->GreyData[seg]->Data = GA[seg];
//                qDebug()<<"Stashed OK";
//                qDebug()<<"Stashed OK"<<GA[seg]->size();
    }
    else
    {
//                qDebug()<<"Putting into cache (compressed)... item "<<newcache;
        Caches[newcache]->GreyData[seg]->CompressedData = new QByteArray;
        QBuffer buffer(Caches[newcache]->GreyData[seg]->CompressedData);
        buffer.open(QIODevice::WriteOnly);
        GA[seg]->save(&buffer, "PNG"); // writes image into ba in PNG format
    }
    Caches[newcache]->CalcMySize();
    mutex.unlock();
}

/*
void OldLoadGreyData(int fnum, int seg)
//Get info from segment file
{
    {
        //check cache
        int i=CacheIndex(fnum);

        if (i>=0)
        {
            CacheDebug(i,"Found cache for this file (reload greys)");
            //Use cache copy
            if (Caches[i]->GreyData.count()>seg) //i.e. if there are enough entries - should always be true
                {
                    if (Caches[i]->GreyData[seg]>0) //i.e. if there's an object there - ditto
                    {
                        bool found=false;
                        if (Caches[i]->GreyData[seg]->Compressed==false && Caches[i]->GreyData[seg]->Data)  //not compressed, data exists
                        {
                            qDebug()<<"Loading a cached copy"<<GAdelete[seg]<<GA[seg];
                            if (GAdelete[seg]) if (GA[seg]) delete GA[seg];
                            GA[seg]->load(Files[fnum]);
                            GA[seg] = Caches[i]->GreyData[seg]->Data;
                            Caches[i]->GreyData[seg]->Compressed=false;
                            GAdelete[i]=false;
                            found=true;
                            goto past;
                        }
                        else
                        if (Caches[i]->GreyData[seg]->Compressed==true && Caches[i]->GreyData[seg]->CompressedData)//compressed data
                        {
                            if (GAdelete[seg])  if (GA[seg]) delete GA[seg];
                            if (Caches[i]->GreyData[seg]->Data) delete Caches[i]->GreyData[seg]->Data;  //delete any uncomp data
                            GA[seg]=new QImage;
                            GA[seg]->loadFromData(*(Caches[i]->GreyData[seg]->CompressedData));
                            Caches[i]->GreyData[seg]->Compressed=true;
                            GAdelete[i]=true;
                            found=true;
                        }
                        if (found==false) goto past; //bodgy structure or what!
                    }
                     else goto past;
                } //isn't an entry after all
                else goto past;
        }
        else
        {
    past:  //so can get here with a valid cache entry but no grey file
            qDebug()<<"Didn't find cache entry, loading from disk";

            int lastsep, lastdot;
            QString Fname = Files.at(fnum);
            lastsep=Fname.lastIndexOf("\\");  //this is last separator in path
            lastdot=Fname.lastIndexOf(".");
            QString sfname=Fname.left(lastsep);
            QString actfn=Fname.mid(lastsep+1, lastdot-lastsep-1);
            QString temp = "/" + SettingsFileName + "/" + "s";
            QString t2; t2.sprintf("%d_",seg+1);
            temp.append(t2); temp.append(actfn);
            sfname.append(temp);

            if (QFile(sfname+".bmp").exists()) sfname+=".bmp";
            else if (QFile(sfname+".png").exists()) sfname+=".png";
            //Load it from disk
            QImage *Data;

            QFile f(sfname);
            if ((f.exists()))
            {
                Data = new QImage(sfname);
            }
            else
            {
                Data = new QImage(fwidth, fheight, QImage::Format_Indexed8);
                //set up the CLUT
                QVector <QRgb> clut(256);

                for (int i=0; i<256; i++)
                    clut[i]=qRgb(i,i,i);
                Data->setColorTable(clut);
            }

            if (Data->isNull()) Error("Couldn't load segment image, oh dear!  " + sfname);
            //if (GAdelete[seg]) delete GA[seg];
            //temp code
            //if (GA[seg]) delete GA[seg];
            GA[seg] = Data; //assign to seg array

            //Store in cache
            int newcache=GetCacheIndex(fnum); //gets index (may be new) for this to be cached in
            //add new entries for segments

            if (CacheCompressionLevel==0)
            {
                //delete any compressed data
                if ( Caches[newcache]->GreyData[seg]->CompressedData) {qDebug()<<"Deleting some compressed data";delete Caches[newcache]->GreyData[seg]->CompressedData;}
                //shouldn't be any uncomp data at all!
                if (Caches[newcache]->GreyData[seg]->Data) {qDebug ("Found  data in loadgrey - shouldn't be any!"); delete Caches[newcache]->GreyData[seg]->Data;}

                Caches[newcache]->GreyData[seg]->Data= Data; //Store copy of this data
                GAdelete[seg]=false; //there's a cache copy of this, so don't delete it!
                Caches[newcache]->GreyData[seg]->Compressed=false;
            }
            else
            {
                //delete any uncompressed data - unless it's also pointer to by a GA? A sanity check!
                if ( Caches[newcache]->GreyData[seg]->Data)
                {
                    for (int j=0; j<SegmentCount; j++) if (GA[j]==Caches[j]->GreyData[seg]->Data) goto past2;
                    delete Caches[newcache]->GreyData[seg]->Data;
                }//shouldn't be any uncomp data at all!
past2:
                if (Caches[newcache]->GreyData[seg]->CompressedData) Error ("Found  data in loadgrey(comp) - shouldn't be any!");

                Caches[newcache]->GreyData[seg]->CompressedData=new QByteArray;
                QBuffer buffer(Caches[newcache]->GreyData[seg]->CompressedData);
                buffer.open(QIODevice::WriteOnly);
                Data->save(&buffer, "PNG"); // writes image into ba in PNG format
                GAdelete[seg]=true; //there's no cache copy of this, so DO delete it when replacing!
                Caches[newcache]->GreyData[seg]->Compressed=true;
            }
            Caches[newcache]->CalcMySize();

        }
    }
    mutex.unlock();
}
*/
void WriteAllData(int fnum)
{
    //save all data for this slice
    for (int i = 0; i < SegmentCount; i++)
    {
        if (Segments[i]->Dirty) SaveGreyData(fnum, i);
        Segments[i]->Dirty = false;
    }
    if (MasksDirty) SaveMasks(fnum);
    if (LocksDirty) SaveLocks(fnum);
    MasksDirty = false;
    LocksDirty = false;

//  for (int i=0; i<Caches.count(); i++)
//       CacheDebug(i,"");
}

void SimpleSaveGreyData(int fnum, int seg, QImage greydata)
{
    int lastsep, lastdot;
    QString Fname = FullFiles.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp = "/" + SettingsFileName + "/" + "s";
    QString t2;
    t2.sprintf("%d_", seg + 1);
    temp.append(t2);
    temp.append(actfn);
    sfname.append(temp);

    QString sfnamebmp = sfname + ".bmp";
    QString sfnamepng = sfname + ".png";

    //now write the data - autoguesses format, so this should do it!
    if (FileCompressionLevel == 0)
    {
        greydata.save(sfnamebmp);
        QFile(sfnamepng).remove();
    }
    else
    {
        greydata.save(sfnamepng);
        QFile(sfnamebmp).remove();
    }
}

void SaveGreyData(int fnum, int seg)
{
    FilesDirty[fnum] = true;

    mutex.lock();
    int lastsep, lastdot;

    QString Fname = Files.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp = "/" + SettingsFileName + "/" + "s";
    QString t2;
    t2.sprintf("%d_", seg + 1);
    temp.append(t2);
    temp.append(actfn);
    sfname.append(temp);

    QString sfnamebmp = sfname + ".bmp";
    QString sfnamepng = sfname + ".png";

    //now write the data - autoguesses format, so this should do it!
    if (FileCompressionLevel == 0)
    {
        GA[seg]->save(sfnamebmp);
        QFile(sfnamepng).remove();
    }
    else
    {
        GA[seg]->save(sfnamepng);
        QFile(sfnamebmp).remove();
    }

    //and insert into cache
    int newcache = GetCacheIndex(fnum); //gets index (may be new) for this to be cached in

    //Caching
    if (CacheCompressionLevel == 0)
    {
        //if nothing stored, set to the GA array (if changed setting maybe)
        if (Caches[newcache]->GreyData[seg]->Data == 0)
        {
            Caches[newcache]->GreyData[seg]->Data = GA[seg];
        }
        else if (Caches[newcache]->GreyData[seg]->Data != GA[seg])    Error("Oh dear, cache screw up");
    }
    else
    {
        //compressed storage - need to actually compress/store the data
        if (Caches[newcache]->GreyData[seg]->CompressedData)
            delete Caches[newcache]->GreyData[seg]->CompressedData;

        Caches[newcache]->GreyData[seg]->CompressedData = new QByteArray;
        QBuffer buffer(Caches[newcache]->GreyData[seg]->CompressedData);
        buffer.open(QIODevice::WriteOnly);
        GA[seg]->save(&buffer, "PNG"); // writes image into ba in PNG format
    }
    Caches[newcache]->CalcMySize();
    mutex.unlock();
}

void LoadMasks(int fnum)
//Load the masks as a byte array
{
//  qDebug()<<"In Load Masks"<<fnum;
    mutex.lock();

    //check cache
    int i = CacheIndex(fnum);
    if (i >= 0)
    {
        //Use cache copy
        if (Caches[i]->Masks != 0) //i.e. if there is a cached maskre enough entries
        {

            if (CacheCompressionLevel > 0)
            {
                Masks = qUncompress(*(Caches[i]->Masks));
            }
            else
            {
                Masks = *(Caches[i]->Masks);   //a physical copy
            }


        } //isn't an entry after all
        else goto past;
    }
    else
    {
past:  //so can get here with a valid cache entry but no grey file

        int lastsep, lastdot;

        QString Fname = Files.at(fnum);
        lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
        lastdot = Fname.lastIndexOf(".");
        QString sfname = Fname.left(lastsep);
        QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
        QString temp;
        temp.append("/" + SettingsFileName + "/m_");
        temp.append(actfn);
        temp.append(".bmp");
        sfname.append(temp);

        QFile file(sfname);
        QByteArray *m;
        m = new QByteArray;
        if (!(file.exists()) || file.size() == 0)
        {
            m->resize(fwidth * fheight);
            for (int i = 0; i < fwidth * fheight; i++) (*m)[i] = 0;
        }
        else
        {
            if (!file.open(QIODevice::ReadOnly)) Error("Can't open masks file");
            //Use read to read directly into the byte array
            *m = file.readAll();
        }
        if (m->size() == fwidth * fheight) //no compression
            Masks = *m;
        else
        {
            Masks = qUncompress(*m);
        }

        //Store a copy in cache
        int newcache = GetCacheIndex(fnum); //gets index (may be new) for this to be cached in
        if (newcache != -1) if (Caches[newcache]->Masks != 0) delete  Caches[newcache]->Masks; //delete any old one


        //OK, compression scheme here - if I LOADED a compressed file, store in ram with this level (whatever it is)
        //Otherwise store the uncompressed version
        if (CacheCompressionLevel == 0)
        {
            Caches[newcache]->Masks = new QByteArray(Masks);
            delete m;
        }
        else
        {
            if (m->size() != fwidth * fheight) //compressed file - stash it
            {
                Caches[newcache]->Masks = m;
            }
            else
            {
                //use existing compression array
                QByteArray *m2 = new QByteArray;
                *m2 = qCompress(*m, CacheCompressionLevel);
                Caches[newcache]->Masks = m2;
                delete m;
            }
        }
        Caches[newcache]->CalcMySize();
    }
    mutex.unlock();
}

void SaveMasks(int fnum)
//Save the masks as a byte array
{
    mutex.lock();
    FilesDirty[fnum] = true;
    int lastsep, lastdot;

    QString Fname = Files.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp;
    temp.append("/" + SettingsFileName + "/m_");
    temp.append(actfn);
    temp.append(".bmp");
    sfname.append(temp);

    QFile file(sfname);
    if (!file.open(QIODevice::WriteOnly)) Error("Can't open masks file to write");
    QByteArray *OutData;
    OutData = new QByteArray;
    if (FileCompressionLevel > 0)
    {
        *OutData = qCompress(Masks, FileCompressionLevel);
        file.write(OutData->data(), (qint64)OutData->size());
    }
    else file.write(Masks.data(), (qint64)Masks.size());

    //and insert into cache
    int newcache = GetCacheIndex(fnum); //gets index (may be new) for this to be cached in
    if (newcache != -1) if (Caches[newcache]->Masks != 0) delete  Caches[newcache]->Masks; //delete any old one

    //OK, work out what to do with compression
    if (CacheCompressionLevel == 0)
    {
        Caches[newcache]->Masks = new QByteArray(Masks);
        delete OutData;
    }
    else if (CacheCompressionLevel > 0 && CacheCompressionLevel != FileCompressionLevel)
    {
        //have to do my own compression
        Caches[newcache]->Masks = new QByteArray(qCompress(Masks, CacheCompressionLevel));
        delete OutData;
    }
    else
    {
        //use existing compression array
        Caches[newcache]->Masks = OutData; //move pointer, no need to create new object at all
    }
    Caches[newcache]->CalcMySize();
    mutex.unlock();
}

void SimpleSaveLocks(int fnum, QByteArray array)
{
    QString Fname = FullFiles.at(fnum);
    int lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    int lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp;
    temp.append("/" + SettingsFileName + "/l_");
    temp.append(actfn);
    temp.append(".bmp");
    sfname.append(temp);

    QFile file(sfname);
    if (!file.open(QIODevice::WriteOnly)) Error("Can't open locks file for writing");

    QByteArray *OutData;
    OutData = new QByteArray;
    if (FileCompressionLevel > 0)
    {
        *OutData = qCompress(array, FileCompressionLevel);
        file.write(OutData->data(), (qint64)OutData->size());
    }
    else file.write(array.data(), (qint64)array.size());
    delete OutData;
}

void SimpleSaveMasks(int fnum, QByteArray array)
{
    QString Fname = FullFiles.at(fnum);
    int lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    int lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp;
    temp.append("/" + SettingsFileName + "/m_");
    temp.append(actfn);
    temp.append(".bmp");
    sfname.append(temp);

    QFile file(sfname);
    if (!file.open(QIODevice::WriteOnly)) Error("Can't open masks file for writing");

    QByteArray *OutData;
    OutData = new QByteArray;
    if (FileCompressionLevel > 0)
    {

        *OutData = qCompress(array, FileCompressionLevel);

        file.write(OutData->data(), (qint64)OutData->size());
    }
    else
    {
        file.write(array.data(), (qint64)array.size());
    }
    delete OutData;
}


bool SimpleLoadLocks(int fnum, int expectedsize, QByteArray *array)
{
    int lastsep, lastdot;

    QString Fname = FullFiles.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp;
    temp.append("/" + SettingsFileName + "/l_");
    temp.append(actfn);
    temp.append(".bmp");
    sfname.append(temp);

    QFile file(sfname);
    if (!(file.exists()) || file.size() == 0)
    {
        return false;
    }
    else
    {
        if (!file.open(QIODevice::ReadOnly)) Error("Can't open locks file");
        //Use read to read directly into the byte array

        (*array) = file.readAll();//these are short ints irritatingly
    }
    if (array->size() != expectedsize) *array = qUncompress(*array);

    return true;
}

bool SimpleLoadMasks(int fnum, int expectedsize, QByteArray *array)
{
    int lastsep, lastdot;

    QString Fname = FullFiles.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp;
    temp.append("/" + SettingsFileName + "/m_");
    temp.append(actfn);
    temp.append(".bmp");
    sfname.append(temp);

    QFile file(sfname);
    if (!(file.exists()) || file.size() == 0)
    {
        return false;
    }
    else
    {
        if (!file.open(QIODevice::ReadOnly)) Error("Can't open masks file");
        //Use read to read directly into the byte array

        (*array) = file.readAll();//these are short ints irritatingly
    }

    if (array->size() != expectedsize) *array = qUncompress(*array);
    return true;
}

void LoadLocks(int fnum)
//Load the locks as a byte array
{
    mutex.lock();
    //qDebug()<<"LL";
    //check cache
    int i;
    if (RenderCache) goto past;
    i = CacheIndex(fnum);
    if (i >= 0)
    {
        //Use cache copy
        if (Caches[i]->Locks != 0) //i.e. if there is a cached maskre enough entries
        {
            if (CacheCompressionLevel > 0)
            {
                Locks = qUncompress(*(Caches[i]->Locks));
            }
            else
            {
                Locks = *(Caches[i]->Locks);   //a physical copy
            }
        } //isn't an entry after all
        else goto past;
    }
    else
    {
past:  //so can get here with a valid cache entry but no grey file
        //qDebug()<<"Didn't find cache entry, loading from disk";

        int lastsep, lastdot;

        QString Fname = Files.at(fnum);
        lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
        lastdot = Fname.lastIndexOf(".");
        QString sfname = Fname.left(lastsep);
        QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
        QString temp;
        temp.append("/" + SettingsFileName + "/l_");
        temp.append(actfn);
        temp.append(".bmp");
        sfname.append(temp);

        QFile file(sfname);
        QByteArray *m;
        m = new QByteArray;
        if (!(file.exists()) || file.size() == 0)
        {
            m->resize(fwidth * fheight * 2);
            for (int i = 0; i < (fwidth * fheight * 2); i++) (*m)[i] = 0;
            //qDebug()<<"H1";
        }
        else
        {
            if (!file.open(QIODevice::ReadOnly)) Error("Can't open locks file");
            //Use read to read directly into the byte array

            (*m) = file.readAll();//these are short ints irritatingly
            //qDebug()<<"H2";
        }

        //m is now locks data
        if (m->size() == fwidth * fheight * 2) //no compression
            Locks = (*m);
        else
        {
            Locks = qUncompress(*m);
        }

        if (!RenderCache)
        {
            //qDebug()<<"H3-storing";
            //Store a copy in cache
            int newcache = GetCacheIndex(fnum); //gets index (may be new) for this to be cached in
            if (newcache != -1) if (Caches[newcache]->Locks != 0) delete  Caches[newcache]->Locks; //delete any old one

            //OK, compression scheme here - if I LOADED a compressed file, store in ram with this level (whatever it is)
            //Otherwise store the uncompressed version
            if (CacheCompressionLevel == 0)
            {
                //qDebug()<<"H4";
                Caches[newcache]->Locks = new QByteArray(Locks);
                delete m;
            }
            else
            {
                //qDebug()<<"H5";
                if (m->size() != fwidth * fheight * 2) //compressed file - stash it
                {
                    Caches[newcache]->Locks = m;
                    //qDebug()<<"H6";
                }
                else
                {
                    //use existing compression array
                    QByteArray *m2 = new QByteArray;
                    *m2 = qCompress(*m, CacheCompressionLevel);
                    Caches[newcache]->Locks = m2;
                    delete m;
                    //qDebug()<<"H7";
                }
            }
            Caches[newcache]->CalcMySize();
        }
        else
        {
            //
            //qDebug()<<"H8";

            delete m;
        }
    }
    mutex.unlock();

}

void SaveLocks(int fnum)
{
    mutex.lock();
    int lastsep, lastdot;

    QString Fname = Files.at(fnum);
    lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    lastdot = Fname.lastIndexOf(".");
    QString sfname = Fname.left(lastsep);
    QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
    QString temp;
    temp.append("/" + SettingsFileName + "/l_");
    temp.append(actfn);
    temp.append(".bmp");
    sfname.append(temp);

    QFile file(sfname);
    if (!file.open(QIODevice::WriteOnly)) Error("Can't open locks file for writing");

    QByteArray *OutData;
    OutData = new QByteArray;
    if (FileCompressionLevel > 0)
    {
        *OutData = qCompress(Locks, FileCompressionLevel);
        file.write(OutData->data(), (qint64)OutData->size());
    }
    else
        file.write(Locks.data(), (qint64)Locks.size());

    if (!RenderCache)
    {
        //Store a copy in cache
        int newcache = GetCacheIndex(fnum); //gets index (may be new) for this to be cached in
        if (newcache != -1) if (Caches[newcache]->Locks != 0) delete  Caches[newcache]->Locks; //delete any old one
        //OK, work out what to do with compression
        if (CacheCompressionLevel == 0)
        {
            Caches[newcache]->Locks = new QByteArray(Locks);
            delete OutData;
        }
        else if (CacheCompressionLevel > 0 && CacheCompressionLevel != FileCompressionLevel)
        {
            //have to do my own compression
            Caches[newcache]->Locks = new QByteArray(qCompress(Locks, CacheCompressionLevel));
            delete OutData;
        }
        else
        {
            //use existing compression array
            Caches[newcache]->Locks = OutData; //move pointer, no need to create new object at all
        }
        Caches[newcache]->CalcMySize();
    }
    mutex.unlock();
}


void ApplyDefaultSettings()
{
    Notes = "";
    ThreeDmode = false;
    yaw = 0;
    pitch = 0;
    roll = 0;
    zsparsity = 1;
    GreyImage = false;
    Stretches.clear();
    ShowSlicePosition = false;
    bodgeflag = false;
    HorribleBodgeFlagDontStoreUndo = false;
    qDeleteAll(Segments.begin(), Segments.end());
    OutputObjects.clear();
    qDeleteAll(MasksSettings.begin(), MasksSettings.end());
    MasksSettings.clear();
    qDeleteAll(Curves.begin(), Curves.end());
    Curves.clear();
    qDeleteAll(OutputObjects.begin(), OutputObjects.end());
    OutputObjects.clear();
    qDeleteAll(UndoEvents.begin(), UndoEvents.end());
    UndoEvents.clear();
    qDeleteAll(RedoEvents.begin(), RedoEvents.end());
    RedoEvents.clear();
    TotalUndoSize = 0;
    Segments.clear();
    MasksSettings.clear();
    Curves.clear();
    OutputObjects.clear();
    OutputObjectsCount = 0;
    LastMouseX = -1; //no cursor yet
    SquareBrush = true;
    CurrentFile = 0;
    CurrentZoom = 1;
    //ColMonoScale=2;
    CMax = 255;
    CMin = 0;
    Trans = 0;
    if (QThread::idealThreadCount () > 1) BackgroundCacheFilling = true;
    else BackgroundCacheFilling = false;
    SegmentCount = 1;
    Segments.append(new Segment("Default"));
    Segments[0]->Colour[0] = 255;
    Segments[0]->Colour[1] = 255;
    Segments[0]->Colour[2] = 255;


    CurrentMode = 0;
    CurrentSegment = 0;
    CurrentRSegment = -2;
    Brush_Size = 5;
    BrushY = Brush_Size, BrushZ = Brush_Size;
    BrightUp = 10;
    BrightDown = 10;
    BrightSoft = 0;
    LastTrans = 0;
    ThreshFlag = true;
    MasksFlag = false;
    SegsFlag = false;
    SampleArraySize = 0;
    MaxUsedMask = 0;
    MasksSettings.append(new Mask("Background"));
    MasksSettings[0]->ForeColour[0] = 255;
    MasksSettings[0]->ForeColour[1] = 255;
    MasksSettings[0]->ForeColour[2] = 255;
    MasksSettings[0]->BackColour[0] = 0;
    MasksSettings[0]->BackColour[1] = 0;
    MasksSettings[0]->BackColour[2] = 0;
    MasksSettings[0]->Contrast = 4;

    SelectedMask = 0;
    SelectedRMask = 0;
    SelectedCurve = -1;
    CurveCount = 0;

    PixPerMM = 1;
    SlicePerMM = 1;
    SkewDown = 0;
    SkewLeft = 0;
    FirstOutputFile = 0;
    LastOutputFile = FileCount - 1;
    MaxTriangles = 4000;
    RangeHardFill = false;
    RangeSelectedOnly = false;
    OutputMirroring = false;
    HiddenMasksLockedForGeneration = false;
    SegmentBrushAppliesMasks = false;
    CurveShapeLocked = false;
    SegmentsLocked = false;
    CurveMarkersAsCrosses = false;
    CurveShapeLocked = false;
    PixSens = 1;
    XYDownsample = 1;
    ZDownsample = 1;

    MasksDirty = false;
    LocksDirty = false;
    CurvesDirty = false;
    MasksUndoDirty = false;
    LocksUndoDirty = false;
    CurvesUndoDirty = false;


    MenuGenChecked = true;
    MenuMasksChecked = true;
    MenuSegsChecked = false;
    MenuCurvesChecked = false;
    MenuOutputChecked = false;
    MenuToolboxChecked = true;
    MenuSliceSelectorChecked = true;
    MenuHistSelectedOnly = false;
    MenuHistChecked = false;
}


int GetShort(QDataStream *in)
{
    qint16 val;
    * in >> val;
    Counter += 2;
    if (in->status())
    {
        qDebug() << "getshort" << in->status();
        Errors++;
    }
    return (int) val;
}

int GetInt(QDataStream *in)
{
    qint32 val;
    *in >> val;           // extract int - hope endianness is correct!
    Counter += 4;
    if (in->status())
    {
        qDebug() << "getint" << in->status();
        Errors++;
    }
    return val;
}

int GetBool(QDataStream *in)
{
    qint16 val;
    *in >> val;           // extract int - hope endianness is correct!
    Counter += 2;
    if (in->status())
    {
        qDebug() << "getbool" << in->status();
        Errors++;
    }
    if (val < 0) return true;
    else return false;
}

int GetVariantInt(QDataStream *in)
{
    int test;
    test = GetShort(in);
    if (test == 0)
    {
        if (in->status()) Errors++;
        return 0;
    }
    else
    {
        test = GetShort(in);
        if (in->status())
        {
            qDebug() << "getvariantint" << in->status();
            Errors++;
        };
        return test;
    }
}

double GetDouble(QDataStream *in)
{
    double test;
    *in >> test;
    Counter += 8;
    if (in->status())
    {
        qDebug() << "getdouble" << in->status();
        Errors++;
    };
    return test;
}

void GetBytes(QDataStream *in, char *ptr, int bytes)
{
    in->readRawData(ptr, bytes);
    Counter += bytes;
    if (in->status())
    {
        qDebug() << "getbytes" << in->status();
        Errors++;
    };
    return;
}

QString GetString(QDataStream *in)
//All strings have a double terminator - it's something <32 followed by 0.
//Nope, have found a case where it's a legitimate character followed by a 0!

{
    qint8 a;
    QString text = "";
    do
    {
        *in >> a;
        Counter++;
        text.append(a);
    }
    while (a != 0);
    *in >> a;
    Counter++;
    if (a == 0) //2nd 0 - lose first as well
    {
        text = text.left(text.size() - 1);
    }
    else
        //oops, we've read into next string
    {
        int pos = in->device()->pos();
        in->device()->seek(pos - 1);
        text = text.left(text.size() - 2);
    }


    /* //Old Version
    qint8 a=99, b;
    QChar c;
    QString text="";


    do //should read until we have a double terminator
    {
        b=a; //keep last one
        c = (QChar) a;
        *in >> a; Counter++;
        if (a>=32) text.append(a);
    }
    while (a!=0 || b>=32);

    */
    if (in->status())
    {
        qDebug() << "getstring" << in->status();
        Errors++;
    };
    return text;
}


void OldGetSettings(QDataStream *in, int tempint)
{
    Q_UNUSED(in);
    Q_UNUSED(tempint);
    Error("Import from this version of SPIERS not implemented - convert to most recent VB version, then try again");   //pass initial value
}
void GetSettingsMinus1(QDataStream *in)
{
    Q_UNUSED(in);
    Error("Import from this version of SPIERS (file version 1) not implemented - convert to most recent VB version, then try again");
}
void GetSettingsMinus2(QDataStream *in)
{
    Q_UNUSED(in);
    Error("Import from this version of SPIERS (file version 2) not implemented - convert to most recent VB version, then try again");
}
void GetSettingsMinus3(QDataStream *in)
{
    Q_UNUSED(in);
    Error("Import from this version of SPIERS (file version 3) not implemented at all, anywhere, sorry! I hope you never see this message...");
}
void GetSettingsMinus4(QDataStream *in)
{
    Q_UNUSED(in);
    Error("Import from this version of SPIERS (file version 4) not implemented - convert to most recent VB version, then try again");
}
void GetSettingsMinus5(QDataStream *in)
{
    Q_UNUSED(in);
    Error("Import from this version of SPIERS (file version 5) not implemented - convert to most recent VB version, then try again");
}

void GetSettingsMinus6(QDataStream *in)
{
    //Import from last VB version of SPIERS. This is a painful series of bodges, but if it works it works!
    int n, m, o;
    bool temp;
    QString stemp;
    int itemp;
    double dtemp;
    QString sstring;
    Errors = 0;
    BrightUp = GetShort(in);
    BrightDown = GetShort(in);
    SegmentCount = GetShort(in);

    //set up segment structures - clear any existing
    qDeleteAll(Segments.begin(), Segments.end());
    Segments.clear();
    for (n = 0; n < SegmentCount; n++)
    {
        sstring.sprintf("Segment %d", n + 1);
        Segments.append(new Segment(sstring));
    }


    //two dummys
    stemp = GetString(in);
    stemp = GetString(in);

    //get segment names
    for (n = 0; n < 8; n++)
    {
        stemp = GetString(in);
        if (n < SegmentCount) Segments[n]->Name = stemp;
    }

    //now seg colours - a 2D variant array - yuck! This seems to work...

    for (n = 0; n < 8; n++)
    {
        itemp = GetVariantInt(in);
        if (n < SegmentCount) Segments[n]->Colour[0] = itemp;
    }

    itemp = GetVariantInt(in); //dummy - should be 0
    for (n = 0; n < 8; n++)
    {
        itemp = GetVariantInt(in);
        if (n < SegmentCount) Segments[n]->Colour[1] = itemp;
    }

    itemp = GetVariantInt(in); //dummy - should be 0
    for (n = 0; n < 8; n++)
    {
        itemp = GetVariantInt(in);
        if (n < SegmentCount) Segments[n]->Colour[2] = itemp;
    }

    //now LinPercents
    for (n = 0; n < 8; n++)
        for (m = 0; m < 3; m++)
        {
            itemp = GetShort(in);
            if (n < SegmentCount) Segments[n]->LinPercent[m] = itemp;
        }


    for (n = 0; n < 8; n++)
    {
        itemp = GetShort(in);
        if (n < SegmentCount) Segments[n]->LinGlobal = itemp;
    }
    for (n = 0; n < 8; n++)
    {
        temp = GetBool(in);
        if (n < SegmentCount) Segments[n]->LinInvert = temp;
    }
    for (n = 0; n < 8; n++)
    {
        dtemp = GetDouble(in);
        if (n < SegmentCount) Segments[n]->NeighbourBright = dtemp;
    }
    for (n = 0; n < 8; n++)
    {
        itemp = GetInt(in);
        if (n < SegmentCount) Segments[n]->NeighbourSparse = itemp;
    }
    for (n = 0; n < 8; n++)
    {
        temp = GetBool(in);
        if (n < SegmentCount) Segments[n]->NeighbourSingle = temp;
    }


    SampleArraySize = GetInt(in);
    SampleArray.resize(SampleArraySize * 4);
    GetBytes(in, SampleArray.data(), SampleArraySize * 4);

    //a dummy
    stemp = GetString(in);

    //Now Masks!
    //First problem - MaxUsedMask (MasksCount) is NOT the first item, so
    //read all into local fixed size arrays - then do the object creation and copying
    //note that the 255 limit will have to stay - Masks array is bytes - so could actually create static arrays here! Will be simpler!
    //No, stick to dynamic.


    QString MaskNames[256];
    int MaskForeColours[256][3];
    int MaskBackColours[256][3];
    bool MaskShow[256];
    bool MaskWrite[256];
    int MaskContrasts[256];
    bool MaskLock[256];

    for (n = 0; n < 255; n++) //Does only seem to want 255 - no idea why, but it works
    {
        MaskNames[n] = GetString(in);
    }

    for (m = 0; m < 3; m++) for (n = 0; n < 256; n++)
        {
            MaskForeColours[n][m] = GetInt(in);
        }
    for (m = 0; m < 3; m++) for (n = 0; n < 256; n++)
        {
            MaskBackColours[n][m] = GetInt(in);
        }
    for (n = 0; n < 256; n++)
    {
        MaskShow[n] = GetBool(in);
    }
    for (n = 0; n < 256; n++)
        MaskWrite[n] = GetBool(in);
    for (n = 0; n < 256; n++)
        MaskContrasts[n] = GetShort(in);
    for (n = 0; n < 256; n++)
        MaskLock[n] = GetBool(in);

    MaxUsedMask = GetShort(in);
    SelectedMask = GetShort(in);
    SelectedRMask = GetShort(in);

    //NOW we can put them into mask settings structure
    qDeleteAll(MasksSettings.begin(), MasksSettings.end());
    MasksSettings.clear();
    for (n = 0; n <= MaxUsedMask; n++)
    {
        sstring.sprintf("Mask %d", n + 1);
        MasksSettings.append(new Mask(sstring));
        MasksSettings[n]->Name = MaskNames[n];
        for (m = 0; m < 3; m++) MasksSettings[n]->ForeColour[m] = MaskForeColours[n][m]; //r,g,b
        for (m = 0; m < 3; m++) MasksSettings[n]->BackColour[m] = MaskBackColours[n][m]; //r,g,b
        MasksSettings[n]->Show = MaskShow[n];
        MasksSettings[n]->Write = MaskWrite[n];
        MasksSettings[n]->Contrast = MaskContrasts[n];
        MasksSettings[n]->Lock = MaskLock[n];
        //MasksSettings[n]->ListOrder=n;
    }

    //Next up is polynomial generation settings for segments.
    for (m = 0; m < 10; m++) for (n = 0; n < 8; n++)
        {
            dtemp = GetDouble(in);
            if (n < SegmentCount) Segments[n]->PolyRedConsts[m] = dtemp;
        }
    for (m = 0; m < 10; m++) for (n = 0; n < 8; n++)
        {
            dtemp = GetDouble(in);
            if (n < SegmentCount) Segments[n]->PolyGreenConsts[m] = dtemp;
        }
    for (m = 0; m < 10; m++) for (n = 0; n < 8; n++)
        {
            dtemp = GetDouble(in);
            if (n < SegmentCount) Segments[n]->PolyBlueConsts[m] = dtemp;
        }
    for (n = 0; n < 8; n++)
    {
        dtemp = GetDouble(in);
        if (n < SegmentCount) Segments[n]->PolyKall = dtemp;
    }
    for (n = 0; n < 8; n++)
    {
        dtemp = GetDouble(in);
        if (n < SegmentCount) Segments[n]->PolyOrder = (int)dtemp;
    }
    for (n = 0; n < 8; n++)
    {
        itemp = GetShort(in);
        if (n < SegmentCount) Segments[n]->PolySparse = itemp;
    }
    for (n = 0; n < 8; n++)
    {
        itemp = GetShort(in);
        if (n < SegmentCount) Segments[n]->PolyRetries = itemp;
    }
    for (n = 0; n < 8; n++)
    {
        itemp = GetShort(in);
        if (n < SegmentCount) Segments[n]->PolyConverge = itemp;
    }
    for (n = 0; n < 8; n++)
    {
        dtemp = GetDouble(in);
        if (n < SegmentCount) Segments[n]->PolyScale = dtemp;
    }
    for (n = 0; n < 8; n++)
    {
        itemp = GetShort(in);
        if (n < SegmentCount) Segments[n]->PolyContrast = itemp;
    }
    //all seems to work fine here... as if I care!
    //for (n=0; n<SegmentCount; n++) Segments[n]->ListOrder=n;

    //Curves. All easy I hope.
    CurveCount = GetShort(in);
    //qDeleteAll(Curves.begin(), Curves.end());
    //Curves.clear();

    FullFiles = Files; //need this for appending a curve.... might do some harm later, so will clear it
    for (n = 0; n < CurveCount; n++)
    {
        sstring.sprintf("Curve %d", n + 1);
        Curves.append(new Curve(sstring));
    }

    //a dummy - don't ask why
    stemp = GetString(in);
    for (n = 0; n < 254; n++) //Does only seem to want 255 - no idea why, but it works
    {
        stemp = GetString(in);
        if (n < CurveCount) Curves[n]->Name = stemp;
    }


    for (m = 0; m < 3; m++) for (n = 0; n < 255; n++) //There are 255 not 256 curves
        {
            itemp = GetInt(in);
            if (n < CurveCount) Curves[n]->Colour[m] = itemp;
        }
    for (n = 0; n < 255; n++) //There are 255 not 256 curves
    {
        temp = GetBool(in);
        if (n < CurveCount) Curves[n]->Closed = temp;
    }
    for (n = 0; n < 255; n++) //There are 255 not 256 curves
    {
        temp = GetBool(in);
        if (n < CurveCount) Curves[n]->Filled = temp;
    }
    for (n = 0; n < 255; n++) //There are 255 not 256 curves
    {
        itemp = GetShort(in);
        if (n < CurveCount) Curves[n]->Segment = itemp;
    }
    SelectedCurve = GetShort(in);

    for (n = 0; n < CurveCount; n++) Curves[n]->ListOrder = n;

    //Now Output Objects. Again, need to buffer to local arrays.
    //Currently DON'T store any of these - not importing output objects
    int OutKeys[201];
    int OutResamples[201];
    Q_UNUSED(OutKeys);
    Q_UNUSED(OutResamples);


    for (n = 0; n < 201; n++)
        OutKeys[n] = GetShort(in);
    for (n = 0; n < 201; n++)
        OutResamples[n] = GetShort(in);


    itemp = GetShort(in);

    qDeleteAll(OutputObjects.begin(), OutputObjects.end());
    OutputObjects.clear();
    OutputObjectsCount = 0;

    //  for (n=0; n<OutputObjectsCount; n++)
//      {sstring.sprintf("Output Object %d",n+1); OutputObjects.append(new OutputObject(sstring));}

    //stash keys and resamples
    /*  for (n=0; n<OutputObjectsCount; n++)
        {
            OutputObjects[n]->Key=(uchar) OutKeys[n];
            OutputObjects[n]->Resample=(int) OutResamples[n];
        }
    */
    for (o = 0; o < 256; o++) for (m = 0; m < 8; m++) for (n = 0; n < 201; n++)
            {
                temp = GetBool(in);
                /*      if (n<OutputObjectsCount)
                        {
                            if (temp) //mask o and seg m are a pair for object n
                            {
                                OutputObjects[n]->ComponentMasks.append(o);
                                OutputObjects[n]->ComponentSegments.append(m);
                                OutputObjects[n]->ComponentCount++;
                            }
                            //OutputObjects[n]->ListOrder=n;

                        }
                */
            }

    PixPerMM = GetDouble(in);
    SlicePerMM = GetDouble(in);
    SkewDown = GetDouble(in);
    SkewLeft = GetDouble(in);

    LastOutputFile = GetShort(in);
    FirstOutputFile = GetShort(in);

    for (m = 0; m < 3; m++) for (n = 0; n < 201; n++)
        {
            itemp = GetShort(in);
            if (n < OutputObjectsCount) OutputObjects[n]->Colour[m] = itemp;
        }

    temp = GetBool(in); // show toolbox setting - don't keep
    temp = GetBool(in); // show gen toolbox setting - don't keep
    //now some Zoom Mode stuff - ignore
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in);

    //now some GUI settings - again ignore
    itemp = GetShort(in); //Brush Mode
    itemp = GetShort(in); //Mode
    itemp = GetShort(in); //Square/round

    itemp = GetShort(in); //Various menu checks
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in); //trans setting
    itemp = GetShort(in); //position in dataset


    MaxTriangles = GetInt(in);
    if (MaxTriangles < 5) MaxTriangles = 2000;


    //5 more booleans for menu state
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in);
    itemp = GetShort(in);

    //v4 now
    ColMonoScale = GetShort(in);

    itemp = GetShort(in); //allows for 0 base of RangeBases array - which appears to be an error
    for (n = 0; n < 8; n++)
    {
        itemp = GetShort(in);
        if (n < SegmentCount) Segments[n]->RangeBase = itemp;
    }
    for (n = 0; n < 8; n++)
    {
        itemp = GetShort(in);
        if (n < SegmentCount) Segments[n]->RangeTop = itemp;
    }
    for (n = 0; n < 8; n++)
    {
        dtemp = GetDouble(in);
        if (n < SegmentCount) Segments[n]->RangeCenter = dtemp;
    }
    for (n = 0; n < 8; n++)
    {
        dtemp = GetDouble(in);
        if (n < SegmentCount) Segments[n]->RangeGradient = dtemp;
    }

    RangeHardFill = GetBool(in);
    RangeSelectedOnly = GetBool(in);


    //v5 now
    CMin = GetShort(in);
    CMax = GetShort(in);
    ZDownsample = GetShort(in);
    XYDownsample = GetShort(in);
    PixSens = GetShort(in);

    OutputMirroring = GetBool(in);


    for (m = 0; m < 255; m++) for (n = 0; n < 201; n++) //out curves
        {
            temp = GetBool(in);
            /*      if (temp) //curve m is in object n
                    {
                        OutputObjects[n]->CurveComponents.append(m);
                        OutputObjects[n]->CurveComponentCount++;
                    }
            */
        }

//      FullFiles=Files; //back it up
    Stretches.clear();
    for (int i = 0; i <= Files.count(); i++) Stretches.append(i);

    FullStretches = Stretches;

    if (Errors > 0)
    {
        QString Mess;
        Mess.sprintf("Warning - %d read errors encountered in settings file", Errors);
        Message(Mess);
        DumpSettings();
    }

//  qDebug()<<"Finished Import";
    return;
}



bool GetSettings(QDir srcdir)
//This is old Vb import one
{
    //returns false if it can't find a settings file
    int tempint;
    Counter = 0;

    ApplyDefaultSettings(); //will be overridden by anything we can read in

    QString Fname = srcdir.canonicalPath() + "/" +  "settings.dat";

    //Files.at(CurrentFile);
    //int lastsep=Fname.lastIndexOf("\\");  //this is last separator in path
    //QString sfname=Fname.left(lastsep);
    //sfname.append("/"+SettingsFileName+"/settings.dat");

    QFile file(Fname);
    if (!file.open(QIODevice::ReadOnly))
    {
        Message("No settings file - using defaults");
        return false;
    }

    QDataStream in(&file);    // Set up the datastream object
    in.setByteOrder(QDataStream::LittleEndian);  //Windows is LE, so will keep ALL our datafiles LE
    tempint = GetShort(&in);

    if (tempint >= 0) // '-1 or lower for future versions - this catches files predating file versioning system
    {
        OldGetSettings(&in, tempint); //pass initial value
        return true;
    }

    switch (tempint)
    {
    case -1:
        GetSettingsMinus1(&in);
        break;
    case -2:
        GetSettingsMinus2(&in);
        break;
    case -3:
        Error("ERROR - file version 3 not supported");
        break;
    case -4:
        GetSettingsMinus4(&in);
        break;
    case -5:
        GetSettingsMinus5(&in);
        break;
    case -6:
        GetSettingsMinus6(&in);
        break;
    default:
        QString temp;
        temp.sprintf("Settings file is version %d - too recent for this version of SPIERSedit, sorry!", tempint);
        Error(temp);
    }
    file.close();
    return true;
}

void DumpSettings()
//debugging version of write settings
{
    //Message("HERE");
    //First header - standard text, v number, then SettingsFileName (might not be same as current filename - so store!)
    qDebug() << QString("SPIERSedit settings file");
    qDebug() << (int)SPIERS_VERSION;
    qDebug() << SettingsFileName;

    //Do all the simple stuff first
    qDebug() << FileNotes;
    qDebug() << FileCount << CurrentFile << cwidth << cheight;
    qDebug() << fwidth << fheight;
    qDebug() << (int)CurrentZoom << ColMonoScale << Trans;
    qDebug() << CMin << CMax;
    qDebug() << HiddenMasksLockedForGeneration;
    qDebug() << SegmentBrushAppliesMasks;
    qDebug() << SegmentsLocked;
    qDebug() << CurveMarkersAsCrosses << CurveShapeLocked;
    qDebug() << CurrentMode << CurrentSegment << CurrentRSegment;
    qDebug() << Brush_Size << BrightUp << BrightDown;
    qDebug() << BrightSoft << LastTrans << ThreshFlag << MasksFlag;
    qDebug() << SegsFlag << SampleArraySize << MaxUsedMask << SelectedMask;
    qDebug() << SelectedRMask << SelectedCurve << CurveCount << OutputObjectsCount;
    qDebug() << PixPerMM << SlicePerMM << SkewDown << SkewLeft;
    qDebug() << FirstOutputFile << LastOutputFile << MaxTriangles;
    qDebug() << RangeHardFill << RangeSelectedOnly << OutputMirroring;
    qDebug() << PixSens << XYDownsample << ZDownsample << SquareBrush;

    //qDebug().writeRawData(SampleArray.data(),SampleArraySize*4);
    foreach (QString str, Files) qDebug() << str;

    //It seems there ARE no segments - just reading masks -
    foreach (struct Segment *seg, Segments)
    {
        qDebug() << seg->Name;
        //qDebug()<<"HERE";
        qDebug() << seg->Colour[0] << seg->Colour[1] << seg->Colour[2]; //r,g,b
        qDebug() << seg->LinPercent[0] << seg->LinPercent[1] << seg->LinPercent[2]; //r,g,b
        qDebug() << seg->LinGlobal << seg->LinInvert << seg->NeighbourBright << seg->NeighbourSparse;
        qDebug() << seg->NeighbourSingle << seg->PolyKall << seg->PolyOrder << seg->PolySparse;
        qDebug() << seg->PolyRetries << seg->PolyConverge << seg->PolyScale << seg->PolyContrast;
        qDebug() << seg->RangeBase << seg->RangeTop << seg->RangeGradient << seg->RangeCenter;
        qDebug() << seg->widgetitem;
        if (seg->widgetitem) qDebug() << (seg->widgetitem)->text(2);
        //qDebug()<<"HERE";
        //for(n=0; n<10; n++) qDebug()<<seg->PolyRedConsts[n];
        //for(n=0; n<10; n++) qDebug()<<seg->PolyGreenConsts[n];
        //for(n=0; n<10; n++) qDebug()<<seg->PolyBlueConsts[n];
    }

    foreach (struct Mask *m, MasksSettings)
    {
        qDebug() << m->Name;
        qDebug() << m->ForeColour[0] << m->ForeColour[1] << m->ForeColour[2]; //r,g,b
        qDebug() << m->BackColour[0] << m->BackColour[1] << m->BackColour[2]; //r,g,b
        qDebug() << m->Show << m->Write << m->Contrast << m->Lock;
    }

    foreach (struct Curve *c, Curves)
    {
        qDebug() << c->Name;
        qDebug() << c->Colour[0] << c->Colour[1] << c->Colour[2]; //r,g,b
        qDebug() << c->Closed << c->Filled << c->Segment;
        //for (n=0; n<c->SplineCount; n++) qDebug()<<c->SplineX[n];
        //for (n=0; n<c->SplineCount; n++) qDebug()<<c->SplineY[n];
    }

    foreach (struct OutputObject *o, OutputObjects)
    {
        qDebug() << o->Name;
        qDebug() << o->Resample;
        qDebug() << o->Colour[0] << o->Colour[1] << o->Colour[2]; //r,g,b
        //for (n=0; n<o->ComponentCount; n++) qDebug()<<o->ComponentMasks[n];
        //for (n=0; n<o->ComponentCount; n++) qDebug()<<o->ComponentSegments[n];
        //qDebug() << o->ComponentCount;
        //for (n=0; n<o->CurveComponentCount; n++) qDebug()<<o->CurveComponents[n];
        //qDebug() << o->CurveComponentCount;
    }
}

void WriteSettings()
//Write Settings File
{
    //DumpSettings();

    Notes = mainwin->plainTextEdit->toPlainText();
    int n;
    QString Fname = Files.at(0);
    int lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
    QString sfname = Fname.left(lastsep);
    sfname.append("/" + SettingsFileName + ".spe");
    QString dummy;

    QFile file(sfname);
    if (!file.open(QIODevice::WriteOnly))
    {
        Message("Warning - can't open settings file " + SettingsFileName + " to write to");
        return;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_3);
    out.setByteOrder(QDataStream::LittleEndian);

    //First header - standard text, v number, then SettingsFileName (might not be same as current filename - so store!)
    out << QString("SPIERSedit settings file");
    out << (int)SPIERS_VERSION;
    out << SettingsFileName;

    //Do all the simple stuff first
    out << FileNotes;

    out << FullFiles.count();

    out << CurrentFile << cwidth << cheight;
    out << fwidth << fheight;
    out << (int)CurrentZoom << ColMonoScale << Trans;
    out << CMin << CMax;
    out << HiddenMasksLockedForGeneration;
    out << SegmentBrushAppliesMasks;
    out << SegmentsLocked;
    out << CurveMarkersAsCrosses << CurveShapeLocked;
    out << CurrentMode << CurrentSegment << CurrentRSegment;
    out << Brush_Size << BrightUp << BrightDown;
    out << BrightSoft << LastTrans << ThreshFlag << MasksFlag;
    out << SegsFlag << SampleArraySize << MaxUsedMask << SelectedMask;
    out << SelectedRMask << SelectedCurve << CurveCount << OutputObjectsCount;
    out << PixPerMM << SlicePerMM << SkewDown << SkewLeft;
    out << FirstOutputFile << LastOutputFile << MaxTriangles;
    out << RangeHardFill << RangeSelectedOnly << OutputMirroring;
    out << PixSens << XYDownsample << ZDownsample << SquareBrush;
    out << SegmentCount;

    out.writeRawData(SampleArray.data(), SampleArraySize * 4);

    foreach (QString str, FullFiles) out << str;

    foreach (struct Segment *seg, Segments)
    {
        out << seg->Name;
        out << seg->Colour[0] << seg->Colour[1] << seg->Colour[2]; //r,g,b
        out << seg->LinPercent[0] << seg->LinPercent[1] << seg->LinPercent[2]; //r,g,b
        out << seg->LinGlobal << seg->LinInvert << seg->NeighbourBright << seg->NeighbourSparse;
        out << seg->NeighbourSingle << seg->PolyKall << (double)seg->PolyOrder << seg->PolySparse;
        out << seg->PolyRetries << seg->PolyConverge << seg->PolyScale << seg->PolyContrast;
        out << seg->RangeBase << seg->RangeTop << seg->RangeGradient << seg->RangeCenter;
        for (n = 0; n < 10; n++) out << seg->PolyRedConsts[n];
        for (n = 0; n < 10; n++) out << seg->PolyGreenConsts[n];
        for (n = 0; n < 10; n++) out << seg->PolyBlueConsts[n];
        out << seg->ListOrder;
        out << seg->Locked;
        out << seg->Activated;
    }

    foreach (struct Mask *m, MasksSettings)
    {
        out << m->Name;
        out << m->ForeColour[0] << m->ForeColour[1] << m->ForeColour[2]; //r,g,b
        out << m->BackColour[0] << m->BackColour[1] << m->BackColour[2]; //r,g,b
        out << m->Show << m->Write << m->Contrast << m->Lock;
        out << m->ListOrder;
    }

    foreach (struct Curve *c, Curves)
    {
        out << c->Name;
        out << c->Colour[0] << c->Colour[1] << c->Colour[2]; //r,g,b
        out << c->Closed << c->Filled << c->Segment;
        out << c->ListOrder;
        //now the hard stuff
        for (int i = 0; i < FullFiles.count(); i++)
        {
            out << c->SplinePoints[i]->Count;
            for (int k = 0; k < (c->SplinePoints[i]->Count); k++)
            {
                out << c->SplinePoints[i]->X[k];
                out << c->SplinePoints[i]->Y[k];
            }
        }
    }

    foreach (struct OutputObject *o, OutputObjects)
    {
        out << o->Name;
        out << o->Resample;
        out << o->Colour[0] << o->Colour[1] << o->Colour[2]; //r,g,b
        out << o->ComponentMasks.count();
        out << o->ComponentSegments.count();
        out << o->CurveComponents.count();
        for (n = 0; n < o->ComponentMasks.count(); n++) out << o->ComponentMasks[n];
        for (n = 0; n < o->ComponentSegments.count(); n++) out << o->ComponentSegments[n];
        for (n = 0; n < o->CurveComponents.count(); n++) out << o->CurveComponents[n];
        out << o->ListOrder;
        out << o->IsGroup;
        out << o->Parent;
        out << o->Key;
        out << o->Show;
        out << o->Merge;
    }

    QByteArray b =  AppMainWindow->saveState(SPIERS_VERSION);
    out << b;

    out << MenuGenChecked;
    out << MenuMasksChecked;
    out << MenuSegsChecked;
    out << MenuCurvesChecked;
    out << MenuOutputChecked;
    out << MenuToolboxChecked;
    out << MenuSliceSelectorChecked;

    //new - check available on reload
    out << MenuHistChecked;
    out << MenuHistSelectedOnly;
    out << CurveShapeLocked;
    out << MasksMoveBack;
    out << MasksMoveForward;
    out << MenuInfoChecked;
    out << CurrentZoom;
    out << ShowSlicePosition;
    for (int i = 0; i < FullStretches.count(); i++)
        out << FullStretches[i];
    out << zsparsity;
    out << ThreeDmode;
    out <<  BrushY;
    out <<  BrushZ;
    out <<  yaw;
    out <<  pitch;
    out <<  roll;
    out << Notes;
    file.close();
}

void ReadSettings()
//Read Settings File - QT version (i.e. the spe file
{
    //initially SettingsFileName has full name and path
    QString dummy;
    int version, dummy_int;
    int n, i;
    double dummy_double;
    QString path = FullSettingsFileName.left(qMax(FullSettingsFileName.lastIndexOf("\\"), FullSettingsFileName.lastIndexOf("/")));
    //qDebug()<<"Path is :"<<path<<":";
    //qDebug()<<"Reading"<<FullSettingsFileName;
    QFile file(FullSettingsFileName);

    int lastsep = qMax(FullSettingsFileName.lastIndexOf("\\"), FullSettingsFileName.lastIndexOf("/")); //this is last separator in path
    QString sfname = FullSettingsFileName.mid(lastsep + 1);
    int lastsep2 = sfname.lastIndexOf("."); //this is last separator in path
    SettingsFileName = sfname.left(lastsep2);
    //qDebug()<<"Trimmed name="<<SettingsFileName;
    //qDebug()<<"SFN"<<SettingsFileName;
    if (!file.open(QIODevice::ReadOnly))
    {
        Message("Warning - can't open settings file " + SettingsFileName + ".spe to read from");
        return;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_3);
    in.setByteOrder(QDataStream::LittleEndian);


    //First header - check it has standard text, appropriate v number, then SettingsFileName=myname
    in >> dummy;
    if (dummy != "SPIERSedit settings file")
    {
        Message("Warning - settings file " + SettingsFileName + ".spe is not a valid SPIERSedit file");
        return;
    }

    in >> version;

    if (version > SPIERS_VERSION) Message("Warning - settings file " + SettingsFileName + " is too recent for this version of SPIERSedit. Will try to read anyway...");

    in >> dummy;
    //qDebug()<<"Read"<<dummy;
    if (dummy != SettingsFileName) Message("Warning - settings file name has been changed (from " + dummy + ") - it will be reset to " + SettingsFileName + ".spe when next saved");


    //Do all the simple stuff first
    in >> FileNotes;
    in >> FileCount >> CurrentFile >> cwidth >> cheight;

    in >> fwidth >> fheight;
    in >> dummy_int >> ColMonoScale >> Trans;
    in >> CMin >> CMax;
    in >> HiddenMasksLockedForGeneration;
    in >> SegmentBrushAppliesMasks;
    in >> SegmentsLocked; //this no longer used
    in >> CurveMarkersAsCrosses >> CurveShapeLocked;
    in >> CurrentMode >> CurrentSegment >> CurrentRSegment;
    in >> Brush_Size >> BrightUp >> BrightDown;
    in >> BrightSoft >> LastTrans >> ThreshFlag >> MasksFlag;
    in >> SegsFlag >> SampleArraySize >> MaxUsedMask >> SelectedMask;
    in >> SelectedRMask >> SelectedCurve >> CurveCount >> OutputObjectsCount;
    in >> PixPerMM >> SlicePerMM >> SkewDown >> SkewLeft;
    in >> FirstOutputFile >> LastOutputFile >> MaxTriangles;
    in >> RangeHardFill >> RangeSelectedOnly >> OutputMirroring;
    in >> PixSens >> XYDownsample >> ZDownsample >> SquareBrush;
    in >> SegmentCount;
    //Read sample array
    SampleArray.resize(SampleArraySize * 4);
    in.readRawData(SampleArray.data(), SampleArraySize * 4);


    //Read Filenames
    Files.clear();
    for (n = 0; n < FileCount; n++)
    {
        in >> dummy;

        dummy = path + dummy.mid(qMax(dummy.lastIndexOf("\\"), dummy.lastIndexOf("/")));

        Files.append(dummy);
    }

    //fix path if necessary

    FullFiles = Files; //back it up - needed for curve import

    //Segments
    qDeleteAll(Segments.begin(), Segments.end());
    Segments.clear();
    Segment *seg;
    for (i = 0; i < SegmentCount; i++)
    {
        seg = new Segment("Default");
        Segments.append(seg);
        in >> seg->Name;
        in >> seg->Colour[0] >> seg->Colour[1] >> seg->Colour[2]; //r,g,b
        in >> seg->LinPercent[0] >> seg->LinPercent[1] >> seg->LinPercent[2]; //r,g,b
        in >> seg->LinGlobal >> seg->LinInvert >> seg->NeighbourBright >> seg->NeighbourSparse;
        in >> seg->NeighbourSingle >> seg->PolyKall >> dummy_double >> seg->PolySparse;
        seg->PolyOrder = qRound(dummy_double);
        in >> seg->PolyRetries >> seg->PolyConverge >> seg->PolyScale >> seg->PolyContrast;
        in >> seg->RangeBase >> seg->RangeTop >> seg->RangeGradient >> seg->RangeCenter;
        for (n = 0; n < 10; n++) in >> seg->PolyRedConsts[n];
        for (n = 0; n < 10; n++) in >> seg->PolyGreenConsts[n];
        for (n = 0; n < 10; n++) in >> seg->PolyBlueConsts[n];
        in >> seg->ListOrder;
        in >> seg->Locked;
        in >> seg->Activated;
    }

    //qDebug("RS3");
    //Masks
    qDeleteAll(MasksSettings.begin(), MasksSettings.end());
    MasksSettings.clear();
    Mask *m;
    for (i = 0; i <= MaxUsedMask; i++)
    {
        m = new Mask("Default");
        MasksSettings.append(m);
        in >> m->Name;
        in >> m->ForeColour[0] >> m->ForeColour[1] >> m->ForeColour[2]; //r,g,b
        in >> m->BackColour[0] >> m->BackColour[1] >> m->BackColour[2]; //r,g,b

        //deal with bug in early versions that wrote out very large values for 3rd colour
        if (m->BackColour[2] > 255) m->BackColour[2] = 255;
        if (m->ForeColour[2] > 255) m->ForeColour[2] = 255;
        in >> m->Show >> m->Write >> m->Contrast >> m->Lock;
        in >> m->ListOrder;
    }

    //qDebug("RS4");
    //Curves - delete the internal lists, then all existing curves.
//  qDebug()<<"RS4b1"<<CurveCount;
//  qDebug()<<"RS4b2"<<CurveCount<<Curves.count();
    //for (i=0; i<Curves.count(); i++) {qDebug()<<"Removing spline points"; qDeleteAll(Curves[i]->SplinePoints); Curves[i]->SplinePoints.clear(); qDebug()<<"Curve"<<i<<Curves[i]; delete Curves[i];}
    qDeleteAll(Curves);
//  qDebug()<<"RS4b3"<<CurveCount;
    Curves.clear();
//  qDebug()<<"RS4b4"<<CurveCount;
    Curve *c;
//  qDebug()<<"RS4b5"<<CurveCount;
    for (i = 0; i < CurveCount; i++)
    {
//      qDebug()<<"RS4b-in loop"<<i;
        c = new Curve("Default");
        Curves.append(c);
        in >> c->Name;
        in >> c->Colour[0] >> c->Colour[1] >> c->Colour[2]; //r,g,b
        //deal with bug in early versions that wrote out very large values for 3rd colour
        if (c->Colour[2] > 255) c->Colour[2] = 255;
        in >> c->Closed >> c->Filled >> c->Segment;
        in >> c->ListOrder;

        //now the hard stuff
//      qDebug()<<"RS4b-in loop 2"<<i;

        for (int j = 0; j < Files.count(); j++)
        {
            in >> c->SplinePoints[j]->Count;
            for (int k = 0; k < (c->SplinePoints[j]->Count); k++)
            {
                double dummy;
                in >> dummy;
                c->SplinePoints[j]->X.append(dummy);
                in >> dummy;
                c->SplinePoints[j]->Y.append(dummy);
            }
        }

//      qDebug()<<"RS4b-in loop 3"<<i;

    }

    //OutputObjects
    //qDebug("RS5");
    qDeleteAll(OutputObjects.begin(), OutputObjects.end());
    OutputObjects.clear();
    OutputObject *o;
    for (i = 0; i < OutputObjectsCount; i++)
    {
        o = new OutputObject("Default");
        OutputObjects.append(o);
        in >> o->Name;
        in >> o->Resample;
        in >> o->Colour[0] >> o->Colour[1] >> o->Colour[2]; //r,g,b
        o->ComponentMasks.clear();
        o->ComponentSegments.clear();
        o->CurveComponents.clear();
        int cmcount;
        int cscount;
        int cccount;
        in >> cmcount;
        in >> cscount;
        in >> cccount;
        for (n = 0; n < cmcount; n++)
        {
            in >> dummy_int;
            o->ComponentMasks.append(dummy_int);
        }
        for (n = 0; n < cscount; n++)
        {
            in >> dummy_int;
            o->ComponentSegments.append(dummy_int);
        }
        for (n = 0; n < cccount; n++)
        {
            in >> dummy_int;
            o->CurveComponents.append(dummy_int);
        }
        in >> o->ListOrder;
        in >> o->IsGroup;
        in >> o->Parent;
        in >> o->Key;
        in >> o->Show;
        in >> o->Merge;
    }

    //the QByteArray for
    //qDebug("RS6");
    //if (in.atEnd()) {file.close(); return;} //possible finishing point!

    QByteArray b;
    in >> b;
    AppMainWindow->restoreState(b, SPIERS_VERSION);

    in >> MenuGenChecked;
    in >> MenuMasksChecked;
    in >> MenuSegsChecked;
    in >> MenuCurvesChecked;
    in >> MenuOutputChecked;
    in >> MenuToolboxChecked;
    in >> MenuSliceSelectorChecked;

    //qDebug("RS7");
    //new - check available on reload
    if (!in.atEnd()) in >> MenuHistChecked;
    if (!in.atEnd()) in >> MenuHistSelectedOnly;
    if (!in.atEnd()) in >> CurveShapeLocked;
    if (!in.atEnd()) in >> MasksMoveBack;
    if (!in.atEnd()) in >> MasksMoveForward;
    if (!in.atEnd()) in >> MenuInfoChecked;
    if (!in.atEnd()) in >> CurrentZoom;
    if (!in.atEnd()) in >> ShowSlicePosition;
    FullStretches.clear();
    if (!in.atEnd()) for (int i = 0; i < FileCount; i++)
        {
            in >> dummy_double;
            FullStretches.append(dummy_double);
        }


    //now a bodge
    //Filenames are stored as absolute files - means can't relocate SPEs
    //To fix and retain back-compatibility, code belows fixes path of files
    //to path of SPE

    QString PathOfSPE = FullSettingsFileName.left(qMax(FullSettingsFileName.lastIndexOf("\\"), FullSettingsFileName.lastIndexOf("/")));
    for (int i = 0; i < FileCount; i++)
        Files[i] = PathOfSPE + Files[i].mid
                   (qMax(Files[i].lastIndexOf("\\"), Files[i].lastIndexOf("/")));

    if (!in.atEnd()) in >> zsparsity;
    if (!in.atEnd()) in >> ThreeDmode;
    if (!in.atEnd()) in >> BrushY;
    if (!in.atEnd()) in >> BrushZ;
    if (!in.atEnd()) in >> yaw;
    if (!in.atEnd()) in >> pitch;
    if (!in.atEnd()) in >> roll;

    if (!in.atEnd()) in >> Notes;

    //now doctor Files array using zsparsity, if necessary
    if (zsparsity > 1)
    {
        Files.clear();
        for (int i = 0; i < FullFiles.count(); i += zsparsity)
        {
            Files.append(FullFiles[i]);
        }
        FileCount = Files.count();
    }
    //do similar for stretches
    if (zsparsity > 1)
    {
        Stretches.clear();
        for (int i = 0; i < FullStretches.count(); i += zsparsity)
        {
            Stretches.append(FullStretches[i]);
        }
    }
    else Stretches = FullStretches;
    file.close();

}

