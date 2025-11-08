#ifndef LOGGING_H
#define LOGGING_H

#include <QString>

namespace logging
{

void printCritical(const QString &message);
void printWarning(const QString &message);
void printInfo(const QString &message);
void printDebug(const QString &message);

}

#endif // LOGGING_H
