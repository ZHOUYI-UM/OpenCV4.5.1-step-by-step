#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <opencv2/opencv.hpp>

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QWorker;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slotCameraImage(const cv::Mat &);
    void slotFaceDetection();

    void on_pbSelDir_clicked();

    void on_pbStartDetect_clicked();

private:
    QImage cvMat2QImage(const cv::Mat &mat);

private:
    Ui::MainWindow *ui;
    QWorker *_threadCamera{nullptr};
    CascadeClassifier face_detector;
    CascadeClassifier eyes_detector;
    QTimer *_timer{nullptr};
    cv::Mat _matFace;
    QString _strDir{""};
    QString _strID{"win10"};
    unsigned int _picCnt{0};
};
#endif // MAINWINDOW_H
