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
    enum CellType {Path=0x10, Red=0x11, Green=0x12, Blue=0x14, White=0x18,  //可移动路径，White为飞行路径
                   Start=0x3f, End=0x5f,                                    //始末格
                   Blocked=0x100,                                           //阻碍
                   Placable=0x1000                                         //可放置塔
                    };
private:
    Ui::Cell *ui;
    int _type;
    CellType _cellType;
    int _resourceType;   //0:none, 1:gold, 2:coppper
    int _r;
    int _c;


public:
    Cell(int _type= Blocked, int _resourceType = 0, QWidget* parent=nullptr, int _r=0, int _c=0);
    inline CellType GetCellType() const {return _cellType;}
    inline int GetCellTypeID()const {return _type;}
    inline int GetResourceType() const {return _resourceType;}
    inline void AddType(int type){SetType(this->_type | type);}
    inline int row(){return _r;}
    inline int col(){return _c;}

    void mousePressEvent(QMouseEvent *ev) override;

private:
    void SetType(int _type);

signals:
    void CellPressed(Cell*);

};

#endif // CELL_H
