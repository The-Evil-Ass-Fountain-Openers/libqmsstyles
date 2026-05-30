#include "propertiesmodel.h"

#include "../style/state.h"
#include "../style/property.h"

PropertiesModel::PropertiesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

PropertiesModel::~PropertiesModel()
{
}

QVariant PropertiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    VisualStyle::Property *property = m_state->properties()->internalList().at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column())
        {
            case 0:
                return VisualStyle::NAMES.key((int)property->name());
            case 1:
                return static_cast<QVariant>(*property);
        }
    }

    return {};
}

int PropertiesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (m_state) {
        return m_state->properties()->internalList().length();
    } else {
        return 0;
    }
}

int PropertiesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 2;
}

void PropertiesModel::setState(VisualStyle::State *state)
{
    if (state) {
        beginResetModel();
        m_state = state;
        endResetModel();
    }
}
