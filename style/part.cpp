#include "part.h"

#include "state.h"

namespace VisualStyle
{

Part::Part(int id, QString name)
    : QObject(nullptr)
    , m_id(id)
    , m_name(name)
{
}

QString Part::name() const
{
    return m_name;
}

int Part::id() const
{
    return m_id;
}

QList<State *> &Part::states()
{
    return m_states;
}

void Part::addState(State *state)
{
    state->setParent(this);
    m_states.append(state);
}

}
