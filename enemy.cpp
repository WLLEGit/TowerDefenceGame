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
}

Enemy *Enemy::GenerateEnemy(int type, QWidget *parent, Cell* bornCell, GameWindow* gameWindow, int pathType)
{
    Enemy* enemy;
    if(type == 0)   //pig
    {
        enemy = new Enemy(parent, 10, 5, 1, 0.8, 1.1, 1, CELLWIDTH/FPS, 1);
        connect(enemy->picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->pathType = pathType;
        enemy->enemyName = "pig";
        enemy->maxIndex = 2;
        enemy->curIndex = 0;
        enemy->dir = gameWindow->FindPath(bornCell->row(), bornCell->col(), bornCell->GetCellTypeID());
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->SwithPic();
        enemy->picTimer->start(400);
    }
    else if(type == 1)  //Monster
    {
        enemy = new Enemy(parent, 20, 3, 2, 1.2, 0.8, 1, CELLWIDTH/2/FPS, 2);
        connect(enemy->picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->pathType = pathType;
        enemy->enemyName = "monster";
        enemy->maxIndex = 2;
        enemy->curIndex = 0;
        enemy->dir = gameWindow->FindPath(bornCell->row(), bornCell->col(), bornCell->GetCellTypeID());
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->SwithPic();
        enemy->picTimer->start(400);
    }
    else if(type == 2)  //Boss
    {
        enemy = new Enemy(parent, 50, 20, 5, 1, 1, 1.8, CELLWIDTH/2/FPS, 1);
        connect(enemy->picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->pathType = pathType;
        enemy->enemyName = "boss";
        enemy->maxIndex = 3;
        enemy->curIndex = 0;
        enemy->dir = gameWindow->FindPath(bornCell->row(), bornCell->col(), bornCell->GetCellTypeID());
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
        this->hide();
        return;
    }

    Cell* posCell = gameWindow->Locate(this);
    Cell* nextCell = gameWindow->FindNextCell(posCell->row(), posCell->col(), posCell->GetCellTypeID());
    dir = gameWindow->FindPath(posCell->row(), posCell->col(), posCell->GetCellTypeID());
    //Update Status
    FriendlyUnit* barrierFU = gameWindow->FindPossibleFriendlyUnit(posCell->row(), posCell->col());
    if(barrierFU)
    {
        status = Fighting;
        barrierFU->AddEnemy(this);
        target = barrierFU;
        if(!attackTimer->isActive())
            attackTimer->start(attackInterval * 1000);
    }
    else
    {
        status = Moving;
        if(attackTimer->isActive())
            attackTimer->stop();

        //Update position
        double tmp = sqrt((nextCell->x() - this->x())*(nextCell->x() - this->x()) + (nextCell->y() - this->y())*(nextCell->y() - this->y()));
        int deltaX=int((nextCell->x() - this->x())/ tmp * speed);
        int deltaY=int((nextCell->y() - this->y()) / tmp * speed);
        this->setGeometry(this->x()+deltaX, this->y()+deltaY, this->width(), this->height());
    }

}

void Enemy::SwithPic()
{
    curIndex = (curIndex + 1) % (maxIndex);
    QPixmap pix(QString(":/assets/monsters/%1%2.png").arg(enemyName).arg(curIndex + (dir == Direction::Right ? maxIndex : 0)));
    pix = pix.scaledToHeight(CELLWIDTH*0.8);
    this->resize(pix.width(), pix.height());
    this->setPixmap(pix);
}

void Enemy::Attack()
{
    assert(target != nullptr);
    target->BeAttacked(attack);
}
