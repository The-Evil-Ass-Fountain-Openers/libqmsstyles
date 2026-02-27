#ifndef QMSSTYLES_H
#define QMSSTYLES_H

#include <QObject>
#include <QProcess>

#include "qmsstyles_global.h"

#include "style/style.h"

class QMSSTYLES_EXPORT Qmsstyles : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<VisualStyle::Style *> styles READ styles NOTIFY styleListUpdated)

public:
    static Qmsstyles *self();

    VisualStyle::Style *load(const QString &path);
    void unload(const QString &path);
    void unload(VisualStyle::Style *style);

    VisualStyle::Style *get(const QString &name);
    QList<VisualStyle::Style *> styles();

Q_SIGNALS:
    void styleLoaded(VisualStyle::Style *style);
    void styleUnloaded(VisualStyle::Style *style);

    void styleListUpdated();

private:
    explicit Qmsstyles();
    ~Qmsstyles();
};

#endif // QMSSTYLES_H
