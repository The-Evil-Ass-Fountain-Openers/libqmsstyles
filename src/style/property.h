#ifndef QMSSTYLESPROPERTY_H
#define QMSSTYLESPROPERTY_H

#include "definitions.h"

#include <QObject>
#include <QVariant>
#include <QPixmap>
#include <QSharedPointer>

namespace VisualStyle
{

class Property : public QObject, public QVariant
{
    Q_OBJECT
public:
    Property(IDENTIFIER name, IDENTIFIER type, int resourceId);

    IDENTIFIER name() const;
    void setName(IDENTIFIER name);

    IDENTIFIER type() const;
    void setType(IDENTIFIER type);

    QPixmap imageFile() const;
    void setImageFile(QPixmap imageFile);

    int resourceId() const;

private:
    IDENTIFIER m_name;
    IDENTIFIER m_type;
    QPixmap m_imageFile;
    int m_resourceId;
};

class PropertiesHandler : public QObject
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<PropertiesHandler>;

    PropertiesHandler(QObject *parent = nullptr);

    QList<Property *> internalList() const;
    bool contains(IDENTIFIER nameID, bool fallback = true);
    Property *get(IDENTIFIER nameID, bool fallback = true);
    void add(Property *property);
    void setFallback(QSharedPointer<PropertiesHandler> fallback);

private:
    QSharedPointer<PropertiesHandler> m_fallback;
    QList<Property *> m_properties;
};

}

#endif
