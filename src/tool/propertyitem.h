#ifndef STYLETREEITEM_H
#define STYLETREEITEM_H

#include <QString>
#include <QList>

#include "../style/definitions.h"

namespace VisualStyle
{
class Property;
}

class PropertyItem
{
public:
    PropertyItem(VisualStyle::Property *property);
    ~PropertyItem();

    IDENTIFIER type() const;
    QString name() const;
    QString stateName() const;

private:
    QString m_name;
};

#endif
