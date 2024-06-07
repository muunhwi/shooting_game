#include "Enemy.h"
#include "Common.h"
#include <utility>
bool setCharField(char* ret, const char* str, const char* msg);
bool setIntField(int* ret, char* snum, const char* msg);
bool setEnemyTypeField(EnemyType* ep, MovementContainer* mv, char* field);
int partition(Enemy* ep, int left, int right);
EnemyTypeContainer* et;

bool makeEnemyType()
{
    
    if (!mv)
    {
        return false;
    }

    char* buffer = getFile(ENEMY_TYPE_FILE);

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
        std::cerr << "Failed to read the enemy type header " << '\n';
        return false;
    }

    et = (EnemyTypeContainer*)malloc(sizeof(EnemyTypeContainer));
    if (!et)
    {
        std::cerr << "Failed to make the EnemyType" << '\n';
        return false;
    }
        
    et->types = (EnemyType*)malloc(sizeof(EnemyType) * size);
    et->size = size;

    EnemyType* ep = et->types;
    while (ss.getline(fileName, MAX_BUFFER_SIZE))
    {
        char* enemyType = getFile(fileName);
        inss.str(enemyType);

        ep->timer = 0;
        while (inss.getline(fileDetail, MAX_BUFFER_SIZE))
        {
            
            if (!setEnemyTypeField(ep, mv, fileDetail))
                return false;
        }
        ep++;
        inss.clear();
        free(enemyType);
    }

    free(buffer);

    return true;
}

void deleteEnemyType()
{
    free(et->types);
    free(et);
}

bool setCharField(char* field, const char* str, const char* fieldName)
{
    if (str[0] < 33 || str[0] > 126)
    {
        std::cerr << "Failed to extractt the " << fieldName << " field\n";
        return false;
    }
    field[0] = str[0];

    return true;
}

bool setIntField(int* field, char* snum, const char* fieldName)
{
    if (!my_atoi(field, snum))
    {
        std::cerr << "Failed to extract the " << fieldName << " field\n";
        return false;
    }
    return true;
}

bool setEnemyTypeField(EnemyType* et, MovementContainer* mv, char* field)
{
    const char* del = " .:";
    char* tokenList[20];
  
    int tokenNum = getTokenList(field, del, tokenList);
    int fieldNum;
    
    if (tokenNum == 0)
    {
        std::cerr << "Failed to extract field\n";
        return false;
    }

    my_atoi(&fieldNum, tokenList[0]);
  
    switch (fieldNum)
    {
        case 1:
            return setCharField(&et->shape, tokenList[2], "shape");
        case 2:
            return setCharField(&et->attackShape, tokenList[2], "attackShape");
        case 3:
            return setIntField(&et->cooldown, tokenList[2], "cooldown");
        case 4:
            return setIntField(&et->perAttackCount, tokenList[2], "perAttackCount");
        case 5:
            for (int i{ 0 }; i < mv->size; ++i)
            {
                if (strcmp(mv->movements[i].name, tokenList[2]) == 0)
                {
                    et->mv = mv->movements + i;
                    return true;
                }
            }
            std::cerr << "Failed to extract the movements field\n";
            return false;
        default:
            std::cerr << "Failed to extract field\n";
            return false;
    }

}

void myEnemyQuickSort(Enemy* ep, int begin, int end)
{
    if (begin < end)
    {
        int p = partition(ep, begin, end);
        myEnemyQuickSort(ep, begin, p - 1);
        myEnemyQuickSort(ep, p+1, end);
    }
}

int partition(Enemy* ep, int left, int right)
{
    std::swap(ep[(left + right) / 2], ep[left]);
    int pivot = left;
    int py = ep[pivot].ypos;

    right++;
    while (left <= right) 
    {
        do
        {
            ++left;
        } while (ep[left].ypos > py);

        do
        {
            --right;
        } while (ep[right].ypos < py);

        if (left > right) break;
        std::swap(ep[left], ep[right]);
    }

    std::swap(ep[pivot], ep[right]);
    return right;
}