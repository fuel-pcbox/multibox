#include "core.h"

#include <QMainWindow>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>

Core::Core(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowTitle("MultiBox");
    QMenu *file = menuBar()->addMenu("&File");
    QAction *loadcfg = file->addAction("&Load Config");
    QAction *quit = file->addAction("&Quit");
    QMenu *about = menuBar()->addMenu("&About");
    QAction *aboutm = about->addAction("&About MultiBox");
    connect(loadcfg, &QAction::triggered, this, &Core::load_config);
    connect(quit, &QAction::triggered, qApp, QApplication::quit);
    connect(aboutm, &QAction::triggered, this, &Core::about_multibox);
}

void Core::load_config()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select config file"), "",
        tr("All Files (*)"));
    QFile file;
    file.setFileName(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QByteArray cfgbytes = file.readAll();
    cfg = QJsonDocument::fromBinaryData(cfgbytes);
}

void Core::about_multibox()
{
    QMessageBox::about(this,tr("MultiBox"), tr("MultiBox is a WIP PC Emulator.\n\nCopyright 2018"));
}