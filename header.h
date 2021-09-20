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

enum class Direction{Left, Down, Up, Right, None};

extern QImage MergeImage(const QImage& baseImage, const QImage& overlayImage);  //合并两张图片


#endif // HEADER_H
