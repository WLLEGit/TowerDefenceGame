#include "hero.h"

#include "gamewindow.h"
Hero::Hero(QWidget *parent, GameWindow* gameWindow, Cell* posCell)
    :LivingUnit(parent, gameWindow), _gameWindow(gameWindow), _posCell(posCell)
{
    setGeometry(posCell->x(), posCell->y(), CELLWIDTH, CELLWIDTH);
    _isDestoried = false;
}

void Hero::AddEnemy(Enemy *enemy)
{
    //assert(_capacityUsed);
    _capacityRemain--;
    _enemiesHold.push_back(enemy);
}

bool Hero::RemoveEnemy(Enemy *enemy)
{
    //assert(capacity < maxCapacity);
    _capacityRemain++;
    return _enemiesHold.removeOne(enemy);
}

void Hero::Attack()
{
    if(_isDestoried)
        return;

    FindEnemy();

    if(_attack)
        for(auto& enemy : _targets)
            enemy->BeAttacked(_attack);
}

bool Hero::InRange(int x, int y)
{
    return DISTANCE(x-this->x(), y-this->y()) <= CELLWIDTH * (double)(_range - 0.3);
}

void Hero::FindEnemy()
{
    _targets.clear();
    auto rv = _gameWindow->FindAllEnemiesInRange(this->x(), this->y(), (_range - 0.3) * CELLWIDTH);
    QVector<Enemy*> enemies;
    std::copy_if(rv.begin(), rv.end(), std::back_inserter(enemies), [=](Enemy* e){return (_canAttackFly || !e->IsFlying());});
    if(enemies.size())
    {
        if(_isAttackAll)
            _targets = std::move(enemies);
        else
            _targets.push_back(enemies[0]);
    }
}

void Hero::mousePressEvent(QMouseEvent *ev)
{
    emit HeroPressed(this);
    QLabel::mousePressEvent(ev);
}

void Hero::Update()
{
    if(_isDestoried)
    {
        _attackTimer->stop();
        _picTimer->stop();
        return;
    }

    if(_curHealth <= 0)
    {
        this->hide();
        _healthBar->hide();
        _isDestoried = true;
        _attackTimer->stop();
        _picTimer->stop();
        emit HeroDead(this);
    }
    DrawHealthLine();
}

Hero *Hero::GenerateHero(QWidget *parent, GameWindow* gameWindow, Cell *posCell, int type)
{
    if(posCell->GetCellType() != Cell::Path)
        return nullptr;

    if(type == 1)
        return new Warrior(parent, gameWindow, posCell);
    else if(type == 2)
        return new Magician(parent, gameWindow, posCell);
    else if(type == 3)
        return new Spikeweed(parent, gameWindow, posCell);
    else if(type == 4)
        return  new WallNut(parent, gameWindow, posCell);
    else
        return new Warrior(parent, gameWindow, posCell);
}

void Hero::SwitchPic()
{
    _curIndex = (_curIndex + 1) % (_maxIndex);
    QPixmap pix(QString(":/assets/heros/%1%2.png").arg(_name).arg(_curIndex));
    pix = pix.scaledToWidth(CELLWIDTH*0.8);
    this->setPixmap(pix);
}

void Hero::LoadConfig(QString name)
{
    QJsonObject heroConfig = globalConfig["Heros"].toObject()[name].toObject();
    _maxHealth = heroConfig["maxHealth"].toInt();
    _curHealth = _maxHealth;
    _maxCapacity = heroConfig["maxCapacity"].toInt();
    _capacityRemain = _maxCapacity;
    _attack = heroConfig["attack"].toInt();
    _attackInterval = heroConfig["attackInterval"].toDouble();
    _name = heroConfig["name"].toString();
    _range = heroConfig["range"].toDouble();
    _cost = heroConfig["cost"].toInt();
    _picInterval = heroConfig["picInterval"].toDouble();
    _curIndex = 0;
    _maxIndex = heroConfig["maxIndex"].toInt();
    _isAttackAll = heroConfig["isAttackAll"].toBool();
    _canAttackFly = heroConfig["canAttackFly"].toBool();
}

#define HeroConstructorHelper(className)\
className::className(QWidget *parent, GameWindow* gameWindow, Cell *posCell)\
    :Hero(parent, gameWindow, posCell)\
{\
    LoadConfig(#className);\
    SwitchPic();\
    _picTimer->start(1000*_picInterval);\
    _attackTimer->start(_attackInterval*1000);\
}
HeroConstructorHelper(Warrior)
HeroConstructorHelper(Magician)
HeroConstructorHelper(Spikeweed)
HeroConstructorHelper(WallNut)
