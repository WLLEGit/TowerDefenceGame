#ifndef HEADER_H
#define HEADER_H

#include<QString>
#include<QTextStream>
#include<QFile>
#include<QVector>
#include<QImage>
#include<QPainter>
#include<QLabel>
#include<QDebug>
#include<QPicture>
#include<QPushButton>
#include<QPropertyAnimation>
#include<QTimer>
#include<cmath>
#include<QList>
#include<QObject>
#include<QMap>
#include<QBrush>
#include<QPainter>
#include<QPaintEvent>
#include<QProgressBar>

#define DISTANCE(x,y) (sqrt((x)*(x)+(y)*(y)))

constexpr int CELLWIDTH = 70;   //单元格像素大小
constexpr int FPS = 30;         //帧率

constexpr int HERO1COST=200;
constexpr int HERO2COST=200;
constexpr int ARROWCOST=100;
constexpr int MISSILECOST=150;

constexpr int TOWERMAXLEVEL = 3;
constexpr int RESOURCEUPDATEDURATION = 400; //ms



enum class Direction{Left, Down, Up, Right, None};

extern QImage MergeImage(const QImage& baseImage, const QImage& overlayImage);  //合并两张图片

extern QPixmap RotatePixmap(const QPixmap& pixmap, QLabel* target, QLabel* src);


#endif // HEADER_H
