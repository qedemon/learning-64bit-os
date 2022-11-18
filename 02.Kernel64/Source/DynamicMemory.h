#ifndef DYNAMIC_MEMORY_H
#define DYNAMIC_MEMORY_H

#include "Type.h"
#include "Task.h"

#define DYNAMICMEMORY_START_ADDRESS ((TASK_STACKPOOLADDRESS+TASK_STACKSIZE*TASK_MAXCOUNT+0xFFFFF)&0xFFFFFFFFFFF00000)
#define DYNAMICMEMORY_MIN_SIZE (1*1024)

#define DYNAMICMEMORY_EXIST 0x01
#define DYNAMICMEMORY_EMPTY 0x00

#pragma pack(push, 1)

typedef struct kBitmapStruct{
    BYTE* pbBitmap;
    QWORD qwExistBitCount;
}BITMAP;

typedef struct kDynamicMemoryManagerStruct{
    int iMaxLevelCount;
    int iBlockCountOfSmallestBlock;
    QWORD qwUsedSize;

    QWORD qwStartAddress;
    QWORD qwEndAddress;

    BYTE* pbAllocatedBlockListIndex;
    BITMAP* pstBitMapOfLevel;
}DYNAMICMEMORY;

#pragma pack(push)

void kInitializeDynamicMemory();
void* kAllocateDynamicMemory(QWORD qwRequiredSize);

#endif