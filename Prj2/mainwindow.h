#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <opencv2/opencv.hpp>
#include <QImage>

using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbLoadPicture_clicked();

    void on_pbDetect_clicked();

public:
    QImage cvMat2QImage(const cv::Mat &mat);

private:
    Ui::MainWindow *ui;
    Mat matImageSource;
    Mat matImageGray;
    QImage _imgDis;
};
#endif // MAINWINDOW_H
