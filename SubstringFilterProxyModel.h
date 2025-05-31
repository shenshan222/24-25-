#ifndef SUBSTRINGFILTERPROXYMODEL_H//实现地点模糊匹配的类
#define SUBSTRINGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SubstringFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);
        QString itemText = sourceModel()->data(index).toString();
        return itemText.contains(filterRegularExpression());  // 包含匹配
    }
};

#endif
