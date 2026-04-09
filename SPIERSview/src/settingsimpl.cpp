/**
 * @file
 * Source: SettingsImpl
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "settingsimpl.h"
#include "../../SPIERScommon/src/themeselectorwidget.h"
#include "blenderbridge.h"

#include <QVBoxLayout>
#include <QSettings>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>

SettingsImpl::SettingsImpl(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon(":/icons/ProgramIcon.bmp"));

    // Load saved Blender path
    QSettings settings(QStringLiteral("Palaeoware"), QStringLiteral("SPIERS"));
    QString savedPath = settings.value(QStringLiteral("BlenderPath"), QString()).toString();
    QString blenderPath = savedPath.isEmpty() ? detectBlenderPath() : savedPath;
    blenderPathEdit->setText(blenderPath);

    // Browse button
    connect(browseBtn, &QPushButton::clicked, this, &SettingsImpl::onBlenderBrowse);

    // Auto-detect button
    connect(resetBtn, &QPushButton::clicked, this, &SettingsImpl::onBlenderReset);

    // Embed the shared theme selector into the placeholder widget from the .ui
    m_themeSelector = new ThemeSelectorWidget(this);
    QVBoxLayout *placeholderLayout2 = new QVBoxLayout(appearancePlaceholder);
    placeholderLayout2->setContentsMargins(0, 0, 0, 0);
    placeholderLayout2->addWidget(m_themeSelector);
}

void SettingsImpl::on_buttonBox_accepted()
{
    if (blenderPathEdit)
    {
        QSettings settings(QStringLiteral("Palaeoware"), QStringLiteral("SPIERS"));
        settings.setValue(QStringLiteral("BlenderPath"), blenderPathEdit->text());
    }

    // Apply theme immediately and save preference
    m_themeSelector->applyTheme();

    close();
}

void SettingsImpl::on_buttonBox_rejected()
{
    close();
}

/**
 * @brief AdvancedPreferencesDialog::onBlenderBrowse
 * Opens file dialog to select Blender executable.
 */
void SettingsImpl::onBlenderBrowse()
{
    if (!blenderPathEdit) return;

    QString filter;
#ifdef Q_OS_WIN
    filter = tr("Blender Executable (blender.exe);;All Files (*)");
#else
    filter = tr("Blender Executable (blender);;All Files (*)");
#endif

    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Select Blender Executable"), blenderPathEdit->text(), filter);
    if (!path.isEmpty())
        blenderPathEdit->setText(path);
}

/**
 * @brief AdvancedPreferencesDialog::onBlenderReset
 * Resets Blender path to auto-detected value.
 */
void SettingsImpl::onBlenderReset()
{
    if (blenderPathEdit)
        blenderPathEdit->setText(detectBlenderPath());
}

/**
 * @brief AdvancedPreferencesDialog::detectBlenderPath
 * Auto-detects Blender installation using BlenderBridge detection logic.
 */
QString SettingsImpl::detectBlenderPath()
{
    return BlenderBridge::getBlenderExecutable();
}
