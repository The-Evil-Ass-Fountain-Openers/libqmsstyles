#include "class.h"

#include <QApplication>

namespace VisualStyle
{

Class::Class(int id, QString name)
{
    if(name == "")
    {
        qFatal() << "libqmsstyle<" + qApp->applicationName() + ">: cannot construct StyleClass with an empty class name.";
        return;
    }

    className = name;
    classID = id;
}

const Part *Class::findPart(const QString &name) const
{
    auto it = std::find_if(parts.constBegin(), parts.constEnd(), [&](const Part partObject){
        return partObject.name == name;
    });

    if(it != parts.constEnd()) return &(*it);
    else return nullptr;
}

}
