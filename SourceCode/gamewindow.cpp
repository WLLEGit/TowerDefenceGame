#include "gamewindow.h"
#include "ui_gamewindow.h"

#include<iostream>

GameWindow::GameWindow(QWidget *parent, int mapID, int roundID) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    _round = 0;
    _health = 10;
    _money = 500;
    _enemiesGenerateDone = false;

    //init map
    LoadMap(mapID);
    InitMapLabels();

    LoadRoundInfo(roundID);

    setFixedHeight(CELLWIDTH*_row);

    ui->label->setAlignment(Qt::AlignCenter);


    connect(ui->undoSelectBtn, &QPushButton::clicked, this, [=](){UnitSelected(0);});
    connect(ui->hero1, &QPushButton::clicked, this, [=](){UnitSelected(0x11);});
    connect(ui->hero2, &QPushButton::clicked, this, [=](){UnitSelected(0x12);});
    connect(ui->spikeweed, &QPushButton::clicked, this, [=](){UnitSelected(0x13);});
    connect(ui->wallnut, &QPushButton::clicked, this, [=](){UnitSelected(0x14);});

    connect(ui->arrowTower, &QPushButton::clicked, this, [=](){UnitSelected(0x21);});
    connect(ui->missileTower, &QPushButton::clicked, this, [=](){UnitSelected(0x22);});
    connect(ui->guardTower, &QPushButton::clicked, this, [=](){UnitSelected(0x23);});

    connect(ui->deleteHeroBtn, &QPushButton::clicked, this, [=](){UnitSelected(-1);});

    //ui main loop
    _fpsTimer = new QTimer(this);
    connect(_fpsTimer, &QTimer::timeout, this, &GameWindow::UpdateOneFrame);

    _resourceTimer = new QTimer(this);
    connect(_resourceTimer, &QTimer::timeout, this, &GameWindow::UpdateResource);

    _enemyTimer = new QTimer(this);
    connect(_enemyTimer, &QTimer::timeout, this, &GameWindow::NextEnemy);
    _enemyTimer->start(2000);

    RunMainloop();
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::LoadMap(int id)
{
    QString path = QString(PROJECTPATH + "assets\\MapConfig\\GameMap%1").arg(id);
    QFile fin(path);
    fin.open(QIODevice::ReadOnly);
    QTextStream in(&fin);

    in >> _row >> _col;
    _cells.resize(_row);
    for(int i = 0; i < _row; ++i)
    {
        _cells[i].resize(_col);
        for(int j = 0; j < _col; ++j)
        {
            _cells[i][j] = new Cell(0, 0, this, i, j);
            connect(_cells[i][j], &Cell::CellPressed, this, &GameWindow::OnCellPressed);
            _cells[i][j]->show();
        }

    }

    LoadMapHelper(in, "Red", Cell::Red);
    LoadMapHelper(in, "Green", Cell::Green);
    LoadMapHelper(in, "Blue", Cell::Blue);
    LoadMapHelper(in, "White", Cell::White);
    LoadMapHelper(in, "Placable", Cell::Placable);

    for(int i = 0; i < _row; ++i)
        for(int j = 0; j < _col; ++j)
            if(_cells[i][j]->GetCellTypeID()==0)
                _cells[i][j]->AddType(Cell::Blocked);
}

void GameWindow::LoadMapHelper(QTextStream &in, QString newPathType, Cell::CellType cellType)
{
    QString pathType;
    Cell* start;
    QList<Cell*>* path = nullptr;
    int n, r, c;
    in >> pathType >> n;
    assert(pathType == newPathType);

    for(int i = 0; i < n; ++i)
    {
        in >> r >> c;
        if(cellType == Cell::Placable)
        {
            _cells[r][c]->AddType(cellType);
        }
        else
        {
            if(i == 0)
            {
                _cells[r][c]->AddType(Cell::Start);
                start = _cells[r][c];
                path = new QList<Cell*>();
                _pathMap[QPair<Cell*, Cell::CellType>(start, cellType)] = path;
                _pathMap[QPair<Cell*, Cell::CellType>(start, Cell::Path)] = path;
                _startCellMap[cellType] = start;
            }
            else if(i == n-1)
                _cells[r][c]->AddType(Cell::End);
            else
                _cells[r][c]->AddType(cellType);
            path->push_back(_cells[r][c]);
        }

    }

    if(path)
        path->push_back(path->last());
}

void GameWindow::LoadRoundInfo(int id)
{
    QString path = QString(PROJECTPATH + "assets\\RoundConfig\\RoundConfig%1").arg(id);
    QFile fin(path);
    fin.open(QIODevice::ReadOnly);
    QTextStream in(&fin);

    _round = 0;
    in >> _maxRound;
    _roundinfo.resize(_maxRound);

    LoadRoundHelper(in, "Red", Cell::Red);
    LoadRoundHelper(in, "Green", Cell::Green);
    LoadRoundHelper(in, "Blue", Cell::Blue);
    LoadRoundHelper(in, "White", Cell::White);
}

void GameWindow::LoadRoundHelper(QTextStream &in, QString tarType, Cell::CellType cellType)
{
    QString pathType;
    int cnt;

    int r, type, quantity, interval;

    in >> pathType >> cnt;
    assert(pathType == tarType);
    for(int i = 0; i < cnt; ++i)
    {
        in >> r >> type >> quantity >> interval;
        _roundinfo[r].roundInfo[cellType].push_back(EnemyConfig(type, quantity, interval, cellType));
    }
}

void GameWindow::NextEnemy()
{
    _enemiesGenerateDone = false;
    _enemiesGenerateDone |= NextEnemyHelper(Cell::Red);
    _enemiesGenerateDone |= NextEnemyHelper(Cell::Green);
    _enemiesGenerateDone |= NextEnemyHelper(Cell::Blue);
    _enemiesGenerateDone |= NextEnemyHelper(Cell::White);
}

bool GameWindow::NextEnemyHelper(Cell::CellType cellType)
{
    bool isGenerateDone = true;
    for(auto& enemyConfig : _roundinfo[_round].roundInfo[cellType])
    {
        if(enemyConfig.quantity)
        {
            isGenerateDone = false;
            CreateEnemy(enemyConfig.type, _startCellMap[cellType], cellType);
            enemyConfig.quantity--;
        }
    }
    return isGenerateDone;
}


void GameWindow::SetCellRsrcImg(int r, int c)
{
    assert(0 <= r && r <= _row && 0 <= c && c <= _col);

    Cell* cell = _cells[r][c];
    Cell::CellType cellType = cell->GetCellType();
    QString path;
    if(cellType == Cell::Path)
    {
        if(cell->GetCellTypeID() == Cell::Start)
            path = ":/assets/map/start.png";
        else
        {
            bool u=HelperIsBlocked(r-1, c);
            bool d=HelperIsBlocked(r+1, c);
            bool l=HelperIsBlocked(r, c-1);
            bool ri=HelperIsBlocked(r, c+1);
            path = QString(":/assets/map/%1%2%3%4.png").arg(u).arg(d).arg(l).arg(ri);
        }
        _cells[r][c]->setPixmap(QPixmap(path));
    }
    else if(cellType == Cell::Blocked)
    {
        path = ":/assets/map/block.png";
        QPixmap pix;
        //随机设置一些装饰
        if(qrand() % 10 == 0)
        {
            QMatrix mat;
            mat.rotate(qrand() % 360);
            pix = QPixmap::fromImage(MergeImage(QImage(path), \
                                                QImage(QString(":/assets/map/decoration%1.png").arg(qrand() % 8)).transformed(mat)));
        }
        else
            pix = QPixmap(path);
        _cells[r][c]->setPixmap(pix);
    }

    if(cellType == Cell::Placable)
    {
        path = ":/assets/map/block.png";
        _cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/towers/Tower.png"))));

    }

    if(cell->GetResourceType())
    {
        if(cell->GetResourceType() == 1)
            _cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/gold.png"))));
        else if(cell->GetResourceType() == 2)
            _cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/coppers.png"))));
    }

    if(cell->GetCellTypeID() == Cell::End)
        _cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/end.png"))));
}

void GameWindow::InitMapLabels()
{
    for(int i = 0; i < _row; ++i)
    {
        for(int j = 0; j < _col; ++j)
        {
            _cells[i][j]->setGeometry(CELLWIDTH * j, CELLWIDTH*i, CELLWIDTH, CELLWIDTH);
            _cells[i][j]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            _cells[i][j]->setScaledContents(true);
            SetCellRsrcImg(i, j);
        }
    }
}

Cell *GameWindow::Locate(QLabel *src)
{
    //src 与 Cell 需要在同一个父控件中
    int r = (src->y() + CELLWIDTH/2) / CELLWIDTH;
    int c = (src->x() + CELLWIDTH/2) / CELLWIDTH;
    return _cells[r][c];
}

QList<Cell *> *GameWindow::FindPath(Cell *start, Cell::CellType cellType)
{
    if(!(start->GetCellTypeID() & Cell::Start))
        throw "Cell doesn't marked Cell::Start";

    auto target = QPair<Cell*, Cell::CellType>(start, cellType);
    if(_pathMap.find(target) != _pathMap.end())
        return _pathMap[QPair<Cell*, Cell::CellType>(start, cellType)];
    else
        return _pathMap[QPair<Cell*, Cell::CellType>(start, Cell::Path)];
}

void GameWindow::EnemyHit(int damage)
{
    _health -= damage;
}

bool GameWindow::CanCellPlaceHero(Cell * c)
{
    if(c->GetCellType() != Cell::Path)
        return false;

    for(auto& hero : _heros)
        if(hero->GetPositionCell() == c)
            return false;

    return true;
}

bool GameWindow::CanCellPlaceTower(Cell *cell)
{
    if(cell->GetCellType() != Cell::Placable)
        return false;

    for(auto& tower : _towers)
        if(tower->GetPositionCell() == cell)
            return false;

    return true;
}

bool GameWindow::IsCellHasTower(Cell *cell)
{
    if(cell->GetCellType() != Cell::Placable)
        return false;

    for(auto& tower : _towers)
        if(tower->GetPositionCell() == cell)
            return true;

    return false;
}

Bullet* GameWindow::CreateBullet(Enemy *target, Tower *src, int speed, int damage, QPixmap pic)
{
    Bullet* bullet = new Bullet(this, target, src, speed, damage, pic);
    connect(bullet, &Bullet::HitEnemy, this, &GameWindow::OnBulletHitEnemy);
    bullet->show();
    _bullets.push_back(bullet);
    return bullet;
}


void GameWindow::RunMainloop()
{
    _fpsTimer->start(1000/FPS);
}

void GameWindow::UpdateOneFrame()
{
    if(gameStatus != Running)
    {
        _resourceTimer->stop();
        _enemyTimer->stop();
        return;
    }
    if(!_resourceTimer->isActive())
    {
        _resourceTimer->start(RESOURCEUPDATEDURATION);
        _enemyTimer->start(2000);
    }
    if(_health <= 0)
    {
        gameStatus = Paused;
        ui->label->setText("Fail");
        ui->label->setStyleSheet("color:red;");
    }

    for(auto& enemy : _enemies)
        enemy->Update();

    for(auto& tower : _towers)
        tower->Update();

    for(auto& hero : _heros)
        hero->Update();

    for(auto& bullet : _bullets)
        bullet->Update(this);

    if(_enemiesGenerateDone)
    {
        bool isRoundEnd=true;
        for(auto& enemy : _enemies)
            if(enemy->IsAlive())
                isRoundEnd = false;
        if(isRoundEnd)
        {
            _enemiesGenerateDone = false;
            _round++;
        }
        if(_round == _maxRound)
        {
            gameStatus = Paused;
            ui->label->setText("Win");
            ui->label->setStyleSheet("color:green;");
        }
    }
}

bool GameWindow::OnHeroDead(Hero *hero)
{
    return _heros.removeOne(hero);
}


#define SelectHelper(meta, t, className)\
    else if(type == t)\
    {\
        auto tmp = globalConfig[#meta].toObject()[#className].toObject();\
        cost = tmp["cost"].toInt();\
        if(_money >= cost)\
        {\
            ui->label->setText(QString("%1 Selected").arg(tmp["name"].toString()));\
            _waitToPlaceType = type;\
            _waitToCost = cost;\
        }\
    }
void GameWindow::UnitSelected(int type)
{
    if(type == 0)   //reset
    {
        ui->label->setText("");
        _waitToPlaceType = 0;
        _waitToCost = 0;
    }
    else if(type == -1) //delete hero
    {
        UnitSelected(0);
        ui->label->setText("Delete Hero");
        _waitToPlaceType = -1;
        _waitToCost = 0;
    }
    else if(type & 0x10)    //hero
    {
        UnitSelected(0);
        int cost;
        if(false);
        SelectHelper(Heros, 0x11, Warrior)
        SelectHelper(Heros, 0x12, Magician)
        SelectHelper(Heros, 0x13, Spikeweed)
        SelectHelper(Heros, 0x14, WallNut)
    }
    else if(type & 0x20)    //tower
    {
        UnitSelected(0);

        int cost;
        if(false);
        SelectHelper(Towers, 0x21, ArrowTower)
        SelectHelper(Towers, 0x22, MissleTower)
        SelectHelper(Towers, 0x23, GuardTower)
    }
}

void GameWindow::OnCellPressed(Cell* cell)
{
    if((_waitToPlaceType & 0x10) && CanCellPlaceHero(cell)) //放置英雄
    {
        CreateHero(_waitToPlaceType & 0xf, cell);
        _money -= _waitToCost;
        UnitSelected(0);
    }
    else if((_waitToPlaceType & 0x20) && CanCellPlaceTower(cell))  //放置塔
    {
        CreateTower(_waitToPlaceType & 0xf, cell);
        _money -= _waitToCost;
        UnitSelected(0);
    }
}

void GameWindow::OnTowerPressed(Tower* tower)
{
    if((_waitToPlaceType & 0x20) && ((_waitToPlaceType & 0xf) == tower->Type()) && tower->Level() < tower->MaxLevel())
    {
        tower->Upgrade();
        _money -= _waitToCost;
        UnitSelected(0);
    }
}

void GameWindow::OnHeroPressed(Hero *hero)
{
    if(_waitToPlaceType == -1)
    {
        hero->BeAttacked(1000);
        UnitSelected(0);
    }
}

void GameWindow::OnBulletHitEnemy(Bullet *bullet)
{
    assert(bullet->GetTarget());
    Enemy* target = bullet->GetTarget();
    if(target->IsAlive())
    {
        target->BeAttacked(bullet->GetDamage());
    }
    bullet->hide();
    _bullets.removeOne(bullet);
    delete bullet;
}

Hero* GameWindow::CreateHero(int type, Cell* cell)
{
    auto hero = Hero::GenerateHero(this, this, cell, type);
    hero->Show();
    _heros.push_back(hero);
    connect(hero, &Hero::HeroDead, this, &GameWindow::OnHeroDead);
    connect(hero, &Hero::HeroPressed, this, &GameWindow::OnHeroPressed);
    return hero;
}

Tower *GameWindow::CreateTower(int type, Cell *cell)
{
    Tower* t=Tower::GenerateTower(this, this, cell, type);
    t->Show();
    connect(t, &Tower::TowerPressed, this, &GameWindow::OnTowerPressed);
    _towers.push_back(t);
    return t;
}

Enemy *GameWindow::CreateEnemy(int type, Cell *cell, Cell::CellType cellType)
{
    Enemy* e = Enemy::GenerateEnemy(type, this, cell, this, cellType);
    _enemies.push_back(e);
    e->Show();
    return e;
}

void GameWindow::UpdateResource()
{
    _money += 10;
    ui->resourceLabel->setText(QString("生命值: %1\n资源: %2\n回合：%3\n").arg(_health).arg(_money).arg(_round));
}

#define FindAllHelper(className, str, vec)\
QVector<className *> GameWindow::FindAll##str##InRange(int x, int y, int range)\
{\
    QVector<className *> res;\
    for(auto& tar : vec)\
    {\
        if(tar->IsAlive() && DISTANCE(x-tar->x(), y-tar->y()) <= range)\
            res.push_back(tar);\
    }\
    return res;\
}

FindAllHelper(Enemy, Enemies, _enemies)
FindAllHelper(Hero, Heros, _heros)
FindAllHelper(Tower, Towers, _towers)

#define FindOneHelper(className, vec)\
className * GameWindow::FindOne##className##InRange(int x, int y, int range)\
{\
    for(auto& tar : vec)\
    {\
        if(DISTANCE(tar->IsAlive() && x-tar->x(), y-tar->y()) <= range)\
            return tar;\
    }\
    return nullptr;\
}
FindOneHelper(Enemy, _enemies)
FindOneHelper(Tower, _towers)
Hero * GameWindow::FindOneHeroInRange(int x, int y, int range)
{
    for(auto& hero : _heros)
    {
        if(DISTANCE(x-hero->x(), y-hero->y()) <= range && hero->IsAlive() && hero->CanHoldEnemy())
            return hero;
    }
    return nullptr;
}

Cell* GameWindow::FindOnePathCellInRange(int x, int y, int range, int isFly)
{
    for(auto& line: _cells)
        for(auto& cell : line)
        {
            if(DISTANCE(cell->x()-x, cell->y()-y)<=range && (cell->GetCellTypeID()&Cell::Path) && (isFly || cell->GetCellTypeID() != Cell::White))
                return cell;
        }
    return nullptr;
}

Cell *GameWindow::GetAt(int r, int c)
{
    if(0 > r && r >= _row && 0 > c && c >= _col)
        return nullptr;
    else
        return _cells[r][c];
}


EnemyConfig::EnemyConfig(int type, int quantity, int interval, Cell::CellType bornCellType)
    :type(type), quantity(quantity), bornInterval(interval), bornCellType(bornCellType)
{
}


Round::Round()
{
    roundInfo[Cell::Red] = QVector<EnemyConfig>();
    roundInfo[Cell::Green] = QVector<EnemyConfig>();
    roundInfo[Cell::Blue] = QVector<EnemyConfig>();
    roundInfo[Cell::White] = QVector<EnemyConfig>();
}
