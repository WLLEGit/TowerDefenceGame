#include "gamewindow.h"
#include "ui_gamewindow.h"

#include<iostream>

GameWindow::GameWindow(QWidget *parent, int mapID) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    round = 0;
    health = 10;
    money = 500;

    //init map
    LoadMap(mapID);
    InitMapLabels();

    setFixedHeight(CELLWIDTH*row);

    connect(ui->undoSelectBtn, &QPushButton::clicked, this, [=](){UnitSelected(0);});
    connect(ui->hero1, &QPushButton::clicked, this, [=](){UnitSelected(0x11);});
    connect(ui->hero2, &QPushButton::clicked, this, [=](){UnitSelected(0x12);});
    connect(ui->arrowTower, &QPushButton::clicked, this, [=](){UnitSelected(0x21);});
    connect(ui->missileTower, &QPushButton::clicked, this, [=](){UnitSelected(0x22);});


    //ui main loop
    fpsTimer = new QTimer(this);
    connect(fpsTimer, &QTimer::timeout, this, &GameWindow::UpdateOneFrame);

    resourceTimer = new QTimer(this);
    connect(resourceTimer, &QTimer::timeout, this, &GameWindow::UpdateResource);

    enemyTimer = new QTimer(this);
    connect(enemyTimer, &QTimer::timeout, this, [=](){CreateEnemy(1, cells[1][11]);});
    enemyTimer->start(2000);

    RunMainloop();
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::LoadMap(int id)
{
    QString path = QString("D:\\HomeWork\\GaoCheng\\Project\\Code\\TowerDefence\\assets\\MapConfig\\GameMap%1").arg(id);
    QFile fin(path);
    fin.open(QIODevice::ReadOnly);
    QTextStream in(&fin);

    in >> row >> col;
    cells.resize(row);
    for(int i = 0; i < row; ++i)
    {
        cells[i].resize(col);
        for(int j = 0; j < col; ++j)
        {
            cells[i][j] = new Cell(0, 0, this, i, j);
            connect(cells[i][j], &Cell::CellPressed, this, &GameWindow::OnCellPressed);
            cells[i][j]->show();
        }

    }

    LoadMapHelper(in, "Red", Cell::Red);
    LoadMapHelper(in, "Green", Cell::Green);
    LoadMapHelper(in, "Blue", Cell::Blue);
    LoadMapHelper(in, "White", Cell::White);
    LoadMapHelper(in, "Placable", Cell::Placable);

    for(int i = 0; i < row; ++i)
        for(int j = 0; j < col; ++j)
            if(cells[i][j]->GetCellTypeID()==0)
                cells[i][j]->AddType(Cell::Blocked);
}

void GameWindow::LoadMapHelper(QTextStream &in, QString newPathType, Cell::CellType cellType)
{
    QString pathType;
    Cell* start;
    QList<Cell*>* path;
    int n, r, c;
    in >> pathType >> n;
    assert(pathType == newPathType);
    for(int i = 0; i < n; ++i)
    {
        in >> r >> c;
        if(cellType == Cell::Placable)
        {
            cells[r][c]->AddType(cellType);
        }
        else
        {
            if(i == 0)
            {
                cells[r][c]->AddType(Cell::Start);
                start = cells[r][c];
                path = new QList<Cell*>();
                pathMap[QPair<Cell*, Cell::CellType>(start, cellType)] = path;
                pathMap[QPair<Cell*, Cell::CellType>(start, Cell::Path)] = path;
            }
            else if(i == n-1)
                cells[r][c]->AddType(Cell::End);
            else
                cells[r][c]->AddType(cellType);
            path->push_back(cells[r][c]);
        }

    }
    path->push_back(path->last());
}

void GameWindow::SetCellRsrcImg(int r, int c)
{
    assert(0 <= r && r <= row && 0 <= c && c <= col);

    Cell* cell = cells[r][c];
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
    }
    else if(cellType == Cell::Blocked)
    {
        path = ":/assets/map/block.png";
    }
    cells[r][c]->setPixmap(QPixmap(path));

    if(cellType == Cell::Placable)
    {
        path = ":/assets/map/block.png";
        cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/towers/Tower.png"))));

    }

    if(cell->GetResourceType())
    {
        if(cell->GetResourceType() == 1)
            cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/gold.png"))));
        else if(cell->GetResourceType() == 2)
            cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/coppers.png"))));
    }

    if(cell->GetCellTypeID() == Cell::End)
        cells[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/end.png"))));
}

void GameWindow::InitMapLabels()
{
    for(int i = 0; i < row; ++i)
    {
        for(int j = 0; j < col; ++j)
        {
            cells[i][j]->setGeometry(CELLWIDTH * j, CELLWIDTH*i, CELLWIDTH, CELLWIDTH);
            cells[i][j]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            cells[i][j]->setScaledContents(true);
            SetCellRsrcImg(i, j);
        }
    }
}

Cell *GameWindow::Locate(QLabel *src)
{
    //src 与 Cell 需要在同一个父控件中
    int r = src->y() / CELLWIDTH;
    int c = src->x() / CELLWIDTH;
    return cells[r][c];
}


Hero *GameWindow::FindPossibleFriendlyUnit(int r, int c)
{
    for(auto& fu : heros)
        if(fu->GetPositionCell() == cells[r][c] && fu->CanHoldEnemy())
            return fu;
    return nullptr;
}

void GameWindow::EnemyHit(int damage)
{
    health -= damage;
}

bool GameWindow::CanCellPlaceHero(Cell * c)
{
    if(c->GetCellType() != Cell::Path)
        return false;

    for(auto& hero : heros)
        if(hero->GetPositionCell() == c)
            return false;

    return true;
}

bool GameWindow::CanCellPlaceTower(Cell *cell)
{
    if(cell->GetCellType() != Cell::Placable)
        return false;

    for(auto& tower : towers)
        if(tower->GetPositionCell() == cell)
            return false;

    return true;
}

bool GameWindow::IsCellHasTower(Cell *cell)
{
    if(cell->GetCellType() != Cell::Placable)
        return false;

    for(auto& tower : towers)
        if(tower->GetPositionCell() == cell)
            return true;

    return false;
}

Bullet* GameWindow::CreateBullet(Enemy *target, Tower *src, int speed, int damage, QPixmap pic)
{
    Bullet* bullet = new Bullet(this, target, src, speed, damage, pic);
    connect(bullet, &Bullet::HitEnemy, this, &GameWindow::OnBulletHitEnemy);
    bullets.push_back(bullet);
    return bullet;
}


void GameWindow::RunMainloop()
{
    fpsTimer->start(1000/FPS);
}

void GameWindow::UpdateOneFrame()
{
    if(gameStatus != Running)
    {
        resourceTimer->stop();
        enemyTimer->stop();
        return;
    }
    if(!resourceTimer->isActive())
    {
        resourceTimer->start(RESOURCEUPDATEDURATION);
        enemyTimer->start(2000);
    }

    for(auto& enemy : enemies)
        enemy->Update(this);

    for(auto& tower : towers)
        tower->Update(this);

    for(auto& hero : heros)
        hero->Update(this);

    for(auto& bullet : bullets)
        bullet->Update(this);

}

bool GameWindow::OnHeroDead(Hero *hero)
{
    return heros.removeOne(hero);
}

void GameWindow::UnitSelected(int type)
{
    if(type == 0)   //reset
    {
        ui->label->setText("");
        waitToPlaceType = -1;
        waitToCost = 0;
    }
    else if(type & 0x10)    //hero
    {
        UnitSelected(0);
        int cost = (type == 0x11 ? HERO1COST : (type == 0x12 ? HERO2COST : 10000));
        if(money >= cost)
        {
            ui->label->setText(QString("Hero%1 Selected").arg(type&0xf));
            waitToPlaceType = type;
            waitToCost = cost;
        }
    }
    else if(type & 0x20)    //tower
    {
        UnitSelected(0);
        int cost = type == 0x21 ? ARROWCOST : (type == 0x22 ? MISSILECOST : 10000);
        if(money >= cost)
        {
            ui->label->setText(QString("%1Tower Selected").arg(type == 0x21 ? "Arrow" : (type == 0x22 ? "Missile" : "UNKNOWN")));
            waitToPlaceType = type;
            waitToCost = cost;
        }
    }
}

void GameWindow::OnCellPressed(Cell* cell)
{
    qDebug() << "Cell Pressed" << cell->row() << cell->col();
    if((waitToPlaceType & 0x10) && CanCellPlaceHero(cell)) //放置英雄
    {
        CreateHero(waitToPlaceType & 0xf, cell);
        money -= waitToCost;
        UnitSelected(0);
    }
    else if((waitToPlaceType & 0x20) && CanCellPlaceTower(cell))  //放置塔
    {
        CreateTower(waitToPlaceType & 0xf, cell);
        money -= waitToCost;
        UnitSelected(0);
    }
}

void GameWindow::OnTowerPressed(Tower* tower)
{
    if((waitToPlaceType & 0x20) && ((waitToPlaceType & 0xf) == tower->Type()) && tower->Level() < TOWERMAXLEVEL)
    {
        tower->Upgrade();
        money -= waitToCost;
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
    bullets.removeOne(bullet);
    delete bullet;
}

Hero* GameWindow::CreateHero(int type, Cell* cell)
{
    qDebug() << "CreatHero" << type << cell->row() << cell->col();
    auto hero = Hero::GenerateHero(this, cell, type);
    hero->Show();
    heros.push_back(hero);
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
    towers.push_back(t);
    return t;
}

Enemy *GameWindow::CreateEnemy(int type, Cell *cell)
{
    Enemy* e = Enemy::GenerateEnemy(type, this, cell, this, 20);
    e->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    enemies.push_back(e);
    e->Show();
    return e;
}

void GameWindow::UpdateResource()
{
    money += 10;
    ui->resourceLabel->setText(QString("生命值: %1\n资源: %2").arg(health).arg(money));
}


Cell *GameWindow::GetAt(int r, int c)
{
    if(0 > r && r >= row && 0 > c && c >= col)
        return nullptr;
    else
        return cells[r][c];
}

