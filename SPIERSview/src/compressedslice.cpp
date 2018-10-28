#include "compressedslice.h"
#include <zlib.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "spv.h"
#include "svobject.h"
#include "globals.h"

/**
 * @brief CompressedSlice::CompressedSlice
 * @param parent
 * @param isEmpty
 */
CompressedSlice::CompressedSlice(SVObject *parent, bool isEmpty)
{
    empty = isEmpty; //for now
    svObject = parent;
    data = nullptr;
    datasize = 0;
    grid = static_cast<unsigned char *>(malloc(static_cast<size_t>(parent->spv->GridSize)));

    for (int i = 0; i < parent->spv->GridSize; i++)
        grid[i] = 0;
}

/**
 * @brief CompressedSlice::~CompressedSlice
 */
CompressedSlice::~CompressedSlice()
{
    if (data) free (data);
    free (grid);
}

/**
 * @brief CompressedSlice::dump
 * Dumps the slice as a BMP
 * @param filename
 */
void CompressedSlice::dump(QString filename)
{
    // Work out checksum of data and datasize
    int size = svObject->spv->size;
    uLongf asize = static_cast<uLong>(size);

    // Work out checksum of data and datasize
    long total = 0;
    for (int i = 0; i < datasize; i++) total += static_cast<long>(data[i]);

    int tot = static_cast<int>(total);
    QString fn = QString("_%1_%2_.bmp").arg(datasize).arg(tot);
    filename += fn;

    unsigned char *mergearray = static_cast<unsigned char *>(malloc(static_cast<size_t>(size))); //THIS slice
    uncompress(mergearray, &asize, data, static_cast<uLong>(datasize)); //uncompress THIS slice

    int width = svObject->spv->iDim;
    int height = svObject->spv->jDim;

    QImage bmp(width, height, QImage::Format_RGB32);
    bmp.fill(0);

    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            if (mergearray[y * width + x]) bmp.setPixel(x, y, mergearray[y * width + x]);
        }

    bmp.save(filename);
    free (mergearray);
}

/**
 * @brief CompressedSlice::merge
 * @param s
 * @param fn
 */
void CompressedSlice::merge(CompressedSlice *slice, QString filename)
{
    Q_UNUSED(filename)

    if (slice->empty) return;

    if (empty)
    {
        //I'm empty, this one isn't, so can just copy data
        datasize = slice->datasize;
        data = static_cast<unsigned char *>(malloc(static_cast<size_t>(datasize)));
        memcpy(data, slice->data, static_cast<size_t>(datasize)); //copy it
        empty = false;

        //and copy the grid
        for (int i = 0; i < (svObject->spv->GridSize); i++)
            grid[i] = slice->grid[i];

        return;
    }

    //Both have data.
    //Decompress both arrays, merge, recompress
    int size = svObject->spv->size;
    uLongf asize = static_cast<uLong>(size);

    unsigned char *mergearray = static_cast<unsigned char *>(malloc(static_cast<size_t>(size))); //THIS slice
    unsigned char *mergearray2 = static_cast<unsigned char *>(malloc(static_cast<size_t>(size))); //for the new one to be merged in

    for (int i = 0; i < size; i++) mergearray[i] = static_cast<unsigned char>(0);
    for (int i = 0; i < size; i++) mergearray2[i] = static_cast<unsigned char>(0);

    uncompress(mergearray, &asize, data, static_cast<uLong>(datasize)); //uncompress THIS slice

    asize = static_cast<uLong>(size);
    uncompress(mergearray2, &asize, slice->data, static_cast<uLong>(slice->datasize)); //uncompress the new slice

    // Do the merge
    for (int i = 0; i < size; i++)
        if (mergearray[i])
            mergearray2[i] = static_cast<unsigned char>(255); //New array (merge2) will be the joint one

    // ... and recompress it
    uLongf tempsize;

    // Compress it back to old array
    compress(mergearray, &tempsize, mergearray2, static_cast<uLong>(size));

    datasize = static_cast<int>(tempsize);
    free(data); //free up old memory
    free(mergearray2);
    data = static_cast<unsigned char *>(malloc(static_cast<size_t>(datasize)));
    memcpy(data, mergearray, static_cast<size_t>(datasize));

    free(mergearray);

    // Merge the grids
    for (int i = 0; i < (svObject->spv->GridSize); i++)
        if (slice->grid[i])
            grid[i] = slice->grid[i]; //copy non-zeroes

    return;
}

/**
 * @brief CompressedSlice::getFullData
 * @param dest
 */
void CompressedSlice::getFullData(unsigned char *dest)
{
    int size = svObject->spv->size;
    uLongf asize =  static_cast<uLongf>(size);
    uncompress(dest, &asize, data, static_cast<uLong>(datasize));
    return;
}
