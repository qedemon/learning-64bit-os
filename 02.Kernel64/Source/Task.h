#ifndef TASK_H
#define TASK_H
#include "Type.h"
#include "LinkedList.h"

#define TASK_REGISTERCOUNT (5+19)
#define TASK_REGISTERSIZE 8

#define TASK_GSOFFSET 0
#define TASK_FSOFFSET 1
#define TASK_ESOFFSET 2
#define TASK_DSOFFSET 3
#define TASK_R15OFFSET 4
#define TASK_R14OFFSET 5
#define TASK_R13OFFSET 6
#define TASK_R12OFFSET 7
#define TASK_R11OFFSET 8
#define TASK_R10OFFSET 9
#define TASK_R9OFFSET 10
#define TASK_R8OFFSET 11
#define TASK_RSIOFFSET 12
#define TASK_RDIOFFSET 13
#define TASK_RDXOFFSET 14
#define TASK_RCXOFFSET 15
#define TASK_RBXOFFSET 16
#define TASK_RAXOFFSET 17
#define TASK_RBPOFFSET 18
#define TASK_RIPOFFSET 19
#define TASK_CSOFFSET 20
#define TASK_RFLAGSOFFSET 21
#define TASK_RSPOFFSET 22
#define TASK_SSOFFSET 23

#define TASK_TCBPOOLADDRESS 0x800000
#define TASK_MAXCOUNT 1024

#define TASK_STACKPOOLADDRESS (TASK_TCBPOOLADDRESS+TASK_MAXCOUNT*sizeof(TCB))
#define TASK_STACKSIZE 8192

#define TASK_PROCESSTIME 5

#define TASK_MAXREADYLISTCOUNT 5
#define TASK_FLAG_HIGHEST 0
#define TASK_FLAG_HIGH 1
#define TASK_FLAG_MEDIUM 2
#define TASK_FLAG_LOW 3
#define TASK_FLAG_LOWEST 4

#define TASK_FLAG_WAIT 0xFF
#define TASK_FLAG_ENDTASK 0x8000000000000000
#define TASK_FLAG_IDLE 0x0800000000000000

#define GETPRIORITY(x) ((x)&0xFF)
#define SETPRIORITY(x, priority) ((x)=((x)&0xFFFFFFFFFFFFFF00)|priority)

#define GETTCBOFFSET(ID) ((ID)&0xFFFFFFFF)

#define ISTASKALLOCATED(ID) (((ID)>>32)!=0)

#pragma pack(push, 1)
typedef struct kContextStruct{
    QWORD vqRegister[TASK_REGISTERCOUNT];
}CONTEXT;

typedef struct kTaskControlBlockStruct{
    LISTLINK stLink;
    CONTEXT stContext;
    QWORD qwFlags;
    void* pvStackAddress;
    QWORD qwStackSize;
}TCB;

typedef struct kTCBPoolManager{
    TCB* pstStartAddress;
    int iMaxCount;
    int iUseCount;

    int iAllocatedCount;
}TCBPOOLMANAGER;

typedef struct kSchedulerStruct{
    TCB* pstRunningTask;
    int iProcessorTime;
    LIST stReadyList[TASK_MAXREADYLISTCOUNT];
    LIST stWaitList;

    int viExecuteCount[TASK_MAXREADYLISTCOUNT];

    QWORD qwProcessorLoad;
    QWORD qwSpendProcessorTimeInIdleTask;
}SCHEDULER;

#pragma pack(pop)

void kInitializeTask(TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize);
void kSwitchContext(CONTEXT* pstCurrentContext, CONTEXT* pstNextContext);

void kInitializeTCBPool();
TCB* kAllocateTCB();
void kFreeTCB(QWORD qwID);

TCB* kCreateTask(QWORD qwFlag, QWORD qwEntryPointAddress);
BOOL kEndTask(QWORD qwTaskID);
void kExitTask();
void kClearOtherTask();

void kInitializeScheduler();
void kSetRunningTask(TCB* pstTCB);
TCB* kGetRunningTask();
TCB* kGetNextTaskToRun();
BOOL kAddTaskToReadyList(TCB* pstTCB);
TCB* kRemoveTaskFromReadyList(QWORD qwID);
void kSchedule();
BOOL kScheduleInInterupt(QWORD qwStackStartAddress);
void kDecreaseProcessorTime();
BOOL kIsProcessorTimeExpired();

TCB* kGetTCBFromTCBPool(QWORD qwOffset);
QWORD kGetProcessorLoad();

void kIdleTask();
void kHaltProcessorByLoad();

#endif