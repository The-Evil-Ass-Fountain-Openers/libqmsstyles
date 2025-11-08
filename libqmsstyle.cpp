#include "libqmsstyle.h"

#include <QApplication>

#include <QDir>
#include <QProcess>
#include <QFileInfo>

LibQmsstyle::LibQmsstyle()
    : QObject{nullptr}
{}

Style::Style *LibQmsstyle::loadMsstyle(const QString &path)
{
    QStringList splitPath(path.split("/"));
    Style::Style *style = new Style::Style(splitPath[splitPath.length() - 1], path);
    if(!style->invalid())
    {
        // TODO: put all this debug stuff in a separate class or something idk
        qDebug() << "libqmsstyle<" + qApp->applicationName() + ">: Style object for " + path + " was created succesfully.";

        style->load();

        this->m_loadedMsstyles.push_back(style);
        this->msstyleLoaded(style);
    }

    return style;
}

#include "moc_libqmsstyle.cpp"
