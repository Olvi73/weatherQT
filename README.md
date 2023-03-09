# weatherQT

## 1. 功能

1. 无边框界面，自定义最小化、关闭按钮 - **`setWindowFlag`、修改样式表**
2. 窗口拖动 - **重写`mouseMoveEvent` `mousePressEvent`**
3. 调用天气API接口 - **QUrl** 
4. 解析返回的天气Json数据 - **QJson相关工具**
5. 更新UI界面 - **使用`QMap`定位图片、重写`eventFilter`方法**
6. 绘制温度折线图 - **使用`QPainter`和`QPen`根据`QLabel`位置定位**
7. 回车自动搜索 - **`LineEdit`信号函数`returnPressed()`**

## 2. 环境
Qt Creator 9.0.1 + Qt 5.15.2 MinGW 32-bit
## 3. 测试

### 3.1 主界面

![img1](https://github.com/Olvi73/weatherQT/blob/main/screenshots/mainwindow.png)
![bj](https://github.com/Olvi73/weatherQT/blob/main/screenshots/bj.png)

### 3.2 运行
![img1](https://github.com/Olvi73/weatherQT/blob/main/screenshots/GIF.gif)
