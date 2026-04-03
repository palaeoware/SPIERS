/**
 * @file
 * Header: Dark Style Theme
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

#ifndef DARKSTYLETHEME_H
#define DARKSTYLETHEME_H

#include <QApplication>
#include <QFont>
#include <QFile>
#include <QProxyStyle>
#include <QStyleFactory>

/**
 * @brief The ThemeMode enum
 * Dark  — Blender-inspired dark theme (default)
 * Light — Clean accessible light theme
 * System — Auto-detect OS preference at construction time (Qt 6.5+); falls back to Dark
 */
enum class ThemeMode {
    Dark,
    Light,
    System
};

/**
 * @brief The DarkStyleTheme class
 * Applies a Fusion-based theme with a Blender-inspired colour palette.
 * Supports dark and light modes with WCAG AA contrast compliance.
 */
class DarkStyleTheme : public QProxyStyle
{
    Q_OBJECT

public:
    // Backward-compatible: no-arg => Dark mode
    DarkStyleTheme();

    // Explicit mode selection (use ThemeMode::System for OS-aware behaviour)
    explicit DarkStyleTheme(ThemeMode mode);

    // Internal: used by constructor delegation chain
    explicit DarkStyleTheme(QStyle *style);

    void polish(QPalette &palette) override;
    void polish(QApplication *app) override;
    QStyle *baseStyle() const;

    // Returns the mode that was resolved at construction time
    ThemeMode activeMode() const;

    // Returns the mode applied to the running application (set during polish)
    static ThemeMode currentApplicationMode();

    // Detects the OS colour scheme preference (Qt 6.5+), falls back to Dark
    static ThemeMode detectSystemTheme();

    // Persist and retrieve the user's theme preference.
    // Uses QSettings("Palaeoware", "SPIERS") so the choice is shared across all apps.
    static void writeThemeSetting(ThemeMode mode);
    static ThemeMode readThemeSetting();

    // Apply a new theme to the running application immediately (live switch).
    // Saves the setting and updates style, palette, and stylesheet in one call.
    static void applyToApplication(ThemeMode mode);

private:
    ThemeMode m_mode;
    static ThemeMode s_currentMode;

    void applyDarkPalette(QPalette &palette);
    void applyLightPalette(QPalette &palette);
};

#endif // DARKSTYLETHEME_H
