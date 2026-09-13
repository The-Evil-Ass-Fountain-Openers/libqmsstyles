#ifndef QMSSTYLESPART_H
#define QMSSTYLESPART_H

#include <QObject>

#include <QMap>

namespace VisualStyle
{

class State;
class Class;

class Part : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QList<State *> states READ states)

public:
    Part(int id, QString name);

    QString name() const;
    int id() const;

    bool hasState(int stateID) const;
    State *getState(int stateID, bool fallback = true) const;
    // this generates a new list and is slower
    QList<State *> states();
    void addState(State *state);

    Class *parentClass() const;
    void setParentClass(Class *parentClass);

private:
    QString m_name;
    int m_id;
    QMap<int, State *> m_states;
    Class *m_parentClass;
};

}

#endif
