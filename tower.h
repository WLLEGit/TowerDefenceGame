#ifndef TOWER_H
#define TOWER_H

#include "header.h"

class Tower
{
public:
    Tower();
};

class FriendlyUnit
{
public:
    enum Type{Guard, Miner};
};

class ArrowTower : public Tower
{
public:

};

class GuardTower : public Tower
{

};

#endif // TOWER_H
