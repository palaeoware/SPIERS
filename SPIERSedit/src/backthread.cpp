/**
 * @file
 * Source: BackThread
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "backthread.h"
#include "globals.h"
#include "fileio.h"
#include <QTimer>
#include <QBuffer>
#include <QMutexLocker>
#include <QByteArray>

//warning - this no longer used, probably not working!

MyThread *BackThread;

void MyThread::run()
{
    QTimer *t = new QTimer();
    connect(t, SIGNAL(timeout()), this, SLOT(TimerFired()));
    t->start(2000);
    exec();
}

bool MyThread::TryCaching(int fnumber)
{
    if (CacheIndex(fnumber) == -1)
    {
        QString filename = Files.at(0); //record the one we are cacheing
        //colour first
        QImage *Data = new QImage(Files.at(fnumber));
        if (Data->isNull()) Error("Couldn't load source data file");
        if (Data->width() != cwidth || Data->height() != cheight) Error ("Source file has wrong dimensions - all source images dimensions need to be the same");

        int d = Data->format();
        if (d != QImage::Format_RGB32 && d != QImage::Format_ARGB32 && d != QImage::Format_Indexed8)
        {
            qDebug() << "Format code is " << d;
            Error("File is not in a valid format - [1]");
        }
        QList <QImage *> glist;
        //GREYS
        for (int seg = 0; seg < SegmentCount; seg++)
        {
            int lastsep, lastdot;
            QString Fname = Files.at(fnumber);
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
            QImage *Data2 = new QImage(sfname);

            if (Data2->isNull()) Error("Couldn't load source data file");
            glist.append(Data2);
        }

        //MASKS
        int lastsep, lastdot;
        //uchar *pos;

        QString Fname = Files.at(fnumber);
        lastsep = qMax(Fname.lastIndexOf("\\"), Fname.lastIndexOf("/")); //this is last separator in path
        lastdot = Fname.lastIndexOf(".");
        QString sfname = Fname.left(lastsep);
        QString actfn = Fname.mid(lastsep + 1, lastdot - lastsep - 1);
        QString temp;
        temp.append("/" + SettingsFileName + "/m_");
        temp.append(actfn);
        temp.append(".bmp");
        sfname.append(temp);

        QByteArray  *m = new QByteArray;
        QByteArray *mcomp = 0;
        QFile file(sfname);

        if (!file.open(QIODevice::ReadOnly) ) //no masks file
            m->resize(0);
        else
        {
            //Use read to read directly into the byte array
            (*m) = file.readAll();
            //if (Masks!=NULL) delete Masks;
            //check it's really OK
            if (m->size() != fwidth * fheight && m->size() != 0) //it's compressed
            {
                mcomp = new QByteArray;

                (*mcomp) = qUncompress(*m);

            }
            else
                mcomp = m;
        }

        //LOCKS
        QString Fname2 = Files.at(fnumber);
        lastsep = qMax(Fname2.lastIndexOf("\\"), Fname2.lastIndexOf("/")); //this is last separator in path
        lastdot = Fname2.lastIndexOf(".");
        QString sfname2 = Fname2.left(lastsep);
        QString actfn2 = Fname2.mid(lastsep + 1, lastdot - lastsep - 1);
        QString temp2;
        temp2.append("/" + SettingsFileName + "/l_");
        temp2.append(actfn2);
        temp2.append(".bmp");
        sfname2.append(temp2);

        QByteArray *m2 = new QByteArray;
        QByteArray *m2comp = 0;
        QFile file2(sfname2);
        if (!file2.open(QIODevice::ReadOnly))
            m2->resize(0);
        else
        {
            //Use read to read directly into the byte array

            (*m2) = file2.readAll();
            //check size (if locks file corrupt or similat may not be correct)
            if (m2->size() != fwidth * fheight * 2 && m2->size() != 0)
            {
                m2comp = new QByteArray;
                *m2comp = qUncompress(*m2);
            }
            else
                m2comp = m2;
        }

        int CacheMode = CacheCompressionLevel; //take before in case it changes
        //doesn't matter if it does - all this will be cleared by clearcache;

        //NOW START CACHEING

        bool flag = false;
        if (Files.count() == 0) flag = true;
        else if (Files.at(0) != filename) flag = true;

        if (flag == true)
        {
            //something has changed - get out without cacheing
            delete Data;
            qDeleteAll(glist.begin(), glist.end());
            delete m;
            if (mcomp != 0 && mcomp != m) delete mcomp;
            delete m2;
            if (m2comp != 0 && m2comp != m2) delete m2comp;
            return true;
        }
        Caches.append(new Cache);
        Caches[Caches.count() - 1]->Blank(fnumber);

        if (CacheMode == 0) Caches[Caches.count() - 1]->ColData = Data;
        else
        {
            Caches[Caches.count() - 1]->ColDataCompressed = new QByteArray;
            QBuffer buffer(Caches[Caches.count() - 1]->ColDataCompressed);
            buffer.open(QIODevice::WriteOnly);
            if (CacheCompressionLevel == 1)
                Data->save(&buffer, "PNG", 99); // writes image into ba in PNG format
            else
                Data->save(&buffer, "JPG", 100); //store as jpeg if on high compression!
            delete Data;
        }

        //greys
        for (int seg = 0; seg < SegmentCount; seg++)
        {
            if (CacheMode == 0) Caches[Caches.count() - 1]->GreyData[seg]->Data = glist[seg];
            else
            {
                Caches[Caches.count() - 1]->GreyData[seg]->CompressedData = new QByteArray;
                QBuffer buffer(Caches[Caches.count() - 1]->GreyData[seg]->CompressedData);
                buffer.open(QIODevice::WriteOnly);
                glist[seg]->save(&buffer, "PNG"); // writes image into ba in PNG format

                delete glist[seg];
            }
        }

        //masks and locks
        if (CacheMode == 0)
        {
            if (m->size() == 0)
            {
                Caches[Caches.count() - 1]->Masks = 0;
                delete m;
            }
            else
            {
                Caches[Caches.count() - 1]->Masks = mcomp;
                if (mcomp != m) delete m;
            }
            if (m2->size() == 0)
            {
                Caches[Caches.count() - 1]->Locks = 0;
                delete m2;
            }
            else
            {
                Caches[Caches.count() - 1]->Locks = m2comp;
                if (mcomp != m) delete m;
            }
        }
        else
        {
            if (m->size() == 0)
            {
                Caches[Caches.count() - 1]->Masks = 0;
                delete m;
            }
            else //there is mask data
            {
                if (mcomp == m) //file wasn't compressed - do compression myself
                {
                    Caches[Caches.count() - 1]->Masks = new QByteArray(qCompress(*mcomp, CacheMode));
                    delete mcomp; //same, so only need to remove one
                }
                else
                {
                    Caches[Caches.count() - 1]->Masks = mcomp;
                    delete m; //mcomp pointer used, but delete m
                }
            }

            if (m2->size() == 0)
            {
                Caches[Caches.count() - 1]->Locks = 0;
                delete m2;
            }
            else //there is mask data
            {
                if (m2comp == m2) //file wasn't compressed - do compression myself
                {
                    Caches[Caches.count() - 1]->Locks = new QByteArray(qCompress(*m2comp, CacheMode));
                    delete m2comp; //same, so only need to remove one
                }
                else
                {
                    Caches[Caches.count() - 1]->Locks = m2comp;
                    delete m2; //mcomp pointer used, but delete m
                }
            }
        }
        Caches[Caches.count() - 1]->CalcMySize();
        return true;
    }
    else
        return false;

}

void MyThread::TimerFired()
{
    //qDebug()<<"Back timer in";
    QMutexLocker locker(&mutex);
    if (BackgroundCacheFilling && Active)
    {
        int pos = CurrentFile;
        //The algorithm:

        long long int SizeTotal = 0;
        for (int j = 0; j < Caches.count(); j++)
            SizeTotal += (long long int) Caches[j]->Size;
        if (SizeTotal >= ((long long int)CacheMem * (long long int)(1024 * 1024)))
        {
            return;
        }

        //Method:
        //Count alternating either side of current position - first thing we find without a cached entry, load it!
        for (int j = 1; j < Files.count(); j++)
        {
            bool OK = false;
            if ((pos + j) < Files.count()) OK = TryCaching(pos + j);
            if (OK == false) if ((pos - j) > 0) OK = TryCaching(pos - j);
            if (OK)
            {
                return;
            }
        }
    }

}
