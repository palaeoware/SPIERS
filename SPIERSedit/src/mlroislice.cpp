/**
 * @file
 * Source: Mlroislice
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
#include "mlroislice.h"

#include <QtGlobal>

MLROISlice::MLROISlice()
{
}

MLROISlice::MLROISlice(
    int width,
    int height,
    const QByteArray &excludedPixels,
    int tileSize)
    : m_height(height)
    , m_tileSize(tileSize)
    , m_width(width)
{
    if (m_width <= 0 || m_height <= 0)
        return;

    if (m_tileSize <= 0)
    {
        m_tileSize = adaptiveTileSize(
            m_width,
            m_height,
            TARGET_TILE_COUNT);
    }

    const qsizetype expectedPixelCount =
        static_cast<qsizetype>(m_width) * m_height;
    if (excludedPixels.size() != expectedPixelCount)
        return;

    m_excludedPixels = excludedPixels;
    m_tileColumns = ((m_width - 1) / m_tileSize) + 1;
    m_tileRows = ((m_height - 1) / m_tileSize) + 1;
    m_tileStates.fill(
        static_cast<char>(TileState::Inactive),
        m_tileColumns * m_tileRows);

    for (int y = 0; y < m_height; y++)
    {
        const char *excludedRow =
            m_excludedPixels.constData() + static_cast<qsizetype>(y) * m_width;
        const int tileY = y / m_tileSize;

        for (int x = 0; x < m_width; x++)
        {
            if (excludedRow[x] != 0)
                continue;

            m_activePixelCount++;
            const int tilePosition =
                tileY * m_tileColumns + (x / m_tileSize);
            if (m_tileStates.at(tilePosition)
                == static_cast<char>(TileState::Inactive))
            {
                m_tileStates[tilePosition] =
                    static_cast<char>(TileState::Target);
                m_targetTileCount++;
            }
        }
    }

    m_valid = true;
}

int MLROISlice::activePixelCount() const
{
    return m_activePixelCount;
}

void MLROISlice::addHaloPixels(int radius)
{
    if (!m_valid)
        return;

    m_haloTileCount = 0;
    for (int tile = 0; tile < m_tileStates.size(); tile++)
    {
        if (m_tileStates.at(tile) == static_cast<char>(TileState::Halo))
            m_tileStates[tile] = static_cast<char>(TileState::Inactive);
    }

    if (radius == 0)
        return;

    if (radius < 0)
    {
        for (int tile = 0; tile < m_tileStates.size(); tile++)
        {
            if (m_tileStates.at(tile)
                == static_cast<char>(TileState::Inactive))
            {
                m_tileStates[tile] = static_cast<char>(TileState::Halo);
                m_haloTileCount++;
            }
        }
        return;
    }

    const int tileRadius = (radius + m_tileSize - 1) / m_tileSize;
    QByteArray expandedStates = m_tileStates;

    for (int tileY = 0; tileY < m_tileRows; tileY++)
    {
        for (int tileX = 0; tileX < m_tileColumns; tileX++)
        {
            if (tileState(tileX, tileY) != TileState::Target)
                continue;

            const int firstTileX = qMax(0, tileX - tileRadius);
            const int lastTileX = qMin(m_tileColumns - 1, tileX + tileRadius);
            const int firstTileY = qMax(0, tileY - tileRadius);
            const int lastTileY = qMin(m_tileRows - 1, tileY + tileRadius);

            for (int haloY = firstTileY; haloY <= lastTileY; haloY++)
            {
                for (int haloX = firstTileX; haloX <= lastTileX; haloX++)
                {
                    const int haloPosition =
                        haloY * m_tileColumns + haloX;
                    if (expandedStates.at(haloPosition)
                        == static_cast<char>(TileState::Inactive))
                    {
                        expandedStates[haloPosition] =
                            static_cast<char>(TileState::Halo);
                    }
                }
            }
        }
    }

    m_tileStates = expandedStates;
    for (char state : m_tileStates)
    {
        if (state == static_cast<char>(TileState::Halo))
            m_haloTileCount++;
    }
}

int MLROISlice::adaptiveTileSize(
    int width,
    int height,
    int targetTileCount)
{
    if (width <= 0 || height <= 0 || targetTileCount <= 0)
        return 0;

    int minimumSize = 1;
    int maximumSize = qMax(width, height);

    while (minimumSize < maximumSize)
    {
        const int candidateSize =
            minimumSize + (maximumSize - minimumSize) / 2;
        const qint64 tileColumns =
            (static_cast<qint64>(width) + candidateSize - 1)
            / candidateSize;
        const qint64 tileRows =
            (static_cast<qint64>(height) + candidateSize - 1)
            / candidateSize;

        if (tileColumns * tileRows > targetTileCount)
            minimumSize = candidateSize + 1;
        else
            maximumSize = candidateSize;
    }

    return minimumSize;
}

const QByteArray &MLROISlice::excludedPixels() const
{
    return m_excludedPixels;
}

MLROISlice MLROISlice::expandedByPixels(int radius) const
{
    MLROISlice expanded = *this;
    if (!expanded.m_valid)
        return expanded;

    if (radius < 0 || expanded.m_dependencyExpansionPixels < 0)
    {
        expanded.m_dependencyExpansionPixels = -1;
    }
    else
    {
        expanded.m_dependencyExpansionPixels += radius;
    }

    expanded.addHaloPixels(expanded.m_dependencyExpansionPixels);
    return expanded;
}

int MLROISlice::haloTileCount() const
{
    return m_haloTileCount;
}

int MLROISlice::height() const
{
    return m_height;
}

bool MLROISlice::isPixelActive(int x, int y) const
{
    if (!m_valid || x < 0 || x >= m_width || y < 0 || y >= m_height)
        return false;

    return m_excludedPixels.at(
               static_cast<qsizetype>(y) * m_width + x)
           == 0;
}

bool MLROISlice::isValid() const
{
    return m_valid;
}

int MLROISlice::requiredTileCount() const
{
    return m_targetTileCount + m_haloTileCount;
}

int MLROISlice::targetTileCount() const
{
    return m_targetTileCount;
}

int MLROISlice::tileColumns() const
{
    return m_tileColumns;
}

int MLROISlice::tileRows() const
{
    return m_tileRows;
}

int MLROISlice::tileSize() const
{
    return m_tileSize;
}

MLROISlice::TileState MLROISlice::tileState(int tileX, int tileY) const
{
    if (!m_valid
        || tileX < 0
        || tileX >= m_tileColumns
        || tileY < 0
        || tileY >= m_tileRows)
    {
        return TileState::Inactive;
    }

    return static_cast<TileState>(
        m_tileStates.at(tileY * m_tileColumns + tileX));
}

QRect MLROISlice::tileRect(int tileX, int tileY) const
{
    if (!m_valid
        || tileX < 0
        || tileX >= m_tileColumns
        || tileY < 0
        || tileY >= m_tileRows)
    {
        return QRect();
    }

    const int left = tileX * m_tileSize;
    const int top = tileY * m_tileSize;
    return QRect(
        left,
        top,
        qMin(m_tileSize, m_width - left),
        qMin(m_tileSize, m_height - top));
}

int MLROISlice::totalPixelCount() const
{
    return m_width * m_height;
}

int MLROISlice::totalTileCount() const
{
    return m_tileColumns * m_tileRows;
}

int MLROISlice::width() const
{
    return m_width;
}
