#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

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

void MainWindow::on_pbLoadPicture_clicked()
{
    QString strFilePic = QFileDialog::getOpenFileName(this, tr("Open picture"));
    if (strFilePic.isEmpty())
    {
        QMessageBox::information(this, tr("Warning"), tr("No file selected!"));
        return ;
    }

    //注意：imread不支持中文名字
    matImageSource = imread(strFilePic.toLatin1().data(), 1);  //读取图片
    _imgDis = cvMat2QImage(matImageSource);
    ui->lblImage->setPixmap(QPixmap::fromImage(_imgDis.scaled(ui->lblImage->size(), Qt::KeepAspectRatio)));
    ui->lblImage->setScaledContents(true);
}

void MainWindow::on_pbDetect_clicked()
{
    CascadeClassifier face_detector;
    CascadeClassifier eyes_detector;

    string fDetectorPath = "D:\\opencv4.5.1\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
    face_detector.load(fDetectorPath);
    string eDetectorPath = "D:\\opencv4.5.1\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
    eyes_detector.load(eDetectorPath);
    vector<Rect> faces;

    Mat imgGray;

    cvtColor(matImageSource, imgGray, COLOR_RGB2GRAY);
    equalizeHist(imgGray, imgGray);
    face_detector.detectMultiScale(imgGray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));//-- 多尺寸检测人脸
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        Point center(faces[i].x + faces[i].width * 0.5, faces[i].y + faces[i].height * 0.5);
        ellipse(matImageSource, center, Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
        Mat faceROI = imgGray(faces[i]);
        vector<Rect> eyes;
        eyes_detector.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));//-- 在每张人脸上检测双眼
        for (unsigned int j = 0; j < eyes.size(); j++)
        {
            Point center(faces[i].x + eyes[j].x + eyes[j].width * 0.5, faces[i].y + eyes[j].y + eyes[j].height * 0.5);
            int radius = cvRound((eyes[j].width + eyes[i].height) * 0.25);
            circle(matImageSource, center, radius, Scalar(255, 0, 0), 4, 8, 0);
        }
     }

     _imgDis = cvMat2QImage(matImageSource);
     ui->lblImage->setPixmap(QPixmap::fromImage(_imgDis.scaled(ui->lblImage->size(), Qt::KeepAspectRatio)));
     ui->lblImage->setScaledContents(true);
}
