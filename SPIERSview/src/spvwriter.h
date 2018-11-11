/**
 * @file
 * Header: SPV Writer
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

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
