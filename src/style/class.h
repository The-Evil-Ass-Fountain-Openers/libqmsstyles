#ifndef QMSSTYLESCLASS_H
#define QMSSTYLESCLASS_H

#include <QObject>

#include <QMap>

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
    Class(int id, QString name);

    QString name() const;
    int id() const;

    const Class *baseClass() const;
    void setBaseClass(const Class *baseClass);

    bool hasPart(int partID) const;
    Part *getPart(int partID, bool fallback = true) const;
    // this generates a new list and is slower
    QList<Part *> parts() const;
    void addPart(Part *part);

private:
    QString m_name;
    const Class *m_baseClass;
    int m_id;
    QMap<int, Part *> m_parts;
};

}

#endif
