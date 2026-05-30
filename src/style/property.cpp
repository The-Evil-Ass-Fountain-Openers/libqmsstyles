#include "property.h"

namespace VisualStyle
{

Property::Property(IDENTIFIER name, IDENTIFIER type)
    : QObject(nullptr)
    , QVariant()
    , m_name(name)
    , m_type(type)
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

QPixmap Property::imageFile() const
{
    return m_imageFile;
}

void Property::setImageFile(QPixmap imageFile)
{
    m_imageFile = imageFile;
}

}
