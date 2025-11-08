#include "libqmsstyles.h"
#include "logging.h"

#include <QApplication>

#include <QDir>
#include <QProcess>
#include <QFileInfo>

LibQmsstyles::LibQmsstyles()
    : QObject{nullptr}
{}

Style::Style *LibQmsstyles::loadMsstyles(const QString &path)
{
    QStringList splitPath(path.split("/"));
    Style::Style *style = new Style::Style(splitPath[splitPath.length() - 1], path);
    if(!style->invalid())
    {
        Style::Style *previousStyle = loadedMsstyles;

        logging::printInfo("Style object for " + path + " was created succesfully.");
        style->load();
        loadedMsstyles = style;
        emit msstylesLoaded();

        delete previousStyle;
    }

    return style;
}

#include "moc_libqmsstyles.cpp"
