/*
 *  SPDX-FileCopyrightText: 2026 catpswin56 <catpswin5@proton.me>
 *
 *  SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../style/style.h"

#include <QJSEngine>
#include <qqml.h>

// Qmsstyles wrapper
// needed to convert the shared pointers into raw pointers for QML
class QmsstylesWrapper : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(Qmsstyles)

    Q_PROPERTY(QList<VisualStyle::Style *> styles READ styles NOTIFY styleListUpdated)

public:
    static QmsstylesWrapper *create(QQmlEngine *engine, QJSEngine *jsEngine);

    Q_INVOKABLE VisualStyle::Style *load(const QUrl &url);
    Q_INVOKABLE VisualStyle::Style *load(const QString &path);

    Q_INVOKABLE void unload(const QString &path);
    Q_INVOKABLE void unload(VisualStyle::Style *style);

    QList<VisualStyle::Style *> styles();

Q_SIGNALS:
    void styleLoaded(VisualStyle::Style *style);
    void styleUnloaded(VisualStyle::Style *style);

    void styleListUpdated();

private:
    QmsstylesWrapper();
};
