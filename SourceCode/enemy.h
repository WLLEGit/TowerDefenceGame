#ifndef ENEMY_H
#define ENEMY_H

#include "header.h"
#include "cell.h"


class GameWindow;
class Hero;

class Enemy : public QLabel
{
    Q_OBJECT
protected:
    double _maxHealth;  //生命
    double _curHealth;
    double _attack;  //攻击力
    int _dealHealthDamage;   //到达终点造成的伤害
    double _attackInterval;   //攻击间隔(s)
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
    Cell::CellType _pathType;

    enum EnemyStatus {Moving, Fighting, Dead};
    EnemyStatus _status=Moving;

    Hero* _target=nullptr;

    QProgressBar* _healthBar;

    bool _isFlying;  //是否为飞行单位
    bool _isMilitant;//是否好战


public:
    Enemy(QWidget* parent=nullptr);

    static Enemy* GenerateEnemy(int type, QWidget* parent, Cell* bornCell, GameWindow* gameWindow, Cell::CellType cellType);
    void Update(GameWindow* gameWindow);  //每帧调用
    inline bool IsAlive(){return _status != Dead;}
    void BeAttacked(int damage){_curHealth -= damage;}
    inline float GetHealthRate(){return (float)_curHealth / _maxHealth;}
    void OnDead();
    void Show(){show(); _healthBar->show();}
    inline bool IsFlying(){return _isFlying;}


protected:
    void SwithPic();
    inline void Attack();
    void DrawHealthLine();
    void LoadConfig(QString enemyName);
    void InitBornLocation(GameWindow* gameWindow, Cell* bornCell, Cell::CellType cellType);
    virtual void SpecialAbility(GameWindow*){};
};

//敌方单位派生类生成器
#define EnemyClassGen(className) \
class className : public Enemy  \
{\
public:\
    className(QWidget* parent=nullptr);\
protected:\
    void SpecialAbility(GameWindow* gameWindow) override;\
};

EnemyClassGen(EnemyFly1) //好战飞行单位
EnemyClassGen(EnemyFly2) //畏战飞行单位

EnemyClassGen(EnemyPig) //普通近战单位
EnemyClassGen(EnemyMonster)   //远程单位
EnemyClassGen(EnemyAntiTower)//反塔单位
EnemyClassGen(EnemyHealer)  //治疗单位

#endif // ENEMY_H
