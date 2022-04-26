#ifndef QWORKER_H
#define QWORKER_H

#include <QThread>
#include <opencv2/opencv.hpp>
#include <QImage>

class QWorker : public QThread
{
    Q_OBJECT
public:
    explicit QWorker(QObject *parent = 0);
    ~QWorker();

protected:
    virtual void run() Q_DECL_OVERRIDE;//虚函数  线程处理函数（和主线程不在同一个线程）

signals:
    void sigCameraImage(const QImage &); // 向主线程发送连接状态

private:
    QImage cvMat2QImage(const cv::Mat &mat);
};

#endif // QWORKER_H
