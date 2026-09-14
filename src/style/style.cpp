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
#include <QLoggingCategory>

#include <wres/winlibrary.h>

Q_LOGGING_CATEGORY(QMSSTYLES_VISUALSTYLE_STYLE, "qmsstyles.visualstyle.style")

namespace VisualStyle
{

static const int s_propheaderSection = 4;
static const int s_propheaderSize = s_propheaderSection * 8;

Style::Style(const QString &name, const QString &path, const bool fakeStructure, QObject *parent)
    : QObject(parent)
    , m_invalid(false)
    , m_name(name)
    , m_path(path)
    , m_fakeStructure(fakeStructure)
    , m_resourceTree(new wres::WinLibrary(path.toStdString()))
{
    if (!(m_resourceTree->isLoaded() && m_resourceTree->isValid() && m_resourceTree->isPEBinary())) {
        qCCritical(QMSSTYLES_VISUALSTYLE_STYLE) << "Failed to load PE binary structure";
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
        return cls->name().compare(name, Qt::CaseInsensitive) == 0;
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

    return cls->getPart(partID);
}

State *Style::getState(int classID, int partID, int stateID)
{
    Part *part = getPart(classID, partID);
    if (!part) {
        return nullptr;
    }

    return part->getState(stateID);
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
        qCWarning(QMSSTYLES_VISUALSTYLE_STYLE) << "Trying to load an invalid msstyles";
        return false;
    }

    loadCMAP();

    m_version = getVersion();

    // m_resourceTree->printResourceTree();

    loadBCMAP();
    if (m_fakeStructure) {
        structurize();
    }
    readPropertyHeaders();
    handlePropertiesInheritance();

    Q_EMIT loaded();
    return true;
}

bool Style::save()
{
    if (m_invalid) {
        qCWarning(QMSSTYLES_VISUALSTYLE_STYLE) << "Attempting to save an invalid msstyles";
        return false;
    }

    saveCMAP();

    Q_EMIT saved();
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

    // handle regular inheritance
    for (Class *cls : m_classes) {
        QStringList classes = cls->name().split("::");
        if (classes.length() > 1) {
            QString baseClass = classes.at(1);
            int idx = classNameToIdx(baseClass);
            if (idx == -1) {
                qCCritical(QMSSTYLES_VISUALSTYLE_STYLE) << cls->name() << "attempting to inherit from non-existent class" << baseClass;
                continue;
            }

            cls->setBaseClass(m_classes.at(idx));
        }
    }
}

void Style::loadBCMAP()
{
    wres::WinResource res = m_resourceTree->findResource("BCMAP", "BCMAP", "")->children().at(0);
    QByteArray data = QByteArray(res.offset(), res.size());

    quint32 supposedCount = qFromLittleEndian<quint32>(data.sliced(0, 4).constData());
    int count = qMin(static_cast<int>(supposedCount), (data.length() - 4) / 4);
    QList<int> parents;

    for (int i = 4; i < data.length(); i += 4) {
        // TODO: is it really just an array of uint8 padded to 4 bytes?
        int index = (quint8)data.at(i);
        if (index >= count) {
            index = -1;
        }
        parents.append(static_cast<int>(index));
    }

    for (int i = 1; i < count; i++) {
        int idx = i + 4;
        if (idx > count) {
            idx = count - 1;
        }
        Class *cls = m_classes.at(idx);
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
    // don't bother
    if (m_version == Version::WindowsLonghorn) {
        return;
    }

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
    QString resourceName;
    if (m_version == Version::WindowsLonghorn) {
        resourceName = "NORMALDEFAULT";
    } else {
        resourceName = "NORMAL";
    }

    wres::WinResource res = m_resourceTree->findResource("VARIANT", resourceName.toStdString(), "")->children().at(0);
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

        QByteArray propData{};
        // the ones that aren't 0x0 don't have any data that follows
        if (unknown1 == 0x0) {
            propData = data.sliced(offset + s_propheaderSize, dataSize);
        } else {
            dataSize = 0;
        }

        int propertySize = s_propheaderSize + dataSize;
        int padding = qMin(4, qCeil((qreal)propertySize / 8.0) * 8 - propertySize);
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

        Class *parentClass = getClass((qint32)classID);
        if (!parentClass) {
            parentClass = new Class((qint32)classID, "Class");
            m_classes.append(parentClass);
        }

        Part *parentPart = parentClass->getPart((qint32)partID, false);
        if (!parentPart) {
            parentPart = new Part((qint32)partID, "Part");
            parentClass->addPart(parentPart);
        }

        State *parentState = parentPart->getState((qint32)stateID, false);
        if (!parentState) {
            parentState = new State((qint32)stateID, "State");
            parentPart->addState(parentState);
        }

        IDENTIFIER name = static_cast<IDENTIFIER>(nameID);
        IDENTIFIER type = static_cast<IDENTIFIER>(typeID);

        Property *property = new Property(name, type);
        interpretPropData(propData, unknown1, property);
        parentState->addProperty(property);

        offset = nextOffset;
    }
}

void Style::interpretPropData(QByteArray data, quint32 unknown1, Property *property)
{
    switch (property->type())
    {

    case IDENTIFIER::INTLIST: {
        QList<quint8> list;
        quint8 count = qFromLittleEndian<quint8>(data.sliced(0, 4).constData());

        for (int i = 0; i < count; i++) {
            list.append(qFromLittleEndian<quint32>(data.sliced(i * 4, 4).constData()));
        }

        property->setValue(list);
        break;
    }

    case IDENTIFIER::COLORLIST: {
        QList<QColor> list;

        for (int i = 0; i < data.size(); i++) {
            quint8 r = qFromLittleEndian<quint8>(data.sliced(i, 1).constData());
            quint8 g = qFromLittleEndian<quint8>(data.sliced(i + 1, 1).constData());
            quint8 b = qFromLittleEndian<quint8>(data.sliced(i + 2, 1).constData());

            list.append(QColor::fromRgb(r, g, b));
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

        QString groupName;
        QString fileType;
        if (m_version == Version::WindowsLonghorn) {
            groupName = "2"; // Bitmap
            fileType = "BMP";
        } else {
            groupName = "IMAGE";
            fileType = "PNG";
        }

        wres::WinResource imageRes = m_resourceTree->findResource(groupName.toStdString(), std::to_string(unknown1), "")->children().at(0);
        QByteArrayView imageData(imageRes.offset(), imageRes.size());

        QImage image;
        image.loadFromData(imageData);
        if (image.isNull()) {
            break;
        }

        if (m_version != Version::WindowsLonghorn) {
            image.reinterpretAsFormat(QImage::Format_ARGB32_Premultiplied);
        }

        property->setImageFile(QPixmap::fromImage(image));

        break;
    }

    case IDENTIFIER::DISKSTREAM: {
        property->setValue(unknown1);

        wres::WinResource streamRes = m_resourceTree->findResource("STREAM", std::to_string(unknown1), "")->children().at(0);
        QByteArrayView imageData(streamRes.offset(), streamRes.size());

        QString fileType;
        if (m_version == Version::WindowsLonghorn) {
            fileType = "BMP";
        } else {
            fileType = "PNG";
        }

        QImage image;
        image.loadFromData(imageData);
        if (image.isNull()) {
            break;
        }

        property->setImageFile(QPixmap::fromImage(image));

        break;
    }

    case IDENTIFIER::FILENAME_LITE:
    case IDENTIFIER::FONT: {
        property->setValue(qFromLittleEndian<quint8>(unknown1));
        break;
    }

    case IDENTIFIER::INT:
    case IDENTIFIER::SIZE:
    case IDENTIFIER::ENUM:
    case IDENTIFIER::HIGHCONTRASTCOLORTYPE: {
        if (unknown1 == 0x0) {
            property->setValue(qFromLittleEndian<quint32>(data.sliced(0, 4).constData()));
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
            quint8 r = qFromLittleEndian<quint8>(data.sliced(0, 1).constData());
            quint8 g = qFromLittleEndian<quint8>(data.sliced(1, 1).constData());
            quint8 b = qFromLittleEndian<quint8>(data.sliced(2, 1).constData());

            property->setValue(QColor::fromRgb(r, g, b));
        } else {
            property->setValue(QColor());
        }
        break;
    }

    case IDENTIFIER::POSITION: {
        if (unknown1 == 0x0) {
            quint32 x = qFromLittleEndian<quint32>(data.sliced(0, 4).constData());
            quint32 y = qFromLittleEndian<quint32>(data.sliced(4, 4).constData());

            property->setValue(QPoint(x, y));
        } else {
            property->setValue(QPoint());
        }
        break;
    }

    case IDENTIFIER::RECTTYPE: {
        if (unknown1 == 0x0) {
            quint32 x = qFromLittleEndian<quint32>(data.sliced(0, 4).constData());
            quint32 y = qFromLittleEndian<quint32>(data.sliced(4, 4).constData());
            quint32 w = qFromLittleEndian<quint32>(data.sliced(8, 4).constData());
            quint32 h = qFromLittleEndian<quint32>(data.sliced(12, 4).constData());

            property->setValue(QRect(x, y, w, h));
        } else {
            property->setValue(QRect());
        }
        break;
    }

    case IDENTIFIER::MARGINS: {
        if (unknown1 == 0x0) {
            quint32 l = qFromLittleEndian<quint32>(data.sliced(0, 4).constData());
            quint32 r = qFromLittleEndian<quint32>(data.sliced(4, 4).constData());
            quint32 t = qFromLittleEndian<quint32>(data.sliced(8, 4).constData());
            quint32 b = qFromLittleEndian<quint32>(data.sliced(12, 4).constData());

            property->setValue(QMargins(l, t, r, b));
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

void Style::handlePropertiesInheritance()
{
    for (VisualStyle::Class *cls : m_classes) {
        for (VisualStyle::Part *part : cls->parts()) {
            for (VisualStyle::State *state : part->states()) {
                State *fallbackState = nullptr;
                if (part->id() != 0 && state->id() == 0) {
                    // Common Properties
                    fallbackState = getState(cls->id(), 0, 0);
                } else if (part->id() != 0 && state->id() != 0) {
                    // Common
                    fallbackState = getState(cls->id(), part->id(), 0);
                }

                if (fallbackState) {
                    state->properties()->setFallback(fallbackState->properties());
                }
            }
        }
    }
}

void Style::saveCMAP()
{
    wres::WinResource res = m_resourceTree->findResource("CMAP", "CMAP", "")->children().at(0);
    QFile file(m_path);
}

void Style::saveProperties()
{
}

// TODO: revamp
Style::Version Style::getVersion()
{
    bool foundDWMTouch = false;
    bool foundDWMPen = false;
    bool foundW8Taskband = false;
    bool foundVistaQueryBuilder = false;
    bool foundTaskBand2Light_Taskband2 = false;
    bool foundTravelFwd_AnimationButton = false;

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
        } else if (cls->name() == "TravelFwd::AnimationButton") {
            foundTravelFwd_AnimationButton = true;
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
    } else if (foundTravelFwd_AnimationButton) {
        return Version::WindowsLonghorn;
    } else {
        return Version::Windows7;
    }
}

}

#include "moc_style.cpp"
