#include "state.h"

#include "property.h"

namespace VisualStyle
{

State::State(int id, QString name)
    : QObject(nullptr)
    , m_properties(std::make_unique<PropertiesHandler>(this))
    , m_id(id)
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

PropertiesHandler *State::properties()
{
    return m_properties.get();
}

void State::addProperty(Property *property)
{
    m_properties->add(property);
}

}
