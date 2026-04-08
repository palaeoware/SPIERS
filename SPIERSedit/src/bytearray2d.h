/**
 * @file
 * Header: Bytearray2d
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */
#ifndef BYTEARRAY2D_H
#define BYTEARRAY2D_H

#include <QByteArray>
#include <cstdint>

class ByteArray2D
{
public:
    ByteArray2D(int width, int height);

    uint8_t& at(int x, int y);
    uint8_t at(int x, int y) const;

    int getWidth() const;
    int getHeight() const;

private:
    int m_width;
    int m_height;
    QByteArray m_data;
};

#endif
