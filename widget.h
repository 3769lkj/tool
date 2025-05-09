#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>//图片
#include<QApplication>
#include <QTimer>
#include <QVector>
#include <QMouseEvent>
#include<QMenu>
#include<QMenuBar>
#include<QAction>
#include<QDebug>
#include <QFileDialog>
#include<QVBoxLayout>
#include<QLabel>
#include<QPushButton>
#include<QRegularExpression>
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr):QWidget(parent){
        // 设置窗口无边框
        setWindowFlags(Qt::FramelessWindowHint);
       //设置背景透明
       setAttribute(Qt::WA_TranslucentBackground);
       // 设置一个默认大小，避免窗口不可见
       resize(200, 200);

       // 初始化定时器但不启动,直接启动会崩溃
       timer = new QTimer(this);
       connect(timer, &QTimer::timeout, this, &Widget::nextImage);
       // menuBar->setStyleSheet("background-color: transparent;");

        menuBar = new QMenuBar(this);


        QPushButton *toggleMenuButton = new QPushButton("≡", this); // 你可以用图标或文字
        toggleMenuButton->setStyleSheet("background-color: transparent; border: none; ");
        toggleMenuButton->setFixedSize(40, 30);

        // 创建菜单
        menu = new QMenu(this);
        QAction *openImageAction = new QAction("Open Image", this);
        connect(openImageAction, &QAction::triggered, this, &Widget::openImage);
        menu->addAction(openImageAction);

        QAction *closeAction = new QAction("Close", this);
        connect(closeAction, &QAction::triggered, this, &QWidget::close);
        menu->addAction(closeAction);

        // 菜单初始隐藏
        menu->hide();

        // 点击按钮切换菜单显示
        connect(toggleMenuButton, &QPushButton::clicked, this, [=]() {
            if (menu->isVisible()) {
                menu->hide();
            } else {
                menu->exec(toggleMenuButton->mapToGlobal(QPoint(0, toggleMenuButton->height())));
            }
        });

        // 设置布局：右上角按钮
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QHBoxLayout *topLayout = new QHBoxLayout();
        topLayout->addStretch();
        topLayout->addWidget(toggleMenuButton);
        mainLayout->addLayout(topLayout);
        mainLayout->addStretch();
        mainLayout->setContentsMargins(10, 10, 10, 10);
        setLayout(mainLayout);
    };



    ~Widget();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override {//记录鼠标点击位置
        if (event->button() == Qt::LeftButton) {
            isDragging = true;
            dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (isDragging && (event->buttons() & Qt::LeftButton)) {
            move(event->globalPos() - dragStartPosition);
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            isDragging = false;
        }
    }//实现鼠标拖动窗口
     private:
    QPixmap pixmap;
         QVector<QPixmap>images;//图片
    int currentImageIndex;//图片数
    QTimer *timer;//准备定时器
    bool isDragging;
    QPoint dragStartPosition;
    QMenuBar *menuBar;
      QMenu *menu;
private slots:
      void nextImage() {
          if (images.isEmpty()) return; // 防止崩溃
          currentImageIndex = (currentImageIndex + 1) % images.size();
          update();
      }

      //自动识别文件photo1，然后顺次播放photo2,hoto3...
      void openImage() {
        QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open Image",
            "",
            "Images (*.png *.jpg *.bmp *.jpeg);;All Files (*)");

        if (!fileName.isEmpty()) {
            QFileInfo fi(fileName);
            QString dirPath = fi.absolutePath();
            QString baseName = fi.completeBaseName(); // photo1  自动识别文件
            QRegularExpression re("^" + QRegularExpression::escape(baseName).replace(QRegularExpression("\\d+$"), "") + "\\d+\\.(png|jpg|bmp|jpeg)$");

            QDir dir(dirPath);
            QStringList files = dir.entryList(QDir::Files);
            images.clear();

            for (const QString &file : files) {
                if (re.match(file).hasMatch()) {
                    QPixmap pixmap(dir.absoluteFilePath(file));
                    if (!pixmap.isNull()) {
                        images.append(pixmap);
                    }
                }
            }

            currentImageIndex = 0;
            update();

            if (!timer->isActive() && images.size() > 1) {
                timer->start(1000);  // 每秒切换一张
            }
        }}
};
#endif // WIDGET_H
