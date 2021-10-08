#include "enemy.h"
#include "gamewindow.h"

Enemy::Enemy(QWidget* parent, double health, double attack, int dealHealthDamage, double rangedDamageRate, \
             double closeDamageRate, double attackInterval, double speed, int attackRange)
    :QLabel(parent), _maxHealth(health), _curHealth(health), _attack(attack), _dealHealthDamage(dealHealthDamage),
      _rangedDamageRate(rangedDamageRate), _closeDamageRate(closeDamageRate), _attackInterval(attackInterval), \
      _attackRange(attackRange), _speed(speed)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);   //对鼠标事件穿透

    _picTimer = new QTimer;
    _attackTimer = new QTimer();

    _healthBar = new QProgressBar(parent);
    _healthBar->setMinimum(0);
    _healthBar->setMaximum(100);
    _healthBar->setStyleSheet("QProgressBar{background:rgba(255,255,255,0);} QProgressBar::chunk{border-radius:5px;background:red}");
    _healthBar->setTextVisible(false);
    _healthBar->setValue(100);
    DrawHealthLine();
}

void Enemy::DrawHealthLine()
{
    _healthBar->setGeometry(x(), y()-CELLWIDTH/10, CELLWIDTH*0.8, CELLWIDTH/10);
    _healthBar->setValue((float)_curHealth/_maxHealth * 100);
    _healthBar->update();
}

Enemy *Enemy::GenerateEnemy(int type, QWidget *parent, Cell* bornCell, GameWindow* gameWindow, Cell::CellType cellType)
{
    Enemy* enemy;
    if(type == 0)   //pig
    {
        enemy = new Enemy(parent, 15, 5, 1, 0.8, 1.1, 1, CELLWIDTH/FPS, 1);
        connect(enemy->_picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->_attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->_pathType = cellType;
        enemy->_enemyName = "pig";
        enemy->_maxIndex = 2;
        enemy->_curIndex = 0;
        enemy->_path = gameWindow->FindPath(bornCell, cellType);
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->_picHeight = CELLWIDTH/2;
        enemy->SwithPic();
        enemy->_picTimer->start(400);
    }
    else if(type == 1)  //Monster
    {
        enemy = new Enemy(parent, 30, 8, 2, 1.2, 0.8, 1, CELLWIDTH/2/FPS, 2);
        connect(enemy->_picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->_attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->_pathType = cellType;
        enemy->_enemyName = "monster";
        enemy->_maxIndex = 2;
        enemy->_curIndex = 0;
        enemy->_path = gameWindow->FindPath(bornCell, cellType);
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->_picHeight = CELLWIDTH * 0.8;
        enemy->SwithPic();
        enemy->_picTimer->start(400);
    }
    else if(type == 2)  //Boss
    {
        enemy = new Enemy(parent, 1000, 20, 5, 1, 1, 1.8, (float)CELLWIDTH/3/FPS, 1);
        connect(enemy->_picTimer, &QTimer::timeout, enemy, &Enemy::SwithPic);
        connect(enemy->_attackTimer, &QTimer::timeout, enemy, &Enemy::Attack);
        enemy->_pathType = cellType;
        enemy->_enemyName = "boss";
        enemy->_maxIndex = 3;
        enemy->_curIndex = 0;
        enemy->_path = gameWindow->FindPath(bornCell, cellType);
        enemy->setGeometry(bornCell->x(), bornCell->y(), 0, 0);
        enemy->_picHeight = CELLWIDTH;
        enemy->SwithPic();
        enemy->_picTimer->start(400);
        enemy->_attackTimer->setInterval(1000);
    }
    else
    {
        return GenerateEnemy(0, parent, bornCell, gameWindow, cellType);
    }
    return enemy;
}

void Enemy::Update(GameWindow *gameWindow)
{
    if(_status == Dead)
        return;
    if(_curHealth <= 0)
    {
        _status = Dead;
        OnDead();
        return;
    }

    Cell* posCell = gameWindow->Locate(this);
    if(posCell->GetCellTypeID() == Cell::End)   //检查是否到达终点
    {
        gameWindow->EnemyHit(_dealHealthDamage);
        _curHealth = 0;
        _status = Dead;
        OnDead();
        return;
    }
    if(posCell != (*_path)[_posIndex])    //检查是否到达下一格
    {
        posCell = (*_path)[_posIndex + 1];
        ++_posIndex;
    }
    Cell* nextCell = (*_path)[_posIndex + 1];

    if(_status != Fighting || !_target || !_target->IsAlive())   //如果在战斗中则不寻找新的状态
    {
        Hero* barrierFU = gameWindow->FindPossibleFriendlyUnit(posCell->row(), posCell->col());
        if(barrierFU)
        {
            _status = Fighting;
            barrierFU->AddEnemy(this);
            _target = barrierFU;
            Attack();
            if(!_attackTimer->isActive())
                _attackTimer->start(_attackInterval * 1000);
        }
        else
        {
            _status = Moving;
            if(_attackTimer->isActive())
                _attackTimer->stop();

            //Update position
            double dx = (nextCell->x() + nextCell->width() / 2 - this->x() - this->width() / 2);
            double dy = (nextCell->y() + nextCell->height() / 2 - this->y() - this->height() / 2);
            double dis = DISTANCE(dx, dy);
            int deltaX=round(dx / dis * _speed);
            int deltaY=round(dy / dis * _speed);
            this->setGeometry(this->x()+deltaX, this->y()+deltaY, this->width(), this->height());
        }
    }

    //Update Status
    DrawHealthLine();
}

void Enemy::OnDead()
{
    this->hide();
    _healthBar->hide();
    _attackTimer->stop();
    if(_target && _target->IsAlive())
        _target->RemoveEnemy(this);
}


void Enemy::SwithPic()
{
    _curIndex = (_curIndex + 1) % (_maxIndex);
    QPixmap pix(QString(":/assets/monsters/%1%2.png").arg(_enemyName).arg(_curIndex + ((*_path)[_posIndex+1]->col()-(*_path)[_posIndex]->col() > 0 ? _maxIndex : 0)));
    pix = pix.scaledToHeight(_picHeight);
    this->resize(pix.width(), pix.height());
    this->setPixmap(pix);
}

void Enemy::Attack()
{
    if(_target && _target->IsAlive())
    {
        _target->BeAttacked(_attack);
    }
    else
    {
        _target = nullptr;
        _attackTimer->stop();
    }
}
