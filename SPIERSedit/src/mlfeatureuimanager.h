/**
 * @file
 * Header: Mlfeatureuimanager
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
#ifndef MLFEATUREUIMANAGER_H
#define MLFEATUREUIMANAGER_H

#include <QTableWidget>
#include "opencv2/core.hpp"

class MLCachedAccess;

class MLFeatureUIManager : public QObject
{
    Q_OBJECT
public:
    MLFeatureUIManager(MLCachedAccess *data, QTableWidget *tableWidget);
    ~MLFeatureUIManager();
    void Rebuild();
    void RefreshImportance();
    void ActivateSelectedFeatures(bool activate);
    int DeleteSelectedFeatures();

private slots:
    void OnTableItemChanged(QTableWidgetItem *item);
private:
    MLCachedAccess *_data;
    QTableWidget *_tableWidget;
    void SetUpTableWidget();
};

#endif // MLFEATUREUIMANAGER_H
