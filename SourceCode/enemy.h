#ifndef ENEMY_H
#define ENEMY_H

#include "header.h"
#include "cell.h"
#include "livingunit.h"


class GameWindow;
class Hero;

class Enemy : public LivingUnit
{
    Q_OBJECT
protected:
    int _dealHealthDamage;   //到达终点造成的伤害

    QList<Cell*>* _path;
    int _posIndex=0;
    Cell::CellType _pathType;

    enum EnemyStatus {Moving, Fighting, Dead};
    EnemyStatus _status=Moving;

    Hero* _target=nullptr;

    bool _isFlying;  //是否为飞行单位
    bool _isMilitant;//是否好战
    bool _canAttackTower;


public:
    Enemy(QWidget* parent=nullptr, GameWindow* gameWindow=nullptr);

    static Enemy* GenerateEnemy(int type, QWidget* parent, Cell* bornCell, GameWindow* gameWindow, Cell::CellType cellType);
    void Update() override;  //每帧调用
    inline bool IsAlive() override{return _status != Dead;}
    inline float GetHealthRate(){return (float)_curHealth / _maxHealth;}
    void OnDead();
    void Show(){show(); _healthBar->show();}
    inline bool IsFlying(){return _isFlying;}


protected:
    void SwitchPic() override;
    void Attack() override;
    void LoadConfig(QString enemyName) override;
    void InitBornLocation(GameWindow* gameWindow, Cell* bornCell, Cell::CellType cellType);
    virtual void SpecialAbility() override{};
};

//敌方单位派生类生成器
#define EnemyClassGen(className) \
class className : public Enemy  \
{\
public:\
    className(QWidget* parent, GameWindow* gameWindow);\
protected:\
    void SpecialAbility() override;\
};

EnemyClassGen(EnemyFly1) //好战飞行单位
EnemyClassGen(EnemyFly2) //畏战飞行单位

EnemyClassGen(EnemyPig) //普通近战单位
EnemyClassGen(EnemyMonster)   //远程单位
EnemyClassGen(EnemyAntiTower)//反塔单位
EnemyClassGen(EnemyHealer)  //治疗单位

#endif // ENEMY_H
