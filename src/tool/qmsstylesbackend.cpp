#include "qmsstylesbackend.h"

#include "../qmsstyles.h"

#include "../style/state.h"
#include "../style/property.h"

#include "imagefileitem.h"

QmsstylesBackend::QmsstylesBackend(QObject *parent)
    : QObject(parent)
    , m_currentStyleName("Unknown")
    , m_currentStylePath("Unknown")
    , m_structureModel(new StyleTreeModel(this))
    , m_propertiesModel(new PropertiesModel(this))
{
}

QmsstylesBackend::~QmsstylesBackend() = default;

void QmsstylesBackend::load(QUrl path)
{
    load(path.toLocalFile());
}

void QmsstylesBackend::load(QString path)
{
    if (m_currentStyle) {
        Qmsstyles::self()->unload(m_currentStyle);
    }

    m_currentStylePath = path;
    m_currentStyle = Qmsstyles::self()->load(path);
    if (m_currentStyle) {
        m_currentStyleName = m_currentStyle->name();

        switch (m_currentStyle->version())
        {
        case VisualStyle::Style::WindowsVista:
            m_currentStyleVersion = "Windows Vista";
            break;
        case VisualStyle::Style::Windows7:
            m_currentStyleVersion = "Windows 7";
            break;
        case VisualStyle::Style::Windows8:
            m_currentStyleVersion = "Windows 8/8.1";
            break;
        case VisualStyle::Style::Windows10:
            m_currentStyleVersion = "Windows Vista";
            break;
        case VisualStyle::Style::Windows11:
            m_currentStyleVersion = "Windows 11";
            break;
        }


        m_structureModel->setClassArray(m_currentStyle->classes());
    }

    Q_EMIT loadedChanged();
}

bool QmsstylesBackend::loaded() const
{
    return !!m_currentStyle;
}

QString QmsstylesBackend::currentStyleName() const
{
    return m_currentStyleName;
}

QString QmsstylesBackend::currentStylePath() const
{
    return m_currentStylePath;
}

QString QmsstylesBackend::currentStyleVersion() const
{
    return m_currentStyleVersion;
}

QModelIndex QmsstylesBackend::currentElement()
{
    return m_currentElement;
}

void QmsstylesBackend::setCurrentElement(QModelIndex index)
{
    if (m_imageFileItem) {
        m_imageFileItem->setProperty(nullptr);
    }

    if (!index.isValid() || !m_currentStyle) {
        return;
    }

    int classIdx = index.parent().parent().row();
    int partIdx = index.parent().row();
    int stateIdx = index.row();

    if (stateIdx == -1 || partIdx == -1 || classIdx == -1) {
        return;
    }

    VisualStyle::State *state = m_currentStyle->getState(classIdx, partIdx, stateIdx);
    if (!state) {
        return;
    }

    m_currentElement = index;
    Q_EMIT currentElementChanged();

    m_propertiesModel->setState(state);

    if (m_imageFileItem) {
        // TODO: improve this lazy piece of code
        VisualStyle::Property *prop = state->properties()->get(VisualStyle::IDENTIFIER::IMAGEFILE);
        if (prop) {
            m_imageFileItem->setProperty(prop);
            return;
        }

        prop = state->properties()->get(VisualStyle::IDENTIFIER::IMAGEFILE1);
        if (prop) {
            m_imageFileItem->setProperty(prop);
            return;
        }

        prop = state->properties()->get(VisualStyle::IDENTIFIER::DISKSTREAM);
        if (prop) {
            m_imageFileItem->setProperty(prop);
            return;
        }

        m_imageFileItem->setProperty(nullptr);
    }
}

ImageFileItem *QmsstylesBackend::imageFileItem()
{
    return m_imageFileItem;
}

void QmsstylesBackend::setImageFileItem(ImageFileItem *imageFileItem)
{
    m_imageFileItem = imageFileItem;
    Q_EMIT imageFileItemChanged();
    setCurrentElement(m_currentElement);
}

StyleTreeModel *QmsstylesBackend::structureModel()
{
    return m_structureModel;
}

PropertiesModel *QmsstylesBackend::propertiesModel()
{
    return m_propertiesModel;
}
