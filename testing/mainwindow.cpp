#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../style/class.h"
#include "../style/style.h"

#include <QFileInfo>
#include <QDir>
#include <QFile>

#include <stdexcept>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->readMsstyle, &QPushButton::clicked, this, &MainWindow::readMsstyle_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readMsstyle_clicked()
{
    LibQmsstyle *msstyleParser = new LibQmsstyle;
    QObject::connect(msstyleParser, &LibQmsstyle::msstyleLoaded, this, [=](Style::Style *loadedStyle){
        QString className = "class name here";
        QString partId = "part name here";
        QString stateId = "state id here";

        for(Style::Class classObject : loadedStyle->classes()) {
            if(classObject.className == "Button") {
                className = "class: Button";

                for(Style::Part part : classObject.parts) {
                    if(part.name == "PUSHBUTTON") {
                        partId = "part id: " + QString::number(part.id);

                        for(Style::State state : part.states) {
                            if(state.name == "NORMAL") stateId = "normal state id: " + QString::number(state.id);
                        }
                    }
                }
            }
        }

        ui->classname->setText(className);
        ui->partid->setText(partId);
        ui->stateid->setText(stateId);

        ui->extractedPath->setText(loadedStyle->path().toString());
        ui->version->setText(QString::number(loadedStyle->version()));
    });
    if(msstyleParser->loadMsstyle(QUrl(ui->msstylePath->text()))) {
        m_loadedMsstyle = msstyleParser;
    }
}

#include "moc_mainwindow.cpp"
