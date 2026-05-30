#ifndef PROPERTIESMODEL_H
#define PROPERTIESMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QHash>
#include <QByteArray>
#include <QVariant>

namespace VisualStyle
{
class State;
}

class PropertiesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PropertiesModel(QObject *parent = nullptr);
    ~PropertiesModel();

    enum ItemRoles {
        NameRole,
        ValueRole
    };
    Q_ENUM(ItemRoles)

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    void setState(VisualStyle::State *state);

private:
    VisualStyle::State *m_state = nullptr;
};

#endif
