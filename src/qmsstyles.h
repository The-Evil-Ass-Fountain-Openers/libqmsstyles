#ifndef QMSSTYLES_H
#define QMSSTYLES_H

#include "qmsstyles_global.h"

#include <QObject>
#include <QSharedPointer>
#include <QQmlEngine>
#include <QUrl>

namespace VisualStyle
{
class Style;
}

class QMSSTYLES_EXPORT Qmsstyles : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QSharedPointer<VisualStyle::Style>> styles READ styles NOTIFY styleListUpdated)

public:
    static Qmsstyles *self();

    Q_INVOKABLE QSharedPointer<VisualStyle::Style> load(const QString &path);
    Q_INVOKABLE void unload(const QString &path);
    Q_INVOKABLE void unload(QSharedPointer<VisualStyle::Style> style);

    QSharedPointer<VisualStyle::Style> get(const QString &name);
    QList<QSharedPointer<VisualStyle::Style>> styles();

Q_SIGNALS:
    void styleLoaded(QSharedPointer<VisualStyle::Style> style);
    void styleUnloaded(QSharedPointer<VisualStyle::Style> style);

    void styleListUpdated();

private:
    explicit Qmsstyles();
    ~Qmsstyles();
};

#endif // QMSSTYLES_H
