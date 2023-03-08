#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //设置窗口图标
//    QIcon icon = QIcon(":/res/icon.png");
//    w.setWindowIcon(icon);
    return a.exec();
}
