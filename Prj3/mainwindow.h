#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slotCameraImage(const cv::Mat &);

private:
    QImage cvMat2QImage(const cv::Mat &mat);

private:
    Ui::MainWindow *ui;
    QWorker *_threadCamera{nullptr};
};
#endif // MAINWINDOW_H
