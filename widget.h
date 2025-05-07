#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>//图片
#include<QApplication>
#include <QTimer>
#include <QVector>
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr):QWidget(parent){

        for (int i = 1; i <= 5; ++i) {
            QString imagePath = QString(":/new/prefix1/C:/Users/zhanglongfei/Desktop/resoure/photo%1.png").arg(i);//加载图片
            QPixmap pixmap;
            if (pixmap.load(imagePath)) {
                images.append(pixmap);
            } else {
                qDebug() << "Failed to load image:" << imagePath;//加载错误时弹出
            }
        }
        if (!images.isEmpty()) {
            currentImageIndex = 0;

            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, &Widget::nextImage);//建立一个信号槽连接
            timer->start(1000);// 创建定时器，设置间隔为 1000 毫秒
        }

    };



    ~Widget();
protected:
    void paintEvent(QPaintEvent *event) override;
     private:
    QPixmap pixmap;
         QVector<QPixmap>images;//图片
    int currentImageIndex;//图片数
    QTimer *timer;//准备定时器
private slots:
    void nextImage() {
        currentImageIndex = (currentImageIndex + 1) % images.size();//图片数
        update(); // 触发 paintEvent 重新绘制窗口
    }


};
#endif // WIDGET_H
