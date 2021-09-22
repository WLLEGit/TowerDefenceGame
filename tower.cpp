#include "tower.h"
#include "gamewindow.h"


bool Tower::InRange(int x, int y)
{
    return DISTANCE(x-this->x(), y-this->y()) <= CELLWIDTH * (range+0.2);
}

void Tower::Attack(GameWindow *gameWindow)
{
    if(target && target->IsAlive())
    {
        gameWindow->CreateBullet(target, this, bulletSpeed, attack, QPixmap(bulletPicPath).scaledToHeight(bulletLength));
    }
    else
    {
        target = nullptr;
        attackTimer.stop();
    }
}


Tower::Tower(QWidget *parent, int cost, int level, int range, double attack, double attackInterval, Cell* locationCell)
    :QLabel(parent), cost(cost), level(level), range(range), attack(attack), attackInterval(attackInterval), posCell(locationCell)
{
    connect(&attackTimer, &QTimer::timeout, this, [=](){toAttack=true;});
    setGeometry(locationCell->x(), locationCell->y(), CELLWIDTH*1.2, CELLWIDTH*1.2);
    toAttack = false;
}

void Tower::Update(GameWindow* gameWindow)
{
    if(!target || !target->IsAlive() || !InRange(target->x(), target->y()))
    {
        target = nullptr;
        for(auto& enemy : gameWindow->enemies)
            if(InRange(enemy->x(), enemy->y()) && enemy->IsAlive())
            {
                target = enemy;
                break;
            }
    }
    if(target)
    {
        if(toAttack)
        {
            Attack(gameWindow);
            toAttack = false;
        }
        if(!attackTimer.isActive())
        {
            Attack(gameWindow);
            attackTimer.start(attackInterval * 1000);
        }

        setPixmap(RotatePixmap(towerpic, target, this));

    }
}

void Tower::Upgrade()
{
    level++;
    attack *= 2;
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    setPixmap(towerpic);
}

void Tower::mousePressEvent(QMouseEvent *ev)
{
    emit TowerPressed(this);
    QLabel::mousePressEvent(ev);
}


ArrowTower::ArrowTower(QWidget *parent, Cell* locationCell) //dps=4
    :    Tower(parent, ARROWCOST, 1, 2, 1, 0.2, locationCell)
{
    bulletSpeed = CELLWIDTH;
    bulletLength = CELLWIDTH / 3;
    towerPicPrefix = ":/assets/towers/MG%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPath = ":/assets/towers/Bullet_MG.png";
    setPixmap(towerpic);
}

MissleTower::MissleTower(QWidget *parent, Cell* locationCell)   //dps=6
    :    Tower(parent, MISSILECOST, 1, 5, 6, 2, locationCell)
{
    bulletSpeed = CELLWIDTH * 0.7;
    bulletLength = CELLWIDTH * 0.7;
    towerPicPrefix = ":/assets/towers/Missile_Launcher%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPath = ":/assets/towers/Missile.png";
    setPixmap(towerpic);
    show();
}


Bullet::Bullet(QWidget *parent, Enemy *target, Tower *sender, int speed, int damage, QPixmap pic)
    :QLabel(parent), target(target), sender(sender), speed(speed), damage(damage), pic(pic)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);   //对鼠标事件穿透

    isHit = false;
    this->setGeometry(sender->x(), sender->y(), CELLWIDTH*1.5, CELLWIDTH*1.5);
}

void Bullet::Update(GameWindow *)
{
    if(isHit || !target)
        return;

    double deltaX= target->x()-x();
    double deltaY= target->y()- y();
    double dis = DISTANCE(deltaX, deltaY);
    if(!target->IsAlive() || dis <= speed)
    {
        isHit = true;
        emit HitEnemy(this);
        return;
    }
    setPixmap(RotatePixmap(pic, target, this));
    setGeometry(x() + deltaX / dis * speed, y() + deltaY / dis * speed, this->width(), this->height());

}
