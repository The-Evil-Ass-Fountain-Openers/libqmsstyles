#include "styletreeitem.h"
#include <cstdio>

StyleTreeItem::StyleTreeItem(int id, QString name, int row, StyleTreeItem *parent)
    : m_parent(parent)
    , m_row(row)
    , m_name(name)
    , m_id(id)
{
}

StyleTreeItem::~StyleTreeItem()
{
    qDeleteAll(m_childItems);
}

int StyleTreeItem::id() const
{
    return m_id;
}

QString StyleTreeItem::name() const
{
    return m_name;
}

int StyleTreeItem::row() const
{
    return m_row;
}

StyleTreeItem *StyleTreeItem::parent() const
{
    return m_parent;
}

int StyleTreeItem::childCount() const
{
    return m_childItems.length();
}

StyleTreeItem *StyleTreeItem::childAtIndex(int index) const
{
    return m_childItems.at(index);
}

void StyleTreeItem::addChildItem(StyleTreeItem *item)
{
    m_childItems.append(item);
}

void StyleTreeItem::clearChildItems()
{
    qDeleteAll(m_childItems);
    m_childItems.clear();
}
