#ifndef COMPRESSEDSLICE_H
#define COMPRESSEDSLICE_H
#include <QString>
class SVObject;
class CompressedSlice
{
public:
    CompressedSlice(SVObject *parent, bool IsEmpty);
    ~CompressedSlice();
    void Merge(CompressedSlice *s, QString fn);
    void Dump(QString Filename);

    void GetFullData(unsigned char *);

    unsigned char * data; //this is the actual compressed data
    int datasize;
    unsigned char *grid;
    bool empty;
    SVObject *Object;
};

#endif // COMPRESSEDSLICE_H
