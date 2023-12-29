/**
 * @file
 * Header: VAXML
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef VAXML_H
#define VAXML_H

#include <QString>
#include <QXmlStreamReader>
#include <QStringList>
#include <QFile>
#include <QApplication>

#include "mainwindow.h"
#include "spv.h"
#include "vaxmlgroup.h"
#include "vaxmlobject.h"

/**
 * @brief The VAXML class
 * Provides read and write functions for VAXML and stl interface
 */
class VAXML
{
public:

    //header info
    float matrix[16];
    float scale;
    int version;
    QDataStream dataout;
    QFile file;
    QList <VAXMLGroup *> groups;
    QList <VAXMLObject *> objects;
    QString title;
    QStringList author;
    QStringList comments;
    QStringList classification_name;
    QStringList classification_rank;
    QStringList provenance;
    QStringList reference;
    QStringList specimen;

    VAXML();
    ~VAXML();
    bool readSPVF(QString fname); //yes, in VAXML, as it's a bodged VAXML import really
    bool readVAXML(QString fname);
    bool writeVAXML(QString fname, bool mode);

private:
    bool readElement(QXmlStreamReader *xml);
    bool xmlError(QString ErrorText);
    float convTransExport(int t);
    int findGroupFromName(SPV *spv, QString name);
    int convTrans(float t);
    QString encode(QString text);
};

#endif // VAXML_H
