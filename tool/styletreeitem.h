#ifndef STYLETREEITEM_H
#define STYLETREEITEM_H

#include <QString>
#include <QList>

class StyleTreeItem
{
public:
    StyleTreeItem(int id, QString name, int row, StyleTreeItem *parent);
    ~StyleTreeItem();

    int id() const;
    QString name() const;

    int row() const;
    StyleTreeItem *parent() const;

    int childCount() const;
    StyleTreeItem *childAtIndex(int index) const;
    void addChildItem(StyleTreeItem *item);
    void clearChildItems();

private:
    int m_id;
    QString m_name;

    int m_row;
    StyleTreeItem *m_parent;
    QList<StyleTreeItem *> m_childItems;
};

#endif
