#include "tower.h"
#include "gamewindow.h"


bool Tower::InRange(int x, int y)
{
    return DISTANCE(x-(this->x() + CELLWIDTH/2), y-(this->y()+CELLWIDTH/2)) <= CELLWIDTH * (range+0.2);
}

void Tower::Attack()
{
    if(target && target->IsAlive())
    {

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

void Tower::mousePressEvent(QMouseEvent *ev)
{
    emit TowerPressed(this);
    QLabel::mousePressEvent(ev);
}


ArrowTower::ArrowTower(QWidget *parent, Cell* locationCell) //dps=4
    :    Tower(parent, ARROWCOST, 1, 2, 2, 0.5, locationCell)
{
    towerPicPrefix = ":/assets/towers/MG%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPrefix = ":/assets/towers/Bullet_MG.png";
    setPixmap(towerpic);
}

MissleTower::MissleTower(QWidget *parent, Cell* locationCell)   //dps=6
    :    Tower(parent, MISSILECOST, 1, 5, 6, 1, locationCell)
{
    towerPicPrefix = ":/assets/towers/Missile_Launcher%1.png";
    towerpic = QPixmap(towerPicPrefix.arg(level));
    towerpic = towerpic.scaledToHeight(CELLWIDTH);
    bulletPicPrefix = ":/assets/towers/Missile.png";
    setPixmap(towerpic);
    show();
}

Hero::Hero(QWidget *parent, Cell* posCell, int maxCapacity, int attack, \
           int maxHealth, double attackInterval, QString name, int range, int cost)
    :QLabel(parent), posCell(posCell), capacity(maxCapacity), maxCapacity(maxCapacity), curHealth(maxHealth),\
      maxHealth(maxHealth), attack(attack), attackInterval(attackInterval), name(name), range(range), cost(cost)
{
    setGeometry(posCell->x(), posCell->y(), CELLWIDTH, CELLWIDTH);

    target = nullptr;
    isDestoried = false;

    curIndex = 0;
    maxIndex = 4;
    picTimer = new QTimer(this);
    picTimer->start(400);
    connect(picTimer, &QTimer::timeout, this, [=](){
        curIndex = (curIndex + 1) % (maxIndex);
        QPixmap pix(QString(":/assets/heros/%1_r%2.png").arg(name).arg(curIndex));
        pix = pix.scaledToHeight(CELLWIDTH*0.8);
        this->resize(pix.width(), pix.height());
        this->setPixmap(pix);
    });

    attackTimer = new QTimer(this);
    connect(attackTimer, &QTimer::timeout, this, &Hero::Attack);

    healthBar = new QProgressBar(parent);
    healthBar->setMinimum(0);
    healthBar->setMaximum(100);
    healthBar->setStyleSheet("QProgressBar{background:rgba(255,255,255,0);} QProgressBar::chunk{border-radius:5px;background:red}");
    healthBar->setTextVisible(false);
}

void Hero::AddEnemy(Enemy *enemy)
{
    assert(capacity);
    capacity--;
    targets.push_back(enemy);
}

void Hero::RemoveEnemy(Enemy *enemy)
{
    assert(capacity < maxCapacity);
    capacity++;
    targets.removeOne(enemy);
}

void Hero::Attack()
{
    if(target && target->IsAlive())
    {
        target->BeAttacked(attack);
    }
    else
    {
        target = nullptr;
        attackTimer->stop();
    }
}

bool Hero::InRange(int x, int y)
{
    return DISTANCE(x-(this->x()+CELLWIDTH/2), y-(this->y()+CELLWIDTH/2)) <= CELLWIDTH * (double)(range - 0.5);
}

void Hero::DrawHealthLine()
{
    healthBar->setGeometry(x(), y()-CELLWIDTH/10, CELLWIDTH*0.8, CELLWIDTH/10);
    healthBar->setValue((float)curHealth/maxHealth * 100);
    healthBar->update();
}

void Hero::Update(GameWindow* gameWindow)
{
    if(isDestoried)
        return;

    if(curHealth <= 0)
    {
        this->hide();
        healthBar->hide();
        isDestoried = true;
        emit HeroDead(nullptr);
    }
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
        if(!attackTimer->isActive())
            attackTimer->start(attackInterval * 1000);
    }
    DrawHealthLine();
}

Hero *Hero::GenerateHero(QWidget *parent, Cell *posCell, int type)
{
    if(posCell->GetCellType() != Cell::Path)
        return nullptr;

    if(type == 1)
        return new Hero1(parent, posCell);
    else if(type == 2)
        return new Hero2(parent, posCell);
    else
        return new Hero1(parent, posCell);
}

Hero1::Hero1(QWidget *parent, Cell *posCell)
    :Hero(parent, posCell, 3, 5, 50, 0.8, "one", 2, HERO1COST)
{
}

Hero2::Hero2(QWidget *parent, Cell *posCell)
    :Hero(parent, posCell, 2, 10, 25, 0.8, "two", 1, HERO2COST)
{
}
