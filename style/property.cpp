#include "property.h"

namespace VisualStyle
{

Property::Property(IDENTIFIER name, IDENTIFIER type, QPixmap imageFile)
    : QObject(nullptr)
    , QVariant()
    , m_name(name)
    , m_type(type)
    , m_imageFile(imageFile)
{
}

IDENTIFIER Property::name() const
{
    return m_name;
}

IDENTIFIER Property::type() const
{
    return m_type;
}

}
