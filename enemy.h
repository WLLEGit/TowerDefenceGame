#ifndef ENEMY_H
#define ENEMY_H

#include "header.h"
#include "cell.h"

class GameWindow;
class Hero;

class Enemy : public QLabel
{
    Q_OBJECT
private:
    double maxHealth;  //生命
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

    Hero* target=nullptr;

    QProgressBar* healthBar;
    void DrawHealthLine();


public:
    Enemy(QWidget* parent=nullptr, double health=0, double attack=0, int dealHealthDamage=0, double rangedDamageRate=0, \
          double closeDamageRate=0, double attackInterval=0, double speed=0, int attackRange=0);

    static Enemy* GenerateEnemy(int type, QWidget* parent, Cell* bornCell, GameWindow* gameWindow, int pathType);
    void Update(GameWindow* gameWindow);  //每帧调用
    inline bool IsAlive(){return status != Dead;}
    int tmp =0;
    void BeAttacked(int damage){curHealth -= damage;}
    inline float GetHealthRate(){return (float)curHealth / maxHealth;}
    void OnDead(){this->hide(); healthBar->hide();attackTimer->stop();}
    void Show(){show(); healthBar->show();}


private:
    void SwithPic();
    inline void Attack();
    void InitPath(GameWindow* gameWindow, Cell* start);
};

#endif // ENEMY_H
