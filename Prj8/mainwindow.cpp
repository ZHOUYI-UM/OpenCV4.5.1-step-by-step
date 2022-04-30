#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qworker.h"
#include <QtDebug>
#include <vector>
#include <string>
#include <opencv2/imgproc.hpp>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::string fDetectorPath = "D:\\opencv4.5.1\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
    face_detector.load(fDetectorPath);
    std::string eDetectorPath = "D:\\opencv4.5.1\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
    eyes_detector.load(eDetectorPath);

    _model = face::FisherFaceRecognizer::create();
    _model->read("MyFaceFisherModel.xml");

    _threadCamera = new QWorker(this);
    connect(_threadCamera, SIGNAL(sigCameraImage(const cv::Mat &)), this, SLOT(slotCameraImage(const cv::Mat &)));
    _threadCamera->start();

    QTimer::singleShot(10000, this, SLOT(slotStartDetect()));
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

void MainWindow::slotStartDetect()
{
    //每隔100ms
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(slotFaceDetection()));
    _timer->start(100);
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
    if (faces.size()>0)
    {
        //qDebug()<<"检测到人脸个数:" + QString::number(faces.size());
        //识别到的脸用矩形圈出
        for (int i = 0; i < faces.size(); i++)
        {
            //Face recog
            cv::Mat faceROI = frameGray(faces[i]);//在灰度图中将圈出的脸所在区域裁剪出
            cv::Mat myFace;
            cv::resize(faceROI, myFace, Size(92, 112));//将兴趣域size为92*112
            int faceID = _model->predict(myFace);
            ui->lblFaceID->setText("face id=" + QString::number(faceID));

            std::string desc;
            switch (faceID)
            {
            case 0:
                desc="wang jing feng";
                break;
            case 1:
                desc="zhou yi";
                break;
            default:
                desc="unknown";
                break;
            }

            rectangle(_matFace, faces[i], Scalar(255, 0, 0), 2, 8, 0);
            //rectangle(_matFace, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), color, 1, 8);//放入缓存
            putText(_matFace, desc, Point(faces[i].x, faces[i].y), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));//添加文字
        }

        QImage _imgDis = cvMat2QImage(_matFace);
        ui->lblDetect->setPixmap(QPixmap::fromImage(_imgDis.scaled(ui->lblDetect->size(), Qt::KeepAspectRatio)));
        ui->lblDetect->setScaledContents(true);
    }
}
