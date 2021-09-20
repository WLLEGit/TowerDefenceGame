#ifndef TOWER_H
#define TOWER_H

#include "header.h"
#include "enemy.h"
#include "cell.h"

class Tower : public QLabel     //塔
{
protected:
    int cost;
    int level;
    int range;
    double attack;
    double attackInterval;
    Enemy* target=nullptr;
    Cell* locationCell;
    QString towerPicPrefix;
    QString bulletPicPrefix;
    QPixmap towerpic;
    int rotation;

    QTimer attackTimer;

protected:
    inline bool InRange(int x, int y);
    void Attack();

public:
    Tower(QWidget* parent=nullptr, int cost=0, int level=0, int range=0, double attack=0, double attackInterval=0, Cell* locationCell=nullptr);
    void Update(GameWindow* gameWindow);
    void Upgrade(); //升级
};

class ArrowTower : public Tower     //箭塔
{
public:
    ArrowTower(QWidget* parent, Cell* locationCell);
};

class MissleTower : public Tower    //炮塔
{
public:
    MissleTower(QWidget* parent, Cell* locationCell);
};




class FriendlyUnit : public QLabel  //友方单位
{
public:
    FriendlyUnit(QWidget* parent, Cell* posCell, int capacity, int maxCapacity, int curHealth, \
                 int maxHealth, int attack, double attackInterval, QString name, int range, int cost);
    bool CanHoldEnemy(){return capacity != 0;}
    Cell* GetPositionCell(){return posCell;}
    void AddEnemy(Enemy* enemy);
    void RemoveEnemy(Enemy* enemy);
    void BeAttacked(int damage){curHealth -= damage;}

    void Update();

    static FriendlyUnit* GenerateHero(QWidget* parent, Cell* posCell, int type);

protected:
    Cell* posCell;
    int capacity;
    int maxCapacity;
    int curHealth;
    int maxHealth;
    int attack;
    double attackInterval;
    QString name;
    int range;
    int cost;
    QTimer* picTimer;
    int curIndex;
    int maxIndex;
};

class Hero1 : public FriendlyUnit
{
public:
    Hero1(QWidget* parent, Cell* posCell);
};

class Hero2 : public FriendlyUnit
{
public:
    Hero2(QWidget* parent, Cell* posCell);
};



class Bullet : public QLabel        //子弹，异步发射
{
private:
    double speed;
    Tower* sender;
    Enemy* target;

public:
    Bullet(QWidget* parent=nullptr);
};


#endif // TOWER_H
