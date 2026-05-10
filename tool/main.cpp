#include <QApplication>
#include <QQmlApplicationEngine>

#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.loadFromModule("userinterface", "Main");

    app.setWindowIcon(QIcon::fromTheme("application-x-theme"));

    return app.exec();
}
