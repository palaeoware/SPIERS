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
