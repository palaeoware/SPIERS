#include "compressedslice.h"
//#include <vtk_zlib.h>
#include <zlib.h>
#include <stdlib.h>
#include <spv.h>
//RJG update for ubuntu
#include <svobject.h>
#include <SPIERSviewglobals.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>

CompressedSlice::CompressedSlice(SVObject *parent, bool IsEmpty)
{
    empty=IsEmpty; //for now
    Object=parent;
    data=0; //null pointer
    datasize=0;
    grid=(unsigned char *)malloc(parent->spv->GridSize);
    for (int i=0; i<parent->spv->GridSize; i++) grid[i]=0;
}

CompressedSlice::~CompressedSlice()
{
     if (data) free (data);
     free (grid);
}

void CompressedSlice::Dump(QString Filename)
{
    //dump the slice as a BMP

            //work out checksum of data and datasize
    int size=Object->spv->size;
    uLongf asize=size;

    //work out checksum of data and datasize
    long total=0;
    for (int i=0; i<datasize; i++) total+=(long)(data[i]);

    int tot=(int)total;
    QString fn= QString("_%1_%2_.bmp").arg(datasize).arg(tot);

    Filename+= fn;
    unsigned char *mergearray=(unsigned char *)malloc(size); //THIS slice
    uncompress(mergearray, &asize, data, datasize); //uncompress THIS slice

    int width=Object->spv->iDim;
    int height=Object->spv->jDim;

    QImage bmp(width,height,QImage::Format_RGB32);
    bmp.fill(0);

    for (int x=0; x<width; x++)
    for (int y=0; y<height; y++)
    {
        if (mergearray[y*width+x]) bmp.setPixel(x,y,mergearray[y*width+x]);
    }

    bmp.save(Filename);
    free (mergearray);
}


void CompressedSlice::Merge(CompressedSlice *s, QString fn)
{
        if (s->empty) return;
        //QFile logfile("c:/logfile.txt"); logfile.open(QIODevice::Append);
        //QTextStream log(&logfile);
        //log<<"New not empty\n";

        if (empty)
        {
            //log<<"Old is empty\n";
            //I'm empty, this one isn't, so can just copy data
            datasize=s->datasize;
            data=(unsigned char *)malloc(datasize);
            memcpy(data,s->data,datasize); //copy it
            empty=false;

            //and copy the grid
            for (int i=0; i<(Object->spv->GridSize); i++) grid[i]=s->grid[i];
            return;
        }

        //Both have data.
        //Decompress both arrays, merge, recompress
        int size=Object->spv->size;
        uLongf asize=size;

        //log<<"Neither empty\n";

        unsigned char *mergearray=(unsigned char *)malloc(size); //THIS slice
        unsigned char *mergearray2=(unsigned char *)malloc(size); //for the new one to be merged in

        for (int i=0; i<size; i++) mergearray[i]=(unsigned char) 0;
        for (int i=0; i<size; i++) mergearray2[i]=(unsigned char) 0;

        uncompress(mergearray, &asize, data, datasize); //uncompress THIS slice

        asize=size;
        uncompress(mergearray2, &asize, s->data, s->datasize); //uncompress the new slice

        //Do the merge
        for (int i=0; i<size; i++) if (mergearray[i]) mergearray2[i]=(unsigned char)255; //New array (merge2) will be the joint one
        
        //Write as a bmp
/*        int width=Object->spv->iDim;
        int height=Object->spv->jDim;

        QImage bmp(width,height,QImage::Format_RGB32);
        bmp.fill(0);

        for (int x=0; x<width; x++)
        for (int y=0; y<height; y++)
        {
            if (mergearray2[y*width+x]) bmp.setPixel(x,y,255);
        }
        bmp.save(fn);
*/

        //and recompress it

        uLongf tempsize;

        compress(mergearray, &tempsize, mergearray2, size);  //compress it back to old array
        datasize=(int)tempsize;
        free(data); //free up old memory
        free(mergearray2);
        data = (unsigned char *)malloc(datasize);
        memcpy(data,mergearray,datasize);
        //Dump(fn);
        //data =(unsigned char *)realloc(mergearray,datasize); //squeeze to size
        free(mergearray);
        //merge the grids
        for (int i=0; i<(Object->spv->GridSize); i++) if (s->grid[i]) grid[i]=s->grid[i]; //copy non-zeroes

        return;
}

void CompressedSlice::GetFullData(unsigned char *d)
{
    int size=Object->spv->size;
    uLongf asize=size;
    //unsigned char *outarray=(unsigned char *)malloc(size);
    uncompress(d, &asize, data, datasize);
    return;
}
