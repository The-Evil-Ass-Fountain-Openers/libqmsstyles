#include "state.h"

#include "property.h"

namespace VisualStyle
{

State::State(int id, QString name)
    : QObject(nullptr)
    , m_properties(new PropertiesHandler(this))
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

QSharedPointer<PropertiesHandler> State::properties()
{
    return m_properties;
}

void State::addProperty(Property *property)
{
    m_properties->add(property);
}

}
