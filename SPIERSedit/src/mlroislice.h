/**
 * @file
 * Header: Mlroislice
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
#ifndef MLROISLICE_H
#define MLROISLICE_H

#include <QByteArray>

class MLROISlice
{
public:
    enum class TileState : char
    {
        Inactive = 0,
        Target = 1,
        Halo = 2
    };

    static constexpr int TARGET_TILE_COUNT = 100;

    MLROISlice();
    MLROISlice(
        int width,
        int height,
        const QByteArray &excludedPixels,
        int tileSize = 0);

    int activePixelCount() const;
    void addHaloPixels(int radius);
    static int adaptiveTileSize(
        int width,
        int height,
        int targetTileCount = TARGET_TILE_COUNT);
    const QByteArray &excludedPixels() const;
    int haloTileCount() const;
    int height() const;
    bool isPixelActive(int x, int y) const;
    bool isValid() const;
    int targetTileCount() const;
    int tileColumns() const;
    int tileRows() const;
    int tileSize() const;
    TileState tileState(int tileX, int tileY) const;
    int totalPixelCount() const;
    int totalTileCount() const;
    int width() const;

private:
    int m_activePixelCount = 0;
    QByteArray m_excludedPixels;
    int m_height = 0;
    int m_haloTileCount = 0;
    int m_targetTileCount = 0;
    int m_tileColumns = 0;
    int m_tileRows = 0;
    int m_tileSize = 0;
    QByteArray m_tileStates;
    bool m_valid = false;
    int m_width = 0;
};

#endif // MLROISLICE_H
