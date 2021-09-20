#include "tower.h"
#include "gamewindow.h"


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
    setGeometry(locationCell->x(), locationCell->y(), CELLWIDTH*1.2, CELLWIDTH*1.2);
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
        double dx=target->x()-this->x(), dy=this->y()-target->y();
        double dis = DISTANCE(dx, dy);
        double Sin= dx/dis, Cos=dy/dis;
        QMatrix mat(Cos, Sin, -Sin, Cos, 0, 0);
        setPixmap(towerpic.transformed(mat));
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


ArrowTower::ArrowTower(QWidget *parent, Cell* locationCell) //dps=4
    :    Tower(parent, 100, 1, 2, 2, 0.5, locationCell)
{
    towerPicPrefix = ":/assets/towers/MG%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPrefix = ":/assets/towers/Bullet_MG.png";
    setPixmap(towerpic);
}

MissleTower::MissleTower(QWidget *parent, Cell* locationCell)   //dps=6
    :    Tower(parent, 150, 1, 5, 6, 1, locationCell)
{
    towerPicPrefix = ":/assets/towers/Missile_Launcher%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPrefix = ":/assets/towers/Missile.png";
    setPixmap(towerpic);
    show();
}

FriendlyUnit::FriendlyUnit(QWidget *parent, Cell* posCell, int capacity, int maxCapacity, int attack, \
                           int curHealth, int maxHealth, double attackInterval, QString name, int range, int cost)
    :QLabel(parent), posCell(posCell), capacity(capacity), maxCapacity(maxCapacity), curHealth(curHealth),\
      maxHealth(maxHealth), attack(attack), attackInterval(attackInterval), name(name), range(range), cost(cost)
{
    setGeometry(posCell->x(), posCell->y(), CELLWIDTH, CELLWIDTH);
    curIndex = 0;
    maxIndex = 4;
    picTimer = new QTimer();
    picTimer->start(400);
    connect(picTimer, &QTimer::timeout, this, [=](){
        curIndex = (curIndex + 1) % (maxIndex);
        QPixmap pix(QString(":/assets/heros/%1_r%2.png").arg(name).arg(curIndex));
        pix = pix.scaledToHeight(CELLWIDTH*0.8);
        this->resize(pix.width(), pix.height());
        this->setPixmap(pix);
    });
}

void FriendlyUnit::AddEnemy(Enemy *enemy)
{

}

FriendlyUnit *FriendlyUnit::GenerateHero(QWidget *parent, Cell *posCell, int type)
{
    if(posCell->GetCellType() != Cell::Path)
        return nullptr;

    if(type == 1)
        return new Hero1(parent, posCell);
    else if(type == 2)
        return new Hero2(parent, posCell);
}

Hero1::Hero1(QWidget *parent, Cell *posCell)
    :FriendlyUnit(parent, posCell, 0, 3, 50, 50, 10, 0.8, "one", 1, 200)
{
}

Hero2::Hero2(QWidget *parent, Cell *posCell)
    :FriendlyUnit(parent, posCell, 0, 2, 20, 20, 25, 0.8, "two", 1, 200)
{
}
