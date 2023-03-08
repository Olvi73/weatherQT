#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QContextMenuEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QString>

#include "weathertool.h"

#define INCREMENT 3 //温度升降 像素点增量
#define POINT_RADIUS 3 //曲线描点的大小
#define TEXT_OFFSET_X 12
#define TEXT_OFFSET_Y 12

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint); //设置无边框窗口
    setAttribute(Qt::WA_TranslucentBackground);  //设置背景透明
    setFixedSize(width(), height()); //设置固定窗口大小

    //构建右键菜单
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();

    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/res/close.png"));

    mExitMenu->addAction(mExitAct);
    connect(mExitAct, &QAction::triggered, this, [=]{
        qApp->exit(0);
    });

    //UI初始化，控件添加到控件数组
    //星期 日期
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;

    // 天气 图标
    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    //天气指数
    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    //风力 风向
    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    //根据keys,设置icon的路径
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined",":/res/type/undefined.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雪",":/res/type/ZhongXue.png");
    mTypeMap.insert("小到中雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("中到大雪",":/res/type/DaXue.png");
    mTypeMap.insert("中到大雨",":/res/type/DaYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");

    //请求网络
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplied);
    //直接在构造中，请求天气数据
    getWeatherInfo("苏州");

    //回车触发按钮
    connect(ui->leCity, SIGNAL(returnPressed()),ui->btnSearch, SIGNAL(clicked()));

    //给标签添加事件过滤器
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_min_Button_clicked()
{
    //最小化按钮
    this->showMinimized();
}

void MainWindow::on_close_Button_clicked()
{
    //关闭按钮
    this->close();
}

void MainWindow::onReplied(QNetworkReply *reply)
{
    qDebug() << "onReplied success";

    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug()<< status_code;
/*
    qDebug() << "operation:" << reply->operation();
    qDebug() << "status_code:" << status_code;
    qDebug() << "url:" << reply->url();
    qDebug() << "rawHeader:" << reply->rawHeaderList();
*/
    if(reply->error() != QNetworkReply::NoError || status_code != 200 )
    {
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this, "天气", "请求数据失败", QMessageBox::Ok);
    }
    else
    {
        QByteArray byteArr = reply->readAll();
        qDebug() << "read all:" << byteArr.data();
        //解析数据
        parseJson(byteArr);
    }
    reply->deleteLater();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

void MainWindow::getWeatherInfo(QString cityName)
{
    QString cityCode = WeatherTool::getCityCode(cityName);
    qDebug()<<"testcitycode"<<cityCode;
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    mNetAccessManager->get(QNetworkRequest(url));
}

void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &err);
    if(err.error != QJsonParseError::NoError)
    {
        return ;
    }
    QJsonObject rootObj = doc.object();
    //qDebug() << rootObj.value("message").toString();

    // 解析日期、城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    // 解析yesterday
    QJsonObject objData = rootObj.value("data").toObject();
    QJsonObject objYesterday = objData.value("yesterday").toObject();
    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("ymd").toString();
    mDay[0].type = objYesterday.value("type").toString();
    qDebug()<<objYesterday.value("high").toString();
    QString s;
    s = objYesterday.value("high").toString().split(" ").at(1);
    s = s.left(s.length()-1);
    mDay[0].high = s.toInt();
    qDebug()<< "high:" + s + objYesterday.value("high").toString();

    s = objYesterday.value("low").toString().split(" ").at(1);
    s = s.left(s.length()-1);
    mDay[0].low = s.toInt();
    qDebug()<< "low:" + s + objYesterday.value("low").toString();

    //风向、风力、污染指数
    mDay[0].fx = objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    //获取forcast中数据
    QJsonArray forecastArr = objData.value("forecast").toArray();
    for(int i=0; i < 5; i++)
    {
        QJsonObject objForecast = forecastArr[i].toObject();
        mDay[i+1].week = objForecast.value("week").toString();
        mDay[i+1].date = objForecast.value("ymd").toString();
        mDay[i+1].type = objForecast.value("type").toString();

        s = objForecast.value("high").toString().split(" ").at(1);
        s = s.left(s.length()-1);
        mDay[i+1].high = s.toInt();

        s = objForecast.value("low").toString().split(" ").at(1);
        s = s.left(s.length()-1);
        mDay[i+1].low = s.toInt();

        //风向、风力、污染指数
        mDay[i+1].fx = objForecast.value("fx").toString();
        mDay[i+1].fl = objForecast.value("fl").toString();
        mDay[i+1].aqi = objForecast.value("aqi").toDouble();
    }

    //今日数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu = objData.value("wendu").toString().toInt();
    mToday.pm25 = objData.value("pm25").toInt();
    mToday.quality = objData.value("quality").toString();

    // 首个数据为今日数据
    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    // 更新UI界面
    updateUI();

    //绘制温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

void MainWindow::updateUI()
{
    // 更新日期和城市
    ui->lblDate->setText(QDateTime::fromString(mToday.date, "yyyyMMdd").toString("yyyy/MM/dd") + " " + mDay[1].week);
    ui->lblCity->setText(mToday.city);

    // 更新今天
    qDebug()<<mTypeMap[mToday.type].split(".").at(0)+"128.png";
//    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type]);
    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type].split(".").at(0)+"128.png");
    ui->lblTemp->setText(QString::number(mToday.wendu) + "°");
    ui->lblType->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low) + "~" +QString::number(mToday.high) + "℃");
    ui->lblGanMao->setText("感冒指数：" + mToday.ganmao);
    ui->lblWindFl->setText(mToday.fl);
    ui->lblWindFx->setText(mToday.fx);
    ui->lblPM25->setText(QString::number(mToday.pm25));
    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    // 更新六天
    ui->lblWeek0->setText("昨天");
    ui->lblWeek1->setText("今天");
    ui->lblWeek2->setText("明天");
    for(int i=0; i < 6; i++)
    {
        if(i > 2) {
            mWeekList[i]->setText("周" + mDay[i].week.right(1));
        }

        QStringList ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(ymdList[1] + "/" + ymdList[2]);

        //更新天气类型
        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);
        //空气质量
        if(mDay[i].aqi > 0 && mDay[i].aqi <= 50) {
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(121, 184, 0);");
        } else if(mDay[i].aqi > 50 && mDay[i].aqi <= 100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 183, 23);");
        } else if(mDay[i].aqi > 100 && mDay[i].aqi <= 150){
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255, 87, 97);");
        } else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(235, 17, 27);");
        } else if(mDay[i].aqi > 200 && mDay[i].aqi <= 250){
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170, 0, 0);");
        } else {
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110, 0, 0);");
        }

        //更新风力
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    //平滑曲线 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    //获取x坐标
    int point_x[6]={0};
    for(int i=0; i<6; i++)
    {
        point_x[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    //获取y坐标
    int tempSum = 0;
    int tempAvg = 0;
    int yCenter = ui->lblHighCurve->height()/2;
    for(int i=0; i<6; i++)
    {
        tempSum += mDay[i].high;
    }
    tempAvg = tempSum / 6;

    int point_y[6] = {0};

    for(int i=0; i<6; i++)
    {
        point_y[i] = yCenter - ((mDay[i].high - tempAvg) * INCREMENT);
    }

    //开始绘制background-color: #FAEBD7;
    QPen pen = painter.pen();
    pen.setWidth(1);  //设置画笔宽度
    pen.setColor(QColor(249,101,84)); //设置画笔颜色
    painter.setPen(pen);
    painter.setBrush(QColor(249,101,84)); //设置画刷内部填充颜色
    for(int i=0; i<6; i++)
    {
        //显示点
        painter.drawEllipse(QPoint(point_x[i], point_y[i]), POINT_RADIUS, POINT_RADIUS);

        //显示文本
        painter.drawText(point_x[i] - TEXT_OFFSET_X, point_y[i] - TEXT_OFFSET_Y, QString::number(mDay[i].high) + "°");
    }

    //绘制曲线
    for(int i=0; i<5; i++)
    {
        if(i==0)
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else
        {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(point_x[i], point_y[i], point_x[i+1], point_y[i+1]);
    }
}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    //平滑曲线 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    //获取x坐标
    int point_x[6]={0};
    for(int i=0; i<6; i++)
    {
        point_x[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    //获取y坐标
    int tempSum = 0;
    int tempAvg = 0;
    int yCenter = ui->lblLowCurve->height()/2;
    for(int i=0; i<6; i++)
    {
        tempSum += mDay[i].low;
    }
    tempAvg = tempSum / 6;

    int point_y[6] = {0};

    for(int i=0; i<6; i++)
    {
        point_y[i] = yCenter - ((mDay[i].low - tempAvg) * INCREMENT);
    }

    //开始绘制
    QPen pen = painter.pen();
    pen.setWidth(1);  //设置画笔宽度
    pen.setColor(QColor(56,120,218)); //设置画笔颜色
    painter.setPen(pen);
    painter.setBrush(QColor(56,120,218)); //设置画刷内部填充颜色
    for(int i=0; i<6; i++)
    {
        //显示点
        painter.drawEllipse(QPoint(point_x[i], point_y[i]), POINT_RADIUS, POINT_RADIUS);

        //显示文本
        painter.drawText(point_x[i] - TEXT_OFFSET_X, point_y[i] - TEXT_OFFSET_Y, QString::number(mDay[i].low) + "°");
    }

    //绘制曲线
    for(int i=0; i<5; i++)
    {
        if(i==0)
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else
        {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(point_x[i], point_y[i], point_x[i+1], point_y[i+1]);
    }
}

void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);
}

