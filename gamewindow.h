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
    QVector<Enemy*> enemies;
    QVector<Tower*> towers;
    QVector<FriendlyUnit*> friendUnits;

    int round;
    int health;
    int money;

    QTimer* fpsTimer;

    QMap<Cell*, QList<Cell*>*> pathMap;

private:
    void LoadMap(int id);
    void LoadMapHelper(QTextStream& in, QString pathType, Cell::CellType cellType);
    void SetCellRsrcImg(int r, int c);
    void InitMapLabels();
    inline bool HelperIsBlocked(int r, int c){return 0 <= r && r <= row && 0 <= c && c <= col \
                && (cells[r][c]->GetCellType() == Cell::Blocked || cells[r][c]->GetCellType() == Cell::Placable);}
    void RunMainloop();     //不断更新塔、敌人、友军的状态
    void UpdateOneFrame();

public:
    void OnPlacableClicked(int r, int c);
    Cell* Locate(QLabel* src);
    QList<Cell*>* FindPath(Cell* start) {return pathMap[start];}
    FriendlyUnit* FindPossibleFriendlyUnit(int r, int c);
    void EnemyHit(int damage);
    friend void Enemy::Update(GameWindow*);
};

#endif // GAMEWINDOW_H
