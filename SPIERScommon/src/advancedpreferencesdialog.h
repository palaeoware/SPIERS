/**
 * @file
 * Header: Advanced Preferences Dialog
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

#ifndef ADVANCEDPREFERENCESDIALOG_H
#define ADVANCEDPREFERENCESDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "darkstyletheme.h"

/**
 * @brief The AdvancedPreferencesDialog class
 * A simple shared dialog that lets the user choose the application theme.
 * Used by SPIERSview, SPIERSalign, and SPIERSversion; SPIERSedit embeds the
 * same theme controls directly into its own Global Settings dialog.
 */
class AdvancedPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedPreferencesDialog(QWidget *parent = nullptr);

private slots:
    void onAccepted();

private:
    QComboBox *m_themeCombo;
};

#endif // ADVANCEDPREFERENCESDIALOG_H
