#include "qmsstyles.h"
#include "logging.h"

#include <QApplication>

#include <QDir>
#include <QProcess>
#include <QFileInfo>


static QHash<QString, VisualStyle::Style *> s_loadedStyles;
static int s_accessCount = 0;


Qmsstyles *Qmsstyles::self()
{
    static Qmsstyles *s_instance = new Qmsstyles;
    qDebug() << "libqmsstyles access count:" << ++s_accessCount;
    return s_instance;
}


VisualStyle::Style *Qmsstyles::load(const QString &path)
{
    VisualStyle::Style *style = get(path);

    // this style object is already loaded
    if(style) return style;

    // otherwise, attempt to load it
    QStringList splitPath(path.split("/"));
    style = new VisualStyle::Style(splitPath[splitPath.length() - 1], path);

    if(!style->invalid()) {
        style->load();
        s_loadedStyles[path] = style;
        Q_EMIT styleLoaded(style);
        Q_EMIT styleListUpdated();

        return style;
    } else delete style;

    // fail
    return nullptr;
}

void Qmsstyles::unload(const QString &path)
{
    if(s_loadedStyles.contains(path));
    unload(s_loadedStyles.value(path));
}

void Qmsstyles::unload(VisualStyle::Style *style)
{
    s_loadedStyles.values().removeAt(s_loadedStyles.values().indexOf(style));
    Q_EMIT styleListUpdated();

    Q_EMIT styleUnloaded(style);
    delete style;
}


VisualStyle::Style *Qmsstyles::get(const QString &path)
{
    if(s_loadedStyles.contains(path)) return s_loadedStyles.value(path);
    else return nullptr;
}

QList<VisualStyle::Style *> Qmsstyles::styles()
{
    return s_loadedStyles.values();
}


Qmsstyles::Qmsstyles()
    : QObject{nullptr}
{  }

Qmsstyles::~Qmsstyles()
{ qDeleteAll(s_loadedStyles.values()); }

#include "moc_qmsstyles.cpp"
