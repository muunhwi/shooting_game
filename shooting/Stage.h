#pragma once
#include "Enemy.h"

#define STAGE_FILE "stages.txt"
constexpr int ROW = 24;
constexpr int COL = 80;
constexpr int MAX_ENEMY_COUNT = 200;

struct StageInfo
{
    Enemy enemyPos[MAX_ENEMY_COUNT];
    int enemyPosCount;
};

struct StageInfoContainer
{
    StageInfo* si;
    int size;
};

extern StageInfoContainer* stages;

bool makeStage();
void deleteStageInfo();