#include "logging.h"

#include <QApplication>
#include <QLoggingCategory>

namespace logging
{

QLoggingCategory category("libqmsstyle");

void printCritical(const QString &message)
{
    qCCritical(category) << "in" + qApp->applicationName() + ":" << message;
}

void printWarning(const QString &message)
{
    qCWarning(category) << "in" + qApp->applicationName() + ":" << message;
}

void printInfo(const QString &message)
{
    qCInfo(category) << "in " + qApp->applicationName() + ":" << message;
}

void printDebug(const QString &message)
{
    qCDebug(category) << "in " + qApp->applicationName() + ":" << message;
}

}
