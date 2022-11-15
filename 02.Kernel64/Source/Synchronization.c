#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"

BYTE kLockForSystemData(void){
    return kSetInterruptFlag(FALSE);
}
void kUnlockForSystemData(BYTE bInterruptFlag){
    kSetInterruptFlag(bInterruptFlag);
}

void kInitializeMutex(MUTEX* pstMutex){
    pstMutex->qwTaskID=TASK_INVALID;
    pstMutex->bLockFlag=FALSE;
    pstMutex->dwLockCount=0;
}

void kLock(MUTEX* pstMutex){
    if(pstMutex->bLockFlag==TRUE){
        if(pstMutex->qwTaskID==kGetRunningTask()->stLink.qwID){
            pstMutex->dwLockCount++;
            return;
        }
        else{
            while(pstMutex->bLockFlag==TRUE){
                kSchedule();
            }
        }
    }
    pstMutex->bLockFlag=TRUE;
    pstMutex->qwTaskID=kGetRunningTask()->stLink.qwID;
    pstMutex->dwLockCount=1;
}
void kUnlock(MUTEX* pstMutex){
    if((pstMutex->bLockFlag==FALSE)||(pstMutex->qwTaskID!=kGetRunningTask()->stLink.qwID)){
        return;
    }
    if(pstMutex->dwLockCount>1){
        pstMutex->dwLockCount--;
        return;
    }
    pstMutex->qwTaskID=TASK_INVALID;
    pstMutex->dwLockCount=0;
    pstMutex->bLockFlag=FALSE;
}