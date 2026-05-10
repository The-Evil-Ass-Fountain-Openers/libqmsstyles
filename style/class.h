#ifndef CLASS_H
#define CLASS_H

// #include "property.h"

#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace VisualStyle
{

class Property {};

class State
{
    Q_GADGET
    QML_VALUE_TYPE(state)
    QML_UNCREATABLE("")

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QList<Property> properties READ properties)

public:
    State() = default;
    State(int id, QString name);

    QString name() const;

    int id() const;

    QList<Property> properties() const;
    void addProperty(Property property);

private:
    QString m_name;
    int m_id;
    QList<Property> m_properties;
};

class Part
{
    Q_GADGET
    QML_VALUE_TYPE(part)
    QML_UNCREATABLE("")

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QList<State> states READ states)

public:
    Part() = default;
    Part(int id, QString name);

    QString name() const;

    int id() const;

    QList<State> states() const;
    void addState(State state);

private:
    QString m_name;
    int m_id;
    QList<State> m_states;
};

class Class
{
    Q_GADGET
    QML_VALUE_TYPE(class)
    QML_UNCREATABLE("")

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(const Class *baseClass READ baseClass)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QList<Part> parts READ parts)

public:
    Class() = default; // for qml compatibility
    Class(int id, QString name);

    QString name() const;

    const Class *baseClass() const;
    void setBaseClass(const Class *baseClass);

    int id() const;

    QList<Part> parts() const;
    void addPart(Part part);

private:
    QString m_name;
    const Class *m_baseClass;
    int m_id;
    QList<Part> m_parts;
};

}

#endif // CLASS_H
