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
