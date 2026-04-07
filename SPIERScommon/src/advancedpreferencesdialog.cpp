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
#include "themeselectorwidget.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

/**
 * @brief AdvancedPreferencesDialog::AdvancedPreferencesDialog
 */
AdvancedPreferencesDialog::AdvancedPreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Advanced Preferences"));
    setWindowIcon(QIcon(QStringLiteral(":/logo/palaeoware_square.png")));
    setMinimumWidth(320);

    m_themeWidget = new ThemeSelectorWidget(this);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &AdvancedPreferencesDialog::onAccepted);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_themeWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(buttons);
    setLayout(mainLayout);
}

/**
 * @brief AdvancedPreferencesDialog::onAccepted
 * Applies the chosen theme live and closes the dialog.
 */
void AdvancedPreferencesDialog::onAccepted()
{
    m_themeWidget->applyTheme();
    accept();
}
