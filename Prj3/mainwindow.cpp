#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qworker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _threadCamera = new QWorker(this);
    _threadCamera->start();
}

MainWindow::~MainWindow()
{
    delete _threadCamera;
    _threadCamera = nullptr;
    delete ui;
}

