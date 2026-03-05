#ifndef QMSSTYLES_H
#define QMSSTYLES_H

#include "qmsstyles_global.h"

#include "style/style.h"

#include <QObject>
#include <QSharedPointer>

class QMSSTYLES_EXPORT Qmsstyles : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QSharedPointer<VisualStyle::Style>> styles READ styles NOTIFY styleListUpdated)

public:
    static Qmsstyles *self();

    QSharedPointer<VisualStyle::Style> load(const QString &path);
    void unload(const QString &path);
    void unload(QSharedPointer<VisualStyle::Style> style);

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
