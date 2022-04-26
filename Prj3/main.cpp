#include "mainwindow.h"

#include <QApplication>
#include <opencv2/opencv.hpp>

Q_DECLARE_METATYPE(cv::Mat)

int main(int argc, char *argv[])
{
    qRegisterMetaType<cv::Mat>("cv::Mat");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
