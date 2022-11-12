#include "Type.h"
#include "Task.h"
#include "Utility.h"
#include "Descriptor.h"
#include "LinkedList.h"

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
            pstEmptyTCB->stLink.qwID==(((QWORD)gs_stTCBPoolManager.iAllocatedCount)<<32)|i;
        }
    }
    gs_stTCBPoolManager.iUseCount++;
    gs_stTCBPoolManager.iAllocatedCount++;
    return pstEmptyTCB;
}
void kFreeTCB(QWORD qwID){
    int i;
    i=qwID&0xffffffffffffffff;
    kMemSet(&(gs_stTCBPoolManager.pstStartAddress[i]), 0, sizeof(TCB));
    gs_stTCBPoolManager.pstStartAddress[i].stLink.qwID=i;
    gs_stTCBPoolManager.iUseCount--;
}

static SCHEDULER gs_stScheduler;

void kInitializeScheduler(){
    kInitializeTCBPool();
    kInitializeList(&(gs_stScheduler.stReadyList));
    gs_stScheduler.pstRunningTask=kAllocateTCB();
}

void kSetRuningTask(TCB* pstTCB){
    gs_stScheduler.pstRunningTask=pstTCB;
}

TCB* kGetRunningTask(){
    return gs_stScheduler.pstRunningTask;
}

TCB* kGetNextTaskToRun(){
    if(gs_stScheduler.stReadyList.iItemCount)
        return NULL;
    return (TCB*)kRemoveLinkFromHead(&(gs_stScheduler.stReadyList));
}

void kAddTaskToReadyList(TCB* pstTCB){
    kAddLinkToTail(&(gs_stScheduler.stReadyList), pstTCB);
}

TCB* kCreateTask(QWORD qwFlag, QWORD qwEntryPointAddress){
    TCB* pstNewTask;
    void* pvStackAddress;
    pstNewTask=kAllocateTCB();
    if(pstNewTask==NULL)
        return NULL;
    pvStackAddress=(void*)(TASK_STACKPOOLADDRESS+pstNewTask->stLink.qwID*(TASK_STACKSIZE));
    kInitializeTask(pstNewTask, qwFlag, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);
}

void kSchedule(){
    TCB* pstRunningTask, *pstNextTask;
    BOOL bPreviousFlag;
    if(kGetListCount(&(gs_stScheduler.stReadyList))==0)
        return;
    bPreviousFlag=kSetInterruptFlag(FALSE);
    pstNextTask=kGetNextTaskToRun();
    if(pstNextTask==NULL){
        kSetInterruptFlag(bPreviousFlag);
        return;
    }
    pstRunningTask=gs_stScheduler.pstRunningTask;
    kAddTaskToReadyList(pstRunningTask);
    gs_stScheduler.pstRunningTask=pstNextTask;
    kSwitchContext(&(pstRunningTask->stContext), &(pstNextTask->stContext));

    gs_stScheduler.iProcessorTime=TASK_PROCESSTIME;
    kSetInterruptFlag(bPreviousFlag);
}

BOOL kScheduleInInterupt(){
    TCB* pstRuningTask, *pstNextTask;
    char* pcContextAddress;

    pstNextTask=kGetNextTaskToRun();
    if(pstNextTask==NULL){
        return FALSE;
    }
    pcContextAddress=(char*)(IST_STARTADDRESS+IST_SIZE-sizeof(CONTEXT));
    pstRuningTask=gs_stScheduler.pstRunningTask;
    kMemCpy(&(pstRuningTask->stContext), pcContextAddress, sizeof(CONTEXT));
    kAddLinkToTail(&(gs_stScheduler.stReadyList), pstRuningTask);

    gs_stScheduler.pstRunningTask=pstNextTask;
    kMemCpy(pcContextAddress, &(pstNextTask), sizeof(CONTEXT));

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
