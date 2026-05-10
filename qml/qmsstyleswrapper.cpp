/*
 *  SPDX-FileCopyrightText: 2026 catpswin56 <catpswin5@proton.me>
 *
 *  SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "qmsstyleswrapper.h"

#include "../qmsstyles.h"

#include <QJSEngine>
#include <qqml.h>

QmsstylesWrapper *QmsstylesWrapper::create(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(jsEngine)
    static QmsstylesWrapper *s_instance = new QmsstylesWrapper;
    return s_instance;
}

Q_INVOKABLE VisualStyle::Style *QmsstylesWrapper::load(const QUrl &url)
{
    return load(url.toLocalFile());
}

Q_INVOKABLE VisualStyle::Style *QmsstylesWrapper::load(const QString &path)
{
    return Qmsstyles::self()->load(path).data();
}

Q_INVOKABLE void QmsstylesWrapper::unload(const QString &path)
{
    Qmsstyles::self()->unload(path);
}

Q_INVOKABLE void QmsstylesWrapper::unload(VisualStyle::Style *style)
{
    Qmsstyles::self()->unload(style->path());
}

QList<VisualStyle::Style *> QmsstylesWrapper::styles()
{
    QList<VisualStyle::Style *> list;
    for (QSharedPointer<VisualStyle::Style> style : Qmsstyles::self()->styles()) {
        list.append(style.data());
    }
    return list;
}

QmsstylesWrapper::QmsstylesWrapper()
    : QObject(nullptr)
{
    connect(Qmsstyles::self(), &Qmsstyles::styleLoaded, this, [&](QSharedPointer<VisualStyle::Style> style) {
        Q_EMIT styleLoaded(style.data());
    });

    connect(Qmsstyles::self(), &Qmsstyles::styleUnloaded, this, [&](QSharedPointer<VisualStyle::Style> style) {
        Q_EMIT styleUnloaded(style.data());
    });

    connect(Qmsstyles::self(), &Qmsstyles::styleListUpdated, this, [&] {
        Q_EMIT styleListUpdated();
    });
}

#include "qmsstyleswrapper.moc"
