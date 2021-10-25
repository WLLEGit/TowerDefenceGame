#include "enemy.h"
#include "gamewindow.h"

Enemy::Enemy(QWidget* parent)
    :QLabel(parent)
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

    connect(this->_picTimer, &QTimer::timeout, this, &Enemy::SwithPic);
    connect(this->_attackTimer, &QTimer::timeout, this, &Enemy::Attack);
}

void Enemy::DrawHealthLine()
{
    _healthBar->setGeometry(x(), y()-CELLWIDTH/10, CELLWIDTH*0.8, CELLWIDTH/10);
    _healthBar->setValue((float)_curHealth/_maxHealth * 100);
    _healthBar->update();
}

void Enemy::LoadConfig(QString enemyName)
{
    QJsonObject enemyConfig = globalConfig["Enemies"].toObject()[enemyName].toObject();
    _maxHealth = enemyConfig["maxHealth"].toInt();
    _curHealth = _maxHealth;
    _attack = enemyConfig["attack"].toInt();
    _dealHealthDamage = enemyConfig["dealHealthDamage"].toInt();
    _attackInterval = enemyConfig["attackInterval"].toDouble();
    _attackRange = enemyConfig["attackRangeRate"].toDouble()*CELLWIDTH;
    _speed = enemyConfig["speedRate"].toDouble()*CELLWIDTH/FPS;
    _enemyName = enemyConfig["enemyName"].toString();
    _maxIndex = enemyConfig["maxIndex"].toInt();
    _curIndex = 0;
    _picHeight = CELLWIDTH*enemyConfig["picHeightRate"].toDouble();
    _isMilitant = enemyConfig["isMilitant"].toBool();
    _isFlying = enemyConfig["isFlying"].toBool();
}

#define CaseHelper(type, className) \
    case type:\
        enemy = new className(parent);\
        enemy->InitBornLocation(gameWindow, bornCell, cellType);\
        break;
Enemy *Enemy::GenerateEnemy(int type, QWidget *parent, Cell* bornCell, GameWindow* gameWindow, Cell::CellType cellType)
{
    Enemy* enemy;
    switch (type) {
    CaseHelper(0, EnemyPig)
    CaseHelper(1, EnemyMonster)
    CaseHelper(2, EnemyHealer)
    CaseHelper(3, EnemyAntiTower)
    CaseHelper(4, EnemyFly1)
    CaseHelper(5, EnemyFly2)
    default:
        enemy = nullptr;
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

    if(!_isMilitant)
        _status = Moving;   //畏战敌人无攻击行为
    else if(_status != Fighting || !_target || !_target->IsAlive())   //如果在战斗中则不寻找新的状态
    {
        Hero* barrierFU = gameWindow->FindOneHeroInRange(this->x(), this->y(), _attackRange);
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
            double dy = (nextCell->y() + nextCell->height() / 2 - this->y() - this->height() * 0.7);
            double dis = DISTANCE(dx, dy);

            int deltaX=round(dx / dis * _speed);
            int deltaY=round(dy / dis * _speed);
            if(!deltaX && abs(dx) > CELLWIDTH * 0.1)
                deltaX = SIGN(dx);
            if(!deltaY && abs(dy) > CELLWIDTH * 0.1)
                deltaY = SIGN(dy);

            this->setGeometry(this->x()+deltaX, this->y()+deltaY, this->width(), this->height());
        }
    }
    //调用特殊能力
    SpecialAbility(gameWindow);
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

void Enemy::InitBornLocation(GameWindow *gameWindow, Cell *bornCell, Cell::CellType cellType)
{
    _pathType = cellType;
    _path = gameWindow->FindPath(bornCell, cellType);
    setGeometry(bornCell->x(), bornCell->y(), 0, 0);
    SwithPic();
    _picTimer->start(400);
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
    if(_attack == 0)
        return;
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

#define ConstructorCodeHelper(NAME)\
NAME::NAME(QWidget* parent)\
    :Enemy(parent)\
{\
    InitConfig(QString(#NAME));\
}
ConstructorCodeHelper(EnemyFly1)
ConstructorCodeHelper(EnemyFly2)
ConstructorCodeHelper(EnemyPig)
ConstructorCodeHelper(EnemyMonster)
ConstructorCodeHelper(EnemyAntiTower)
ConstructorCodeHelper(EnemyHealer)

#define SpecialAbilityCodeHelper(NAME) \
    void NAME::SpecialAbility(GameWindow*){}
SpecialAbilityCodeHelper(EnemyFly1)
SpecialAbilityCodeHelper(EnemyFly2)
SpecialAbilityCodeHelper(EnemyPig)
SpecialAbilityCodeHelper(EnemyMonster)
SpecialAbilityCodeHelper(EnemyAntiTower)

void EnemyHealer::SpecialAbility(GameWindow* gameWindow)
{
    //治愈范围内所有敌方单位
    auto targets = gameWindow->FindAllEnemiesInRange(this->x(), this->y(), _attackRange);
    for(auto& tar : targets)
        if(tar->IsAlive())
            tar->BeAttacked(-5);
}
