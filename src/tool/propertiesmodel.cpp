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
        case 1: {
            if (property->canConvert<QString>()) {
                QString string = property->value<QString>();
                return string;
            } else if (property->canConvert<QMargins>()) {
                QMargins margins = property->value<QMargins>();
                return QString("%1, %2, %3, %4").arg(QString::number(margins.left()), QString::number(margins.right()),
                                                     QString::number(margins.top()), QString::number(margins.bottom()));
            } else if (property->canConvert<QPoint>()) {
                QPoint position = property->value<QPoint>();
                return QString("%1, %2").arg(QString::number(position.x()), QString::number(position.y()));
            } else if (property->canConvert<QRect>()) {
                QRect rect = property->value<QRect>();
                return QString("(%1, %2) (%3x%4)").arg(QString::number(rect.x()), QString::number(rect.y()),
                                                       QString::number(rect.width()), QString::number(rect.height()));
            }
        }

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
