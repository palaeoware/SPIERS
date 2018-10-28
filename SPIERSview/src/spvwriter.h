#ifndef SPVWRITER_H
#define SPVWRITER_H


class SPVWriter
{
public:
    SPVWriter();
    void writeFile(bool);
    void writeFinalised();

private:
    void writeSPV(bool withPolydata);
};

#endif // SPVWRITER_H
