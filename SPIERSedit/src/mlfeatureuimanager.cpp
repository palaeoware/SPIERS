#include "mlfeatureuimanager.h"
#include "mlcachedaccess.h"

#include <QHeaderView>
#include <QObject>
#include "globals.h"

MLFeatureUIManager::MLFeatureUIManager(MLCachedAccess *data, QTableWidget *tableWidget)
{

    _data = data;
    _tableWidget = tableWidget;

    connect(_tableWidget,
            &QTableWidget::itemChanged,
            this,
            &MLFeatureUIManager::OnTableItemChanged);
    qDebug()<<"Set up table widget";
    SetUpTableWidget();

}

void MLFeatureUIManager::OnTableItemChanged(QTableWidgetItem *item)
{
    if (item->column() != 0)   // only react to checkbox column
        return;

    bool checked = (item->checkState() == Qt::Checked);
    int featureID = item->data(Qt::UserRole).toInt();

    qDebug() << "Feature" << featureID << "enabled:" << checked;

    _data->SetFeatureInUse(featureID, checked);
}

void MLFeatureUIManager::Rebuild()
{

    _tableWidget->setUpdatesEnabled(false);
    _tableWidget->setSortingEnabled(false);
    _tableWidget->setRowCount(_data->GetFeatureCount());

    for (int i=0; i<_data->GetFeatureCount(); i++)
    {
        MLFeature *feature = _data->GetFeature(i);

        _tableWidget->setItem(i,1,new QTableWidgetItem(feature->GetPrettyName()));
        _tableWidget->setItem(i,2,new QTableWidgetItem(feature->GetPrettyChannel()));
        _tableWidget->setItem(i,3,new QTableWidgetItem(feature->GetPretty3D()));
        _tableWidget->setItem(i,4,new QTableWidgetItem(feature->GetPrettyArgs()));

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (feature->IsSelected())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        item->setData(Qt::UserRole, i);
        _tableWidget->setItem(i,0,item);


    }

    auto *header = _tableWidget->horizontalHeader();
    header->setMinimumSectionSize(1);
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);          // this column expands
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    _tableWidget->setUpdatesEnabled(true);
    _tableWidget->setSortingEnabled(true);
    _tableWidget->clearSelection();
    _tableWidget->setCurrentItem(nullptr);
}

void MLFeatureUIManager::DeleteSelectedFeatures()
{
    int maxDependencyDepth = -1;
    QList<MLFeature *> toDelete;
    for (int i=0; i<_tableWidget->rowCount(); i++)
    {
        if (_tableWidget->selectionModel()->isRowSelected(i, QModelIndex()))
        {
            MLFeature *feature = _data->GetFeature(_tableWidget->item(i,0)->data(Qt::UserRole).toInt());

            toDelete.append(feature);
            int depDepth = feature->GetDependencyDepth();
            if (depDepth>maxDependencyDepth)
                maxDependencyDepth = depDepth;
        }
    }

    //now make lists in dependency depth order. We have to delete highs first
    QList<QList<MLFeature *>> deleteByDepth;
    for (int i=0; i<=maxDependencyDepth; i++)
        deleteByDepth.append(QList<MLFeature*>());

    for (int i=0; i<toDelete.count(); i++)
    {
        MLFeature *feature = toDelete[i];
        deleteByDepth[feature->GetDependencyDepth()].append(feature);
    }

    QList<MLFeature *> failedList;
    for (int j=maxDependencyDepth; j>=0; j--)
    {
        for (int i=0; i<deleteByDepth[j].count(); i++)
        {
            MLFeature *feature = deleteByDepth[j][i];
            if (!_data->RemoveFeature(_data->GetIndexForFeature(feature)))
                failedList.append(feature);
        }
    }

    Rebuild();
    if (failedList.count()==1)
    {
        Message(QString("Could not remove feature %1 as another feature depends on it").arg(failedList[0]->GetPrettyFullName()));
    }
    if (failedList.count()>1)
    {
        Message(QString("Could not remove %1 features as other features depend on them").arg(failedList.count()));
    }
}

void MLFeatureUIManager::ActivateSelectedFeatures(bool activate)
{
    _tableWidget->setUpdatesEnabled(false);
    for (int i=0; i<_tableWidget->rowCount(); i++)
    {
        if (_tableWidget->selectionModel()->isRowSelected(i, QModelIndex()))
        {
            if (activate)
                _tableWidget->item(i,0)->setCheckState(Qt::Checked);
            else
                _tableWidget->item(i,0)->setCheckState(Qt::Unchecked);
        }

        int featureID = _tableWidget->item(i,0)->data(Qt::UserRole).toInt();
        _data->SetFeatureInUse(featureID,activate);
    }
     _tableWidget->setUpdatesEnabled(true);
}

void MLFeatureUIManager::SetUpTableWidget()
{
    _tableWidget->clear();
    _tableWidget->setRowCount(0);
    _tableWidget->setColumnCount(0);

    _tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section { padding-left: 2px; padding-right: 2px; }");
    _tableWidget->setColumnCount(5);
    _tableWidget->setHorizontalHeaderLabels({
        " ",
        "Feature",
        "Channel",
        "Dim",
        "Args"
    });

    //left align them all
    for (int i = 0; i < _tableWidget->columnCount(); ++i)
    {
        QTableWidgetItem *item = _tableWidget->horizontalHeaderItem(i);
        if (item)
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    _tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    _tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _tableWidget->verticalHeader()->setVisible(false);
    _tableWidget->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    _tableWidget->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);

    _tableWidget->setSortingEnabled(true);
    _tableWidget->horizontalHeader()->setSortIndicatorShown(true);


}
