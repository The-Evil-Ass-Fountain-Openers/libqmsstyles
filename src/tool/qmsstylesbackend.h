#ifndef QMSSTYLESBACKEND_H
#define QMSSTYLESBACKEND_H

#include "../style/style.h"

#include "styletreemodel.h"
#include "propertiesmodel.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QtQml/qqmlregistration.h>

class ImageFileItem;

class QmsstylesBackend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

    Q_PROPERTY(QString currentStyleName READ currentStyleName NOTIFY loadedChanged)
    Q_PROPERTY(QString currentStylePath READ currentStylePath NOTIFY loadedChanged)
    Q_PROPERTY(QString currentStyleVersion READ currentStyleVersion NOTIFY loadedChanged)

    Q_PROPERTY(QModelIndex currentElement READ currentElement WRITE setCurrentElement NOTIFY currentElementChanged)
    Q_PROPERTY(ImageFileItem *imageFileItem READ imageFileItem WRITE setImageFileItem NOTIFY imageFileItemChanged)

    Q_PROPERTY(StyleTreeModel *structureModel READ structureModel NOTIFY loadedChanged)
    Q_PROPERTY(PropertiesModel *propertiesModel READ propertiesModel NOTIFY loadedChanged)

public:
    explicit QmsstylesBackend(QObject *parent = nullptr);
    ~QmsstylesBackend();

    Q_INVOKABLE void load(QUrl path);
    Q_INVOKABLE void load(QString path);

    bool loaded() const;

    QString currentStyleName() const;
    QString currentStylePath() const;
    QString currentStyleVersion() const;

    QModelIndex currentElement();
    void setCurrentElement(QModelIndex index);

    ImageFileItem *imageFileItem();
    void setImageFileItem(ImageFileItem *imageFileItem);

    StyleTreeModel *structureModel();
    PropertiesModel *propertiesModel();

Q_SIGNALS:
    void loadedChanged();

    void currentElementChanged();
    void imageFileItemChanged();

private:
    QString m_currentStyleName;
    QString m_currentStylePath;
    QString m_currentStyleVersion;

    QModelIndex m_currentElement;
    ImageFileItem *m_imageFileItem = nullptr;

    StyleTreeModel *m_structureModel;
    PropertiesModel *m_propertiesModel;

    QSharedPointer<VisualStyle::Style> m_currentStyle;
};

#endif
