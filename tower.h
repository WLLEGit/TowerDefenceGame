#ifndef TOWER_H
#define TOWER_H

#include "header.h"
#include "enemy.h"
#include "cell.h"

class Tower : public QLabel     //塔
{
private:
    int cost;
    int level;
    int range;
    double attack;
    double attackInterval;
    Enemy* target;

public:
    Tower(QWidget* parent=nullptr, int cost=0, int level=0, int range=0, double attack=0, double attackInterval=0);
};

class FriendlyUnit : public QLabel  //友方单位
{
public:
    enum Type{Guard, Miner};
    FriendlyUnit(QWidget* parent=nullptr);
    bool CanHoldEnemy(){return capacity != 0;}
    Cell* GetPositionCell(){return posCell;}
    void AddEnemy(Enemy* enemy);
    void RemoveEnemy(Enemy* enemy);
    void BeAttacked(int damage){curHealth -= damage;}

    void Update();

private:
    Cell* posCell;
    int capacity;
    int maxCapacity;
    int curHealth;
    int maxHealth;


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

class ArrowTower : public Tower     //箭塔
{
public:

};

class MissleTower : public Tower    //炮塔
{

};

#endif // TOWER_H
