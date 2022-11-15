#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H
#include "Type.h"


BYTE kLockForSystemData(void);
void kUnlockForSystemData(BYTE bInterruptFlag);

#pragma pack(push, 1)
typedef struct kMutexStruct{
    volatile QWORD qwTaskID;
    volatile DWORD dwLockCount;
    volatile BOOL bLockFlag;

    BYTE vbPadding[3];
}MUTEX;
#pragma pack(pop)

void kInitializeMutex(MUTEX* pstMutex);
void kLock(MUTEX* pstMutex);
void kUnlock(MUTEX* pstMutex);

#endif