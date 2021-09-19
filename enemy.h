#ifndef ENEMY_H
#define ENEMY_H

#include "header.h"

class Enemy
{
private:
    double maxHealth;  //生命力
    double curHealth;
    double attack;  //攻击力
    int dealHealthDamage;   //到达终点造成的伤害
    double rangedDamageRate;  //受远程伤害加成比例
    double closeDamageRate;  //受近程伤害加成比例
    double attackInterval;   //攻击间隔
    QPropertyAnimation* attackAnimation;    //攻击动作
    QPropertyAnimation* moveAnimation;    //移动动作

public:
    double posX, posY;


public:
    Enemy(double health=0, double attack=0, int dealHealthDamage=0, double rangedDamageRate=0, \
            double closeDamageRate=0, double attackInterval=0, QPropertyAnimation* attackAnimation=nullptr\
            , QPropertyAnimation* moveAnimation=nullptr);
};

#endif // ENEMY_H
