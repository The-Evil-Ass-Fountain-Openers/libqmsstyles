#include "imagefileitem.h"

#include "../style/class.h"
#include "../style/part.h"
#include "../style/state.h"
#include "../style/property.h"

#include <QPainter>

ImageFileItem::ImageFileItem(QQuickItem *parent)
{
}

ImageFileItem::~ImageFileItem() = default;

void ImageFileItem::paint(QPainter *painter)
{
    if (!m_property) {
        painter->eraseRect(QRect(0, 0, width(), height()));
        return;
    }

    QPixmap pixmap = m_property->imageFile();
    painter->drawPixmap(0, 0, pixmap.width(), pixmap.height(), pixmap);
}

QString ImageFileItem::name() const
{
    if (!m_property) {
        return {};
    }

    VisualStyle::State *state = qobject_cast<VisualStyle::State *>(m_property->parent()->parent());
    if (!state) {
        return {};
    }

    VisualStyle::Part *part = qobject_cast<VisualStyle::Part *>(state->parent());
    if (!part) {
        return {};
    }

    VisualStyle::Class *cls = qobject_cast<VisualStyle::Class *>(part->parent());
    if (!cls) {
        return {};
    }

    return QString("%1_%2_%3_%4%5").arg(cls->name(), part->name(),state->name(),
                                        VisualStyle::NAMES.key((int)m_property->name()),
                                        QString::number(m_property->value<int>()));
}

void ImageFileItem::extractTo(QUrl path) const
{
    if (!m_property || path.toLocalFile().isEmpty()) {
        return;
    }

    QPixmap pixmap = m_property->imageFile();
    pixmap.save(path.toLocalFile());
}

void ImageFileItem::setProperty(VisualStyle::Property *property)
{
    m_property = property;
    Q_EMIT imageFileChanged();

    if (!property) {
        setImplicitWidth(0);
        setImplicitHeight(0);
        setTextureSize(QSize(0, 0));
        return;
    }

    QPixmap pixmap = m_property->imageFile();
    setImplicitWidth(pixmap.width());
    setImplicitHeight(pixmap.height());
    setTextureSize(pixmap.size());

    update();
}

bool ImageFileItem::hasImageFile() const
{
    return !!m_property;
}
