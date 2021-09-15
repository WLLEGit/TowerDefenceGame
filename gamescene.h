#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "enemy.h"
#include "tower.h"


class Cell
{
public:
    enum CellType {Path=0x10, Red=0x11, Green=0x12, Blue=0x14, White=0x18,  //可移动路径
                   Start=0x3f, End=0x5f,                                    //始末格
                   Blocked=0x100,                                           //阻碍
                   Placable=0x1000,                                          //可放置塔
                   Resource=0x1001f, RsrcGold=0x3001f, RsrcCopper=0x5001f};  //资源格
private:
    int type;
    CellType cellType;


public:
    Cell(int type= Blocked);

private:
    void SetType(int type);

signals:
    void ChangeType(CellType type);
};

class GameScene
{
private:
    int row, col;
    QVector<QVector<Cell>> map;
    QVector<Enemy*> enemies;
    QVector<Tower*> towers;

    int round;
    int health;
    int money;

private:
    void LoadMap(int id);

public:
    GameScene(int mapID=1);
};

#endif // GAMESCENE_H
