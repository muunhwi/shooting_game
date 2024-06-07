#include <stdio.h>
#include <memory.h>
#include <Windows.h>
#include <iostream>
#include "Console.h"
#include "Stage.h"
#include "Bullet.h"
#include "Player.h"
#pragma comment(lib, "winmm.lib")
#undef max

namespace
{
	char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];
	char Title[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];
	char Load[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];
	char Gameover[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];
	char Win[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];

	enum GameState
	{
		TITLE,
		LOAD,
		GAME,
		OVER,
		WIN,
		EXIT
	};

	int FRAME = 0;
	int RANDER = 0;
	DWORD FRAME_DELAY = 50;

	Player p;
	PlayerTitlePos titlePos{ '>', 34, 10 };
	bool isHit = false;

	GameState gameState = GameState::TITLE;
	MyBulletList list;

	int LEVEL = 0;
	Enemy enemyPos[MAX_ENEMY_COUNT];
	int aliveCount;
	int enemyPosCount;

	bool attackKeyPressed = false;
}


void Buffer_Flip(void);
void Buffer_Clear(void);
void Sprite_Draw(int iX, int iY, char chSprite);

void setCurrentPlayer();
void setCurrentEnemyMovement();
void setCurrentBullet();

void updatePlayer();
void updateEnemyMovement();
void updateBullet();

void playerInit();
void stageInit();

bool setUp();
void clear();

void makeTitle();
void makeLoad();
void makeGameOver();
void makeWin();

void printTitle();
void printGameOver();
void printLoad();
void printWin();

GameState moveTitleBar();
GameState inputLoad();
GameState moveGameOverBar();
GameState moveWinBar();

bool isEnemyHit(Bullet* bp, Enemy* ep);

int main(void)
{
	timeBeginPeriod(1);
	
	cs_Initial();
	if (!setUp()) return 0;

	DWORD oldTick = timeGetTime();
	DWORD stdTick = timeGetTime();
	DWORD curTick;
	DWORD dwTime;

	while (1)
	{

		curTick = timeGetTime();
		//FRAME++; // 한바퀴 돌았을 떄 1FPS
		if (curTick - stdTick >= 1000)
		{
			std::cout << "FRAME " << FRAME << '\n';
			std::cout << "RANDER " << RANDER << '\n';
			FRAME = 0;
			RANDER = 0;
			stdTick += 1000;
		}
		switch (gameState)
		{
			case GameState::TITLE:
				printTitle();
				gameState = moveTitleBar();	
				break;
			case GameState::LOAD:
				printLoad();
				gameState = inputLoad();
				break;
			case GameState::GAME:
			{
				int fixedUpdate = (curTick - oldTick) / FRAME_DELAY;
				while (fixedUpdate > -1)
				{
					updatePlayer();
					updateEnemyMovement();
					updateBullet();
					Buffer_Clear();

					setCurrentPlayer();
					setCurrentEnemyMovement();
					setCurrentBullet();

					if (isHit)
					{
						gameState = GameState::OVER;
						break;
					}
					else if (aliveCount == 0)
					{
						gameState = GameState::WIN;
						break;
					}
					else
					{
						gameState = GameState::GAME;
					}
					FRAME++;
					oldTick += FRAME_DELAY;
					fixedUpdate--;
				}
				
				if (gameState == GameState::GAME)
				{
					oldTick -= FRAME_DELAY;
					Buffer_Flip();
					RANDER++;
					//Sleep(35);
				}	
				break;
			}
			case GameState::OVER:
				Buffer_Clear();
				printGameOver();
				gameState = moveGameOverBar();

				if (gameState != GameState::OVER)
					cs_ClearScreen();

				break;
			case GameState::WIN:
				printWin();
				gameState = moveWinBar();
				break;
			default:
				clear();
				timeEndPeriod(1);
				return 0;
		}
		
		dwTime = timeGetTime() - oldTick;

		if (dwTime < FRAME_DELAY)
			Sleep(FRAME_DELAY - dwTime);

		oldTick += FRAME_DELAY;       
	}
	
	clear();
	timeEndPeriod(1);
	return 0;

}

bool isEnemyHit(Bullet* bp, Enemy* ep)
{
	if (!ep->isAlive)
		return false;

	if (ep->xpos == bp->xpos &&
		ep->ypos == bp->ypos)
		return true;

	if (ep->xpos == bp->xpos &&
		ep->ypos == bp->ypos +1)
		return true;

	if (ep->xpos == bp->xpos &&
		ep->ypos == bp->ypos - 1)
		return true;

	return false;
}

void setCurrentPlayer()
{
	Sprite_Draw(p.xpos, p.ypos, p.shape);
}
void updatePlayer()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		if (p.xpos > 0)
			p.xpos--;
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		if (p.xpos < dfSCREEN_WIDTH - 2)
			p.xpos++;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		if (p.ypos > 0)
			p.ypos--;
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		if (p.ypos < dfSCREEN_HEIGHT - 1)
			p.ypos++;
	}

	if (!attackKeyPressed)
	{
		if (GetAsyncKeyState(0x5A)) // z
		{
			if (p.ypos > 0)
			{
				Bullet b = { p.attackShape, true, false, p.xpos, p.ypos - 1 };
				mlInsert(&list, &b);
			}
		}
		else if (GetAsyncKeyState(0x58)) // x
		{
			if (p.ypos < dfSCREEN_HEIGHT-1)
			{
				Bullet b = { p.attackShape, true, true, p.xpos, p.ypos + 1 };
				mlInsert(&list, &b);
			}
		}
		attackKeyPressed = true;
	}
	else
		attackKeyPressed = false;

}

void setCurrentEnemyMovement()
{
	for (int i{ 0 }; i < enemyPosCount; ++i)
	{
		if (enemyPos[i].isAlive)
		{
			if (szScreenBuffer[enemyPos[i].ypos][enemyPos[i].xpos] == p.shape)
			{
				isHit = true;
				return;
			}
			Sprite_Draw(enemyPos[i].xpos, enemyPos[i].ypos, enemyPos[i].type.shape);
		}
			
	}
}
void updateEnemyMovement()
{
	int moveX, moveY;
	int curX, curY, moveIndex;
		
	for (int i{ 0 }; i < enemyPosCount; ++i)
	{
		if (!enemyPos[i].isAlive)
			continue;

		moveIndex = enemyPos[i].mv.moveIndex % enemyPos[i].mv.moveCount;
		
		moveX = enemyPos[i].mv.moveData[moveIndex][0];
		moveY = enemyPos[i].mv.moveData[moveIndex][1];

		curX = enemyPos[i].xpos;
		curY = enemyPos[i].ypos;

		if ((curX + moveX > -1 && curX + moveX < dfSCREEN_WIDTH - 1) &&
			(curY + moveY > -1 && curY + moveY < dfSCREEN_HEIGHT))
		{
			if (enemyPos[i].mv.xHalf && curX + moveX > (dfSCREEN_WIDTH - 1) / 2)
			{
				enemyPos[i].mv.moveData[moveIndex][0] = -moveX;
				enemyPos[i].xpos = curX - moveX;
			}
			else
				enemyPos[i].xpos = curX + moveX;

			if (enemyPos[i].mv.yHalf && curY + moveY > (dfSCREEN_HEIGHT) / 2)
			{
				
				enemyPos[i].mv.moveData[moveIndex][1] = -moveY;
				enemyPos[i].ypos = curY - moveY;
				
			}
			else	
				enemyPos[i].ypos = curY + moveY;
		}
		else
		{
			if (moveX > 0 || moveX < 0)
				enemyPos[i].mv.moveData[moveIndex][0] = -moveX;

			if (!enemyPos[i].mv.yOver && (moveY > 0 || moveY < 0))
				enemyPos[i].mv.moveData[moveIndex][1] = -moveY;

			enemyPos[i].xpos = curX + -moveX;

			if (enemyPos[i].mv.yOver)
			{
				if (curY + moveY < 0)
					enemyPos[i].ypos = dfSCREEN_HEIGHT - 1;
				else
					enemyPos[i].ypos = 0;
			}
			else
				enemyPos[i].ypos = curY + -moveY;
		}

		if (++(enemyPos[i].mv.moveIndex) == enemyPos[i].mv.moveCount)
			enemyPos[i].mv.moveIndex = 0;
	}

}

void setCurrentBullet()
{
	Bullet* bp;

	if (mlFirst(&list, &bp))
	{
		if (!bp->isPlayerBullet)
		{
			if (szScreenBuffer[bp->ypos][bp->xpos] == p.shape)
			{
				isHit = true;
				return;
			}
			Sprite_Draw(bp->xpos, bp->ypos, bp->shape);
		}
		else
		{
			if (szScreenBuffer[bp->ypos][bp->xpos]   != ' ' ||
				szScreenBuffer[bp->ypos+1][bp->xpos] != ' ')
			{
				for (int i{ 0 }; i < enemyPosCount; ++i)
				{
					if (isEnemyHit(bp, enemyPos + i))
					{
						aliveCount--;
						enemyPos[i].isAlive = false;
						break;
					}
				}
			}
			Sprite_Draw(bp->xpos, bp->ypos, bp->shape);
		}

		while (mlNext(&list, &bp))
		{
			if (!bp->isPlayerBullet)
			{
				if (szScreenBuffer[bp->ypos][bp->xpos] == p.shape)
				{
					isHit = true;
					return;
				}
				Sprite_Draw(bp->xpos, bp->ypos, bp->shape);
			}
			else
			{
				if (szScreenBuffer[bp->ypos][bp->xpos]     != ' ' ||
					szScreenBuffer[bp->ypos +1][bp->xpos] != ' ')
				{
					for (int i{ 0 }; i < enemyPosCount; ++i)
					{
						if (isEnemyHit(bp, enemyPos + i))
						{
							aliveCount--;
							enemyPos[i].isAlive = false;
							break;
						}
					}
				}
				Sprite_Draw(bp->xpos, bp->ypos, bp->shape);
			}
		}
	}
}
void updateBullet()
{
	int xpos, ypos;
	
	Bullet* bp;
	if (mlFirst(&list, &bp))
	{
		if (!bp->isPlayerBullet)
		{
			if (++bp->ypos >= dfSCREEN_HEIGHT)
				mlRemove(&list);
		}
		else
		{
			if (!bp->reverse)
			{
				if (--bp->ypos < 0)
					mlRemove(&list);
			}
			else
			{
				if (++bp->ypos >= dfSCREEN_HEIGHT)
					mlRemove(&list);
			}
		}
	
		while (mlNext(&list, &bp))
		{
			if (!bp->isPlayerBullet)
			{
				if (++bp->ypos >= dfSCREEN_HEIGHT)
					mlRemove(&list);
			}
			else
			{
				if (!bp->reverse)
				{
					if (--bp->ypos < 0)
						mlRemove(&list);
				}
				else
				{
					if (++bp->ypos >= dfSCREEN_HEIGHT)
						mlRemove(&list);
				}
			}
			
		}
	}
	
	for (int i{ 0 }; i < enemyPosCount; ++i)
	{
		if (!enemyPos[i].isAlive)
			continue;

		xpos = enemyPos[i].xpos;
		ypos = enemyPos[i].ypos;

		if (FRAME == 0)
			enemyPos[i].type.timer++;

		if (enemyPos[i].type.timer == (enemyPos[i].type.cooldown))
		{
			enemyPos[i].type.timer = 0;
			for (int j{ enemyPos[i].type.perAttackCount }; j > 0; --j)
			{
				Bullet b = { enemyPos[i].type.attackShape, false, false, xpos, ypos + j  };
				mlInsert(&list, &b);
			}
		}
	}
}

void stageInit()
{
	mlRemoveAll(&list);
	memcpy(enemyPos, stages->si[LEVEL].enemyPos, MAX_ENEMY_COUNT * sizeof(Enemy));
	enemyPosCount = stages->si[LEVEL].enemyPosCount;
	aliveCount = stages->si[LEVEL].enemyPosCount;
}
void playerInit() 
{
	isHit = false;
	p.shape = 'P';
	p.attackShape = '.';
	p.xpos = (dfSCREEN_WIDTH-1)/2;
	p.ypos = dfSCREEN_HEIGHT - 1;
}

bool setUp()
{
	if (!makeMovement())
		return false;

	if (!makeEnemyType())
		return false;

	if (!makeStage())
		return false;

	mlInit(&list);
	makeTitle();
	makeLoad();
	makeGameOver();
	makeWin();

	return true;
}
void clear()
{
	mlRemoveAll(&list);
	deleteMovement();
	deleteEnemyType();
	deleteStageInfo();
}

void makeTitle()
{
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i) {
		for (int j = 0; j < dfSCREEN_WIDTH - 1; ++j) {
			Title[i][j] = ' ';
		}
		Title[i][dfSCREEN_WIDTH - 1] = '\0';
	}
	const char* one = "SHOOTING GAME";
	const char* two = "GAME START";
	const char* three = "SET STAGE";
	const char* four = "EXIT";
	const char* five = "menu select enter key";
	const char* six = "menu move arrows keys";
	const char* seven = "attack z key ";
	const char* eight = "reverse attack x key";
	
	strncpy_s(Title[8] + 34, dfSCREEN_WIDTH - 34, one, strlen(one));
	strncpy_s(Title[10] + 35, dfSCREEN_WIDTH - 35, two, strlen(two));
	strncpy_s(Title[11] + 35, dfSCREEN_WIDTH - 35, three, strlen(three));
	strncpy_s(Title[12] + 35, dfSCREEN_WIDTH - 35, four, strlen(four));
	strncpy_s(Title[14] + 35, dfSCREEN_WIDTH - 35, five, strlen(five));
	strncpy_s(Title[15] + 35, dfSCREEN_WIDTH - 35, six, strlen(six));
	strncpy_s(Title[16] + 35, dfSCREEN_WIDTH - 35, seven, strlen(seven));
	strncpy_s(Title[17] + 35, dfSCREEN_WIDTH - 35, eight, strlen(eight));


	Title[titlePos.ypos][titlePos.xpos] = titlePos.shape;
}
void printTitle()
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		std::cout << Title[iCnt];
	}
}
GameState moveTitleBar()
{
	Title[titlePos.ypos][titlePos.xpos] = ' ';
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (10 < titlePos.ypos)
		{
			titlePos.ypos--;
		}
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (titlePos.ypos < 12)
		{
			titlePos.ypos++;
		}
	}
	else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (titlePos.ypos == 10)
		{
			playerInit();
			LEVEL = 0;
			stageInit();
			return GameState::GAME;
		}
		else if (titlePos.ypos == 11)
		{
			titlePos.ypos = 10;
			return GameState::LOAD;
		}
		else
		{
			return GameState::EXIT;
		}

	}
	Title[titlePos.ypos][titlePos.xpos] = titlePos.shape;
		
	
	

	return GameState::TITLE;
}

void makeLoad()
{
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i) 
	{
		for (int j = 0; j < dfSCREEN_WIDTH - 1; ++j) 
			Load[i][j] = ' ';
		
		Load[i][dfSCREEN_WIDTH - 1] = '\0';
	}

	char size = stages->size + '0';
	char one[20] = "STAGE 1 ~ ";

	char* p = one;
	while (*p) p++;
	*p = size;
	*(p + 1) = '\0';

	const char* two = "ENTER THE STAGE : ";

	strncpy_s(Load[8], dfSCREEN_WIDTH, one, strlen(one));
	strncpy_s(Load[10], dfSCREEN_WIDTH , two, strlen(two));

}
void printLoad()
{
	cs_ClearScreen();
	cs_Uninital();
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		std::cout << Load[iCnt];
	}
}
GameState inputLoad()
{
	cs_MoveCursor(0, 11);

	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin >> LEVEL;
	
	if (!std::cin)
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return GameState::LOAD;
	}

	if (LEVEL < 0 || LEVEL > stages->size)
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return GameState::LOAD;
	}

	LEVEL--;

	cs_Initial();
	stageInit();
	Buffer_Clear();

	return GameState::GAME;
}

void makeGameOver()
{
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i) {
		for (int j = 0; j < dfSCREEN_WIDTH - 1; ++j) {
			Gameover[i][j] = ' ';
		}
		Gameover[i][dfSCREEN_WIDTH - 1] = '\0';
	}

	const char* one  = "GAVE OVER";
	const char* two = "TRY AGAIN?";
	const char* three  = "EXIT";

	strncpy_s(Gameover[8] + 34, dfSCREEN_WIDTH - 34, one, strlen(one));
	strncpy_s(Gameover[10] + 35, dfSCREEN_WIDTH - 35, two, strlen(two));
	strncpy_s(Gameover[11] + 35, dfSCREEN_WIDTH - 35, three, strlen(three));

	Gameover[titlePos.ypos][titlePos.xpos] = titlePos.shape;
}
void printGameOver()
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		std::cout << Gameover[iCnt];
	}
}
GameState moveGameOverBar()
{
	Gameover[titlePos.ypos][titlePos.xpos] = ' ';
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (titlePos.ypos == 11)
			titlePos.ypos--;

	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (titlePos.ypos == 10)
			titlePos.ypos++;

	}
	else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (titlePos.ypos == 11)
		{
			mlRemoveAll(&list);
			titlePos.ypos = 10;
			return GameState::EXIT;
		}
		else
		{
			playerInit();
			stageInit();
			return GameState::GAME;
		}
	}
	Gameover[titlePos.ypos][titlePos.xpos] = titlePos.shape;

	return GameState::OVER;
}

void makeWin()
{
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		for (int j = 0; j < dfSCREEN_WIDTH - 1; ++j)
			Win[i][j] = ' ';

		Win[i][dfSCREEN_WIDTH - 1] = '\0';
	}

	const char* one = "YOU WIN";
	const char* two = "NEXT STAGE";
	const char* three = "EXIT";

	strncpy_s(Win[8] + 34, dfSCREEN_WIDTH - 34, one, strlen(one));
	strncpy_s(Win[10] + 35, dfSCREEN_WIDTH - 35, two, strlen(two));
	strncpy_s(Win[11] + 35, dfSCREEN_WIDTH - 35, three, strlen(three));

}
void printWin()
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		std::cout << Win[iCnt];
	}
}
GameState moveWinBar()
{
	Win[titlePos.ypos][titlePos.xpos] = ' ';

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (titlePos.ypos == 11)
			titlePos.ypos--;

	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (titlePos.ypos == 10)
			titlePos.ypos++;

	}
	else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (titlePos.ypos == 10)
		{
			if (LEVEL == stages->size - 1)
			{
				mlRemoveAll(&list);
				return GameState::TITLE;
			}
			else
			{
				++LEVEL;
				playerInit();
				stageInit();
				return GameState::GAME;
			}
		}
		else
		{
			return GameState::EXIT;
		}
	}

	Win[titlePos.ypos][titlePos.xpos] = titlePos.shape;
	
		
	
	return GameState::WIN;
}

void Buffer_Flip(void)
{
	int iCnt;
	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		cs_MoveCursor(0, iCnt);
		std::cout << szScreenBuffer[iCnt];
		//printf(szScreenBuffer[iCnt]);
	}
}
void Buffer_Clear(void)
{
	int iCnt;
	memset(szScreenBuffer, ' ', dfSCREEN_WIDTH * dfSCREEN_HEIGHT);

	for (iCnt = 0; iCnt < dfSCREEN_HEIGHT; iCnt++)
	{
		szScreenBuffer[iCnt][dfSCREEN_WIDTH - 1] = '\0';
	}

}
void Sprite_Draw(int iX, int iY, char chSprite)
{
	if (iX < 0 || iY < 0 || iX >= dfSCREEN_WIDTH - 1 || iY >= dfSCREEN_HEIGHT)
		return;

	szScreenBuffer[iY][iX] = chSprite;
}