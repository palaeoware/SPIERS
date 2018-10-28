#ifndef COMPRESSEDSLICE_H
#define COMPRESSEDSLICE_H

#include <QString>

class SVObject;

/**
 * @brief The CompressedSlice class
 */
class CompressedSlice
{
public:
    CompressedSlice(SVObject *parent, bool isEmpty);
    ~CompressedSlice();

    void merge(CompressedSlice *slice, QString filename);
    void dump(QString filename);
    void getFullData(unsigned char *);

    unsigned char *data;
    unsigned char *grid;
    bool empty;
    int datasize;
    SVObject *svObject;
};

#endif // COMPRESSEDSLICE_H
