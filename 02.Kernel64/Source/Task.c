#include "Type.h"
#include "Task.h"
#include "AssemblyUtility.h"
#include "Utility.h"
#include "Descriptor.h"
#include "LinkedList.h"
#include "TextModeTerminal.h"
#include "string.h"
#include "PIT.h"

void kInitializeTask(TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize){
    kMemSet(pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));
    pstTCB->stContext.vqRegister[TASK_RSPOFFSET]=(QWORD)pvStackAddress+qwStackSize;
    pstTCB->stContext.vqRegister[TASK_RBPOFFSET]=(QWORD)pvStackAddress+qwStackSize;

    pstTCB->stContext.vqRegister[TASK_CSOFFSET]=GDT_KERNELCODESEGMENT;
    pstTCB->stContext.vqRegister[TASK_DSOFFSET]=GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_ESOFFSET]=GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_FSOFFSET]=GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_GSOFFSET]=GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[TASK_SSOFFSET]=GDT_KERNELDATASEGMENT;

    pstTCB->stContext.vqRegister[TASK_RIPOFFSET]=qwEntryPointAddress;

    pstTCB->stContext.vqRegister[TASK_RFLAGSOFFSET]|=0x0200;

    pstTCB->qwFlags=qwFlags;
    pstTCB->pvStackAddress=pvStackAddress;
    pstTCB->qwStackSize=qwStackSize;
}

static TCBPOOLMANAGER gs_stTCBPoolManager;

void kInitializeTCBPool(){
    int i;
    kMemSet(&(gs_stTCBPoolManager), 0, sizeof(gs_stTCBPoolManager));
    gs_stTCBPoolManager.pstStartAddress=(TCB*)TASK_TCBPOOLADDRESS;
    kMemSet((char*)TASK_TCBPOOLADDRESS, 0, sizeof(TCB)*TASK_MAXCOUNT);
    for(i=0; i<TASK_MAXCOUNT; i++){
        gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID=i;
    }
    gs_stTCBPoolManager.iMaxCount=TASK_MAXCOUNT;
    gs_stTCBPoolManager.iAllocatedCount=1;
}

TCB* kAllocateTCB(){
    TCB* pstEmptyTCB;
    int i;
    if(gs_stTCBPoolManager.iUseCount>=gs_stTCBPoolManager.iMaxCount)
        return NULL;
    for(i=0; i<gs_stTCBPoolManager.iMaxCount; i++){
        if((gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID>>32)==0){
            pstEmptyTCB=&(gs_stTCBPoolManager.pstStartAddress[i]);
            pstEmptyTCB->stLink.qwID=(((QWORD)gs_stTCBPoolManager.iAllocatedCount)<<32)|i;
            break;
        }
    }
    gs_stTCBPoolManager.iUseCount++;
    gs_stTCBPoolManager.iAllocatedCount++;
    
    return pstEmptyTCB;
}
void kFreeTCB(QWORD qwID){
    int i;
    i=GETTCBOFFSET(qwID);
    kMemSet(&(gs_stTCBPoolManager.pstStartAddress[i]), 0, sizeof(TCB));
    gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID=i;
    gs_stTCBPoolManager.iUseCount--;
}

TCB* kGetTCBFromTCBPool(QWORD qwOffset){
    if(qwOffset>=gs_stTCBPoolManager.iMaxCount)
        return NULL;
    return &(gs_stTCBPoolManager.pstStartAddress[qwOffset]);
}

static SCHEDULER gs_stScheduler;

void kInitializeScheduler(){
    int i;
    kInitializeTCBPool();
    for(i=0; i<TASK_MAXREADYLISTCOUNT; i++){
        kInitializeList(&(gs_stScheduler.stReadyList[i]));
        gs_stScheduler.viExecuteCount[i]=0;
    }
    gs_stScheduler.pstRunningTask=kAllocateTCB();
    SETPRIORITY(gs_stScheduler.pstRunningTask->qwFlags, TASK_FLAG_HIGHEST);

    gs_stScheduler.qwProcessorLoad=0;
    gs_stScheduler.qwSpendProcessorTimeInIdleTask=0;
}

void kSetRuningTask(TCB* pstTCB){
    gs_stScheduler.pstRunningTask=pstTCB;
}

TCB* kGetRunningTask(){
    return gs_stScheduler.pstRunningTask;
}

TCB* kGetNextTaskToRun(){
    TCB* pstNextTask=NULL;
    int i, j;
    for(j=0; j<2; j++){
        for(i=0; i<TASK_PROCESSTIME; i++){
            if(gs_stScheduler.viExecuteCount[i]<kGetListCount(&gs_stScheduler.stReadyList[i])){
                pstNextTask=kRemoveLinkFromHead(&gs_stScheduler.stReadyList[i]);
                gs_stScheduler.viExecuteCount[i]++;
                break;
            }
            gs_stScheduler.viExecuteCount[i]=0;
        }
        if(pstNextTask!=NULL)
            break;
    }
    return pstNextTask;
}

BOOL kAddTaskToReadyList(TCB* pstTCB){
    BOOL bInterrupt;
    BYTE bPriority;
    bPriority=GETPRIORITY(pstTCB->qwFlags);
    if(bPriority>=TASK_MAXREADYLISTCOUNT)
        return FALSE;
    bInterrupt=kSetInterruptFlag(FALSE);
    kAddLinkToTail(&(gs_stScheduler.stReadyList[bPriority]), pstTCB);
    kSetInterruptFlag(bInterrupt);
    return TRUE;
}

TCB* kRemoveTaskFromReadyList(QWORD qwTaskID){
    TCB* pstTargetTask;
    BYTE bPriority;
    if(GETTCBOFFSET(qwTaskID)>=TASK_MAXCOUNT){
        return NULL;
    }
    pstTargetTask=&(gs_stTCBPoolManager.pstStartAddress[GETTCBOFFSET(qwTaskID)]);
    if(pstTargetTask->stLink.qwID!=qwTaskID){
        return NULL;
    }
    bPriority=GETPRIORITY(pstTargetTask->qwFlags);
    pstTargetTask=kRemoveLink(&(gs_stScheduler.stReadyList[bPriority]), qwTaskID);
    return pstTargetTask;
}

BOOL kChangePriority(QWORD qwTaskID, BYTE bPriority){
    TCB* pstTargetTask;
    TCB* pstRunningTask;
    BYTE bInterrupt;
    if(bPriority>=TASK_MAXREADYLISTCOUNT)
        return FALSE;
    bInterrupt=kSetInterruptFlag(FALSE);
    pstRunningTask=kGetRunningTask();
    if(pstRunningTask->stLink.qwID==qwTaskID){
        SETPRIORITY(pstTargetTask->qwFlags, bPriority);
    }
    else{
        pstTargetTask=kRemoveTaskFromReadyList(qwTaskID);
        SETPRIORITY(pstTargetTask->qwFlags, bPriority);
        kAddTaskToReadyList(pstTargetTask);
    }
    bInterrupt=kSetInterruptFlag(bInterrupt);
}

TCB* kCreateTask(QWORD qwFlag, QWORD qwEntryPointAddress){
    TCB* pstNewTask;
    void* pvStackAddress;
    pstNewTask=kAllocateTCB();
    if(pstNewTask==NULL)
        return NULL;
    pvStackAddress=(void*)(TASK_STACKPOOLADDRESS+GETTCBOFFSET(pstNewTask->stLink.qwID)*(TASK_STACKSIZE));
    kInitializeTask(pstNewTask, qwFlag, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);
    kAddTaskToReadyList(pstNewTask);
#ifdef TASK_DEBUG
    kprintf("0x%q -> 0x%q\n", pstNewTask, qwEntryPointAddress);
#endif
    return pstNewTask;
}

BOOL kEndTask(QWORD qwTaskID){
    TCB* pstTarget;
    BYTE bPriority;
    pstTarget=kGetRunningTask();
    if(pstTarget->stLink.qwID==qwTaskID){
        pstTarget->qwFlags|=TASK_FLAG_ENDTASK;
        SETPRIORITY(pstTarget->qwFlags, TASK_FLAG_WAIT);
        kSchedule();
        while(1);
    }
    else{
        pstTarget=kRemoveTaskFromReadyList(qwTaskID);
        if(pstTarget==NULL){
            pstTarget=kGetTCBFromTCBPool(GETTCBOFFSET(qwTaskID));
            if(pstTarget!=NULL){
                pstTarget->qwFlags|=TASK_FLAG_ENDTASK;
                SETPRIORITY(pstTarget->qwFlags, TASK_FLAG_WAIT);
            }
            return FALSE;
        }
        pstTarget->qwFlags|=TASK_FLAG_ENDTASK;
        SETPRIORITY(pstTarget->qwFlags, TASK_FLAG_WAIT);
        kAddLinkToTail(&gs_stScheduler.stWaitList, pstTarget);
    }
    return TRUE;
}

void kExitTask(){
    kEndTask(kGetRunningTask()->stLink.qwID);
}

void kIdleTask(){
    QWORD qwLastMeasureTickCount, qwLastSpendTickInIdleTask;
    QWORD qwCurrentMeasureTickCount, qwCurrentSpendTickInIdleTask;
    TCB* pstTask;
    qwLastMeasureTickCount=kGetTickCount();
    qwLastSpendTickInIdleTask=gs_stScheduler.qwSpendProcessorTimeInIdleTask;
    BYTE bInterrupt;
    while(1){
        qwCurrentMeasureTickCount=kGetTickCount();
        qwCurrentSpendTickInIdleTask=gs_stScheduler.qwSpendProcessorTimeInIdleTask;
        if(qwCurrentMeasureTickCount-qwLastMeasureTickCount==0){
            gs_stScheduler.qwProcessorLoad=0;
        }
        else{
            gs_stScheduler.qwProcessorLoad=100-(qwCurrentSpendTickInIdleTask-qwLastSpendTickInIdleTask)*100/(qwCurrentMeasureTickCount-qwLastMeasureTickCount);
        }
        qwLastMeasureTickCount=qwCurrentMeasureTickCount;
        qwLastSpendTickInIdleTask=qwCurrentSpendTickInIdleTask;
        kHaltProcessorByLoad();

        if(kGetListCount(&(gs_stScheduler.stWaitList))>0){
            while(1){
                pstTask=kRemoveLinkFromHead(&(gs_stScheduler.stWaitList));
                if(pstTask==NULL){
                    break;
                }
                bInterrupt=kSetInterruptFlag(FALSE);
                kprintf("IDLE: Task ID[0x%q] is completely ended.\n", pstTask->stLink.qwID);
                kSetInterruptFlag(bInterrupt);
                kFreeTCB(pstTask->stLink.qwID);
            }
        }
        kSchedule();
    }
}

void kClearOtherTask(){
    TCB* pstRunningTask;
    BOOL bInterrupt;
    bInterrupt=kSetInterruptFlag(FALSE);
    pstRunningTask=kGetRunningTask();
    while(1){
        TCB* otherTask=kGetNextTaskToRun();
        if(otherTask==NULL){
            break;
        }
        kFreeTCB(otherTask->stLink.qwID);
    }
    kSetInterruptFlag(bInterrupt);
}

void kSchedule(){
    TCB* pstRunningTask, *pstNextTask;
    BOOL bPreviousFlag;
    
    bPreviousFlag=kSetInterruptFlag(FALSE);
    pstNextTask=kGetNextTaskToRun();
    if(pstNextTask==NULL){
        int i;
        for(i; i<5; i++){
            kprintf("%d, ", gs_stScheduler.stReadyList[i].iItemCount);
        }
        kprintf("\n");
        kSetInterruptFlag(bPreviousFlag);
        return;
    }
    pstRunningTask=gs_stScheduler.pstRunningTask;
    if((pstRunningTask->qwFlags&TASK_FLAG_IDLE)==TASK_FLAG_IDLE){
        gs_stScheduler.qwSpendProcessorTimeInIdleTask+=TASK_PROCESSTIME-gs_stScheduler.iProcessorTime;
    }
    if((pstRunningTask->qwFlags&TASK_FLAG_ENDTASK)){
        kAddLinkToTail(&(gs_stScheduler.stWaitList), pstRunningTask);
        gs_stScheduler.pstRunningTask=pstNextTask;
        kSwitchContext(NULL, &(pstNextTask->stContext));
    }
    else{
        kAddTaskToReadyList(pstRunningTask);
        gs_stScheduler.pstRunningTask=pstNextTask;
        kSwitchContext(&(pstRunningTask->stContext), &(pstNextTask->stContext));
    }

    gs_stScheduler.iProcessorTime=TASK_PROCESSTIME;
    kSetInterruptFlag(bPreviousFlag);
}

BOOL kScheduleInInterupt(QWORD qwStackStartAddress){
    TCB* pstRunningTask, *pstNextTask;
    char* pcContextAddress;
    char vcBuffer[100];

    pstNextTask=kGetNextTaskToRun();
    if(pstNextTask==NULL){
        return FALSE;
    }
    pcContextAddress=(char*)(qwStackStartAddress-sizeof(CONTEXT));
#ifdef TASK_DEBUG
    kprintf("0x%q, 0x%q\n", &(((CONTEXT*)pcContextAddress)->vqRegister[TASK_RBPOFFSET]), qwStackBaseAddress);
#endif
    pstRunningTask=gs_stScheduler.pstRunningTask;
    if((pstRunningTask->qwFlags&TASK_FLAG_IDLE)==TASK_FLAG_IDLE){
        gs_stScheduler.qwSpendProcessorTimeInIdleTask+=TASK_PROCESSTIME;
    }
    if(pstRunningTask->qwFlags&TASK_FLAG_ENDTASK){
        kAddLinkToTail(&(gs_stScheduler.stWaitList), pstRunningTask);
    }
    else{
        kMemCpy(&(pstRunningTask->stContext), pcContextAddress, sizeof(CONTEXT));
        kAddLinkToTail(&(gs_stScheduler.stReadyList[GETPRIORITY(pstRunningTask->qwFlags)]), pstRunningTask);
    }
    gs_stScheduler.pstRunningTask=pstNextTask;
    kMemCpy(pcContextAddress, &(pstNextTask->stContext), sizeof(CONTEXT));
    gs_stScheduler.iProcessorTime=TASK_PROCESSTIME;
    return TRUE;
}

void kDecreaseProcessorTime(){
    if(gs_stScheduler.iProcessorTime>0)
        gs_stScheduler.iProcessorTime--;
}

BOOL kIsProcessorTimeExpired(){
    return (gs_stScheduler.iProcessorTime<=0);
}

QWORD kGetProcessorLoad(){
    return gs_stScheduler.qwProcessorLoad;
}

void kHaltProcessorByLoad(){
    int i;
    if(gs_stScheduler.qwProcessorLoad<40){
        for(i=0; i<6; i++)
            kHlt();
    }
    else if(gs_stScheduler.qwProcessorLoad<80){
        for(i=0; i<4; i++)
            kHlt();
    }
    else if(gs_stScheduler.qwProcessorLoad<95){
        for(i=0; i<2; i++)
            kHlt();
    }
}