#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qworker.h"
#include <QtDebug>
#include <vector>
#include <string>
#include <opencv2/imgproc.hpp>
#include <QTimer>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _strDir = QCoreApplication::applicationDirPath();
    qDebug()<<_strDir;

    std::string fDetectorPath = "D:\\opencv4.5.1\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
    face_detector.load(fDetectorPath);
    std::string eDetectorPath = "D:\\opencv4.5.1\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
    eyes_detector.load(eDetectorPath);

    _threadCamera = new QWorker(this);
    connect(_threadCamera, SIGNAL(sigCameraImage(const cv::Mat &)), this, SLOT(slotCameraImage(const cv::Mat &)));
    _threadCamera->start();

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(slotFaceDetection()));
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

void MainWindow::slotCameraImage(const cv::Mat &frame)
{
    _matFace = frame;
    QImage _imgDis = cvMat2QImage(frame);
    ui->lblImage->setPixmap(QPixmap::fromImage(_imgDis.scaled(ui->lblImage->size(), Qt::KeepAspectRatio)));
    ui->lblImage->setScaledContents(true);
}

void MainWindow::slotFaceDetection()
{
    cv::Mat frameGray;
    cvtColor(_matFace, frameGray, COLOR_BGR2GRAY);//转灰度化，减少运算
    std::vector<Rect> faces;
    face_detector.detectMultiScale(frameGray, faces, 1.1, 2,
                             0 | CASCADE_SCALE_IMAGE,
                             Size(70, 70),
                             Size(1000, 1000));
    //ui->lblFaces->setText("face=" + QString::number(faces.size()));
    if (1==faces.size())
    {
        _picCnt++;
        ui->lblFaces->setText(QString::number(_picCnt)+" Captured");

        rectangle(frameGray, faces[0], Scalar(255, 0, 0), 2, 8, 0);

        cv::Mat faceROI = frameGray(faces[0]);//在灰度图中将圈出的脸所在区域裁剪出
        cv::Mat myFace;
        cv::resize(faceROI, myFace, Size(92, 112));//将兴趣域size为92*112

        QImage _imgDis = cvMat2QImage(myFace);
        ui->lblDetect->setPixmap(QPixmap::fromImage(_imgDis.scaled(ui->lblDetect->size(), Qt::KeepAspectRatio)));
        ui->lblDetect->setScaledContents(true);

        //图片的存放位置
        QString dir_str = _strDir+"/faces/face" + QString::number(_picCnt) + ".jpg";
        //qDebug()<<dir_str;
        //string filename = format("%s%d.jpg",pic_num);
        std::string filename = dir_str.toStdString();
        imwrite(filename, myFace);//存在当前目录下
    }
}

void MainWindow::on_pbSelDir_clicked()
{
    _strDir = QFileDialog::getExistingDirectory(this, tr("Select a directory"));
}

void MainWindow::on_pbStartDetect_clicked()
{
    QString fullPath=_strDir+"/faces";
    QDir dir(fullPath);
    if (false==dir.exists())
    {
        dir.mkpath(fullPath);
    }

    if (_timer->isActive())
    {
        _timer->stop();
        ui->pbStartDetect->setText(tr("Start"));
    }
    else
    {
        ui->pbStartDetect->setText(tr("Stop"));
        //每隔100ms
        _timer->start(500);
    }
}
