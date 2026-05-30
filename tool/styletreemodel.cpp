#include "styletreemodel.h"

#include "../style/class.h"
#include "../style/part.h"
#include "../style/state.h"

StyleTreeModel::StyleTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_rootItem(std::make_unique<StyleTreeItem>(-1, "", 0, nullptr))
{
}

StyleTreeModel::~StyleTreeModel() = default;

QVariant StyleTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    StyleTreeItem *item = static_cast<StyleTreeItem *>(index.internalPointer());

    switch ((ItemRoles)role) {
    case IdRole:
        return item->id();
    case NameRole:
        return item->name();
    }

    return {};
}

QHash<int, QByteArray> StyleTreeModel::roleNames() const
{
    QHash<int, QByteArray> names;

    names[IdRole] = "id";
    names[NameRole] = "display";

    return names;
}

QModelIndex StyleTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    StyleTreeItem *item;
    if (!parent.isValid()) {
        item = m_rootItem.get();
    } else {
        item = static_cast<StyleTreeItem *>(parent.internalPointer());
    }

    if (StyleTreeItem *child = item->childAtIndex(row)) {
        return createIndex(row, column, child);
    }

    return {};
}

QModelIndex StyleTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }

    StyleTreeItem *item = static_cast<StyleTreeItem *>(index.internalPointer())->parent();
    QModelIndex returnedIndex{};

    if (item != m_rootItem.get()) {
        returnedIndex = createIndex(item->row(), 0, item);
    }

    return returnedIndex;
}

int StyleTreeModel::rowCount(const QModelIndex &parent) const
{
    int count;

    if (parent.isValid()) {
        count = static_cast<StyleTreeItem *>(parent.internalPointer())->childCount();
    } else {
        count = m_rootItem->childCount();
    }

    return count;
}

int StyleTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

int StyleTreeModel::count() const
{
    return rowCount();
}

void StyleTreeModel::setClassArray(QList<VisualStyle::Class *> &classArray)
{
    beginRemoveRows({}, 0, m_rootItem->childCount() - 1);
    m_rootItem->clearChildItems();
    endRemoveRows();

    m_classArray = classArray;
    if (!m_classArray.isEmpty()) {
        instantiateItems();
    }
}

void StyleTreeModel::instantiateItems()
{
    beginInsertRows({}, 0, m_classArray.length() - 1);
    for (VisualStyle::Class *cls : m_classArray) {
        StyleTreeItem *classItem = new StyleTreeItem(cls->id(), cls->name(), m_rootItem->childCount(), m_rootItem.get());
        m_rootItem->addChildItem(classItem);
    }
    endInsertRows();

    for (int i = 0; i < m_classArray.length(); i++) {
        VisualStyle::Class *cls = m_classArray.at(i);
        StyleTreeItem *classItem = m_rootItem->childAtIndex(i);
        QModelIndex classRoot = index(i, 0, {});

        beginInsertRows(classRoot, 0, cls->parts().length() - 1);
        for (VisualStyle::Part *part : cls->parts()) {
            StyleTreeItem *partItem = new StyleTreeItem(part->id(), part->name(), classItem->childCount(), classItem);
            classItem->addChildItem(partItem);
        }
        endInsertRows();
    }

    for (int i = 0; i < m_classArray.length(); i++) {
        VisualStyle::Class *cls = m_classArray.at(i);
        StyleTreeItem *classItem = m_rootItem->childAtIndex(i);
        QModelIndex classRoot = index(i, 0, {});

        for (int i = 0; i < cls->parts().length(); i++) {
            VisualStyle::Part *part = cls->parts().at(i);
            StyleTreeItem *partItem = classItem->childAtIndex(i);
            QModelIndex partRoot = index(i, 0, classRoot);

            beginInsertRows(partRoot, 0, part->states().length() - 1);
            for (VisualStyle::State *state : part->states()) {
                StyleTreeItem *stateItem = new StyleTreeItem(state->id(), state->name(), partItem->childCount(), partItem);
                partItem->addChildItem(stateItem);
            }
            endInsertRows();
        }
    }
}
