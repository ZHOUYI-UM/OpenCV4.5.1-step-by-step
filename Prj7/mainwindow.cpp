#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/face/facerec.hpp>
#include <fstream>

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pbRecogImage->setEnabled(false);
    _modeEigenFaceRecognizer   = face::EigenFaceRecognizer::create();
    _modelFisherFaceRecognizer = face::FisherFaceRecognizer::create();
    _modelLBPHFaceRecognizer   = face::LBPHFaceRecognizer::create();
}

MainWindow::~MainWindow()
{
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

void MainWindow::on_pbGenerateData_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"),
                                                    QString(""),
                                                    tr("txt Files (*.txt)"));

    vector<cv::Mat> images;
    vector<int> labels;

    //read csv file
    char separator=';';
    try {
        std::ifstream file(filename.toStdString(), ifstream::in);
        if (!file)
        {
            CV_Error(cv::Error::StsBadArg, "No valid input file was given, please check the given filename.");
            return;
        }
        std::string line, path, classlabel;
        while (getline(file, line))
        {
            std::stringstream liness(line);
            getline(liness, path, separator);
            getline(liness, classlabel);
            if (!path.empty() && !classlabel.empty())
            {
                // 下面的几行代码仅仅是从你的数据集中移除最后一张图片，作为测试图片
                if (std::string::npos == path.find("10.pgm", 0)) {
                    images.push_back(imread(path, 0));
                    labels.push_back(atoi(classlabel.c_str()));
                }
            }
        }
    } catch (cv::Exception &e) {
        cout<<"Error opening file "<<filename.toStdString()<<". Reason:"<<e.msg;
        return;
    }

    //调用其中的成员函数train()来完成分类器的训练
    _modeEigenFaceRecognizer->train(images, labels);
    _modeEigenFaceRecognizer->save("MyFacePCAModel.xml");//保存路径可自己设置，但注意用“\\”

    _modelFisherFaceRecognizer->train(images, labels);
    _modelFisherFaceRecognizer->save("MyFaceFisherModel.xml");

    _modelLBPHFaceRecognizer->train(images, labels);
    _modelLBPHFaceRecognizer->save("MyFaceLBPHModel.xml");

    cv::Mat matT = imread("D:\\projects\\Qt\\OpenCV4.5.1-step-by-step\\Prj6\\att_faces\\s36\\10.pgm", 0);
    int predictedLabel = _modeEigenFaceRecognizer->predict(matT);//加载分类器
    int predictedLabel1 = _modelFisherFaceRecognizer->predict(matT);
    int predictedLabel2 = _modelLBPHFaceRecognizer->predict(matT);

    QString ans=QString("EigenFaceRecognizer predicted %1\nFisherFaceRecognizer predicted %2\nLBPHFaceRecognizer predicted %3\n")
            .arg(predictedLabel)
            .arg(predictedLabel1)
            .arg(predictedLabel2);
    ui->label->setText(ans);

    ui->pbRecogImage->setEnabled(true);
    QMessageBox::information(this,
                             tr("Information"),
                             tr("Traning finished!"));
}

void MainWindow::on_pbLoadImage_clicked()
{
    _strFileImage = QFileDialog::getOpenFileName(this, tr("Open picture"));
    if (_strFileImage.isEmpty())
    {
        QMessageBox::information(this, tr("Warning"), tr("No file selected!"));
        return ;
    }

    //注意：imread不支持中文名字
    _matSource = imread(_strFileImage.toLatin1().data(), 0);  //读取图片
    QImage _imgDis = cvMat2QImage(_matSource);
    ui->lblImage->setPixmap(QPixmap::fromImage(_imgDis.scaled(ui->lblImage->size(), Qt::KeepAspectRatio)));
    ui->lblImage->setScaledContents(true);
    ui->lblImageName->setText(_strFileImage);
    ui->label->setText("");
}

void MainWindow::on_pbRecogImage_clicked()
{
    // 下面对测试图像进行预测，predictedLabel是预测标签结果
    //注意predict()入口参数必须为单通道灰度图像，如果图像类型不符，需要先进行转换
    //predict()函数返回一个整形变量作为识别标签
    int predictedLabel = _modeEigenFaceRecognizer->predict(_matSource);//加载分类器
    int predictedLabel1 = _modelFisherFaceRecognizer->predict(_matSource);
    int predictedLabel2 = _modelLBPHFaceRecognizer->predict(_matSource);

    QString ans=QString("EigenFaceRecognizer predicted %1\nFisherFaceRecognizer predicted %2\nLBPHFaceRecognizer predicted %3\n")
            .arg(predictedLabel)
            .arg(predictedLabel1)
            .arg(predictedLabel2);
    ui->label->setText(ans);
}
