#ifndef HERO_H
#define HERO_H

#include "header.h"
#include "enemy.h"
#include "cell.h"

class Hero : public LivingUnit  //友方单位
{
    Q_OBJECT

public:
    Hero(QWidget* parent=nullptr, GameWindow* gameWindow=nullptr, Cell* _posCell=nullptr);
    Cell* GetPositionCell(){return _posCell;}

    bool CanHoldEnemy(){return IsAlive() && _capacityRemain > 0;}
    void AddEnemy(Enemy* enemy);
    bool RemoveEnemy(Enemy* enemy);

    bool IsAlive() override{return !_isDestoried && _curHealth > 0;}
    inline float GetHealthRate(){return (float)_curHealth / _maxHealth;}

    void Update() override;

    static Hero* GenerateHero(QWidget* parent, GameWindow* gameWindow, Cell* _posCell, int type);

    int Cost(){return _cost;}

    void Show(){show(); _healthBar->show();}

protected:
    void SwitchPic() override;
    void LoadConfig(QString name) override;
    void Attack() override;
    bool InRange(int x, int y);
    void FindEnemy();


protected:
    bool _isDestoried;

    GameWindow* _gameWindow;

    Cell* _posCell;
    int _capacityRemain;
    int _maxCapacity;
    int _cost;

    QList<Enemy*> _enemiesHold;

    QVector<Enemy*> _targets;

    bool _isAttackAll;
    bool _canAttackFly;


public:
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
