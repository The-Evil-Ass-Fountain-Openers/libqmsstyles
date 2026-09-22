#include "property.h"

namespace VisualStyle
{

// Property
Property::Property(IDENTIFIER name, IDENTIFIER type, int resourceId)
    : QObject(nullptr)
    , QVariant()
    , m_name(name)
    , m_type(type)
    , m_resourceId(resourceId)
{
}

IDENTIFIER Property::name() const
{
    return m_name;
}

void Property::setName(IDENTIFIER name)
{
    m_name = name;
}

IDENTIFIER Property::type() const
{
    return m_type;
}

void Property::setType(IDENTIFIER type)
{
    m_type = type;
}

QPixmap Property::imageFile() const
{
    return m_imageFile;
}

void Property::setImageFile(QPixmap imageFile)
{
    m_imageFile = imageFile;
}

int Property::resourceId() const
{
    return m_resourceId;
}

// PropertiesHandler
PropertiesHandler::PropertiesHandler(QObject *parent)
    : QObject(parent)
    , m_fallback(nullptr)
{
}

QList<Property *> PropertiesHandler::internalList() const
{
    return m_properties;
}

bool PropertiesHandler::contains(IDENTIFIER nameID, bool fallback )
{
    auto *prop = get(nameID, fallback);
    return !!prop;
}

Property *PropertiesHandler::get(IDENTIFIER nameID, bool fallback)
{
    auto it = std::find_if(m_properties.begin(), m_properties.end(), [&](Property *property) {
        return property->name() == nameID;
    });

    if (it != m_properties.end()) {
        return *it;
    } else if (m_fallback && fallback) {
        return m_fallback->get(nameID);
    } else {
        return nullptr;
    }
}

void PropertiesHandler::add(Property *property)
{
    property->setParent(this);
    m_properties.append(property);
}

void PropertiesHandler::setFallback(QSharedPointer<PropertiesHandler> fallback)
{
    Q_ASSERT(fallback.data() != this);
    m_fallback = fallback;
}

}
