#ifndef STYLE_H
#define STYLE_H

#include "definitions.h"

#include <QObject>
#include <QDir>
#include <QUrl>
#include <QHash>

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
Q_NAMESPACE

class Class;
class Part;
class State;
class Property;

class Style : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool invalid READ invalid NOTIFY invalidChanged)

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(Version version READ version NOTIFY versionChanged)

    Q_PROPERTY(QList<Class *> classes READ classes NOTIFY loaded)

public:
    enum Version {
        WindowsVista = 0,
        Windows7,
        Windows8,
        Windows10,
        Windows11
    };
    Q_ENUM(Version)

    explicit Style(const QString &name, const QString &path, QObject *parent = nullptr);
    ~Style();

    bool invalid();

    QString name();
    QString path();
    Version version();

    QList<Class *> &classes();

    int classNameToIdx(QString name);

    Class *getClass(int classID);
    Part *getPart(int classID, int partID);
    State *getState(int classID, int partID, int stateID);
    Property *getProperty(int classID, int partID, int stateID, IDENTIFIER nameID);

    bool load();

Q_SIGNALS:
    void loaded();

    void invalidChanged();

    void nameChanged(QString name);
    void pathChanged(QString path);
    void versionChanged(VisualStyle::Style::Version version);

private:
    void loadCMAP();
    void loadBCMAP();
    void structurize();
    void readPropertyHeaders();
    void interpretPropData(QByteArray data, quint32 unknown1, Property *property);

    Version getVersion();

    bool m_invalid{false};

    QString m_name;
    QString m_path;
    Version m_version = Version::Windows7;

    QList<Class *> m_classes;

    // pointer needed because of forward declaration
    wres::WinLibrary *m_resourceTree;
};

}

#endif
