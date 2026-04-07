/**
 * @file
 * Header: Colour Swatch Label
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2019 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef COLOURSWATCHLABEL_H
#define COLOURSWATCHLABEL_H

#include <QColor>
#include <QLabel>

/**
 * @brief The ColourSwatchLabel class
 * A fixed-size QLabel that paints a solid colour rectangle. In light theme mode
 * a 1-pixel black border is drawn around the swatch so it remains visible
 * against a light background.
 *
 * This replaces the repeated QPicture/QPainter boilerplate used in tree widget
 * item refresh functions across SPIERSedit and SPIERSview.
 */
class ColourSwatchLabel : public QLabel
{
    Q_OBJECT

public:
    /**
     * @brief ColourSwatchLabel
     * @param colour  The solid fill colour for the swatch.
     * @param width   Swatch width in pixels (default 28).
     * @param height  Swatch height in pixels (default 20).
     * @param parent  Parent widget.
     */
    explicit ColourSwatchLabel(const QColor &colour, int width = 28, int height = 20, QWidget *parent = nullptr);

    /**
     * @brief setColour
     * Updates the swatch colour and schedules a repaint.
     * @param colour  The new fill colour.
     */
    void setColour(const QColor &colour);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor m_colour; /// Current fill colour for the swatch
};

#endif // COLOURSWATCHLABEL_H
