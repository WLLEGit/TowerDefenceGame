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
#include<QMetaObject>

constexpr int CELLWIDTH = 65;   //单元格像素大小

class Cell
{
public:
    enum CellType {Path=0x10, Red=0x11, Green=0x12, Blue=0x14, White=0x18,  //可移动路径
                   Start=0x3f, End=0x5f,                                    //始末格
                   Blocked=0x100,                                           //阻碍
                   Placable=0x1000                                         //可放置塔
                    };
private:
    int type;
    CellType cellType;
    int resourceType;   //0:none, 1:gold, 2:coppper


public:
    double posX, posY;
    Cell(int type= Blocked, int resourceType = 0);
    inline CellType GetCellType() const {return cellType;}
    inline int GetCellTypeID()const {return type;}
    inline int GetResourceType() const {return resourceType;}
    inline void SetResourceType(int type) {resourceType = type;}

private:
    void SetType(int type);
};

extern QImage MergeImage(const QImage& baseImage, const QImage& overlayImage);  //合并两张图片


#endif // HEADER_H
