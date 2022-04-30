#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/face/facerec.hpp>

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
    void slotStartDetect();
    void slotFaceDetection();

private:
    QImage cvMat2QImage(const cv::Mat &mat);

private:
    Ui::MainWindow *ui;
    QWorker *_threadCamera{nullptr};
    CascadeClassifier face_detector;
    CascadeClassifier eyes_detector;
    QTimer *_timer{nullptr};
    cv::Mat _matFace;
    Ptr<face::BasicFaceRecognizer> _model;
};
#endif // MAINWINDOW_H
