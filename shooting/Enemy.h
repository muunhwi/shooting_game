#pragma once
#include "Movement.h"

#define ENEMY_TYPE_FILE "enemies.txt"

struct EnemyType
{
    char shape;
    char attackShape;
    int cooldown;
    int timer;
    int perAttackCount;
    Movement* mv;
};

struct EnemyTypeContainer
{
    EnemyType* types;
    int size;
};

struct Enemy
{
    bool isAlive;
    int xpos;
    int ypos;
    EnemyType type;
    Movement mv;
};
extern EnemyTypeContainer* et;
bool makeEnemyType();
void deleteEnemyType();
