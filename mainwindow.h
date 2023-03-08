#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "weatherdata.h"
#include <QMainWindow>
#include <QLabel>
#include <QActionEvent>
#include <QMenu>
#include <QMessageBox>

#include <QNetworkAccessManager>  //获取数据网络接口
#include <QNetworkReply>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    //获取天气数据、解析Json 更新UI界面
    void getWeatherInfo(QString cityCode);
    void parseJson(QByteArray& byteArray);
    void updateUI();

    //重写父类eventFilter方法
    bool eventFilter(QObject* watched, QEvent* event);

    //绘制高低温曲线函数
    void paintHighCurve();
    void paintLowCurve();

private slots:
    void on_min_Button_clicked();
    void on_close_Button_clicked();
    void onReplied(QNetworkReply* reply);
    void on_btnSearch_clicked();

private:
    Ui::MainWindow *ui;

    QMenu* mExitMenu;  //右键退出菜单
    QAction* mExitAct;  //退出行为 菜单项
    QPoint mOffset; //窗口移动偏移量
    QNetworkAccessManager* mNetAccessManager;

    Today mToday;
    Day mDay[6];

    // 星期 日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;

    // 天气 天气图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;

    // 污染指数
    QList<QLabel*> mAqiList;

    // 风力 风向
    QList<QLabel*> mFlList;
    QList<QLabel*> mFxList;

    QMap<QString, QString> mTypeMap;
};
#endif // MAINWINDOW_H
