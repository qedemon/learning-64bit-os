#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"
#include "AssemblyUtility.h"

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

void kLockMutex(MUTEX* pstMutex){
    if(!kTestAndSet(&(pstMutex->bLockFlag), FALSE, TRUE)){
        if(pstMutex->qwTaskID==kGetRunningTask()->stLink.qwID){
            pstMutex->dwLockCount++;
            return;
        }
        else{
            while(1){
                if(kTestAndSet(&(pstMutex->bLockFlag), FALSE, TRUE))
                    break;
                kSchedule();
            }
        }
    }
    pstMutex->qwTaskID=kGetRunningTask()->stLink.qwID;
    pstMutex->dwLockCount=1;
}
void kUnlockMutex(MUTEX* pstMutex){
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