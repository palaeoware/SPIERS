#ifndef SPVREADER_H
#define SPVREADER_H

#include <QString>
#include <QProgressBar>
#include <QLabel>

class SPV;

/**
 * @brief The SPVReader class
 */
class SPVReader
{
public:
    SPVReader();
    void processFile(QString filename);
    int processFileReplacement(QString filename, int spvIndex);

private:
    int replaceIndex;
    QString currentFilename;

    void fixKeyCodeData();
    void internalProcessFile(QString filename);
    void fileReadFailed(QString filename, bool write, int n);
    int processSPV(QString filename, float *passedMatrix);
    void version5Below(QString filename, float *passedMatrix);
    void version6Plus(QString filename);
};

#endif // SPVREADER_H
