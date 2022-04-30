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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbSelImage_clicked()
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
                images.push_back(imread(path, 0));
                labels.push_back(atoi(classlabel.c_str()));
            }
        }
    } catch (cv::Exception &e) {
        cout<<"Error opening file "<<filename.toStdString()<<". Reason:"<<e.msg;
        return;
    }

    // 下面的几行代码仅仅是从你的数据集中移除最后一张图片，作为测试图片
    cv::Mat testSample = images[images.size() - 1];
    int testLabel = labels[labels.size() - 1];
    images.pop_back();//删除最后一张照片，此照片作为测试图片
    labels.pop_back();//删除最有一张照片的labels

    //调用其中的成员函数train()来完成分类器的训练
    Ptr<face::BasicFaceRecognizer> model = face::EigenFaceRecognizer::create();
    model->train(images, labels);
    model->save("MyFacePCAModel.xml");//保存路径可自己设置，但注意用“\\”

    Ptr<face::BasicFaceRecognizer> model1 = face::FisherFaceRecognizer::create();
    model1->train(images, labels);
    model1->save("MyFaceFisherModel.xml");

    Ptr<face::LBPHFaceRecognizer> model2 = face::LBPHFaceRecognizer::create();
    model2->train(images, labels);
    model2->save("MyFaceLBPHModel.xml");

    // 下面对测试图像进行预测，predictedLabel是预测标签结果
    //注意predict()入口参数必须为单通道灰度图像，如果图像类型不符，需要先进行转换
    //predict()函数返回一个整形变量作为识别标签
    int predictedLabel = model->predict(testSample);//加载分类器
    int predictedLabel1 = model1->predict(testSample);
    int predictedLabel2 = model2->predict(testSample);
    string result_message = format("Predicted class = %d / Actual class = %d.", predictedLabel, testLabel);
    string result_message1 = format("Predicted class = %d / Actual class = %d.", predictedLabel1, testLabel);
    string result_message2 = format("Predicted class = %d / Actual class = %d.", predictedLabel2, testLabel);
    cout << result_message << endl;
    cout << result_message1 << endl;
    cout << result_message2 << endl;

    QMessageBox::information(this,
                             tr("Information"),
                             tr("Traning finished!"));
}
