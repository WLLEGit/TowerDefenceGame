#ifndef CELL_H
#define CELL_H

#include <QWidget>
#include "header.h"

namespace Ui {
class Cell;
}

class Cell : public QLabel  //单元格类
{
    Q_OBJECT

public:
    enum CellType {Path=0x10, Red=0x11, Green=0x12, Blue=0x14, White=0x18,  //可移动路径
                   Start=0x3f, End=0x5f,                                    //始末格
                   Blocked=0x100,                                           //阻碍
                   Placable=0x1000                                         //可放置塔
                    };
private:
    Ui::Cell *ui;
    int type;
    CellType cellType;
    int resourceType;   //0:none, 1:gold, 2:coppper
    int r;
    int c;


public:
    Cell(int type= Blocked, int resourceType = 0, QWidget* parent=nullptr, int r=0, int c=0);
    inline CellType GetCellType() const {return cellType;}
    inline int GetCellTypeID()const {return type;}
    inline int GetResourceType() const {return resourceType;}
    inline void AddType(int type){SetType(this->type | type);}
    inline int row(){return r;}
    inline int col(){return c;}

private:
    void SetType(int type);

};

#endif // CELL_H
