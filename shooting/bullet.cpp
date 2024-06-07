#include "Bullet.h"
#include <iostream>
#include <memory.h>
#include <stdlib.h>
#include <utility>


/*
int partition(Bullet* ep, int left, int right);
void mqInit(MyBulletQueue* q)
{
    q->front = 0;
    q->rear = 0;
}
bool mqFull(MyBulletQueue* q)
{
    if (q->front == (q->rear + 1) % MAX_QUEUE_SIZE)
        return true;

    return false;
}
bool mqEmpty(MyBulletQueue* q)
{
    if (q->front == q->rear)
        return true;

    return false;
}
void mqPush(MyBulletQueue* q, Bullet* elem)
{
    if (mqFull(q))
        return;

    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    memcpy(q->data + q->rear, elem, sizeof(Bullet));

}
Bullet* mqFront(MyBulletQueue* q)
{
    if (mqEmpty(q))
    {
        return nullptr;
    }

    return q->data + (q->front + 1);
}
void mqPop(MyBulletQueue* q)
{
    if (mqEmpty(q))
        return;

    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
}
void myBulletQuickSort(Bullet* ep, int begin, int end)
{
    if (begin < end)
    {
        int p = partition(ep, begin, end);
        myBulletQuickSort(ep, begin, p - 1);
        myBulletQuickSort(ep, p + 1, end);
    }
}
int partition(Bullet* ep, int left, int right)
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
*/
void mlInit(MyBulletList* list)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->next = nullptr;

    list->prev = nullptr;
    list->cur = nullptr;
    list->head = newNode;
    list->size = 0;
}
void mlInsert(MyBulletList* list, Bullet* b)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    memcpy(&newNode->data, b, sizeof(Bullet));

    newNode->next = list->head->next;
    list->head->next = newNode;

    (list->size)++;
}

bool mlFirst(MyBulletList* list, Bullet** b)
{
    if (!list->head->next)
        return false;

    list->prev = list->head;
    list->cur = list->head->next;
    *b = &(list->cur->data);

    return true;
}

bool mlFirst(MyBulletList* list)
{
    if (!list->head->next)
        return false;

    list->prev = list->head;
    list->cur = list->head->next;

    return true;
}

bool mlNext(MyBulletList* list, Bullet** b)
{
    if (!list->cur->next)
        return false;

    list->prev = list->cur;
    list->cur = list->cur->next;
    *b = &(list->cur->data);

    return true;
}
bool mlNext(MyBulletList* list)
{
    if (!list->cur->next)
        return false;

    list->prev = list->cur;
    list->cur = list->cur->next;
  
    return true;
}

void mlRemove(MyBulletList* list)
{
    Node* del = list->cur;

    list->prev->next = list->cur->next;
    list->cur = list->prev;
    list->size--;

    free(del);
}

void mlRemoveAll(MyBulletList* list)
{
   
    if (mlFirst(list))
    {
        mlRemove(list);
        while (mlNext(list))
            mlRemove(list); 
    }
}