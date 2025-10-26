#ifndef STYLECLASS_H
#define STYLECLASS_H

#include <QObject>

class StyleClass : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString className READ className NOTIFY classNameChanged)
    Q_PROPERTY(int classID READ classID NOTIFY classIDChanged)

public:
    explicit StyleClass(int classID, QString className, QObject *parent = nullptr);

    QString className() { return m_className; }
    int classID() { return m_classID; }

signals:
    void classNameChanged(QString className);
    void classIDChanged(int classID);

private:
    QString m_className;
    int m_classID;
};

#endif // STYLECLASS_H
