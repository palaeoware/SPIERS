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
    int ProcessFileReplacement(QString filename, int SPVindex);
private:

    int ReplaceIndex;
    QString FileName;

    void FixUpData();
    void InternalProcessFile(QString filename);
    bool is_sp2(char *fn);
    void FileFailed(QString fname, bool write, int n);
    int ProcessSPV(QString filename, unsigned int index, float *PassedMatrix);
    void ReadSPV6(QString Filename);
};

#endif // SPVREADER_H
