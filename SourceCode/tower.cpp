#include "tower.h"
#include "gamewindow.h"


bool Tower::InRange(int x, int y)
{
    return DISTANCE(x-this->x(), y-this->y()) <= CELLWIDTH * (_range+0.2);
}

void Tower::LoadConfig(QString towerName)
{
    QJsonObject towerConfig = globalConfig["Towers"].toObject()[towerName].toObject();
    _cost = towerConfig["cost"].toInt();
    _level = 1;
    _maxLevel = towerConfig["maxLevel"].toInt();
    _range = towerConfig["range"].toDouble();
    _attack = towerConfig["attack"].toInt();
    _attackInterval = towerConfig["attackInterval"].toDouble();
    _towerPicPrefix = towerConfig["name"].toString();
    _bulletPicPath = towerConfig["bulletPicPath"].toString();
    _bulletSpeed = towerConfig["bulletSpeed"].toDouble();
    _bulletLength = towerConfig["bulletLength"].toDouble();
    _canAttackFly = towerConfig["canAttackFly"].toBool();
    _canPlaceGuarder = towerConfig["canPlaceGuarder"].toBool();
}

void Tower::Attack(GameWindow *gameWindow)
{
    if(_target && _target->IsAlive())
    {
        gameWindow->CreateBullet(_target, this, _bulletSpeed, _attack, QPixmap(_bulletPicPath).scaledToHeight(_bulletLength));
    }
    else
    {
        _target = nullptr;
        _attackTimer.stop();
    }
}


Tower::Tower(QWidget *parent, int cost, int level, int range, double attack, double attackInterval, Cell* locationCell)
    :QLabel(parent), _cost(cost), _level(level), _range(range), _attack(attack), _attackInterval(attackInterval), _posCell(locationCell)
{
    connect(&_attackTimer, &QTimer::timeout, this, [=](){_toAttack=true;});
    setGeometry(locationCell->x(), locationCell->y(), CELLWIDTH*1.2, CELLWIDTH*1.2);
    _toAttack = false;
}

void Tower::Update(GameWindow* gameWindow)
{
    if(!_target || !_target->IsAlive() || !InRange(_target->x(), _target->y()))
    {
        _target = nullptr;
        for(auto& enemy : gameWindow->_enemies)
            if(InRange(enemy->x(), enemy->y()) && enemy->IsAlive())
            {
                _target = enemy;
                break;
            }
    }
    if(_target)
    {
        if(_toAttack)
        {
            Attack(gameWindow);
            _toAttack = false;
        }
        if(!_attackTimer.isActive())
        {
            Attack(gameWindow);
            _attackTimer.start(_attackInterval * 1000);
        }

        setPixmap(RotatePixmap(_towerpic, _target, this));

    }
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


ArrowTower::ArrowTower(QWidget *parent, Cell* locationCell) //dps=4
    :    Tower(parent, ARROWCOST, 1, 2, 1, 0.2, locationCell)
{
    _bulletSpeed = CELLWIDTH;
    _bulletLength = CELLWIDTH / 3;
    _towerPicPrefix = ":/assets/towers/MG%1.png";
    _towerpic = QPixmap(_towerPicPrefix.arg(_level));
    _towerpic = _towerpic.scaledToHeight(CELLWIDTH);
    _bulletPicPath = ":/assets/towers/Bullet_MG.png";
    setPixmap(_towerpic);
}

MissleTower::MissleTower(QWidget *parent, Cell* locationCell)   //dps=6
    :    Tower(parent, MISSILECOST, 1, 5, 6, 2, locationCell)
{
    _bulletSpeed = CELLWIDTH * 0.7;
    _bulletLength = CELLWIDTH * 0.7;
    _towerPicPrefix = ":/assets/towers/Missile_Launcher%1.png";
    _towerpic = QPixmap(_towerPicPrefix.arg(_level));
    _towerpic = _towerpic.scaledToHeight(CELLWIDTH);
    _bulletPicPath = ":/assets/towers/Missile.png";
    setPixmap(_towerpic);
    show();
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
