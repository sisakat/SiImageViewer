#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btOpenImage_clicked()
{
    QFileDialog fd(this);
    fd.setNameFilter(tr("Images (*.png *.jpg)"));
    fd.setViewMode(QFileDialog::Detail);
    fd.setFileMode(QFileDialog::ExistingFile);
    if (fd.exec()) {
        auto selectedFiles = fd.selectedFiles();
        if (selectedFiles.size() > 0) {
            ui->siImageViewer->setImage(QImage(selectedFiles.first()));
        }
    }
}

