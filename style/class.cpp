#include "class.h"

namespace VisualStyle
{

State::State(int id, QString name)
    : m_id(id)
    , m_name(name)
{
}

QString State::name() const
{
    return m_name;
}

int State::id() const
{
    return m_id;
}

QList<Property> State::properties() const
{
    return m_properties;
}

void State::addProperty(Property property)
{
    m_properties.append(property);
}

Part::Part(int id, QString name)
    : m_id(id)
    , m_name(name)
{
}

QString Part::name() const
{
    return m_name;
}

int Part::id() const
{
    return m_id;
}

QList<State> Part::states() const
{
    return m_states;
}

void Part::addState(State state)
{
    m_states.append(state);
}

Class::Class(int id, QString name)
    : m_id(id)
    , m_name(name)
    , m_baseClass(nullptr)
{
}

QString Class::name() const
{
    return m_name;
}

const Class *Class::baseClass() const
{
    return m_baseClass;
}

void Class::setBaseClass(const Class *baseClass)
{
    m_baseClass = baseClass;
}

int Class::id() const
{
    return m_id;
}

QList<Part> Class::parts() const
{
    return m_parts;
}

void Class::addPart(Part part)
{
    m_parts.append(part);
}

}
