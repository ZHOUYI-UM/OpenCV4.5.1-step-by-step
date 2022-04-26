#include "qworker.h"
#include <opencv2/opencv.hpp>
#include <QtDebug>

using namespace cv;

QWorker::QWorker(QObject *parent)
    : QThread(parent)
{
}

QWorker::~QWorker()
{
    requestInterruption();
    quit();
    wait();
    qDebug()<<"QWorker::~QWorker";
}

QImage QWorker::cvMat2QImage(const cv::Mat &mat)
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

void QWorker::run()
{
    cv::VideoCapture cap;   //声明相机捕获对象

    //设置编码格式，//https://www.fourcc.org/codecs.php
    //更多属性设置参考：https://docs.opencv.org/4.5.0/d9/df8/tutorial_root.html
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640); //图像的宽，需要相机支持此宽
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); //图像的高，需要相机支持此高
    //图像分辨率640×480

    //我是一个工控机，用的USB摄像头。由于机器本身没有摄像头。
    //如果是带摄像头的笔记本电脑，使用外接摄像头，应该就是1
    int deviceID = 0; //相机设备号，不是0就是1

    cap.open(deviceID); //打开相机

    if (!cap.isOpened()) //判断相机是否打开
    {
        std::cerr << "ERROR!!Unable to open camera\n";
        return;
    }

    cv::Mat img;
    while (!isInterruptionRequested())
    {
        cap >> img; //以流形式捕获图像
        QImage _imgDis = cvMat2QImage(img);
        emit sigCameraImage(_imgDis);
    }
    cap.release(); //释放相机捕获对象
    cv::destroyAllWindows(); //关闭所有窗口
}
