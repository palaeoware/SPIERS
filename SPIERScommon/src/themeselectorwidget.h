/**
 * @file
 * Header: Theme Selector Widget
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

#ifndef THEMESELECTORWIDGET_H
#define THEMESELECTORWIDGET_H

#include <QComboBox>
#include <QWidget>

#include "customstyletheme.h"

/**
 * @brief The ThemeSelectorWidget class
 * A self-contained widget providing a labelled theme combo box inside an
 * "Appearance" group box, plus a brief note label. It can be embedded in any
 * settings dialog without duplicating the population or apply logic.
 *
 * Usage:
 *   - Instantiate and add to a layout.
 *   - Call applyTheme() when the parent dialog is accepted.
 */
class ThemeSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeSelectorWidget(QWidget *parent = nullptr);

    /**
     * @brief applyTheme
     * Reads the current combo selection, saves the preference, and applies the
     * chosen theme to the running application immediately.
     */
    void applyTheme();

private:
    QComboBox *m_combo; /// The theme selection combo box
};

#endif // THEMESELECTORWIDGET_H
