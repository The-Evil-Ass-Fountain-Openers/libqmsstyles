// this file is essen
#include "../../style/style.h"
#include "../../style/class.h"

#include <QObject>

class Hello : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    Hello() = default;
};
