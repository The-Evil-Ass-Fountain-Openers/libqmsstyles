#ifndef STYLETREEMODEL_H
#define STYLETREEMODEL_H

#include "styletreeitem.h"

#include <QAbstractItemModel>
#include <QObject>
#include <QHash>
#include <QByteArray>

namespace VisualStyle
{
class Class;
}

class StyleTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY classArrayChanged)

public:
    explicit StyleTreeModel(QObject *parent = nullptr);
    ~StyleTreeModel();

    enum ItemRoles {
        IdRole,
        NameRole
    };
    Q_ENUM(ItemRoles)

    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    int count() const; // QML

    void setClassArray(QList<VisualStyle::Class *> &classArray);

Q_SIGNALS:
    void classArrayChanged();

private:
    void instantiateItems();

    QList<VisualStyle::Class *> m_classArray;
    std::unique_ptr<StyleTreeItem> m_rootItem;
};

#endif
