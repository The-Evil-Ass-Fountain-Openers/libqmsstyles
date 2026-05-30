#include "class.h"

#include "part.h"

namespace VisualStyle
{

Class::Class(int id, QString name)
    : QObject(nullptr)
    , m_id(id)
    , m_name(name)
    , m_baseClass(nullptr)
{
}

QString Class::name() const
{
    return m_name;
}

int Class::id() const
{
    return m_id;
}

const Class *Class::baseClass() const
{
    return m_baseClass;
}

void Class::setBaseClass(const Class *baseClass)
{
    m_baseClass = baseClass;
}

QList<Part *> &Class::parts()
{
    return m_parts;
}

void Class::addPart(Part *part)
{
    part->setParent(this);
    m_parts.append(part);
}

}
