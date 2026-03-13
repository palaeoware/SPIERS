#ifndef MLFEATUREUIMANAGER_H
#define MLFEATUREUIMANAGER_H

#include <QTableWidget>

class MLCachedAccess;

class MLFeatureUIManager : public QObject
{
    Q_OBJECT
public:
    MLFeatureUIManager(MLCachedAccess *data, QTableWidget *tableWidget);
    void Rebuild();
    void ActivateSelectedFeatures(bool activate);
private slots:
    void OnTableItemChanged(QTableWidgetItem *item);
private:
    MLCachedAccess *_data;
    QTableWidget *_tableWidget;
    void SetUpTableWidget();
};

#endif // MLFEATUREUIMANAGER_H
