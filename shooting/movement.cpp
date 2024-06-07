#include "movement.h"
#include "Common.h"

bool setMovementField(Movement* mp, char* fileName, char* fileDetail);
bool setMoveData(Movement* mp, char* moveStr, char** moveList, int* index);

MovementContainer* mv;

bool makeMovement()
{
    char* buffer = getFile(MOVEMENTS_FILE);

    if (!buffer)
        return false;
   
    std::istringstream ss(buffer);
    std::istringstream inss;

    char fileName[MAX_BUFFER_SIZE];
    char fileDetail[MAX_BUFFER_SIZE];

    char fileSize[MAX_HEADERSIZE];
    int size;

    ss.getline(fileSize, MAX_BUFFER_SIZE);
    if (!my_atoi(&size, fileSize))
    {
        std::cerr << "Failed to read the movements header " << '\n';
        return false;
    }

    mv = (MovementContainer*)malloc(sizeof(MovementContainer));

    if (!mv)
    {
        std::cerr << "Failed to make the movement" << '\n';
        return false;
    }

    mv->movements = (Movement*)malloc(sizeof(Movement) * size);
    mv->size = size;


    Movement* mp = mv->movements;
    while (ss.getline(fileName, MAX_BUFFER_SIZE))
    {
        char* movement = getFile(fileName);
        inss.str(movement);

        mp->moveIndex = 0;
        while (inss.getline(fileDetail, MAX_BUFFER_SIZE))
        {
            if (!setMovementField(mp, fileName, fileDetail))
                return false;
        }
        mp++;
        inss.clear();
        free(movement);
    }

    free(buffer);
    return true;
}


void deleteMovement()
{
    free(mv->movements);
    free(mv);
}

bool setMovementField(Movement* mp, char* fileName, char* fileDetail)
{
    const char* del = " .*";
    char* tokenList[MAX_MOVEMENT] = { 0, };
    char* moveList[MAX_MOVEMENT] = { 0, };

    int code;
    int moveCount = 0;

    int tokenNum = getTokenList(fileDetail, del, tokenList);
    my_atoi(&code, tokenList[0]);

    switch (code)
    {
        case 1:
            strcpy_s(mp->name, tokenList[1]);
            return true;
        case 2:
            for (int i = 1; i < tokenNum; ++i)
            {
                if (!setMoveData(mp, tokenList[i], moveList, &moveCount))
                    return false;
            }
            mp->moveCount = moveCount;
            return true;
        case 3:
            if (strlen(tokenList[1]) < MAX_STATE)
            {
                std::cerr << "Movement stateFields are incorrect\n";
                return false;
            }

            if (tokenList[1][0] == '1')
                mp->xHalf = true;
            else
                mp->xHalf = false;
            
            if (tokenList[1][1] == '1')
                mp->yHalf = true;
            else
                mp->yHalf = false;

            if (tokenList[1][2] == '1')
                mp->yOver = true;
            else
                mp->yOver = false;

            return true;
        default:
            std::cerr << "The file " << fileName << " format is incorrect" << '\n';
            return false;
    }
}
bool setMoveData(Movement* mp, char* moveStr, char** moveList, int* index)
{
    int xpos, ypos, vpos;
    int posNum = getTokenList(moveStr, " ,()", moveList);

    if (!(my_atoi(&xpos, moveList[0]) &&
          my_atoi(&ypos, moveList[1])))
    {
        std::cerr << "The coordinates are incorrect\n";
        return false;
    }

    if (posNum > 2)
    {
        if (!my_atoi(&vpos, moveList[3]))
        {
            std::cerr << "The coordinates are incorrect\n";
            return false;
        }

        switch (moveList[2][0])
        {
            case 'x':
                for (int i{ 0 }; i < vpos; ++i)
                {
                    mp->moveData[*index][0] = xpos;
                    mp->moveData[*index][1] = ypos;
                    (*index)++;
                }
        }
    }
    else
    {
        mp->moveData[*index][0] = xpos;
        mp->moveData[*index][1] = ypos;
        (*index)++;
    }

    return true;
}