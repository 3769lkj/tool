#include "widget.h"
#include <QPainter>//绘图
#include <QPaintEvent>


Widget::~Widget() {

}

/*void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!images.isEmpty()) {//判断是否还有图片
        painter.drawPixmap(0, 0, images[currentImageIndex].scaled(300,300));//控制图片位置和大小
    }
    QWidget::paintEvent(event);
}

*/
void Widget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    if (!images.isEmpty()) {
        // 将图片绘制在菜单栏下方（避免覆盖菜单）
        QRect imageRect(0, menuBar->height(), width(), height() - menuBar->height());
        painter.drawPixmap(imageRect, images[currentImageIndex].scaled(imageRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
