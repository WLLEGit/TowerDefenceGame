#ifndef TOWER_H
#define TOWER_H

#include "header.h"
#include "enemy.h"
#include "cell.h"


class Tower : public QLabel     //塔
{
    Q_OBJECT
protected:
    int cost;
    int level;
    int range;
    double attack;
    double attackInterval;
    Enemy* target=nullptr;
    Cell* posCell;
    QString towerPicPrefix;
    QString bulletPicPath;
    QPixmap towerpic;
    int rotation;
    int bulletSpeed;
    int bulletLength;   //子弹长度

    QTimer attackTimer;

    bool toAttack;

protected:
    inline bool InRange(int x, int y);
    void Attack(GameWindow* gameWindow);

public:
    Tower(QWidget* parent=nullptr, int cost=0, int level=0, int range=0, double attack=0, double attackInterval=0, Cell* posCell=nullptr);
    void Update(GameWindow* gameWindow);
    void Upgrade(); //升级
    inline Cell* GetPositionCell(){return posCell;}
    inline int Level(){return level;}
    inline int Type(){return 0;}

    void mousePressEvent(QMouseEvent *ev) override;


    int Cost(){return cost;}

signals:
    void TowerPressed(Tower*);

};

class ArrowTower : public Tower     //箭塔
{
public:
    ArrowTower(QWidget* parent, Cell* posCell);
    inline int Type(){return 1;}
};

class MissleTower : public Tower    //炮塔
{
public:
    MissleTower(QWidget* parent, Cell* posCell);
    inline int Type(){return 2;}
};


class Bullet : public QLabel        //子弹
{
    Q_OBJECT
private:
    Enemy* target;
    Tower* sender;
    int speed;
    int damage;
    QPixmap pic;

    bool isHit;

public:
    Bullet(QWidget* parent, Enemy* target, Tower* sender, int speed, int damage, QPixmap pic);
    void Update(GameWindow* gameWindow);
    int GetDamage(){return damage;}
    Enemy* GetTarget(){return target;}

signals:
    void HitEnemy(Bullet*);


};


#endif // TOWER_H
