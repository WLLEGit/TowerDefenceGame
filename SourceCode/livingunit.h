#ifndef LIVINGUNIT_H
#define LIVINGUNIT_H

#include "header.h"

class GameWindow;
class LivingUnit : public QLabel
{
    Q_OBJECT
protected:
    GameWindow * gameWindow;
    double _maxHealth;  //生命
    double _curHealth;


    QTimer* _attackTimer;
    double _attack;  //攻击力
    double _attackInterval;   //攻击间隔(s)
    int _range;

    QTimer* _picTimer;
    double _picInterval;
    int _curIndex;
    int _maxIndex;
    int _picHeight;

    QString _name;

    double _speed;


    QProgressBar* _healthBar;

public:
    LivingUnit(QWidget* parent, GameWindow* gameWindow);
    virtual void Update(){}
    virtual inline bool IsAlive(){return _curHealth>0;}
    virtual void BeAttacked(int damage){_curHealth -= damage;}
    void Show(){this->show(); _healthBar->show();}

protected:
    virtual void SwitchPic(){}
    virtual void Attack(){}
    void DrawHealthLine();
    virtual void LoadConfig(QString){}
    virtual void SpecialAbility(){};
};
#endif // LIVINGUNIT_H
