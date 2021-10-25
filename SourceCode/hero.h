#ifndef HERO_H
#define HERO_H

#include "header.h"
#include "enemy.h"
#include "cell.h"

class Hero : public QLabel  //友方单位
{
    Q_OBJECT

public:
    Hero(QWidget* parent=nullptr, GameWindow* gameWindow=nullptr, Cell* _posCell=nullptr);
    Cell* GetPositionCell(){return _posCell;}

    bool CanHoldEnemy(){return IsAlive() && _capacityUsed > 0;}
    void AddEnemy(Enemy* enemy);
    bool RemoveEnemy(Enemy* enemy);

    inline void BeAttacked(int damage){_curHealth -= damage; DrawHealthLine();}
    bool IsAlive(){return !_isDestoried && _curHealth > 0;}
    inline float GetHealthRate(){return (float)_curHealth / _maxHealth;}

    void Update(GameWindow* gameWindow=nullptr);

    static Hero* GenerateHero(QWidget* parent, GameWindow* gameWindow, Cell* _posCell, int type);

    int Cost(){return _cost;}

    void Show(){show(); healthBar->show();}

protected:
    void SwitchPic();
    void LoadConfig(QString name);
    void Attack();
    bool InRange(int x, int y);
    void FindEnemy();


protected:
    bool _isDestoried;

    GameWindow* _gameWindow;

    Cell* _posCell;
    int _capacityUsed;
    int _maxCapacity;
    int _curHealth;
    int _maxHealth;
    int _attack;
    double _attackInterval;
    QString _name;
    int _range;
    int _cost;
    QTimer* _picTimer;
    double _picInterval;
    int _curIndex;
    int _maxIndex;
    QList<Enemy*> _enemiesHold;
    QTimer* _attackTimer;

    QVector<Enemy*> _targets;

    bool _isAttackAll;
    bool _canAttackFly;


public:
    QProgressBar* healthBar;
    void DrawHealthLine();

    void mousePressEvent(QMouseEvent *ev) override;

signals:
    void HeroDead(Hero*);
    void HeroPressed(Hero*);
};

#define HeroDefinitionHelper(className)\
class className : public Hero\
{\
public:\
    className(QWidget* parent, GameWindow* gameWindow, Cell* _posCell);\
};
HeroDefinitionHelper(Warrior)
HeroDefinitionHelper(Magician)
HeroDefinitionHelper(Spikeweed)
HeroDefinitionHelper(WallNut)

#endif // HERO_H
