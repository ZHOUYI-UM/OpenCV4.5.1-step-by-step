#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QMessageBox>

using namespace cv;

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

void MainWindow::on_pbLoadPicture_clicked()
{
    QString strFilePic = QFileDialog::getOpenFileName(this, tr("Open picture"));
    if (strFilePic.isEmpty())
    {
        QMessageBox::information(this, tr("Warning"), tr("No file selected!"));
        return ;
    }
    //注意：imread不支持中文名字
    cv::Mat img = imread(strFilePic.toLatin1().data(), 1);//一定要使用绝对路径，其他可以回报错
    namedWindow(strFilePic.toLatin1().data(), WINDOW_AUTOSIZE);
    imshow(strFilePic.toLatin1().data(), img);
}
