#include "widget.h"

class TransparentWindow : public QWidget {
public:
    TransparentWindow(QWidget *parent = nullptr) : QWidget(parent) {
        // 设置无边框+背景透明
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        // 主布局
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);  // 去除边距

        // 菜单栏
        QMenuBar *menuBar = new QMenuBar(this);
        menuBar->setStyleSheet("background-color: rgba(255, 255, 255, 220);");

        QMenu *fileMenu = menuBar->addMenu("文件");
        fileMenu->addAction("打开");
        fileMenu->addAction("退出", this, &QWidget::close);

        layout->setMenuBar(menuBar);

        // 可视内容区（测试用）
        QLabel *label = new QLabel("这是一个透明窗口，但菜单可点击", this);
        label->setStyleSheet("color: black; font-size: 18px; padding: 10px;");
        layout->addWidget(label);

        setLayout(layout);
        resize(400, 300);
    }};
int main(int argc, char *argv[])
{
    QFont font("Comic Sans MS", 10);//字体样式

    QApplication a(argc, argv);
    Widget w;
    a.setFont(font);
    w.show();
    return a.exec();
}
