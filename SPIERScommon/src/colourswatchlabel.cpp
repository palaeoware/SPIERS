/**
 * @file
 * Colour Swatch Label
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

#include "colourswatchlabel.h"
#include "customstyletheme.h"

#include <QPaintEvent>
#include <QPainter>

/**
 * @brief ColourSwatchLabel::ColourSwatchLabel
 * @param colour  Solid fill colour for the swatch.
 * @param width   Swatch width in pixels.
 * @param height  Swatch height in pixels.
 * @param parent  Parent widget.
 */
ColourSwatchLabel::ColourSwatchLabel(const QColor &colour, int width, int height, QWidget *parent)
    : QLabel(parent)
    , m_colour(colour)
{
    setFixedSize(width, height);
}

/**
 * @brief ColourSwatchLabel::setColour
 * Updates the fill colour and triggers a repaint.
 * @param colour  The new fill colour.
 */
void ColourSwatchLabel::setColour(const QColor &colour)
{
    m_colour = colour;
    update();
}

/**
 * @brief ColourSwatchLabel::paintEvent
 * Fills the widget with the swatch colour. In light theme mode a 1-pixel off-black
 * border is drawn so the swatch remains distinguishable against a light
 * background.
 */
void ColourSwatchLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), m_colour);

    if (CustomStyleTheme::currentApplicationMode() == ThemeMode::Light)
    {
        painter.setPen(QPen(QColor(0x1A, 0x1A, 0x1A), 1));
        painter.setBrush(Qt::NoBrush);
        /// adjusted() insets by 1px so the pen stroke falls entirely within the widget bounds.
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
}
