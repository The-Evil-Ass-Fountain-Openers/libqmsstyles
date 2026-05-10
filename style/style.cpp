#include "style.h"

#include "visualpartsmap.h"

#include <QFileInfo>
#include <QByteArrayView>
#include <QApplication>
#include <QRegularExpression>

#include <QtEndian>
#include <QtLogging>

#include <wres/winlibrary.h>

namespace VisualStyle
{

Style::Style(const QString &name, const QString &path, QObject *parent)
    : QObject{parent}
    , m_name(name)
    , m_path(path)
    , m_resourceTree(new wres::WinLibrary(path.toStdString()))
{
    if (!(m_resourceTree->isLoaded() && m_resourceTree->isValid() && m_resourceTree->isPEBinary())) {
        qFatal() << "failed to parse msstyles";
        m_invalid = true;
        Q_EMIT invalidChanged();
        return;
    }
}

Style::~Style()
{
    delete m_resourceTree;
}

bool Style::invalid()
{
    return m_invalid;
}

QString Style::name()
{
    return m_name;
}

QString Style::path()
{
    return m_path;
}

Style::Version Style::version()
{
    return m_version;
}

QList<Class> Style::classes()
{
    return m_classes;
}

bool Style::load()
{    
    if (m_invalid) {
        qWarning() << "invalid msstyles";
        return false;
    }

    loadCMAP();

    m_version = getVersion();

    loadBCMAP();
    structurize();

    return true;
}

void Style::loadCMAP()
{
    wres::WinResource res = m_resourceTree->findResource("CMAP", "CMAP", "")->children().at(0);
    QByteArray data = QByteArray(res.offset(), res.size());

    int lastPos = 0;
    for(int i = 0; i < data.length(); i += 2) {
        if (data[i] == 0 && data[i + 1] == 0) {
            // to avoid grabbing null bytes and trying to make a name out of that
            if (i - lastPos > 2) {
                QString cname = QString::fromLocal8Bit(data.sliced(lastPos, i - lastPos)).replace('\x00', "");
                m_classes.append({(int)m_classes.length(), cname});
            }
            lastPos = i + 2;
        }
    }
}

void Style::loadBCMAP()
{
    wres::WinResource res = m_resourceTree->findResource("BCMAP", "BCMAP", "")->children().at(0);
    QByteArray data = QByteArray(res.offset(), res.size());

    quint32 supposedCount = qFromLittleEndian<quint32>(data.sliced(0, 4).constData());
    int count = qMin((int)supposedCount, (data.length() - 4) / 4);
    QList<int> parents;

    for (int i = 4; i < data.length(); i += 4) {
        // TODO: is it really just an array of uint8 padded to 4 bytes?
        int index = (quint8)data.at(i);
        if (index >= count) {
            index = -1;
        }
        parents.append((int)index);
    }

    for (int i = 0; i < count; ++i) {
        Class styleClass = m_classes.at(i + 4);
        if (i < parents.length()) {
            int parent = parents.at(i);
            if (parent > 0) {
                styleClass.setBaseClass(&m_classes.at(parent + 4));
            }
        }
    }
}

void Style::structurize()
{
    for (Class &styleClass : m_classes) {
        auto visualParts = VisualPartsMap::find(styleClass.name(), m_version);

        for (VisualPart &visualPart : visualParts) {
            Part part(visualPart.id, visualPart.name);

            for (VisualState &visualState : visualPart.states) {
                State state(visualState.value, visualState.name);
                part.addState(state);
            }

            styleClass.addPart(part);
        }
    }
}

Style::Version Style::getVersion()
{
    bool foundDWMTouch = false;
    bool foundDWMPen = false;
    bool foundW8Taskband = false;
    bool foundVistaQueryBuilder = false;
    bool foundTaskBand2Light_Taskband2 = false;

    for (Class &styleClass : m_classes) {
        if (styleClass.name() == "DWMTouch") {
            foundDWMTouch = true;

        } else if (styleClass.name() == "DWMPen") {
            foundDWMPen = true;

        } else if (styleClass.name() == "W8::TaskbandExtendedUI") {
            foundW8Taskband = true;

        } else if (styleClass.name() == "QueryBuilder") {
            foundVistaQueryBuilder = true;

        } else if (styleClass.name() == "DarkMode::TaskManager") {
            foundTaskBand2Light_Taskband2 = true;

        }
    }

    if (foundTaskBand2Light_Taskband2) {
        return Version::Windows11;
    } else if (foundW8Taskband) {
        return Version::Windows8;
    } else if (foundDWMTouch || foundDWMPen) {
        return Version::Windows10;
    } else if (foundVistaQueryBuilder) {
        return Version::WindowsVista;
    } else {
        return Version::Windows7;
    }
}

}

#include "moc_style.cpp"
