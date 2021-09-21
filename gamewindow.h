#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "header.h"
#include "tower.h"
#include "cell.h"

class Enemy;

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum GameStatus{Running, Paused, End};
    GameStatus gameStatus = Paused;

    explicit GameWindow(QWidget *parent = nullptr, int mapID=1);
    ~GameWindow();

private:
    Ui::GameWindow *ui;

private:
    int row, col;
    QVector<QVector<Cell*>> cells;
    QList<Enemy*> enemies;
    QList<Tower*> towers;
    QList<Hero*> heros;

    int round;
    int health;
    int money;

    QTimer* fpsTimer;
    QTimer* resourceTimer;

    QMap<QPair<Cell*, Cell::CellType>, QList<Cell*>*> pathMap;

    Tower* towerSelected;
    Cell* cellSelected;

    int waitToPlaceType;    //等待放置的单位
    int waitToCost;         //将要消耗的资金

private:
    void LoadMap(int id);
    void LoadMapHelper(QTextStream& in, QString pathType, Cell::CellType cellType);
    void SetCellRsrcImg(int r, int c);
    void InitMapLabels();
    inline bool HelperIsBlocked(int r, int c){return 0 <= r && r < row && 0 <= c && c < col \
                && (cells[r][c]->GetCellType() == Cell::Blocked || cells[r][c]->GetCellType() == Cell::Placable);}
    void RunMainloop();     //不断更新塔、敌人、友军的状态
    void UpdateOneFrame();
    void OnHeroDead(Hero* hero);
    void UnitSelected(int type);

    void OnCellPressed(Cell *);
    void OnTowerPressed(Tower *);

    Hero* CreatHero(int type, Cell* cell);
    Tower* CreatTower(int type, Cell* cell);

    void UpdateResource();

public:
    void OnPlacableClicked(int r, int c);
    inline Cell* GetAt(int r, int c);
    Cell* Locate(QLabel* src);
    QList<Cell*>* FindPath(Cell* start) {return pathMap[QPair<Cell*, Cell::CellType>(start, Cell::Path)];}
    Hero* FindPossibleFriendlyUnit(int r, int c);
    void EnemyHit(int damage);

    bool CanCellPlaceHero(Cell*); //判断能否放置英雄
    bool CanCellPlaceTower(Cell* cell); //判断能否放置塔
    bool IsCellHasTower(Cell* cell);    //判断是否存在塔


    friend void Enemy::Update(GameWindow*);
    friend void Tower::Update(GameWindow*);
    friend void Hero::Update(GameWindow*);
};

#endif // GAMEWINDOW_H
