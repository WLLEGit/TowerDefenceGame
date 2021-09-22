#ifndef HERO_H
#define HERO_H

#include "header.h"
#include "enemy.h"
#include "cell.h"

class Hero : public QLabel  //友方单位
{
    Q_OBJECT

public:
    Hero(QWidget* parent, Cell* posCell, int maxCapacity,  \
         int maxHealth, int attack, double attackInterval, QString name, int range, int cost);
    Cell* GetPositionCell(){return posCell;}

    bool CanHoldEnemy(){return curHealth > 0 && capacity > 0;}
    void AddEnemy(Enemy* enemy);
    bool RemoveEnemy(Enemy* enemy);

    inline void BeAttacked(int damage){curHealth -= damage; DrawHealthLine();}
    bool IsAlive(){return !isDestoried && curHealth > 0;}
    inline float GetHealthRate(){return (float)curHealth / maxHealth;}

    void Update(GameWindow* gameWindow);

    static Hero* GenerateHero(QWidget* parent, Cell* posCell, int type);

    int Cost(){return cost;}

    void Show(){show(); healthBar->show();}

protected:
    void SwitchPic();


protected:
    bool isDestoried;

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

#endif // HERO_H
