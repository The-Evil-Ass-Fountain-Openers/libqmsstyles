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
        NameRole = Qt::DisplayRole,
        FormatHelpRole = Qt::ToolTipRole,
    };
    Q_ENUM(ItemRoles)

    Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;
    Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setState(VisualStyle::State *state);

private:
    VisualStyle::State *m_state = nullptr;
};

#endif
