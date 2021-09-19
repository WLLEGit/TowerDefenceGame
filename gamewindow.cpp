#include "gamewindow.h"
#include "ui_gamewindow.h"

GameWindow::GameWindow(QWidget *parent, int mapID) :
    QMainWindow(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    LoadMap(mapID);
    InitMapLabels();

    this->setFixedSize(col * CELLWIDTH, row * CELLWIDTH);
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
            cells[i][j] = Cell(type);
        }
    }
}

void GameWindow::SetCellRsrcImg(int r, int c)
{
    assert(0 <= r && r <= row && 0 <= c && c <= col);

    Cell& cell = cells[r][c];
    Cell::CellType cellType = cell.GetCellType();
    QString path;
    if(cellType == Cell::Path)
    {
        if(cell.GetCellTypeID() == Cell::Start)
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
    map[r][c]->setPixmap(QPixmap(path));

    if(cellType == Cell::Placable)
    {
        path = ":/assets/map/block.png";
        map[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/towers/Tower.png"))));

    }

    if(cell.GetResourceType())
    {
        if(cell.GetResourceType() == 1)
            map[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/gold.png"))));
        else if(cell.GetResourceType() == 2)
            map[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/coppers.png"))));
    }

    if(cell.GetCellTypeID() == Cell::End)
        map[r][c]->setPixmap(QPixmap::fromImage(MergeImage(QImage(path), QImage(":/assets/map/end.png"))));
}

void GameWindow::InitMapLabels()
{
    map.resize(row);
    for(int i = 0; i < row; ++i)
    {
        map[i].resize(col);
        for(int j = 0; j < col; ++j)
        {
            map[i][j] = new QLabel(this);
            map[i][j]->setGeometry(CELLWIDTH * j, CELLWIDTH*i, CELLWIDTH, CELLWIDTH);
            map[i][j]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            map[i][j]->setScaledContents(true);
            SetCellRsrcImg(i, j);
        }
    }
}

