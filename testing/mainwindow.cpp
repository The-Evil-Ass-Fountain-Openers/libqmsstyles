#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../style/property.h"
#include "../style/class.h"
#include "../style/style.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMenuBar>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("libqmsstyles testing application");
    this->setWindowIcon(QIcon::fromTheme("folder"));

    QObject::connect(ui->currentClass, &QComboBox::currentIndexChanged, this, &MainWindow::refreshParts);
    QObject::connect(ui->currentPart, &QComboBox::currentIndexChanged, this, &MainWindow::refreshStates);
    QObject::connect(ui->currentState, &QComboBox::currentIndexChanged, this, &MainWindow::refreshProperties);
    QObject::connect(ui->currentProperty, &QComboBox::currentIndexChanged, this, &MainWindow::refreshProperty);

    QObject::connect(ui->actionOpen, &QAction::triggered, [&]() {
        auto *fileDlg = new QFileDialog(this, "Open an msstyles file", QDir::homePath(), "Windows msstyles files (*.msstyles)");
        QObject::connect(fileDlg, &QFileDialog::fileSelected, this, &MainWindow::readMsstyles);

        fileDlg->open();
    });
    QObject::connect(ui->actionSaveImageAs, &QAction::triggered, [&]() {
        if (ui->imagefile->pixmap().isNull()) return;

        auto *fileDlg = new QFileDialog(nullptr, "Save image as", QDir::homePath(), "PNG images (*.png)");
        fileDlg->setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
        fileDlg->selectFile(ui->imagefile->property("resName").value<QString>() + ".png");
        QObject::connect(fileDlg, &QFileDialog::fileSelected, [&](const QString &file) {
            ui->imagefile->pixmap().save(file, "PNG");
        });

        fileDlg->open();
    });

    m_msstyleParser = Qmsstyles::self();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readMsstyles(const QString &file)
{
    m_loadedStyle = m_msstyleParser->load(file);

    switch(m_loadedStyle->version()) {
    case VisualStyle::Style::WindowsXP:
        ui->statusBar->showMessage("Version: Windows XP");
        break;
    case VisualStyle::Style::WindowsVista:
        ui->statusBar->showMessage("Version: Windows Vista");
        break;
    case VisualStyle::Style::Windows7:
        ui->statusBar->showMessage("Version: Windows 7");
        break;
    case VisualStyle::Style::Windows8:
        ui->statusBar->showMessage("Version: Windows 8/8.1");
        break;
    case VisualStyle::Style::Windows10:
        ui->statusBar->showMessage("Version: Windows 10");
        break;
    case VisualStyle::Style::Windows11:
        ui->statusBar->showMessage("Version: Windows 11");
        break;
    }

    ui->currentClass->clear();

    for(VisualStyle::Class classObject : m_loadedStyle->classes())
        ui->currentClass->addItem(classObject.className);
}

void MainWindow::refreshParts(int index) {
    ui->currentPart->clear();
    if(m_msstyleParser && index >= 0) {
        const VisualStyle::Class *currentClass = &m_loadedStyle->classes().at(index);

        for(const VisualStyle::Part &partObject : currentClass->parts)
            ui->currentPart->addItem(QString::number(partObject.id) + " - " + partObject.name);
    }
}

void MainWindow::refreshStates(int index) {
    ui->currentState->clear();
    if(m_msstyleParser && index >= 0) {
        const VisualStyle::Class *currentClass = &m_loadedStyle->classes().at(ui->currentClass->currentIndex());
        const VisualStyle::Part *currentPart = &currentClass->parts.at(index);

        for(const VisualStyle::State &stateObject : currentPart->states)
            ui->currentState->addItem(QString::number(stateObject.id) + " - " + stateObject.name);
    }
}

void MainWindow::refreshProperties(int index) {
    ui->currentProperty->clear();
    if(m_msstyleParser && index >= 0) {
        const VisualStyle::Class *currentClass = &m_loadedStyle->classes().at(ui->currentClass->currentIndex());
        const VisualStyle::Part *currentPart = &currentClass->parts.at(ui->currentPart->currentIndex());
        const VisualStyle::State *currentState = &currentPart->states.at(index);

        for(const VisualStyle::Property &propertyObject : currentState->properties)
            ui->currentProperty->addItem(propertyObject.name);
    }
}

// this sucks
void MainWindow::refreshProperty(int index) {
    if(m_msstyleParser && index >= 0) {
        const VisualStyle::Class *currentClass = &m_loadedStyle->classes().at(ui->currentClass->currentIndex());
        const VisualStyle::Part *currentPart = &currentClass->parts.at(ui->currentPart->currentIndex());
        const VisualStyle::State *currentState = &currentPart->states.at(ui->currentState->currentIndex());
        const VisualStyle::Property *currentProperty = &currentState->properties.at(ui->currentProperty->currentIndex());

        if(currentProperty->isImage()) {
            ui->imagefile->setPixmap(currentProperty->imagefile);
            ui->imagefile->setProperty("resName", currentProperty->valueString());
        }
        ui->propertyvalue->setText(currentProperty->valueString());
    }
}

#include "moc_mainwindow.cpp"
