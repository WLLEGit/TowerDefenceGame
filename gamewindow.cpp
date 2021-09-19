#include "gamewindow.h"
#include "ui_gamewindow.h"

GameWindow::GameWindow(QWidget *parent, int mapID) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    LoadMap(mapID);
    InitMapLabels();

    enemies.push_back(Enemy::GenerateEnemy(1, this, cells[1][11], this, 20));

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
    int type;
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
            in >> type;
            cells[i][j] = new Cell(type, 0, this, i, j);
        }
    }
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

Direction GameWindow::FindPath(int r, int c, int srcType, Direction preDir)
{
    if(r>0 && (cells[r-1][c]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Down)
        return Direction::Up;
    if(r<row-1 && (cells[r+1][c]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Up)
        return Direction::Down;
    if(c>0 && (cells[r][c-1]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Right)
        return Direction::Left;
    if(c<col-1 && (cells[r][c+1]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Left)
        return Direction::Right;
    return Direction::None;
}

Cell *GameWindow::FindNextCell(int r, int c, int srcType, Direction preDir)
{
    if(r>0 && (cells[r-1][c]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Down)
        return cells[r-1][c];
    if(r<row-1 && (cells[r+1][c]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Up)
        return cells[r+1][c];
    if(c>0 && (cells[r][c-1]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Right)
        return cells[r][c-1];
    if(c<col-1 && (cells[r][c+1]->GetCellTypeID() & srcType & 0xf) && preDir != Direction::Left)
        return cells[r][c+1];
    return cells[r][c];
}

FriendlyUnit *GameWindow::FindPossibleFriendlyUnit(int r, int c)
{
    for(auto& fu : friendUnits)
        if(fu->GetPositionCell() == cells[r][c] && fu->CanHoldEnemy())
            return fu;
    return nullptr;
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
}

