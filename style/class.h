#ifndef CLASS_H
#define CLASS_H

#include "property.h"

#include <QObject>

#include <algorithm>

namespace VisualStyle
{

struct State
{
    int id;
    QString name;
    QList<Property> properties;

    const Property *getProperty(const QString &name) const {
        qDebug() << "libqmsstyles: starting property search";
        auto it = std::find_if(properties.constBegin(), properties.constEnd(), [&](const Property property){
            qDebug() << "libqmsstyles: looking for property " + name + " in " + this->name + "; current property: " + property.name;
            return property.name == name;
        });

        if(it != properties.constEnd()) return &(*it);
        else return nullptr;
    }
};

struct Part
{
    int id;
    QString name;
    QList<State> states;

    const State *getState(const QString &name) const {
        auto it = std::find_if(states.constBegin(), states.constEnd(), [&](const State state){
            return state.name == name;
        });

        if(it != states.constEnd()) return &(*it);
        else return nullptr;
    }
};

class Class
{
public:
    Class(int id, QString name);

    QString className;
    int classID;

    QList<Part> parts;

    const Part *findPart(const QString &name) const;
};

}

#endif // CLASS_H
