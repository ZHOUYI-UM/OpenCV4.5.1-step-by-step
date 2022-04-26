#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qworker.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _threadCamera = new QWorker(this);
    connect(_threadCamera, SIGNAL(sigCameraImage(const cv::Mat &)), this, SLOT(slotCameraImage(const cv::Mat &)));
    _threadCamera->start();
}

MainWindow::~MainWindow()
{
    delete _threadCamera;
    _threadCamera = nullptr;
    delete ui;
}

QImage MainWindow::cvMat2QImage(const cv::Mat &mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        cv::cvtColor(mat,mat,cv::COLOR_BGR2RGB);
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        //image = image.rgbSwapped();
        return image.copy();
    }
    else if(mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void MainWindow::slotCameraImage(const cv::Mat &img)
{
    QImage _imgDis = cvMat2QImage(img);
    ui->lblImage->setPixmap(QPixmap::fromImage(_imgDis.scaled(ui->lblImage->size(), Qt::KeepAspectRatio)));
    ui->lblImage->setScaledContents(true);
}
