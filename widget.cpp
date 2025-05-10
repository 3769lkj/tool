#include "widget.h"
#include <QPainter>//绘图
#include <QPaintEvent>


Widget::~Widget() {

}

void Widget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    QRect imageRect(0, menuBar->height(), width(), height() - menuBar->height());

    if (isGifPlaying && gifMovie) {//兼容gif和普通图片
        QPixmap currentFrame = gifMovie->currentPixmap();
        painter.drawPixmap(imageRect, currentFrame.scaled(imageRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else if (!images.isEmpty()) {
        painter.drawPixmap(imageRect, images[currentImageIndex].scaled(imageRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
