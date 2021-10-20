#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "header.h"
#include "tower.h"
#include "cell.h"
#include "hero.h"

class Enemy;

namespace Ui {
class GameWindow;
}

struct EnemyConfig
{
    int type;
    int quantity;
    int bornInterval;
    Cell::CellType bornCellType;
    EnemyConfig(int type, int quantity, int interval, Cell::CellType bornCellType = Cell::Path);
};

struct Round
{
    QMap<Cell::CellType, QVector<EnemyConfig>> roundInfo;
    Round();
};

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum GameStatus{Running, Paused, End};
    GameStatus gameStatus = Paused;

    explicit GameWindow(QWidget *parent = nullptr, int mapID=1, int roundID=1);
    ~GameWindow();

private:
    Ui::GameWindow *ui;

private:
    int _row, _col;
    QVector<QVector<Cell*>> _cells;
    QList<Enemy*> _enemies;
    QList<Tower*> _towers;
    QList<Hero*> _heros;
    QList<Bullet*> _bullets;

    int _round;
    int _maxRound;
    QVector<Round> _roundinfo;
    bool _enemiesGenerateDone;

    int _health;
    int _money;

    QTimer* _fpsTimer;
    QTimer* _resourceTimer;
    QTimer* _enemyTimer;

    QMap<QPair<Cell*, Cell::CellType>, QList<Cell*>*> _pathMap;
    QMap<Cell::CellType, Cell*> _startCellMap;

    int _waitToPlaceType;    //等待放置的单位
    int _waitToCost;         //将要消耗的资金

private:
    void LoadMap(int id);
    void LoadMapHelper(QTextStream& in, QString pathType, Cell::CellType cellType);
    void LoadRoundInfo(int id);
    void LoadRoundHelper(QTextStream& in, QString pathType, Cell::CellType cellType);
    void NextEnemy();
    bool NextEnemyHelper(Cell::CellType cellType);
    void SetCellRsrcImg(int r, int c);
    void InitMapLabels();
    inline bool HelperIsBlocked(int r, int c){return 0 <= r && r < _row && 0 <= c && c < _col \
                && (_cells[r][c]->GetCellType() == Cell::Blocked || _cells[r][c]->GetCellType() == Cell::Placable);}
    void RunMainloop();     //不断更新塔、敌人、友军的状态
    void UpdateOneFrame();
    bool OnHeroDead(Hero* hero);
    void UnitSelected(int type);

    void OnCellPressed(Cell *);
    void OnTowerPressed(Tower *);
    void OnHeroPressed(Hero*);
    void OnBulletHitEnemy(Bullet*);

    Hero* CreateHero(int type, Cell* cell);
    Tower* CreateTower(int type, Cell* cell);
    Enemy* CreateEnemy(int type, Cell* cell, Cell::CellType cellType);

    void UpdateResource();


public:
    void OnPlacableClicked(int r, int c);
    inline Cell* GetAt(int r, int c);
    Cell* Locate(QLabel* src);
    QList<Cell*>* FindPath(Cell* start, Cell::CellType cellType=Cell::Path);
    void EnemyHit(int damage);

    bool CanCellPlaceHero(Cell*); //判断能否放置英雄
    bool CanCellPlaceTower(Cell* cell); //判断能否放置塔
    bool IsCellHasTower(Cell* cell);    //判断是否存在塔

    Bullet* CreateBullet(Enemy* target, Tower* src, int speed, int damage, QPixmap pic);

    QVector<Enemy*> FindAllEnemiesInRange(int x, int y, int range);
    QVector<Hero*> FindAllHerosInRange(int x, int y, int range);
    QVector<Tower*> FindAllTowersInRange(int x, int y, int range);

    Enemy* FindOneLivingEnemyInRange(int x, int y, int range);
    Hero* FindOneLivingHeroInRange(int x, int y, int range);
    Tower* FindOneLivingTowerInRange(int x, int y, int range);


    friend void Enemy::Update(GameWindow*);
    friend void Tower::Update(GameWindow*);
    friend void Hero::Update(GameWindow*);
};

#endif // GAMEWINDOW_H
