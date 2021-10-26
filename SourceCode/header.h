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
#include<QtGlobal>
#include<QTime>
#include<QJsonObject>
#include<QJsonParseError>

#define DISTANCE(x,y) (sqrt((x)*(x)+(y)*(y)))
#define SIGN(x) (x == 0 ? 0 : (x < 0 ? -1 : 1))

extern int CELLWIDTH;   //单元格像素大小
extern int FPS;         //帧率
extern int MAPID;
extern int ROUNDID;

const int RESOURCEUPDATEDURATION = 100;

#ifdef QT_DEBUG
const QString PROJECTPATH = "D:\\HomeWork\\GaoCheng\\Project\\TowerDefence\\SourceCode\\";
#else
const QString PROJECTPATH = "./";
#endif

enum class Direction{Left, Down, Up, Right, None};

extern QImage MergeImage(const QImage& baseImage, const QImage& overlayImage);  //合并两张图片

extern QPixmap RotatePixmap(const QPixmap& pixmap, QLabel* target, QLabel* src);    //旋转图片，使向上的图片朝向target

extern QJsonObject globalConfig;
void ReadConfigFromJsonFile();


#endif // HEADER_H
