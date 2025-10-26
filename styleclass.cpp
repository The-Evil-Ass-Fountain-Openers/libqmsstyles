#include "styleclass.h"

#include <QApplication>

StyleClass::StyleClass(int classID, QString className, QObject *parent)
    : QObject{parent}
{
    if(className == "") {
        qFatal() << "libqmsstyle<" + qApp->applicationName() + ">: cannot construct StyleClass with an empty class name.";
        return;
    }

    m_className = className;
    emit classNameChanged(m_className);

    m_classID = classID;
    emit classIDChanged(m_classID);
}

#include "moc_styleclass.cpp"
