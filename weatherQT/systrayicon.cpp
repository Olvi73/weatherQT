#include "systrayicon.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

#pragma execution_character_set("utf-8")    // qt支持显示中文

sysTrayIcon::sysTrayIcon(QWidget *parent) : mParent(parent), QSystemTrayIcon(parent)
{
    initSysTray();		// 初始化托盘
    addSysTrayMenu();	// 初始化菜单
    this->show();		// 显示托盘
}

sysTrayIcon::~sysTrayIcon()
{

}

// 事件触发
void sysTrayIcon::onIconActivated(QSystemTrayIcon::ActivationReason reason) {
    qDebug() << reason;
    switch (reason) {
        case QSystemTrayIcon::Trigger: // 单击托盘图标
            // 显示菜单
            mParent->activateWindow();
            mParent->showNormal();
            break;
        case QSystemTrayIcon::Context: // 双击托盘图标
            // 显示主程序窗口
            trayMenu->exec(QCursor::pos());
            break;
        default:
            break;
        }

}



// 初始化
void sysTrayIcon::initSysTray() {
    setToolTip("weatherQT");  //设置工具提示
    setIcon(QIcon(":/weatherQT.ico"));  //设置托盘图标
    setVisible(true);

    // 点击托盘所作的操作
    connect(this, &sysTrayIcon::activated, this, &sysTrayIcon::onIconActivated);

    // 点击消息框后显示主页面
    connect(this, &sysTrayIcon::messageClicked, [this] {
        mParent->activateWindow();
        mParent->showNormal();
    });

    // 显示系统托盘消息框
    connect(this, &sysTrayIcon::showTrayMessage, this, &sysTrayIcon::showMessage);
}



// 菜单
void sysTrayIcon::addSysTrayMenu() {
    trayMenu = new QMenu(mParent);  // 新建菜单，指定父对象
    // 新建菜单子项
    QAction *action_forecast = new QAction("预报", trayMenu);
    action_forecast->setIcon(QIcon::fromTheme("audio-volume-high"));
    QAction *action_set = new QAction("设置", trayMenu);
    action_set->setIcon(QIcon::fromTheme("set"));
    QAction *action_about = new QAction("关于", trayMenu);
    action_about->setIcon(QIcon::fromTheme("help-about"));
    QAction *action_quit = new QAction("退出", trayMenu);
    action_quit->setIcon(QIcon::fromTheme("application-exit"));

    // 添加菜单子项
    trayMenu->addAction(action_forecast);
//    trayMenu->addAction(action_set);
    trayMenu->addAction(action_about);
    trayMenu->addAction(action_quit);

    //菜单功能
    connect(action_forecast, SIGNAL(triggered(bool)), this, SLOT(showForecast()));
//    connect(action_set, SIGNAL(triggered(bool)), this, SLOT(set()));
    connect(action_about, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(action_quit, SIGNAL(triggered()), qApp, SLOT(quit()));
}


// 消息框
void sysTrayIcon::showMessage() {
    QSystemTrayIcon::showMessage("Notice","The form is minimized!");
}

//显示主界面
void sysTrayIcon::showForecast()
{
    //mParent->move((QApplication::desktop()->width() - mParent->width())/2, (QApplication::desktop()->height() - mParent->height())/2);
    mParent->activateWindow();
    mParent->showNormal();
}

//关于信息
void sysTrayIcon::about()
{
    QMessageBox MB(QMessageBox::NoIcon, "关于", "weatherQT 天气预报小工具\n环境：\nQt Creator 9.0.1 \nQt 5.15.2 MinGW 32-bit");
    QPixmap pixmap = QPixmap(":/res/me.png");
    //调整pixmap大小，KeepAspectRatio保持图片比例，缩放到size内最大矩形
    pixmap = pixmap.scaled(250, 250, Qt::KeepAspectRatio);
    MB.setIconPixmap(pixmap);
    MB.exec();
}
