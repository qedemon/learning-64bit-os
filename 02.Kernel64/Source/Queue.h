#ifndef QUEUE_H
#define QUEUE_H
#include "Type.h"

typedef struct kQueueManagerStruct{
    int iDataSize;
    int iMaxDataCount;

    void* pvQueueArray;
    int iPutIndex;
    int iGetIndex;

    BOOL bLastOperationPut;
}QUEUE;

void kInitializeQueue(QUEUE* pstQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize);
BOOL kIsQueueFull(const QUEUE* pstQueue);
BOOL kIsQueueEmpty(const QUEUE* pstQueue);
BOOL kPutData(QUEUE* pstQueue, const void* pvData);
BOOL kGetData(QUEUE* pstQueue, void* pvData);

#endif