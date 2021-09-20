#include "tower.h"
#include "gamewindow.h"

void FriendlyUnit::AddEnemy(Enemy *enemy)
{

}


bool Tower::InRange(int x, int y)
{
    return DISTANCE(x-this->x(), y-this->y()) <= CELLWIDTH * (range+0.2);
}

void Tower::Attack()
{

}

Tower::Tower(QWidget *parent, int cost, int level, int range, double attack, double attackInterval, Cell* locationCell)
    :QLabel(parent), cost(cost), level(level), range(range), attack(attack), attackInterval(attackInterval), locationCell(locationCell)
{
    connect(&attackTimer, &QTimer::timeout, this, &Tower::Attack);
    setGeometry(locationCell->x(), locationCell->y(), 0, 0);
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
        if(!attackTimer.isActive())
        {
            Attack();
            attackTimer.start(attackInterval * 1000);
        }
        double dx=target->x()-this->x(), dy=target->y()-this->y();
        double dis = DISTANCE(dx, dy);
        double Sin= dx/dis, Cos=dy/dis;
        QMatrix mat(Cos, -Sin, Sin, Cos, 0, 0);
        setPixmap(towerpic.transformed(mat));
    }
}


ArrowTower::ArrowTower(QWidget *parent, Cell* locationCell) //dps=4
    :    Tower(parent, 100, 1, 2, 2, 0.5, locationCell)
{
    towerPicPrefix = ":/assets/towers/MG%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPrefix = ":/assets/towers/Bullet_MG.png";
}

MissleTower::MissleTower(QWidget *parent, Cell* locationCell)   //dps=6
    :    Tower(parent, 150, 1, 5, 6, 1, locationCell)
{
    towerPicPrefix = ":/assets/towers/Missile_Launcher%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPrefix = ":/assets/towers/Missile.png";
}
