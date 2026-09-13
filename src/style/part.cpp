#include "part.h"

#include "state.h"
#include "class.h"

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

bool Part::hasState(int stateID) const
{
    return m_states.contains(stateID);
}

State *Part::getState(int stateID, bool fallback) const
{
    if (!hasState(stateID) && fallback && m_parentClass && m_parentClass->baseClass()) {
        Part *otherPart = m_parentClass->baseClass()->getPart(m_id);
        Q_ASSERT(otherPart != this);
        if (otherPart) {
            return otherPart->getState(stateID);
        }
    } else if (!hasState(stateID)) {
        return nullptr;
    }

    return m_states.value(stateID);
}

QList<State *> Part::states()
{
    return m_states.values();
}

void Part::addState(State *state)
{
    state->setParent(this);
    m_states[state->id()] = state;
}

Class *Part::parentClass() const
{
    return m_parentClass;
}

void Part::setParentClass(Class *parentClass)
{
    m_parentClass = parentClass;
    setParent(m_parentClass);
}

}
