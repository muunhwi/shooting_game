#pragma once

constexpr int MAX_BULLET_COUNT = 100;
constexpr int MAX_QUEUE_SIZE = 500;

struct Bullet
{
	char shape;
	bool isPlayerBullet;
	bool reverse;
	int xpos;
	int ypos;
};

struct Node
{
	Bullet data;
	Node* next;
};

struct MyBulletList
{
	Node* head;
	Node* cur;
	Node* prev;
	int size;
};

void mlInit(MyBulletList* list);
void mlInsert(MyBulletList* list, Bullet* b);
void mlRemove(MyBulletList* list);
bool mlFirst(MyBulletList* list, Bullet** b);
bool mlNext(MyBulletList* list, Bullet** b);
void mlRemoveAll(MyBulletList* list);

/*
struct MyBulletQueue
{
	Bullet data[MAX_QUEUE_SIZE];
	int front;
	int rear;
};
void mqInit(MyBulletQueue* q);
void mqPush(MyBulletQueue* q, Bullet* elem);
Bullet* mqFront(MyBulletQueue* q);
void mqPop(MyBulletQueue* q);
bool mqEmpty(MyBulletQueue* q);
bool mqFull(MyBulletQueue* q);

void myBulletQuickSort(Bullet* ep, int begin, int end);
*/