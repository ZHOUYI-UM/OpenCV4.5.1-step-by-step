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

        cv::namedWindow("example", 1); //创建一个窗口用于显示图像，1代表窗口适应图像的分辨率进行拉伸。
        if (img.empty() == false) //图像不为空则显示图像
        {
            cv::imshow("example", img);
        }

        int  key = cv::waitKey(30); //等待30ms
        if (key ==  int('q')) //按下q退出
        {
            break;
        }

    }
    cap.release(); //释放相机捕获对象
    cv::destroyAllWindows(); //关闭所有窗口
}
