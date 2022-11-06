#include "Queue.h"
#include "Utility.h"

void kInitializeQueue(QUEUE* pstQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize){
    pstQueue->iDataSize=iDataSize;
    pstQueue->pvQueueArray=pvQueueBuffer;
    pstQueue->iPutIndex=0;
    pstQueue->iGetIndex=0;
    pstQueue->bLastOperationPut=FALSE;
}

BOOL kIsQueueFull(const QUEUE* pstQueue){
    return (pstQueue->iGetIndex==pstQueue->iPutIndex)&&(pstQueue->bLastOperationPut)?TRUE:FALSE;
}

BOOL kIsQueueEmpty(const QUEUE* pstQueue){
    return (pstQueue->iGetIndex==pstQueue->iPutIndex)&&(!pstQueue->bLastOperationPut)?TRUE:FALSE;
}


BOOL kPutData(QUEUE* pstQueue, const void* pvData){
    if(kIsQueueFull(pstQueue)){
        return FALSE;
    }
    kMemCpy((char*) pstQueue->pvQueueArray+(pstQueue->iDataSize*pstQueue->iPutIndex), pvData, pstQueue->iDataSize);
    pstQueue->iPutIndex=(pstQueue->iPutIndex+1)&pstQueue->iMaxDataCount;
    pstQueue->bLastOperationPut=TRUE;
    return TRUE;
}
BOOL kGetData(QUEUE* pstQueue, void* pvData){
    if(kIsQueueEmpty(pstQueue)){
        return FALSE;
    }
    kMemCpy(pvData, (char*) pstQueue->pvQueueArray+(pstQueue->iDataSize*pstQueue->iGetIndex), pstQueue->iDataSize);
    pstQueue->iGetIndex=(pstQueue->iGetIndex+1)&pstQueue->iMaxDataCount;
    pstQueue->bLastOperationPut=FALSE;
    return TRUE;
}