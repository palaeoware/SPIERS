/**
 * @file
 * Theme Selector Widget
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

#include "themeselectorwidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief ThemeSelectorWidget::ThemeSelectorWidget
 * Builds the "Appearance" group box containing a Theme label and combo box,
 * followed by a brief note, and selects the currently saved preference.
 */
ThemeSelectorWidget::ThemeSelectorWidget(QWidget *parent)
    : QWidget(parent)
{
    // --- Theme combo ---
    m_combo = new QComboBox(this);
    m_combo->setToolTip(tr("Choose the application colour theme. "
                           "The change is applied immediately."));
    m_combo->addItem(tr("Follow system"), static_cast<int>(ThemeMode::System));
    m_combo->addItem(tr("Dark"),          static_cast<int>(ThemeMode::Dark));
    m_combo->addItem(tr("Light"),         static_cast<int>(ThemeMode::Light));

    const ThemeMode saved = CustomStyleTheme::readThemeSetting();
    for (int i = 0; i < m_combo->count(); ++i)
    {
        if (m_combo->itemData(i).toInt() == static_cast<int>(saved))
        {
            m_combo->setCurrentIndex(i);
            break;
        }
    }

    // --- Group box row ---
    QLabel *themeLabel = new QLabel(tr("Theme:"), this);

    QHBoxLayout *comboRow = new QHBoxLayout;
    comboRow->addWidget(themeLabel);
    comboRow->addWidget(m_combo, 1);

    QGroupBox *group = new QGroupBox(tr("Appearance"), this);
    group->setLayout(comboRow);

    // --- Note label ---
    QLabel *noteLabel = new QLabel(
        tr("<small>The theme is applied immediately.</small>"), this);
    noteLabel->setWordWrap(true);

    // --- Outer layout ---
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(group);
    outerLayout->addWidget(noteLabel);
    setLayout(outerLayout);
}

/**
 * @brief ThemeSelectorWidget::applyTheme
 * Reads the current combo selection and applies the chosen theme to the running
 * application, saving the preference at the same time.
 */
void ThemeSelectorWidget::applyTheme()
{
    const ThemeMode chosen = static_cast<ThemeMode>(m_combo->currentData().toInt());
    CustomStyleTheme::applyToApplication(chosen);
}
