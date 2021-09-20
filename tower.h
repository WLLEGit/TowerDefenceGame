#ifndef TOWER_H
#define TOWER_H

#include "header.h"
#include "enemy.h"
#include "cell.h"

class Tower : public QLabel     //塔
{
    Q_OBJECT
protected:
    int cost;
    int level;
    int range;
    double attack;
    double attackInterval;
    Enemy* target=nullptr;
    Cell* locationCell;
    QString towerPicPrefix;
    QString bulletPicPrefix;
    QPixmap towerpic;
    int rotation;

    QTimer attackTimer;

protected:
    inline bool InRange(int x, int y);
    void Attack();

public:
    Tower(QWidget* parent=nullptr, int cost=0, int level=0, int range=0, double attack=0, double attackInterval=0, Cell* locationCell=nullptr);
    void Update(GameWindow* gameWindow);
    void Upgrade(); //升级

};

class ArrowTower : public Tower     //箭塔
{
public:
    ArrowTower(QWidget* parent, Cell* locationCell);
};

class MissleTower : public Tower    //炮塔
{
public:
    MissleTower(QWidget* parent, Cell* locationCell);
};




class Hero : public QLabel  //友方单位
{
    Q_OBJECT

public:
    Hero(QWidget* parent, Cell* posCell, int maxCapacity,  \
         int maxHealth, int attack, double attackInterval, QString name, int range, int cost);
    Cell* GetPositionCell(){return posCell;}

    bool CanHoldEnemy(){return curHealth > 0 && capacity > 0;}
    void AddEnemy(Enemy* enemy);
    void RemoveEnemy(Enemy* enemy);

    void BeAttacked(int damage){curHealth -= damage;}
    bool IsAlive(){return curHealth > 0;}
    inline float GetHealthRate(){return (float)curHealth / maxHealth;}

    void Update(GameWindow* gameWindow);

    static Hero* GenerateHero(QWidget* parent, Cell* posCell, int type);


protected:
    Cell* posCell;
    int capacity;
    int maxCapacity;
    int curHealth;
    int maxHealth;
    int attack;
    double attackInterval;
    QString name;
    int range;
    int cost;
    QTimer* picTimer;
    int curIndex;
    int maxIndex;
    QList<Enemy*> targets;
    QTimer* attackTimer;

    Enemy* target;
    void Attack();
    bool InRange(int x, int y);

public:
    QProgressBar* healthBar;
    void DrawHealthLine();

signals:
    void HeroDead(Hero*);
};

class Hero1 : public Hero
{
public:
    Hero1(QWidget* parent, Cell* posCell);
};

class Hero2 : public Hero
{
public:
    Hero2(QWidget* parent, Cell* posCell);
};



class Bullet : public QLabel        //子弹，异步发射
{
    Q_OBJECT
private:
    double speed;
    Tower* sender;
    Enemy* target;

public:
    Bullet(QWidget* parent=nullptr);
};


#endif // TOWER_H
