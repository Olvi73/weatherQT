#ifndef SYSTRAYICON_H
#define SYSTRAYICON_H

#include <QSystemTrayIcon>
#include <QWidget>
#include <QMenu>

class sysTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    sysTrayIcon(QWidget *parent);
    ~sysTrayIcon();

private slots:
    void onIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showForecast();
    void about();

private:
    QWidget *mParent; // 父类 显示窗体
    QMenu *trayMenu;  // 菜单

    void initSysTray(); //初始化托盘
    void addSysTrayMenu(); //添加菜单
    void showMessage(); //显示消息框

signals:
    void showTrayMessage(); //触发显示系统托盘消息框
};

#endif // SYSTRAYICON_H
