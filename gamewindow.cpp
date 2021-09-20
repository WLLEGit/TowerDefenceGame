#include "gamewindow.h"
#include "ui_gamewindow.h"

#include<iostream>

GameWindow::GameWindow(QWidget *parent, int mapID) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    LoadMap(mapID);
    InitMapLabels();

    //-------------------Test Units--------------
    enemies.push_back(Enemy::GenerateEnemy(1, this, cells[1][11], this, 20));
    towers.push_back(new MissleTower(this, cells[3][10]));
    auto hero = Hero::GenerateHero(this, cells[6][10], 1);
    heros.push_back(hero);
    connect(hero, &Hero::HeroDead, this, &GameWindow::OnHeroDead);
    //------------------------------------------

    this->setFixedSize(col * CELLWIDTH, row * CELLWIDTH);

    fpsTimer = new QTimer(this);
    connect(fpsTimer, &QTimer::timeout, this, &GameWindow::UpdateOneFrame);

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

void GameWindow::RunMainloop()
{
    fpsTimer->start(1000/FPS);
}

void GameWindow::UpdateOneFrame()
{
    if(gameStatus != Running)
        return;

    for(auto& enemy : enemies)
        enemy->Update(this);

    for(auto& tower : towers)
        tower->Update(this);

    for(auto& hero : heros)
        hero->Update(this);
}

void GameWindow::OnHeroDead(Hero *hero)
{
    delete hero;
    heros.removeOne(hero);
}

Cell *GameWindow::GetAt(int r, int c)
{
    if(0 > r && r >= row && 0 > c && c >= col)
        return nullptr;
    else
        return cells[r][c];
}

