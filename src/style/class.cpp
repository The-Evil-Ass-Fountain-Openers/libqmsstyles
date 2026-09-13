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

bool Class::hasPart(int partID) const
{
    return m_parts.contains(partID);
}

Part *Class::getPart(int partID, bool fallback) const
{
    if (!hasPart(partID) && fallback && m_baseClass) {
        return m_baseClass->getPart(partID);
    } else if (!hasPart(partID)) {
        return nullptr;
    }

    return m_parts.value(partID);
}

QList<Part *> Class::parts() const
{
    return m_parts.values();
}

void Class::addPart(Part *part)
{
    part->setParentClass(this);
    m_parts[part->id()] = part;
}

}
