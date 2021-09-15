#include "gamescene.h"

void GameScene::LoadMap(int id)
{
    int type;
    QString path = QString("./map/GameMap%1").arg(id);
    QFile fin(path);
    fin.open(QIODevice::ReadOnly);
    QTextStream in(&fin);

    in >> row >> col;
    map.resize(row * col);
    for(int i = 0; i < row; ++i)
        for(int j = 0; j < col; ++j)
        {
            in >> type;
            map[i][j] = Cell(type);
        }
}

GameScene::GameScene(int mapID)
{
    LoadMap(mapID);
}

Cell::Cell(int type)
{
    SetType(type);
}

void Cell::SetType(int type)
{
    this->type = type;
    if(type & Resource)
        cellType = (CellType)type;
    else if(type & Path)
        cellType = Path;
    else if(type & Blocked)
        cellType = Blocked;
    else if(type & Placable)
        cellType = Placable;
    emit ChangeType(cellType);
}
