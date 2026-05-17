#ifndef CLASS_H
#define CLASS_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace VisualStyle
{

class Part;

class Class : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(const Class *baseClass READ baseClass)
    Q_PROPERTY(QList<Part *> parts READ parts)

public:
    explicit Class(int id, QString name);

    QString name() const;
    int id() const;

    const Class *baseClass() const;
    void setBaseClass(const Class *baseClass);

    QList<Part *> &parts();
    void addPart(Part *part);

private:
    QString m_name;
    const Class *m_baseClass;
    int m_id;
    QList<Part *> m_parts;
};

}

#endif
