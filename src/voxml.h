#ifndef VOXML_H
#define VOXML_H

#include <QString>
#include <QXmlStreamReader>
#include <QStringList>
#include <QFile>
#include <QApplication>
#include "mainwindow.h"

#include "spv.h"

//Provides read and write functions for voxml and stl interface


class voxml_group
{
public:
     QString name;
     QChar key;
     bool visible;
     QString ingroup;
     int position;

     voxml_group();
};

class voxml_object
{
public:
     QString name;
     QChar key;
     bool visible;
     QString file;
     QString url;
     QString ingroup;
     float matrix[16];
     int red,green,blue;
     float transparency;
     int position;
     voxml_object();

};

class voxml
{
public:

    //header info
    int version;
    QString title;
    float scale;
    float matrix[16];
    QStringList comments, reference, author, classification_name, classification_rank, provenance, specimen;

    QList <voxml_group *> groups;
    QList <voxml_object *> objects;
    QFile file;
    QDataStream dataout;
    voxml();
    ~voxml();

    bool read_voxml(QString fname);
    bool write_voxml(QString fname, bool mode);
    bool read_spvf(QString fname); //yes, in voxml, as it's a bodged voxml import really

private:
    bool read_element(QXmlStreamReader *xml);
    bool xml_error(QString ErrorText);
    int find_group_from_name(SPV *spv, QString name);
    int conv_trans(float t);
    float conv_trans_export(int t);
    QString encode(QString text);
};

#endif // VOXML_H
