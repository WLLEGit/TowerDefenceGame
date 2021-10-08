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
    double _maxHealth;  //生命
    double _curHealth;
    double _attack;  //攻击力
    int _dealHealthDamage;   //到达终点造成的伤害
    double _rangedDamageRate;  //受远程伤害加成比例
    double _closeDamageRate;  //受近程伤害加成比例
    double _attackInterval;   //攻击间隔
    int _attackRange;

    QTimer* _picTimer;
    int _curIndex;
    int _maxIndex;
    QString _enemyName;
    int _picHeight;

    QTimer* _attackTimer;

    QList<Cell*>* _path;
    int _posIndex=0;
    double _speed;
    int _pathType;

    enum EnemyStatus {Moving, Fighting, Dead};
    EnemyStatus _status=Moving;

    Hero* _target=nullptr;

    QProgressBar* _healthBar;


public:
    Enemy(QWidget* parent=nullptr, double health=0, double _attack=0, int _dealHealthDamage=0, double _rangedDamageRate=0, \
          double _closeDamageRate=0, double _attackInterval=0, double _speed=0, int _attackRange=0);

    static Enemy* GenerateEnemy(int type, QWidget* parent, Cell* bornCell, GameWindow* gameWindow, int _pathType);
    void Update(GameWindow* gameWindow);  //每帧调用
    inline bool IsAlive(){return _status != Dead;}
    void BeAttacked(int damage){_curHealth -= damage;}
    inline float GetHealthRate(){return (float)_curHealth / _maxHealth;}
    void OnDead();
    void Show(){show(); _healthBar->show();}


private:
    void SwithPic();
    inline void Attack();
    void DrawHealthLine();
    void InitPath(GameWindow* gameWindow, Cell* start);
};

#endif // ENEMY_H
