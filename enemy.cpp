#include "enemy.h"
#include "gamewindow.h"

Enemy::Enemy(QWidget* parent, double health, double attack, int dealHealthDamage, double rangedDamageRate, \
             double closeDamageRate, double attackInterval, double speed, int attackRange)
    :QLabel(parent), maxHealth(health), curHealth(health), attack(attack), dealHealthDamage(dealHealthDamage),
      rangedDamageRate(rangedDamageRate), closeDamageRate(closeDamageRate), attackInterval(attackInterval), \
      attackRange(attackRange), speed(speed)
{
    picTimer = new QTimer;
    attackTimer = new QTimer();

    healthBar = new QProgressBar(parent);
    healthBar->setMinimum(0);
    healthBar->setMaximum(100);
    healthBar->setStyleSheet("QProgressBar{background:rgba(255,255,255,0);} QProgressBar::chunk{border-radius:5px;background:red}");
    healthBar->setTextVisible(false);
    healthBar->setValue(100);
    DrawHealthLine();
}

void Enemy::DrawHealthLine()
{
    healthBar->setGeometry(x(), y()-CELLWIDTH/10, CELLWIDTH*0.8, CELLWIDTH/10);
    healthBar->setValue((float)curHealth/maxHealth * 100);
    healthBar->update();
}

Enemy *Enemy::GenerateEnemy(int type, QWidget *parent, Cell* bornCell, GameWindow* gameWindow, int pathType)
{
    Enemy* enemy;
    if(type == 0)   //pig
    {
        enemy = new Enemy(parent, 15, 5, 1, 0.8, 1.1, 1, CELLWIDTH/FPS, 1);
        connect(enemy->picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->pathType = pathType;
        enemy->enemyName = "pig";
        enemy->maxIndex = 2;
        enemy->curIndex = 0;
        enemy->path = gameWindow->FindPath(bornCell);
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->SwithPic();
        enemy->picTimer->start(400);
    }
    else if(type == 1)  //Monster
    {
        enemy = new Enemy(parent, 30, 8, 2, 1.2, 0.8, 1, CELLWIDTH/2/FPS, 2);
        connect(enemy->picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->pathType = pathType;
        enemy->enemyName = "monster";
        enemy->maxIndex = 2;
        enemy->curIndex = 0;
        enemy->path = gameWindow->FindPath(bornCell);
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->SwithPic();
        enemy->picTimer->start(400);
    }
    else if(type == 2)  //Boss
    {
        enemy = new Enemy(parent, 1000, 20, 5, 1, 1, 1.8, (float)CELLWIDTH/3/FPS, 1);
        connect(enemy->picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->pathType = pathType;
        enemy->enemyName = "boss";
        enemy->maxIndex = 3;
        enemy->curIndex = 0;
        enemy->path = gameWindow->FindPath(bornCell);
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->SwithPic();
        enemy->picTimer->start(400);
        enemy->attackTimer->setInterval(1000);
    }
    else
    {
        return GenerateEnemy(0, parent, bornCell, gameWindow, pathType);
    }
    return enemy;
}

void Enemy::Update(GameWindow *gameWindow)
{
    if(curHealth <= 0)
    {
        status = Dead;
        OnDead();
        return;
    }

    Cell* posCell = gameWindow->Locate(this);
    if(posCell->GetCellTypeID() == Cell::End)   //检查是否到达终点
    {
        gameWindow->EnemyHit(dealHealthDamage);
        curHealth = 0;
        status = Dead;
        OnDead();
        return;
    }
    if(posCell != (*path)[posIndex])    //检查是否到达下一格
    {
        posCell = (*path)[posIndex + 1];
        ++posIndex;
    }
    Cell* nextCell = (*path)[posIndex + 1];

    //Update Status
    Hero* barrierFU = gameWindow->FindPossibleFriendlyUnit(posCell->row(), posCell->col());
    if(barrierFU)
    {
        if(target != barrierFU)
        {
            status = Fighting;
            barrierFU->AddEnemy(this);
            target = barrierFU;
            Attack();
            if(!attackTimer->isActive())
                attackTimer->start(attackInterval * 1000);
        }
    }
    else
    {
        status = Moving;
        if(attackTimer->isActive())
            attackTimer->stop();

        //Update position
        double tmp = sqrt((nextCell->x() - this->x())*(nextCell->x() - this->x()) + (nextCell->y() - this->y())*(nextCell->y() - this->y()));
        int deltaX=round((nextCell->x() - this->x())/ tmp * speed);
        int deltaY=round((nextCell->y() - this->y()) / tmp * speed);
        this->setGeometry(this->x()+deltaX, this->y()+deltaY, this->width(), this->height());
    }

    DrawHealthLine();
}


void Enemy::SwithPic()
{
    curIndex = (curIndex + 1) % (maxIndex);
    QPixmap pix(QString(":/assets/monsters/%1%2.png").arg(enemyName).arg(curIndex + ((*path)[posIndex+1]->col()-(*path)[posIndex]->col() > 0 ? maxIndex : 0)));
    pix = pix.scaledToHeight(CELLWIDTH*0.8);
    this->resize(pix.width(), pix.height());
    this->setPixmap(pix);
}

void Enemy::Attack()
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
