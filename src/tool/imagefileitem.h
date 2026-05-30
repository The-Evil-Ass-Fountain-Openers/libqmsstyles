#ifndef IMAGEFILEITEM_H
#define IMAGEFILEITEM_H

#include <QtQml/qqmlregistration.h>
#include <QQuickPaintedItem>

namespace VisualStyle
{
class Property;
}

class ImageFileItem : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name READ name NOTIFY imageFileChanged)

    Q_PROPERTY(bool hasImageFile READ hasImageFile NOTIFY imageFileChanged)

public:
    explicit ImageFileItem(QQuickItem *parent = nullptr);
    ~ImageFileItem();

    void paint(QPainter *painter) override;

    QString name() const;

    Q_INVOKABLE void extractTo(QUrl path) const;
    void setProperty(VisualStyle::Property *property);

    bool hasImageFile() const;

Q_SIGNALS:
    void imageFileChanged();

private:
    VisualStyle::Property *m_property = nullptr;
};

#endif
