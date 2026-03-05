#include "qmsstyles.h"
#include "logging.h"

#include <QApplication>

#include <QDir>
#include <QProcess>
#include <QFileInfo>


static QHash<QString, QSharedPointer<VisualStyle::Style>> s_loadedStyles;


Qmsstyles *Qmsstyles::self()
{
    static Qmsstyles *s_instance = new Qmsstyles;
    return s_instance;
}


QSharedPointer<VisualStyle::Style> Qmsstyles::load(const QString &path)
{
    QSharedPointer<VisualStyle::Style> style = get(path);

    // this style object is already loaded
    if(style) return style;

    // empty (why)
    if(path.isEmpty()) return nullptr;

    // does not exist
    QFileInfo info(path);
    if(!info.exists()) return nullptr;

    // otherwise, attempt to load it
    style.reset(new VisualStyle::Style(info.baseName(), path, this));

    if(!style->invalid()) {
        style->load();
        s_loadedStyles[path] = style;
        Q_EMIT styleLoaded(style);
        Q_EMIT styleListUpdated();

        return style;
    } else style.clear();

    // fail
    return style;
}

void Qmsstyles::unload(const QString &path)
{
    if(s_loadedStyles.contains(path))
        unload(s_loadedStyles.value(path));
}

void Qmsstyles::unload(QSharedPointer<VisualStyle::Style> style)
{
    s_loadedStyles.values().removeAt(s_loadedStyles.values().indexOf(style));
    Q_EMIT styleListUpdated();

    Q_EMIT styleUnloaded(style);
    style.clear();
}


QSharedPointer<VisualStyle::Style> Qmsstyles::get(const QString &path)
{
    if(s_loadedStyles.contains(path)) {
        return s_loadedStyles.value(path);
    } else {
        return nullptr;
    }
}

QList<QSharedPointer<VisualStyle::Style>> Qmsstyles::styles()
{ return s_loadedStyles.values(); }


Qmsstyles::Qmsstyles()
    : QObject{nullptr}
{  }

Qmsstyles::~Qmsstyles()
{  }

#include "moc_qmsstyles.cpp"
