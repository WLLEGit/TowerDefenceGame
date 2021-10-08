#include "hero.h"

#include "gamewindow.h"
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
    SwitchPic();
    picTimer->start(400);
    connect(picTimer, &QTimer::timeout, this, &Hero::SwitchPic);

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

bool Hero::RemoveEnemy(Enemy *enemy)
{
    //assert(capacity < maxCapacity);
    capacity++;
    return targets.removeOne(enemy);
}

void Hero::Attack()
{
    if(isDestoried)
        return;
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
    return DISTANCE(x-this->x(), y-this->y()) <= CELLWIDTH * (double)(range - 0.3);
}

void Hero::DrawHealthLine()
{
    healthBar->setGeometry(x(), y()-CELLWIDTH/10, CELLWIDTH*0.8, CELLWIDTH/10);
    healthBar->setValue((float)curHealth/maxHealth * 100);
    healthBar->update();
}

void Hero::mousePressEvent(QMouseEvent *ev)
{
    emit HeroPressed(this);
    QLabel::mousePressEvent(ev);
}

void Hero::Update(GameWindow* gameWindow)
{
    if(isDestoried)
    {
        attackTimer->stop();
        picTimer->stop();
        return;
    }

    if(curHealth <= 0)
    {
        this->hide();
        healthBar->hide();
        isDestoried = true;
        attackTimer->stop();
        picTimer->stop();
        emit HeroDead(this);
    }
    if(!target || !target->IsAlive() || !InRange(target->x(), target->y()))
    {
        target = nullptr;
        for(auto& enemy : gameWindow->_enemies)
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

void Hero::SwitchPic()
{
    curIndex = (curIndex + 1) % (maxIndex);
    QPixmap pix(QString(":/assets/heros/%1_r%2.png").arg(name).arg(curIndex));
    pix = pix.scaledToHeight(CELLWIDTH*0.8);
    this->resize(pix.width(), pix.height());
    this->setPixmap(pix);
}

Hero1::Hero1(QWidget *parent, Cell *posCell)
    :Hero(parent, posCell, 3, 5, 50, 0.8, "one", 1, HERO1COST)
{
}

Hero2::Hero2(QWidget *parent, Cell *posCell)
    :Hero(parent, posCell, 2, 10, 25, 0.8, "two", 3, HERO2COST)
{
}
