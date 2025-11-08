#ifndef LIBQMSSTYLES_H
#define LIBQMSSTYLES_H

#include <QObject>
#include <QProcess>

#include "libqmsstyles_global.h"

#include "style/style.h"

class LIBQMSSTYLES_EXPORT LibQmsstyles : public QObject
{
    Q_OBJECT

public:
    LibQmsstyles();

    Style::Style *loadMsstyles(const QString &path);
    Style::Style *loadedMsstyles = nullptr;

signals:
    void msstylesLoaded();

};

#endif // LIBQMSSTYLES_H
