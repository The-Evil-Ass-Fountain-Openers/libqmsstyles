#ifndef QMSSTYLES_GLOBAL_H
#define QMSSTYLES_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QMSSTYLES_LIBRARY)
#define QMSSTYLES_EXPORT Q_DECL_EXPORT
#else
#define QMSSTYLES_EXPORT Q_DECL_IMPORT
#endif

#endif // QMSSTYLES_GLOBAL_H
