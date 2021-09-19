#ifndef TOWER_H
#define TOWER_H

#include "header.h"

class Tower : QLabel     //塔
{
private:
    int cost;
    int level;
    int range;
    double attack;
    double attackInterval;



public:
    Tower(int cost=0, int level=0, int range=0, double attack=0, double attackInterval=0);
};

class FriendlyUnit : QLabel  //友方单位
{
public:
    enum Type{Guard, Miner};
    FriendlyUnit();

};

class Bullet : QLabel        //子弹，异步发射
{
public:
    Bullet();
};

class ArrowTower : public Tower     //箭塔
{
public:

};

class MissleTower : public Tower    //炮塔
{

};

#endif // TOWER_H
