#include "Type.h"
#include "Task.h"
#include "AssemblyUtility.h"
#include "Utility.h"
#include "Descriptor.h"
#include "LinkedList.h"
#include "TextModeTerminal.h"
#include "string.h"
#include "PIT.h"
#include "Synchronization.h"

void kInitializeTask(TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize){
    kMemSet(pstTCB->stContext.vqRegister, 0, sizeof(pstTCB->stContext.vqRegister));
    pstTCB->stContext.vqRegister[TASK_RSPOFFSET]=((QWORD)pvStackAddress)+qwStackSize-8;
    pstTCB->stContext.vqRegister[TASK_RBPOFFSET]=((QWORD)pvStackAddress)+qwStackSize-8;

    *((QWORD*)(((QWORD)pvStackAddress)+qwStackSize-8))=(QWORD)kExitTask;

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
    if(gs_stTCBPoolManager.iAllocatedCount==0)
        gs_stTCBPoolManager.iAllocatedCount=1;
    
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
    BYTE bLockInfo;
    TCB* pstTask;
    kInitializeTCBPool();
    bLockInfo=kLockForSystemData();
    for(i=0; i<TASK_MAXREADYLISTCOUNT; i++){
        kInitializeList(&(gs_stScheduler.stReadyList[i]));
        gs_stScheduler.viExecuteCount[i]=0;
    }
    pstTask=kAllocateTCB();
    SETPRIORITY(pstTask->qwFlags, TASK_FLAG_HIGHEST|TASK_FLAG_PROCESS|TASK_FLAG_SYSTEM);
    pstTask->pvMemoryAddress=(void*)0x100000;
    pstTask->qwMemorySize=0x500000;
    pstTask->pvStackAddress=(void*)0x600000;
    pstTask->qwStackSize=0x100000;
    gs_stScheduler.pstRunningTask=pstTask;

    gs_stScheduler.qwProcessorLoad=0;
    gs_stScheduler.qwSpendProcessorTimeInIdleTask=0;
    kUnlockForSystemData(bLockInfo);
}

void kSetRuningTask(TCB* pstTCB){
    BYTE bLockInfo;
    bLockInfo=kLockForSystemData();
    gs_stScheduler.pstRunningTask=pstTCB;
    kUnlockForSystemData(bLockInfo);
}

TCB* kGetRunningTask(){
    TCB* pstRunningTask;
    BYTE bLockInfo=kLockForSystemData();
    pstRunningTask=gs_stScheduler.pstRunningTask;
    kUnlockForSystemData(bLockInfo);
    return pstRunningTask;
}

int kGetReadyTaskCount(){
    int iCount=0;
    int i;
    for(i=0; i<TASK_MAXREADYLISTCOUNT; i++)
        iCount+=gs_stScheduler.stReadyList[i].iItemCount;
    return iCount;
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
    BYTE bLockInfo;
    BYTE bPriority;
    bPriority=GETPRIORITY(pstTCB->qwFlags);
    if(bPriority>=TASK_MAXREADYLISTCOUNT)
        return FALSE;
    bLockInfo=kLockForSystemData();
    kAddLinkToTail(&(gs_stScheduler.stReadyList[bPriority]), pstTCB);
    kUnlockForSystemData(bLockInfo);
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
    BYTE bLockInfo;
    if(bPriority>=TASK_MAXREADYLISTCOUNT)
        return FALSE;
    pstTargetTask=kGetRunningTask();
    bLockInfo=kLockForSystemData();
    if(pstTargetTask->stLink.qwID==qwTaskID){
        SETPRIORITY(pstTargetTask->qwFlags, bPriority);
    }
    else{
        pstTargetTask=kRemoveTaskFromReadyList(qwTaskID);
        SETPRIORITY(pstTargetTask->qwFlags, bPriority);
        kAddTaskToReadyList(pstTargetTask);
    }
    kUnlockForSystemData(bLockInfo);
    return TRUE;
}

TCB* kCreateTask(QWORD qwFlag, QWORD qwEntryPointAddress, void* pvMemoryAddress, QWORD qwMemorySize){
    TCB* pstNewTask, *pstProcess;
    void* pvStackAddress;
    BYTE bLockInfo;
    bLockInfo=kLockForSystemData();
    pstNewTask=kAllocateTCB();
    if(pstNewTask==NULL){
        kUnlockForSystemData(bLockInfo);
        return NULL;
    }

    pstProcess=kGetProcessByThread(kGetRunningTask());
    if(pstProcess==NULL){
        kFreeTCB(pstNewTask->stLink.qwID);
        kUnlockForSystemData(bLockInfo);
        return NULL;
    }
    pvStackAddress=(void*)(TASK_STACKPOOLADDRESS+GETTCBOFFSET(pstNewTask->stLink.qwID)*(TASK_STACKSIZE));
    kInitializeTask(pstNewTask, qwFlag, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);

    if(qwFlag&TASK_FLAG_THREAD){
        pstNewTask->qwParentProcessID=pstProcess->stLink.qwID;
        pstNewTask->pvMemoryAddress=pstProcess->pvMemoryAddress;
        pstNewTask->qwMemorySize=pstProcess->qwMemorySize;
        kAddLinkToTail(&pstProcess->stChildThereadList, &pstNewTask->stThreadLink);
    }
    else{
        pstNewTask->qwParentProcessID=pstProcess->stLink.qwID;
        pstNewTask->pvMemoryAddress=pvMemoryAddress;
        pstNewTask->qwMemorySize=qwMemorySize;
        kInitializeList(&pstNewTask->stChildThereadList);
    }
    pstNewTask->stThreadLink.qwID=pstNewTask->stLink.qwID;

    kAddTaskToReadyList(pstNewTask);
    
    kUnlockForSystemData(bLockInfo);
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
        BYTE bLockInfo;
        bLockInfo=kLockForSystemData();
        pstTarget=kRemoveTaskFromReadyList(qwTaskID);
        if(pstTarget==NULL){
            kUnlockForSystemData(bLockInfo);
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
        kUnlockForSystemData(bLockInfo);
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
                BYTE bLockInfo;
                bLockInfo=kLockForSystemData();
                pstTask=kRemoveLinkFromHead(&(gs_stScheduler.stWaitList));
                if(pstTask==NULL){
                    kUnlockForSystemData(bLockInfo);
                    break;
                }
                if(pstTask->qwFlags&TASK_FLAG_PROCESS){
                    int iCount, i;
                    iCount=kGetListCount(&pstTask->stChildThereadList);
                    for(i=0; i<iCount; i++){
                        void* pvLink;
                        TCB* pstChild;
                        pvLink=kRemoveLinkFromHead(&pstTask->stChildThereadList);
                        if(pvLink==NULL){
                            break;
                        }
                        pstChild=GETTCBFROMTHREADLINK(pvLink);
                        kAddLinkToTail(&pstTask->stChildThereadList, &pstChild->stThreadLink);
                        kEndTask(pstChild->stLink.qwID);
                    }
                    if(kGetListCount(&pstTask->stChildThereadList)>0){
                        kAddLinkToTail(&gs_stScheduler.stWaitList, &pstTask);
                        kUnlockForSystemData(bLockInfo);
                        continue;
                    }
                    else{
                        //Process Memory code reset code here.
                    }
                }
                else if(pstTask->qwFlags&TASK_FLAG_THREAD){
                    TCB* pstParentProcess;
                    pstParentProcess=kGetProcessByThread(pstTask);
                    if(pstParentProcess!=NULL){
                        kRemoveLink(&pstParentProcess->stChildThereadList, pstTask->stThreadLink.qwID);
                    }
                }
                kprintf("IDLE: Task ID[0x%q] is completely ended.\n", pstTask->stLink.qwID);
                kFreeTCB(pstTask->stLink.qwID);
                kUnlockForSystemData(bLockInfo);
            }
        }
        kSchedule();
    }
}

void kClearOtherTask(){
    TCB* pstRunningTask;
    BYTE bLockInfo;
    bLockInfo=kLockForSystemData();
    pstRunningTask=kGetRunningTask();
    while(1){
        TCB* otherTask=kGetNextTaskToRun();
        if(otherTask==NULL){
            break;
        }
        kFreeTCB(otherTask->stLink.qwID);
    }
    kUnlockForSystemData(bLockInfo);
}

void kSchedule(){
    TCB* pstRunningTask, *pstNextTask;
    BYTE bLockInfo;
    if(kGetReadyTaskCount()<1){
        return;
    }
    bLockInfo=kLockForSystemData();
    pstNextTask=kGetNextTaskToRun();
    if(pstNextTask==NULL){
        kUnlockForSystemData(bLockInfo);
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
    kUnlockForSystemData(bLockInfo);
}

BOOL kScheduleInInterupt(QWORD qwStackStartAddress){
    TCB* pstRunningTask, *pstNextTask;
    char* pcContextAddress;
    char vcBuffer[100];
    BYTE bLockInfo;

    bLockInfo=kLockForSystemData();
    pstNextTask=kGetNextTaskToRun();
    if(pstNextTask==NULL){
        kUnlockForSystemData(bLockInfo);
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
    kUnlockForSystemData(bLockInfo);
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

static TCB* kGetProcessByThread(TCB* pstThread){
    TCB* pstProcess;
    if(pstThread->qwFlags&TASK_FLAG_PROCESS){
        return pstThread;
    }
    pstProcess=kGetTCBFromTCBPool(GETTCBOFFSET(pstProcess->qwParentProcessID));
    if(pstProcess->stLink.qwID!=pstThread->qwParentProcessID){
        return NULL;
    }
    return pstThread;
}