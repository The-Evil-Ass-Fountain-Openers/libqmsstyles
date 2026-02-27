#include "style.h"

#include "visualpartsmap.h"
#include "propertystream.h"

#include <QFileInfo>
#include <QByteArrayView>
#include <QApplication>
#include <QRegularExpression>

#include <QtLogging>

#include <wres/winlibrary.h>

namespace VisualStyle
{

Style::Style(const QString &name, const QString &path)
    : QObject{nullptr}
{
    m_name = name;
    Q_EMIT nameChanged(m_name);

    m_path = path;
    Q_EMIT pathChanged(m_path);

    m_resourceTree = new wres::WinLibrary(path.toStdString());

    if(!(m_resourceTree->isLoaded() && m_resourceTree->isValid() && m_resourceTree->isPEBinary()))
    {
        qFatal() << "libqmsstyle<" + qApp->applicationName() + ">: extracted msstyle path does not exist. Style object is invalid.";

        m_invalid = true;
        Q_EMIT invalidChanged();

        return;
    }
}

const Class *Style::findClass(const QString &name) const
{
    auto it = std::find_if(m_classes.constBegin(), m_classes.constEnd(), [&](const Class classObject){
        return classObject.className == name;
    });

    if(it != m_classes.constEnd()) return &(*it);
    else return nullptr;
}

QByteArray Style::removeNull(const QByteArray &bytes, const int &start, const int &end)
{
    QByteArray result;

    for(int i = 0; i < bytes.length(); i++)
    {
        char byte = bytes[i];
        if(byte == 0) continue;
        result.push_back(byte);
    }

    return result;
}

Style::Version Style::getVersion()
{
    bool foundDWMTouch = false;
    bool foundDWMPen = false;
    bool foundW8Taskband = false;
    bool foundVistaQueryBuilder = false;
    bool foundTaskBand2Light_Taskband2 = false;

    for(Class classObject : m_classes)
    {
        if(classObject.className == "DWMTouch")
        {
            foundDWMTouch = true;
            continue;
        }
        else if(classObject.className == "DWMPen")
        {
            foundDWMPen = true;
            continue;
        }
        else if(classObject.className == "W8::TaskbandExtendedUI")
        {
            foundW8Taskband = true;
            continue;
        }
        else if(classObject.className == "QueryBuilder")
        {
            foundVistaQueryBuilder = true;
            continue;
        }
        else if(classObject.className == "DarkMode::TaskManager")
        {
            foundTaskBand2Light_Taskband2 = true;
            continue;
        }
    }

    if (foundTaskBand2Light_Taskband2)
        return Version::Windows11;
    else if (foundW8Taskband)
        return Version::Windows8;
    else if (foundDWMTouch || foundDWMPen)
        return Version::Windows10;
    else if (foundVistaQueryBuilder)
        return Version::WindowsVista;
    else
        return Version::Windows7;
}

bool Style::load()
{    
    if(m_invalid)
    {
        qWarning() << "libqmsstyle<" + qApp->applicationName() + ">: attempted to load an invalid Style object.";
        return false;
    }

    // read CMAP (class map)
    {
        wres::WinResource *classmapResource = &m_resourceTree->findResource("CMAP", "CMAP", "")->children().at(0);

        if(!classmapResource)
        {
            qFatal() << "libqmsstyle<" + qApp->applicationName() + ">: CMAP does not exist.";

            m_invalid = true;
            Q_EMIT invalidChanged();

            return false;
        }

        QByteArray classmap_data(classmapResource->offset(), classmapResource->size());

        int lastClass = 0;
        int foundClasses = 0;

        for(int i = 0; i < classmap_data.length(); i += 2)
        {
            if(classmap_data[i] == 0 && classmap_data[i + 1] == 0)
            {
                if (i - lastClass > 2)
                {
                    Class classObject(
                        foundClasses,
                        QString::fromUtf8(removeNull(classmap_data.sliced(lastClass, i - lastClass), lastClass, i))
                    );
                    m_classes.push_back(classObject);
                    Q_EMIT classAdded(&classObject);
                    foundClasses++;
                }
                lastClass = i + 2;
            }
        }
    }

    // TODO: add support for amap later

    m_version = getVersion();
    Q_EMIT versionChanged(m_version);

    // build property tree
    {
        for(Class &classObject : m_classes) {
            const QList<VisualPart> visualParts = VisualPartsMap::find(classObject.className, m_version);

            for(VisualPart visualPart : visualParts) {
                Part part{visualPart.id, visualPart.name};

                for(VisualState visualState : visualPart.states) {
                    State state{visualState.value, visualState.name};
                    part.states.append(state);
                }

                classObject.parts.append(part);
            }
        }
    }

    // load properties
    {
        wres::WinResource *varmapResource = &m_resourceTree->findResource("VARIANT", "NORMAL", "")->children().at(0);

        if(!varmapResource)
        {
            qFatal() << "libqmsstyle<" + qApp->applicationName() + ">: VARIANT NORMAL does not exist.";

            m_invalid = true;
            Q_EMIT invalidChanged();

            return false;
        }

        QByteArray varmap_data(varmapResource->offset(), varmapResource->size());

        int cursor = 0;

        // i love this
        while(cursor < varmap_data.length() - 4)
        {
            Property prop = PropertyStream::readNextProperty(varmap_data, cursor);

            // read image data, interpret as premultiplied and load into the property
            if(prop.isImage())
            {
                wres::WinResource *imageFile = nullptr;

                if(prop.name == "DISKSTREAM")
                    imageFile = &m_resourceTree->findResource("STREAM", std::to_string(prop.value().toInt()), "")->children().at(0);
                else
                    imageFile = &m_resourceTree->findResource("IMAGE", std::to_string(prop.value().toInt()), "")->children().at(0);

                if(imageFile) {
                    QByteArray imageData(imageFile->offset(), imageFile->size());

                    QImage image;
                    image.loadFromData(imageData, "PNG");
                    if(prop.name != "DISKSTREAM")
                        image.reinterpretAsFormat(QImage::Format_ARGB32_Premultiplied);

                    prop.imagefile.convertFromImage(image);
                }
            }

            for(Class &classObject : m_classes) {
                if(classObject.classID == prop.header.classID) {

                    for(Part &partObject : classObject.parts) {
                        if(partObject.id == prop.header.partID) {

                            for(State &stateObject : partObject.states) {
                                if(stateObject.id == prop.header.stateID) {
                                    stateObject.properties.append(prop);
                                    break;
                                }
                            }

                        }
                    }

                }
            }
        }
    }

    return true;
}

}

#include "moc_style.cpp"
