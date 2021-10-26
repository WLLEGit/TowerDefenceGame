#include "tower.h"
#include "gamewindow.h"


bool Tower::InRange(int x, int y)
{
    return DISTANCE(x-this->x(), y-this->y()) <= CELLWIDTH * (_range+0.2);
}

void Tower::LoadConfig(QString towerName)
{
    QJsonObject towerConfig = globalConfig["Towers"].toObject()[towerName].toObject();
    _maxHealth = towerConfig["maxHealth"].toInt();
    _curHealth = _maxHealth;
    _cost = towerConfig["cost"].toInt();
    _level = 1;
    _maxLevel = towerConfig["maxLevel"].toInt();
    _range = towerConfig["range"].toDouble();
    _attack = towerConfig["attack"].toInt();
    _attackInterval = towerConfig["attackInterval"].toDouble();
    _towerPicPrefix = QString(":/assets/towers/%1%2.png").arg(towerConfig["name"].toString());
    _bulletPicPath = towerConfig["bulletPicPath"].toString();
    _bulletSpeed = towerConfig["bulletSpeed"].toDouble() * CELLWIDTH;
    _bulletLength = towerConfig["bulletLength"].toDouble()*CELLWIDTH;
    _canAttackFly = towerConfig["canAttackFly"].toBool();
}

void Tower::Attack()
{
    if(_attack)
    {
        if(_target && _target->IsAlive())
        {
            gameWindow->CreateBullet(_target, this, _bulletSpeed, _attack, QPixmap(_bulletPicPath).scaledToHeight(_bulletLength));
        }
        else
        {
            _target = nullptr;
            _attackTimer->stop();
        }
    }
    else
        _attackTimer->stop();
}


Tower::Tower(QWidget* parent, GameWindow* gameWindow, Cell* _posCell)
    :LivingUnit(parent, gameWindow), gameWindow(gameWindow), _posCell(_posCell)
{
    setGeometry(_posCell->x(), _posCell->y(), CELLWIDTH*1.2, CELLWIDTH*1.2);
}

void Tower::Update()
{
    if(_attack)
    {
        if(!_target || !_target->IsAlive() || !InRange(_target->x(), _target->y()))
        {
            _target = gameWindow->FindOneEnemyInRange(x(), y(), _range*CELLWIDTH);
        }
        if(_target)
        {
            setPixmap(RotatePixmap(_towerpic, _target, this));
            if(!_attackTimer->isActive())
                _attackTimer->start(_attackInterval*1000);
        }
    }
    else
        SpecialAbility();
}

void Tower::Upgrade()
{
    _level++;
    _attack *= 2;
    _towerpic = QPixmap(_towerPicPrefix.arg(_level));
    _towerpic = _towerpic.scaledToHeight(CELLWIDTH);
    setPixmap(_towerpic);
}

void Tower::mousePressEvent(QMouseEvent *ev)
{
    emit TowerPressed(this);
    QLabel::mousePressEvent(ev);
}

Tower *Tower::GenerateTower(QWidget *parent, GameWindow *gameWindow, Cell *posCell, int type)
{
    if(type == 1)
        return new ArrowTower(parent, gameWindow, posCell);
    else if(type == 2)
        return new MissileTower(parent, gameWindow, posCell);
    else if(type == 3)
        return new GuardTower(parent, gameWindow, posCell);
    return nullptr;
}

#define TowerConstructorHelper(className)\
className::className(QWidget *parent, GameWindow* gameWindow, Cell* locationCell) \
    :Tower(parent, gameWindow, locationCell)\
{\
    LoadConfig(#className);\
    _towerpic = QPixmap(_towerPicPrefix.arg(_level));\
    _towerpic = _towerpic.scaledToHeight(CELLWIDTH);\
    setPixmap(_towerpic);\
}

TowerConstructorHelper(ArrowTower)
TowerConstructorHelper(MissileTower)
TowerConstructorHelper(GuardTower)

void ArrowTower::SpecialAbility(){}
void MissileTower::SpecialAbility(){}
void GuardTower::SpecialAbility()
{
    //放置防卫英雄
    if(!_heroGened && !_attackTimer->isActive())
    {
        Cell* pos = gameWindow->FindOnePathCellInRange(x(), y(), _range*CELLWIDTH);
        if(pos)
            _heroGened = gameWindow->CreateHero(4, pos);
    }
    else if(_heroGened && !_heroGened->IsAlive())
    {
        _heroGened = nullptr;
        _attackTimer->start(_attackInterval*1000);
    }
}



Bullet::Bullet(QWidget *parent, Enemy *target, Tower *sender, int speed, int damage, QPixmap pic)
    :QLabel(parent), _target(target), _sender(sender), _speed(speed), _damage(damage), _pic(pic)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);   //对鼠标事件穿透

    _isHit = false;
    this->setGeometry(sender->x(), sender->y(), CELLWIDTH*1.5, CELLWIDTH*1.5);
}

void Bullet::Update(GameWindow *)
{
    if(_isHit || !_target)
        return;

    double deltaX= _target->x()-x();
    double deltaY= _target->y()- y();
    double dis = DISTANCE(deltaX, deltaY);
    if(!_target->IsAlive() || dis <= _speed)
    {
        _isHit = true;
        emit HitEnemy(this);
        return;
    }
    setPixmap(RotatePixmap(_pic, _target, this));
    setGeometry(x() + deltaX / dis * _speed, y() + deltaY / dis * _speed, this->width(), this->height());
}
