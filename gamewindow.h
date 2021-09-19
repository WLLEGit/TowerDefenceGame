#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "header.h"
#include "enemy.h"
#include "tower.h"

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum GameStatus{Running, Paused, End};
    explicit GameWindow(QWidget *parent = nullptr, int mapID=1);
    ~GameWindow();

private:
    Ui::GameWindow *ui;

private:
    int row, col;
    QVector<QVector<Cell>> cells;
    QVector<QVector<QLabel*>> map;
    QVector<Enemy*> enemies;
    QVector<Tower*> towers;

    int round;
    int health;
    int money;

private:
    void LoadMap(int id);
    void SetCellRsrcImg(int r, int c);
    void InitMapLabels();
    inline bool HelperIsBlocked(int r, int c){return 0 <= r && r <= row && 0 <= c && c <= col \
                                                     && (cells[r][c].GetCellType() == Cell::Blocked || cells[r][c].GetCellType() == Cell::Placable);}
    void Run();     //不断更新塔、敌人、友军的状态

public:
    void OnPlacableClicked(int r, int c);
};

#endif // GAMEWINDOW_H
