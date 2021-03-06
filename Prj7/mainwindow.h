#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/face/facerec.hpp>
#include <QImage>

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbGenerateData_clicked();

    void on_pbLoadImage_clicked();

    void on_pbRecogImage_clicked();

private:
    void startTraining();
    QImage cvMat2QImage(const cv::Mat &mat);

private:
    Ui::MainWindow *ui;

private:
    Ptr<face::BasicFaceRecognizer> _modeEigenFaceRecognizer;
    Ptr<face::BasicFaceRecognizer> _modelFisherFaceRecognizer;
    Ptr<face::LBPHFaceRecognizer>  _modelLBPHFaceRecognizer;

    cv::Mat _matSource;
    QString _strFileImage;
};
#endif // MAINWINDOW_H
