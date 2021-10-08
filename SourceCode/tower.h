#ifndef TOWER_H
#define TOWER_H

#include "header.h"
#include "enemy.h"
#include "cell.h"


class Tower : public QLabel     //塔
{
    Q_OBJECT
protected:
    int _cost;
    int _level;
    int _range;
    double _attack;
    double _attackInterval;
    Enemy* _target=nullptr;
    Cell* _posCell;
    QString _towerPicPrefix;
    QString _bulletPicPath;
    QPixmap _towerpic;
    int _bulletSpeed;
    int _bulletLength;   //子弹长度

    QTimer _attackTimer;

    bool _toAttack;

protected:
    inline bool InRange(int x, int y);
    void Attack(GameWindow* gameWindow);

public:
    Tower(QWidget* parent=nullptr, int _cost=0, int _level=0, int _range=0, double _attack=0, double _attackInterval=0, Cell* _posCell=nullptr);
    void Update(GameWindow* gameWindow);
    void Upgrade(); //升级
    inline Cell* GetPositionCell(){return _posCell;}
    inline int Level(){return _level;}
    virtual int Type() {return 0;}

    void mousePressEvent(QMouseEvent *ev) override;


    int Cost(){return _cost;}

signals:
    void TowerPressed(Tower*);

};

class ArrowTower : public Tower     //箭塔
{
public:
    ArrowTower(QWidget* parent, Cell* _posCell);
    int Type() override{return 1;}
};

class MissleTower : public Tower    //炮塔
{
public:
    MissleTower(QWidget* parent, Cell* _posCell);
    int Type()override{return 2;}
};


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
