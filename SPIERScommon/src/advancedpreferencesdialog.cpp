/**
 * @file
 * Source: Advanced Preferences Dialog
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

#include "advancedpreferencesdialog.h"
#include <QMessageBox>

/**
 * @brief AdvancedPreferencesDialog::AdvancedPreferencesDialog
 */
AdvancedPreferencesDialog::AdvancedPreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Advanced Preferences"));
    setWindowIcon(QIcon(":/logo/palaeoware_square.png"));
    setMinimumWidth(320);

    // --- Theme group ---
    QGroupBox *themeGroup = new QGroupBox(tr("Appearance"), this);

    QLabel *themeLabel = new QLabel(tr("Theme:"), themeGroup);

    m_themeCombo = new QComboBox(themeGroup);
    m_themeCombo->addItem(tr("Follow system"),  static_cast<int>(ThemeMode::System));
    m_themeCombo->addItem(tr("Dark"),           static_cast<int>(ThemeMode::Dark));
    m_themeCombo->addItem(tr("Light"),          static_cast<int>(ThemeMode::Light));
    m_themeCombo->setToolTip(tr("Choose the application colour theme. "
                                "The change is applied immediately."));

    // Select current saved preference
    const ThemeMode saved = DarkStyleTheme::readThemeSetting();
    for (int i = 0; i < m_themeCombo->count(); ++i)
    {
        if (m_themeCombo->itemData(i).toInt() == static_cast<int>(saved))
        {
            m_themeCombo->setCurrentIndex(i);
            break;
        }
    }

    QHBoxLayout *themeRow = new QHBoxLayout;
    themeRow->addWidget(themeLabel);
    themeRow->addWidget(m_themeCombo, 1);
    themeGroup->setLayout(themeRow);

    // --- Note label ---
    QLabel *noteLabel = new QLabel(
        tr("<small>The theme is applied immediately. "
           "Icons in open file trees will update on next file open.</small>"),
        this);
    noteLabel->setWordWrap(true);

    // --- Button box ---
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &AdvancedPreferencesDialog::onAccepted);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // --- Layout ---
    QVBoxLayout *main = new QVBoxLayout(this);
    main->addWidget(themeGroup);
    main->addWidget(noteLabel);
    main->addStretch();
    main->addWidget(buttons);
    setLayout(main);
}

/**
 * @brief AdvancedPreferencesDialog::onAccepted
 * Saves the chosen theme and applies it live to the running application.
 */
void AdvancedPreferencesDialog::onAccepted()
{
    const ThemeMode chosen = static_cast<ThemeMode>(
        m_themeCombo->currentData().toInt());
    DarkStyleTheme::applyToApplication(chosen);
    accept();
}
