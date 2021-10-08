#include "gamewindow.h"
#include "ui_gamewindow.h"

#include<iostream>

/*TODO:
 * BUG：怪物移动有时候会超出道路
 */


GameWindow::GameWindow(QWidget *parent, int mapID, int roundID) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    _round = 0;
    _health = 10;
    _money = 500;

    //init map
    LoadMap(mapID);
    InitMapLabels();

    LoadRoundInfo(roundID);

    setFixedHeight(CELLWIDTH*_row);

    ui->label->setAlignment(Qt::AlignCenter);


    connect(ui->undoSelectBtn, &QPushButton::clicked, this, [=](){UnitSelected(0);});
    connect(ui->hero1, &QPushButton::clicked, this, [=](){UnitSelected(0x11);});
    connect(ui->hero2, &QPushButton::clicked, this, [=](){UnitSelected(0x12);});
    connect(ui->arrowTower, &QPushButton::clicked, this, [=](){UnitSelected(0x21);});
    connect(ui->missileTower, &QPushButton::clicked, this, [=](){UnitSelected(0x22);});


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
    _enemiesGenerateDone = true;
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


Hero *GameWindow::FindPossibleFriendlyUnit(int r, int c)
{
    for(auto& fu : _heros)
        if(fu->GetPositionCell() == _cells[r][c] && fu->CanHoldEnemy())
            return fu;
    return nullptr;
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
        enemy->Update(this);

    for(auto& tower : _towers)
        tower->Update(this);

    for(auto& hero : _heros)
        hero->Update(this);

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

void GameWindow::UnitSelected(int type)
{
    if(type == 0)   //reset
    {
        ui->label->setText("");
        _waitToPlaceType = -1;
        _waitToCost = 0;
    }
    else if(type & 0x10)    //hero
    {
        UnitSelected(0);
        int cost = (type == 0x11 ? HERO1COST : (type == 0x12 ? HERO2COST : 10000));
        if(_money >= cost)
        {
            ui->label->setText(QString("Hero%1 Selected").arg(type&0xf));
            _waitToPlaceType = type;
            _waitToCost = cost;
        }
    }
    else if(type & 0x20)    //tower
    {
        UnitSelected(0);
        int cost = type == 0x21 ? ARROWCOST : (type == 0x22 ? MISSILECOST : 10000);
        if(_money >= cost)
        {
            ui->label->setText(QString("%1Tower Selected").arg(type == 0x21 ? "Arrow" : (type == 0x22 ? "Missile" : "UNKNOWN")));
            _waitToPlaceType = type;
            _waitToCost = cost;
        }
    }
}

void GameWindow::OnCellPressed(Cell* cell)
{
    qDebug() << "Cell Pressed" << cell->row() << cell->col();
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
    if((_waitToPlaceType & 0x20) && ((_waitToPlaceType & 0xf) == tower->Type()) && tower->Level() < TOWERMAXLEVEL)
    {
        tower->Upgrade();
        _money -= _waitToCost;
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
    qDebug() << "CreatHero" << type << cell->row() << cell->col();
    auto hero = Hero::GenerateHero(this, cell, type);
    hero->Show();
    _heros.push_back(hero);
    connect(hero, &Hero::HeroDead, this, &GameWindow::OnHeroDead);
    return hero;
}

Tower *GameWindow::CreateTower(int type, Cell *cell)
{
    Tower* t=nullptr;
    if(type == 1)
        t = new ArrowTower(this, cell);
    else if(type == 2)
        t = new MissleTower(this, cell);
    else
        t = new ArrowTower(this, cell);
    t->show();
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
