#include "Stage.h"
#include "Common.h"

void printScreen(StageInfo* sp);
bool setStage(StageInfo* si, EnemyTypeContainer* etc, char* line, int row);

StageInfoContainer* stages;

bool makeStage()
{
 
    if (!et)
    {
        return false;
    }
    
    char* buffer = getFile(STAGE_FILE);

    if (!buffer)
        return false;
    
    std::istringstream ss(buffer);
    std::istringstream inss;

    char fileName[MAX_BUFFER_SIZE];
    char fileDetail[MAX_BUFFER_SIZE];
    char fileSize[MAX_HEADERSIZE];

    int size;

    ss.getline(fileSize, MAX_HEADERSIZE);
    if (!my_atoi(&size, fileSize))
    {
        std::cerr << "Failed to read the stage header " << '\n';
        return false;
    }

    stages = (StageInfoContainer*)malloc(sizeof(StageInfoContainer));

    if (!stages)
    {
        std::cerr << "Failed to make stage " << '\n';
        return false;
    }

    stages->si = (StageInfo*)malloc(sizeof(StageInfo) * size);
    stages->size = size;

    StageInfo* sp = stages->si;
    int row;
    while (ss.getline(fileName, MAX_BUFFER_SIZE))
    {
        char* stage = getFile(fileName);
        inss.str(stage);
        
        sp->enemyPosCount = 0;
        row = 0;
        while (inss.getline(fileDetail, MAX_BUFFER_SIZE))
        {
            if (!setStage(sp, et, fileDetail, row))
                return false;

            row++;
        }

        sp++;
        inss.clear();
        free(stage);
    }

    //printScreen(container->si);
    free(buffer);
    return true;
}

void deleteStageInfo()
{
    free(stages->si);
    free(stages);
}

bool setStage(StageInfo* si, EnemyTypeContainer* etc, char* line, int row)
{
    int len = strlen(line);

    if (len > COL)
    {
        for (int i{ COL }; i < len; ++i)
        {
            if (line[i] != ' ' && line[i] != '\0')
            {
                std::cerr << "There is an enemy that exceeds the range\n";
                return false;
            }
        }
    }

    for (int i{ 0 }; i < COL; ++i)
    {
        if (line[i] == ' ')
            continue;

        if (line[i] == '\0')
            break;

        if (si->enemyPosCount >= MAX_ENEMY_COUNT)
        {
            std::cerr << "Maximum enemy count exceeded\n";
            return false;
        }

        for (int j{ 0 }; j < etc->size; ++j)
        {
            if (etc->types[j].shape == line[i])
            {
                si->enemyPos[si->enemyPosCount].isAlive = true;
                si->enemyPos[si->enemyPosCount].xpos = i;
                si->enemyPos[si->enemyPosCount].ypos = row;
                memcpy(&(si->enemyPos[si->enemyPosCount].type), &etc->types[j], sizeof(EnemyType));
                memcpy(&(si->enemyPos[si->enemyPosCount].mv), etc->types[j].mv, sizeof(Movement));
                si->enemyPosCount++;
                break;
            }
        }
    }

    return true;

}

void printScreen(StageInfo* sp)
{
    for (int i{ 0 }; i < sp->enemyPosCount; ++i)
    {
        std::cout << sp->enemyPos[i].xpos;
        std::cout <<", " << sp->enemyPos[i].ypos << '\n';
    }
}
