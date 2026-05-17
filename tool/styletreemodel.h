#ifndef STYLETREEMODEL_H
#define STYLETREEMODEL_H

#include "styletreeitem.h"

#include <QAbstractItemModel>
#include <QtQml/qqmlregistration.h>
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
    QML_ELEMENT

    Q_PROPERTY(QList<VisualStyle::Class *> classArray READ classArray WRITE setClassArray NOTIFY classArrayChanged)

public:
    StyleTreeModel(QObject *parent = nullptr);
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

    QList<VisualStyle::Class *> &classArray();
    void setClassArray(QList<VisualStyle::Class *> &classArray);

Q_SIGNALS:
    void classArrayChanged();

private:
    void instantiateItems();

    QList<VisualStyle::Class *> m_classArray;
    std::unique_ptr<StyleTreeItem> m_rootItem;
};

#endif
