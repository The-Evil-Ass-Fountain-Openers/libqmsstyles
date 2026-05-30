#ifndef STATE_H
#define STATE_H

#include <QObject>
#include <QSharedPointer>

namespace VisualStyle
{

class Property;
class PropertiesHandler;

class State : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QSharedPointer<PropertiesHandler> properties READ properties)

public:
    explicit State(int id, QString name);

    QString name() const;
    int id() const;

    QSharedPointer<PropertiesHandler> properties();
    void addProperty(Property *property);

private:
    QString m_name;
    int m_id;
    QSharedPointer<PropertiesHandler> m_properties;
};

}

#endif
