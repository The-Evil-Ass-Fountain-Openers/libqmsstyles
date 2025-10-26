#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../styleclass.h"

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
    QObject::connect(msstyleParser, &LibQmsstyle::msstyleLoaded, this, [=](Style *loadedStyle){
        QString className = "class name here";

        for(StyleClass *classObject : loadedStyle->classes()) {
            if(classObject->className() == "Button") className = "class: Button";
        }

        ui->classname->setText(className);

        ui->extractedPath->setText(loadedStyle->path().toString());
        ui->version->setText(QString::number(loadedStyle->version()));
    });
    if(msstyleParser->loadMsstyle(QUrl(ui->msstylePath->text()))) {
        m_loadedMsstyle = msstyleParser;
    }
}

#include "moc_mainwindow.cpp"
