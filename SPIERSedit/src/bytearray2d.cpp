/**
 * @file
 * Source: Bytearray2d
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
#include "bytearray2d.h"

ByteArray2D::ByteArray2D(int width, int height)
    : m_width(width),
      m_height(height),
      m_data(width * height, 0)
{
}

uint8_t &ByteArray2D::at(int x, int y)
{
    return reinterpret_cast<uint8_t &>(m_data[y * m_width + x]);
}

uint8_t ByteArray2D::at(int x, int y) const
{
    return static_cast<uint8_t>(m_data[y * m_width + x]);
}

int ByteArray2D::getWidth() const
{
    return m_width;
}

int ByteArray2D::getHeight() const
{
    return m_height;
}
