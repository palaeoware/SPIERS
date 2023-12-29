/**
 * @file
 * Header: VAXML Object
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

#ifndef VAXMLOBJECT_H
#define VAXMLOBJECT_H

#include <QString>

/**
 * @brief The VAXMLObject class
 */
class VAXMLObject
{
public:
    bool visible;

    float matrix[16];
    float transparency;

    int red;
    int green;
    int blue;
    int position;

    QChar key;
    QString name;
    QString file;
    QString url;
    QString ingroup;

    VAXMLObject();
};

#endif // VAXMLOBJECT_H
