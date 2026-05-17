#ifndef PROPERTY_H
#define PROPERTY_H

#include "definitions.h"

#include <QObject>
#include <QVariant>
#include <QPixmap>
#include <QtQmlIntegration/qqmlintegration.h>

namespace VisualStyle
{

class Property : public QObject, public QVariant
{
    Q_OBJECT

public:
    explicit Property(IDENTIFIER name, IDENTIFIER type, QPixmap imageFile = QPixmap());

    IDENTIFIER name() const;
    IDENTIFIER type() const;

private:
    IDENTIFIER m_name;
    IDENTIFIER m_type;
    QPixmap m_imageFile;
};

class PropertiesHandler : public QObject
{
    Q_OBJECT

public:
    explicit PropertiesHandler(QObject *parent = nullptr)
        : QObject(parent)
        , m_fallback(nullptr)
    {
    }

    Property *get(IDENTIFIER nameID)
    {
        auto it = std::find_if(m_properties.begin(), m_properties.end(), [&](Property *property) {
            return property->name() == nameID;
        });

        if (it != m_properties.end()) {
            return *it;
        } else if (m_fallback) {
            return m_fallback->get(nameID);
        } else {
            return nullptr;
        }
    }

    void add(Property *property)
    {
        property->setParent(this);
        m_properties.append(property);
    }

    void setFallback(PropertiesHandler *fallback)
    {
        m_fallback = fallback;
    }

private:
    PropertiesHandler *m_fallback;
    QList<Property *> m_properties;
};

}

#endif
