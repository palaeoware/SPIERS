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
