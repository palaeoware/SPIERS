#ifndef SPVREADER_H
#define SPVREADER_H

#include <QString>
#include <QProgressBar>
#include <QLabel>

class SPV;

//Reads and writes an SPV/sp2
class SPVreader
{
public:
    SPVreader();
    void ProcessFile(QString filename);
    void WriteFile(bool);
    void WriteFinalised();
    int ProcessFileReplacement(QString filename, int SPVindex);
    void TransposeMatrix(float *m);
private:
    QString FileName;

    void FixUpData();
    void InternalProcessFile(QString filename);
    bool is_sp2(char *fn);
    void FileFailed(QString fname, bool write, int n);
    int ProcessSPV(QString filename, unsigned int index, float *PassedMatrix);
    void WriteWithEndian(FILE *file, unsigned char *pointer, int size, int count);
    void invert_endian(unsigned char *data, int count);

    int ReplaceIndex;
    void WriteSPV(bool withpd);
    void ReadSPV6(QString Filename);
};

#endif // SPVREADER_H
