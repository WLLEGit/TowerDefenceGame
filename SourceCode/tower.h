#ifndef TOWER_H
#define TOWER_H

#include "header.h"
#include "enemy.h"
#include "cell.h"


class Tower : public LivingUnit     //塔
{
    Q_OBJECT
protected:
    GameWindow* gameWindow;

    int _cost;
    int _level;
    int _maxLevel;
    Enemy* _target=nullptr;
    Hero* _heroGened=nullptr;
    Cell* _posCell;
    QString _towerPicPrefix;
    QString _bulletPicPath;
    QPixmap _towerpic;
    int _bulletSpeed;
    int _bulletLength;   //子弹长度

    bool _canAttackFly;
    bool _canPlaceGuarder;

protected:
    inline bool InRange(int x, int y);
    void LoadConfig(QString towerName) override;
    void Attack() override;

public:
    Tower(QWidget* parent=nullptr, GameWindow* gameWindow=nullptr, Cell* _posCell=nullptr);
    void Update() override;
    void Upgrade(); //升级
    inline Cell* GetPositionCell(){return _posCell;}
    inline int Level(){return _level;}

    void mousePressEvent(QMouseEvent *ev) override;

    int Cost(){return _cost;}
    virtual int Type(){return -1;}

    static Tower* GenerateTower(QWidget* parent, GameWindow* gameWindow, Cell* _posCell, int type);


signals:
    void TowerPressed(Tower*);

};

#define TowerDefineHelper(className, type)\
class className : public Tower     \
{\
public:\
    className(QWidget* parent, GameWindow* gameWindow, Cell* _posCell);\
    void SpecialAbility() override;\
    int Type() override{return type;}\
};

TowerDefineHelper(ArrowTower, 1)
TowerDefineHelper(MissleTower, 2)
TowerDefineHelper(GuardTower, 3)


class Bullet : public QLabel        //子弹
{
    Q_OBJECT
private:
    Enemy* _target;
    Tower* _sender;
    int _speed;
    int _damage;
    QPixmap _pic;

    bool _isHit;

public:
    Bullet(QWidget* parent, Enemy* _target, Tower* _sender, int _speed, int _damage, QPixmap _pic);
    void Update(GameWindow* gameWindow);
    int GetDamage(){return _damage;}
    Enemy* GetTarget(){return _target;}

signals:
    void HitEnemy(Bullet*);
};


#endif // TOWER_H
