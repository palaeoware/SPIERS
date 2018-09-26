#ifndef COMPRESSEDSLICE_H
#define COMPRESSEDSLICE_H
#include <QString>
class svobject;
class CompressedSlice
{
public:
    CompressedSlice(svobject *parent, bool IsEmpty);
    ~CompressedSlice();
    void Merge(CompressedSlice *s, QString fn);
    void Dump(QString Filename);

    void GetFullData(unsigned char *);

    unsigned char * data; //this is the actual compressed data
    int datasize;
    unsigned char *grid;
    bool empty;
    svobject *Object;
};

#endif // COMPRESSEDSLICE_H
