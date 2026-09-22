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
    switch ((ItemRoles)role)
    {
        case ItemRoles::NameRole: {
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
            break;
        }

        case ItemRoles::FormatHelpRole: {
            if (property->canConvert<QMargins>()) {
                return "Convention: LEFT, RIGHT, TOP, BOTTOM";
            } else if (property->canConvert<QPoint>()) {
                return "Convention: X, Y";
            } else if (property->canConvert<QRect>()) {
                return "Convention: (X, Y) (WIDTHxHEIGHT)";
            } else {
                return "ignore";
            }
            break;
        }

        default:
            break;
    }

    return {};
}

bool PropertiesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    VisualStyle::Property *property = m_state->properties()->internalList().at(index.row());
    if (role == ItemRoles::NameRole) {
        switch (index.column())
        {
            // TODO: need to make something that maps every name to type
            case 0:
                return false;
            case 1: {
                QString inputStr = value.value<QString>();
                if (property->canConvert<QString>()) {
                    property->setValue(inputStr);
                } else if (property->canConvert<QMargins>()) {
                    QStringList split = inputStr.replace(" ", "").split(",");
                    if (split.isEmpty() || split.length() != 4) {
                        return false;
                    }

                    QMargins margins;
                    margins.setLeft(split[0].toInt());
                    margins.setRight(split[1].toInt());
                    margins.setTop(split[2].toInt());
                    margins.setBottom(split[3].toInt());

                    property->setValue(margins);
                } else if (property->canConvert<QPoint>()) {
                    QStringList split = inputStr.replace(" ", "").split(",");
                    if (split.isEmpty() || split.length() != 2) {
                        return false;
                    }

                    QPoint point;
                    point.setX(split[0].toInt());
                    point.setY(split[1].toInt());

                    property->setValue(point);
                } else if (property->canConvert<QRect>()) {
                    QStringList split = inputStr.replace("(", "").replace(") ", ",").replace(")", "").replace("x", "").split(",");
                    if (split.isEmpty() || split.length() != 4) {
                        return false;
                    }

                    QRect rect;
                    rect.moveLeft(split[0].toInt());
                    rect.moveTop(split[1].toInt());
                    rect.setWidth(split[2].toInt());
                    rect.setHeight(split[3].toInt());

                    property->setValue(rect);
                }
            }
        }
    }


    Q_EMIT dataChanged(index, index);

    return true;
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

Qt::ItemFlags PropertiesModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QHash<int, QByteArray> PropertiesModel::roleNames() const
{
    return QHash<int, QByteArray>{
        { ItemRoles::NameRole, "name" },
        { ItemRoles::FormatHelpRole, "formatHelp" }
    };
}

void PropertiesModel::setState(VisualStyle::State *state)
{
    if (state) {
        beginResetModel();
        m_state = state;
        endResetModel();
    }
}
