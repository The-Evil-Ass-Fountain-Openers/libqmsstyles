#ifndef STYLE_H
#define STYLE_H

#include <QObject>
#include <QDir>
#include <QUrl>
#include <QHash>

#include "class.h"

// for some reason including wres library directly causes for the
// C++ compiler to think QObject is part of wres, and causes a ton
// more issues depending if it's included before or after QObject.
// I suck at C++ so there might be a way better fix, who knows.
namespace wres
{
class WinLibrary;
}

namespace VisualStyle
{

class Style : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool invalid READ invalid NOTIFY invalidChanged)

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
    Q_PROPERTY(Version version READ version NOTIFY versionChanged)

    Q_PROPERTY(QList<Class> classes READ classes NOTIFY classAdded)

public:
    enum Version
    {
        WindowsXP,
        WindowsVista,
        Windows7,
        Windows8,
        Windows10,
        Windows11
    };

    explicit Style(const QString &name, const QString &path);

    bool invalid() { return m_invalid; }

    QString name() { return m_name; }
    QUrl path() { return m_path; }
    Version version() { return m_version; }

    QList<Class> classes() { return m_classes; }

    const Class *findClass(const QString &name) const;

    bool load();

Q_SIGNALS:
    void loaded();

    void invalidChanged();

    void nameChanged(QString name);
    void pathChanged(QUrl path);
    void versionChanged(VisualStyle::Style::Version version);

    void classAdded(Class *addedClass);

private:
    bool m_invalid = false;

    QString m_name;
    QUrl m_path;
    Version m_version = Version::Windows7;

    QList<Class> m_classes;

    // pointer needed because of forward declaration
    wres::WinLibrary *m_resourceTree;

    QByteArray removeNull(const QByteArray &bytes, const int &start, const int &end);
    Version getVersion();
};

}

#endif // STYLE_H
