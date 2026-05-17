#ifndef PART_H
#define PART_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace VisualStyle
{

class State;

class Part : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QList<State *> states READ states)

public:
    explicit Part(int id, QString name);

    QString name() const;
    int id() const;

    QList<State *> &states();
    void addState(State *state);

private:
    QString m_name;
    int m_id;
    QList<State *> m_states;
};

}

#endif
