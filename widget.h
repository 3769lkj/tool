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
#include<QMovie>
#include<QPropertyAnimation>
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr):QWidget(parent){
        // 设置窗口无边框
setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        //设置背景透明
        setAttribute(Qt::WA_TranslucentBackground);
        // 设置一个默认大小，避免窗口不可见
        resize(200, 200);

        // 初始化定时器但不启动,直接启动会崩溃
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Widget::nextImage);
        // menuBar->setStyleSheet("background-color: transparent;");

        menuBar = new QMenuBar(this);


        QPushButton *toggleMenuButton = new QPushButton("≡", this); // 菜单按钮
        toggleMenuButton->setStyleSheet(R"(
    QPushButton {
        background-color: #fff176;
        border: 2px solid #fbc02d;
        border-radius: 15px;
        font-size: 18px;
        font-weight: bold;
        color: #5d4037;
        padding: 4px 12px;
    }
    QPushButton:hover {
        background-color: #fff59d;
    }
)");//按钮样式

        toggleMenuButton->setFixedSize(40, 30);

        // 创建菜单
        menu = new QMenu(this);
        menu->setStyleSheet(R"(
    QMenu {
        background-color: #fff3e0;
        border: 2px solid #ffb74d;
        border-radius: 12px;
    }
    QMenu::item {
        padding: 8px 20px;
        font-size: 16px;
        color: #5d4037;
    }
    QMenu::item:selected {
        background-color: #ffe082;
        color: #4e342e;
        border-radius: 8px;
    }
)");
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
        this->setWindowOpacity(0.0);
        QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
        fadeIn->setDuration(600);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
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
    QMovie *gifMovie = nullptr;
    bool isGifPlaying = false;
private slots:
    void nextImage() {
        if (images.isEmpty()) return; // 防止崩溃
        currentImageIndex = (currentImageIndex + 1) % images.size();
        update();
    }

    //自动识别文件photo1，然后顺次播放photo2,hoto3...
    void openImage() {//实现既能播放gif动图，也能实现普通图片连续播放
        QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open Image",
            "",
            "Images (*.png *.jpg *.bmp *.jpeg *.gif);;All Files (*)");

        if (!fileName.isEmpty()) {
            QString fileName = QFileDialog::getOpenFileName(
                this,
                "Open Image",
                "",
                "Images (*.png *.jpg *.bmp *.jpeg *.gif);;All Files (*)");

            if (fileName.isEmpty())
                return;

            // 如果是GIF
            if (fileName.endsWith(".gif", Qt::CaseInsensitive)) {
                if (gifMovie) {
                    delete gifMovie;
                }

                gifMovie = new QMovie(fileName);
                if (!gifMovie->isValid()) {
                    delete gifMovie;
                    gifMovie = nullptr;
                    return;
                }

                isGifPlaying = true;
                connect(gifMovie, &QMovie::frameChanged, this, QOverload<>::of(&Widget::update));
                gifMovie->start();

                // 停止定时器（防止与轮播冲突）
                timer->stop();
                images.clear();
                return;
            }

            // 非GIF处理
            if (gifMovie) {
                gifMovie->stop();
                delete gifMovie;
                gifMovie = nullptr;
            }
            isGifPlaying = false;

            QFileInfo fi(fileName);
            QString dirPath = fi.absolutePath();
            QString baseName = fi.completeBaseName();
            QRegularExpression re("^" + QRegularExpression::escape(baseName).replace(QRegularExpression("\\d+$"), "") + "\\d+\\.(png|jpg|bmp|jpeg)$");

            QDir dir(dirPath);
            QStringList files = dir.entryList(QDir::Files);
            images.clear();

            for (const QString &file : files) {
                if (re.match(file).hasMatch()) {
                    QPixmap pix(dir.absoluteFilePath(file));
                    if (!pix.isNull()) {
                        images.append(pix);
                    }
                }
            }

            currentImageIndex = 0;
            update();

            if (!timer->isActive() && images.size() > 1) {
                timer->start(1000);
            }
        }
    }
};
#endif // WIDGET_H
