#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../qmsstyles.h"

#include <QMainWindow>

#include <QAction>
#include <QProcess>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readMsstyles(const QString &file);

    void refreshParts(int index);
    void refreshStates(int index);
    void refreshProperties(int index);
    void refreshProperty(int index);

private:
    Ui::MainWindow *ui;

    Qmsstyles *m_msstyleParser{nullptr};
    QSharedPointer<VisualStyle::Style> m_loadedStyle;
    QFileDialog *m_fileDlg{nullptr};
};
#endif // MAINWINDOW_H
