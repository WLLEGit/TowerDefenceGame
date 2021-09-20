#ifndef ENEMY_H
#define ENEMY_H

#include "header.h"
#include "cell.h"

class GameWindow;
class FriendlyUnit;

class Enemy : public QLabel
{
private:
    double maxHealth;  //生命力
    double curHealth;
    double attack;  //攻击力
    int dealHealthDamage;   //到达终点造成的伤害
    double rangedDamageRate;  //受远程伤害加成比例
    double closeDamageRate;  //受近程伤害加成比例
    double attackInterval;   //攻击间隔
    int attackRange;

    QTimer* picTimer;
    int curIndex;
    int maxIndex;
    QString enemyName;

    QTimer* attackTimer;

    QList<Cell*>* path;
    int posIndex=0;
    double speed;
    int pathType;

    enum EnemyStatus {Moving, Fighting, Dead};
    EnemyStatus status=Moving;

    FriendlyUnit* target=nullptr;


public:
    Enemy(QWidget* parent=nullptr, double health=0, double attack=0, int dealHealthDamage=0, double rangedDamageRate=0, \
          double closeDamageRate=0, double attackInterval=0, double speed=0, int attackRange=0);

    static Enemy* GenerateEnemy(int type, QWidget* parent, Cell* bornCell, GameWindow* gameWindow, int pathType);
    void Update(GameWindow* gameWindow);  //每帧调用

private:
    void SwithPic();
    void Attack();
    void InitPath(GameWindow* gameWindow, Cell* start);
};

#endif // ENEMY_H
