#include "style.h"

#include "visualpartsmap.h"

#include "class.h"
#include "part.h"
#include "state.h"
#include "property.h"

#include <QFileInfo>
#include <QByteArrayView>
#include <QApplication>
#include <QRegularExpression>

#include <QtEndian>
#include <QtLogging>

#include <wres/winlibrary.h>

namespace VisualStyle
{

static const int s_propheaderSection = 4;
static const int s_propheaderSize = s_propheaderSection * 8;

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

QList<Class *> &Style::classes()
{
    return m_classes;
}

int Style::classNameToIdx(QString name)
{
    auto it = std::find_if(m_classes.begin(), m_classes.end(), [&](Class *cls) {
        return cls->name() == name;
    });

    if (it != m_classes.end()) {
        return (*it)->id();
    } else {
        return -1;
    }
}

Class *Style::getClass(int classID)
{
    if (classID >= m_classes.length()) {
        return nullptr;
    } else {
        return m_classes.at(classID);
    }
}

Part *Style::getPart(int classID, int partID)
{
    Class *cls = getClass(classID);
    if (!cls) {
        return nullptr;
    }

    if (partID >= cls->parts().length()) {
        return nullptr;
    } else {
        return cls->parts().at(partID);
    }
}

State *Style::getState(int classID, int partID, int stateID)
{
    Part *part = getPart(classID, partID);
    if (!part) {
        return nullptr;
    }

    if (stateID >= part->states().length()) {
        return nullptr;
    } else {
        return part->states().at(stateID);
    }
}

Property *Style::getProperty(int classID, int partID, int stateID, IDENTIFIER nameID)
{
    State *state = getState(classID, partID, stateID);
    if (!state) {
        return nullptr;
    }

    return state->properties()->get(nameID);
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
    readPropertyHeaders();

    Q_EMIT loaded();

    return true;
}

void Style::loadCMAP()
{
    wres::WinResource res = m_resourceTree->findResource("CMAP", "CMAP", "")->children().at(0);
    QByteArray data = QByteArray(res.offset(), res.size());

    int lastPos = 0;
    for (int i = 0; i < data.length(); i += 2) {
        if (data[i] == 0 && data[i + 1] == 0) {
            // to avoid grabbing null bytes and trying to make a name out of that
            if (i - lastPos > 2) {
                QString cname = QString::fromLocal8Bit(data.sliced(lastPos, i - lastPos)).replace('\x00', "");
                m_classes.append(new Class((int)m_classes.length(), cname));
            }
            lastPos = i + 2;
        }
    }
}

// TODO: actually use this info
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
        Class *cls = m_classes.at(i + 4);
        if (i < parents.length()) {
            int parent = parents.at(i);
            if (parent > 0) {
                cls->setBaseClass(m_classes.at(parent + 4));
            }
        }
    }
}

void Style::structurize()
{
    for (Class *cls : m_classes) {
        auto visualParts = VisualPartsMap::find(cls->name(), m_version);

        for (VisualPart &visualPart : visualParts) {
            Part *part = new Part(visualPart.id, visualPart.name);

            for (VisualState &visualState : visualPart.states) {
                part->addState(new State(visualState.value, visualState.name));
            }

            cls->addPart(part);
        }
    }
}

void Style::readPropertyHeaders()
{
    // TODO: read from other variants if available too
    wres::WinResource res = m_resourceTree->findResource("VARIANT", "NORMAL", "")->children().at(0);
    QByteArray data = QByteArray(res.offset(), res.size());

    int offset = 0;

    while (offset < data.size()) {
        // see DOCUMENTATION.md for more info
        quint32 nameID   = qFromLittleEndian<quint32>(data.sliced(offset, 4).constData());
        quint32 typeID   = qFromLittleEndian<quint32>(data.sliced(offset + s_propheaderSection, 4).constData());
        quint32 classID  = qFromLittleEndian<quint32>(data.sliced(offset + s_propheaderSection * 2, 4).constData());
        quint32 partID   = qFromLittleEndian<quint32>(data.sliced(offset + s_propheaderSection * 3, 4).constData());
        quint32 stateID  = qFromLittleEndian<quint32>(data.sliced(offset + s_propheaderSection * 4, 4).constData());
        quint32 unknown1 = qFromLittleEndian<quint32>(data.sliced(offset + s_propheaderSection * 5, 4).constData());
        quint32 unknown2 = qFromLittleEndian<quint32>(data.sliced(offset + s_propheaderSection * 6, 4).constData());
        quint32 dataSize = qFromLittleEndian<quint32>(data.sliced(offset + s_propheaderSection * 7, 4).constData());

        int propertySize = s_propheaderSize + dataSize;
        int padding = qCeil((qreal)propertySize / 8.0) * 8 - propertySize;
        int nextOffset = offset + propertySize + padding;

        // check if it's valid
        {
            if (nameID > (quint32)ATLASRECT || nameID < (quint32)DIBDATA) {
                offset = nextOffset;
                continue;
            }

            if (typeID > (quint32)ATLASRECT || typeID < (quint32)DIBDATA) {
                offset = nextOffset;
                continue;
            }
        }

        if (classID >= m_classes.length()) {
            offset = nextOffset;
            continue;
        }
        Class *parentClass = m_classes.at(classID);

        if (partID >= parentClass->parts().length()) {
            offset = nextOffset;
            continue;
        }
        Part *parentPart = parentClass->parts().at(partID);

        if (stateID >= parentPart->states().length()) {
            offset = nextOffset;
            continue;
        }
        State *parentState = parentPart->states().at(stateID);

        IDENTIFIER name = static_cast<IDENTIFIER>(nameID);
        IDENTIFIER type = static_cast<IDENTIFIER>(typeID);

        Property *property = new Property(name, type);
        interpretPropData(data.sliced(offset + s_propheaderSize, dataSize), unknown1, property);
        parentState->addProperty(property);

        if (stateID == 0 && partID != 0) {
            // Common Properties
            parentState->properties()->setFallback(getState(classID, 0, 0)->properties());
        } else if (stateID != 0 && partID != 0) {
            // Common
            parentState->properties()->setFallback(getState(classID, partID, 0)->properties());
        }

        offset = nextOffset;
    }
}

void Style::interpretPropData(QByteArray data, quint32 unknown1, Property *property)
{
    switch (property->type())
    {

    case IDENTIFIER::INTLIST: {
        QList<int> list;
        int count = qFromLittleEndian<int>(data.sliced(0, 4).constData());

        for (int i = 0; i < count; i++) {
            list.append(qFromLittleEndian<int>(data.sliced(i * 4, 4).constData()));
        }

        property->setValue(list);
        break;
    }

    case IDENTIFIER::COLORLIST: {
        QList<QColor> list;

        for (int i = 0; i < data.size(); i++) {
            int r = qFromLittleEndian<int>(data.sliced(i, 1).constData());
            int g = qFromLittleEndian<int>(data.sliced(i + 1, 1).constData());
            int b = qFromLittleEndian<int>(data.sliced(i + 2, 1).constData());
            int a = qFromLittleEndian<int>(data.sliced(i + 3, 1).constData());

            list.append(QColor::fromRgb(r, g, b, a));
        }

        property->setValue(list);
        break;
    }

    case IDENTIFIER::STRING: {
        QString string;
        int charCount = data.size() / 2;

        for (int i = 0; i < charCount - 1; i += 2) {
            string.append(data.sliced(i, 1).constData());
        }

        property->setValue(string);
        break;
    }

    case IDENTIFIER::FILENAME: {
        property->setValue(unknown1);

        wres::WinResource imageRes = m_resourceTree->findResource("IMAGE", std::to_string(unknown1), "")->children().at(0);
        QByteArray imageData(imageRes.offset(), imageRes.size());

        QImage image;
        image.loadFromData(imageData, "PNG");
        if (image.isNull()) {
            break;
        }

        image.reinterpretAsFormat(QImage::Format_ARGB32_Premultiplied);

        property->setImageFile(QPixmap::fromImage(image));

        break;
    }

    case IDENTIFIER::DISKSTREAM: {
        property->setValue(unknown1);

        wres::WinResource streamRes = m_resourceTree->findResource("STREAM", std::to_string(unknown1), "")->children().at(0);
        QByteArray imageData(streamRes.offset(), streamRes.size());

        QImage image;
        image.loadFromData(imageData, "PNG");

        property->setImageFile(QPixmap::fromImage(image));

        break;
    }

    case IDENTIFIER::FILENAME_LITE:
    case IDENTIFIER::FONT: {
        property->setValue(qFromLittleEndian<int>(unknown1));
        break;
    }

    case IDENTIFIER::INT:
    case IDENTIFIER::SIZE:
    case IDENTIFIER::ENUM:
    case IDENTIFIER::HIGHCONTRASTCOLORTYPE: {
        if (unknown1 == 0x0) {
            property->setValue(qFromLittleEndian<int>(data.sliced(0, 4).constData()));
        } else {
            property->setValue(0);
        }
        break;
    }

    case IDENTIFIER::BOOLTYPE: {
        if (unknown1 == 0x0) {
            property->setValue(qFromLittleEndian<bool>(data.sliced(0, 4).constData()));
        } else {
            property->setValue(false);
        }
        break;
    }

    case IDENTIFIER::COLOR: {
        if (unknown1 == 0x0) {
            int r = qFromLittleEndian<int>(data.sliced(0, 1).constData());
            int g = qFromLittleEndian<int>(data.sliced(1, 1).constData());
            int b = qFromLittleEndian<int>(data.sliced(2, 1).constData());
            int a = qFromLittleEndian<int>(data.sliced(3, 1).constData());

            property->setValue(QColor::fromRgb(r, g, b, a));
        } else {
            property->setValue(QColor());
        }
        break;
    }

    case IDENTIFIER::POSITION: {
        if (unknown1 == 0x0) {
            int x = qFromLittleEndian<int>(data.sliced(0, 4).constData());
            int y = qFromLittleEndian<int>(data.sliced(4, 4).constData());
            property->setValue(QPoint(x, y));
        } else {
            property->setValue(QPoint());
        }
        break;
    }

    case IDENTIFIER::RECTTYPE: {
        if (unknown1 == 0x0) {
            int x = qFromLittleEndian<int>(data.sliced(0, 4).constData());
            int y = qFromLittleEndian<int>(data.sliced(4, 4).constData());
            int w = qFromLittleEndian<int>(data.sliced(8, 4).constData());
            int h = qFromLittleEndian<int>(data.sliced(12, 4).constData());
            property->setValue(QRect(x, y, w, h));
        } else {
            property->setValue(QRect());
        }
        break;
    }

    case IDENTIFIER::MARGINS: {
        if (unknown1 == 0x0) {
            int l = qFromLittleEndian<int>(data.sliced(0, 4).constData());
            int r = qFromLittleEndian<int>(data.sliced(4, 4).constData());
            int t = qFromLittleEndian<int>(data.sliced(8, 4).constData());
            int b = qFromLittleEndian<int>(data.sliced(12, 4).constData());
            property->setValue(QMargins(l, r, t, b));
        } else {
            property->setValue(QMargins());
        }
        break;
    }

    default: {
        if (unknown1 == 0x0) {
            property->setValue(data);
        } else {
            property->setValue(QByteArray());
        }
        break;
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

    for (Class *cls : m_classes) {
        if (cls->name() == "DWMTouch") {
            foundDWMTouch = true;

        } else if (cls->name() == "DWMPen") {
            foundDWMPen = true;

        } else if (cls->name() == "W8::TaskbandExtendedUI") {
            foundW8Taskband = true;

        } else if (cls->name() == "QueryBuilder") {
            foundVistaQueryBuilder = true;

        } else if (cls->name() == "DarkMode::TaskManager") {
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
