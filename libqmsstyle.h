#ifndef LIBQMSSTYLE_H
#define LIBQMSSTYLE_H

#include <QObject>
#include <QProcess>

#include "libqmsstyle_global.h"

#include "style/style.h"

class LIBQMSSTYLE_EXPORT LibQmsstyle : public QObject
{
    Q_OBJECT

public:
    explicit LibQmsstyle();

    Style::Style *loadMsstyle(const QString &path);
    QList<Style::Style *> loadedStyles() { return m_loadedMsstyles; }

signals:
    void msstyleLoaded(Style::Style *loadedStyle);

private:
    QList<Style::Style *> m_loadedMsstyles;
};

#endif // LIBQMSSTYLE_H
