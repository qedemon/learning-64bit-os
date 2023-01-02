#include "InterruptHandler.h"
#include "AssemblyUtility.h"
#include "Utility.h"
#include "PIC.h"
#include "PIT.h"
#include "Keyboard.h"
#include "TextModeTerminal.h"
#include "string.h"
#include "Task.h"
#include "HDD.h"

void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode){
    char vcBuffer[]="[VEC:  ]";
    vcBuffer[5]=iVectorNumber/10+'0';
    vcBuffer[6]=iVectorNumber%10+'0';
    kprintf("=======================================\n");
    kprintf("Common Exception Handler Execute!!\n");
    kprintf("%d\n", iVectorNumber);
    kprintf("Error Code : 0x%q", qwErrorCode);
    kprintf("=======================================\n");
    while(1);
}

void kDeviceNotAvailableHandler(int iVectorNumber){
    char vcBuffer[]="[VEC:  ]";
    TCB* pstRunningTask, *pstFPULastUsedTask;
    QWORD qwFPULastUseTaskID;

    vcBuffer[5]=iVectorNumber/10+'0';
    vcBuffer[6]=iVectorNumber%10+'0';
    kPrintString(0, 0, vcBuffer);

    kClearTSBit();
    qwFPULastUseTaskID=kGetLastFPUUsedTaskID();
    pstRunningTask=kGetRunningTask();
    if(qwFPULastUseTaskID==pstRunningTask->stLink.qwID){
        return;
    }
    else if(qwFPULastUseTaskID!=TASK_INVALID){
        pstFPULastUsedTask=kGetTCBFromTCBPool(GETTCBOFFSET(qwFPULastUseTaskID));
        if((pstFPULastUsedTask!=NULL)&&(pstFPULastUsedTask->stLink.qwID==qwFPULastUseTaskID)){
            kSaveFPUContext(&pstFPULastUsedTask->vqwFPUContext);
        }
    }
    if(pstRunningTask->bFPUUsed==FALSE){
        kInitializeFPU();
        pstRunningTask->bFPUUsed=TRUE;
    }
    else{
        kLoadFPUContext(&pstRunningTask->vqwFPUContext);
    }
    kSetLastFPUUsedTaskID(pstRunningTask->stLink.qwID);
}

void kCommonInterruptHandler(int iVectorNumber){
    static int g_iCommonInterruptCount=0;
    char vcBuffer[100];
    ksprintf(vcBuffer, "[INT:%d,%d]", iVectorNumber, g_iCommonInterruptCount%10);
    kTerminalPrintString(70, 0, vcBuffer);
    g_iCommonInterruptCount++;
    kSendEOIToPIC(iVectorNumber-32);
}



void kTimerInterruptHandler(int iVectorNumber, QWORD qwStackStartAddress){
    static int g_iTimerInterruptCount=0;
    char vcBuffer[100];
    g_iTimerInterruptCount++;
    kAddTickCount(1);
    ksprintf(vcBuffer, "[INT:%d,%d]", iVectorNumber, g_iTimerInterruptCount%10);
    kTerminalPrintString(70, 0, vcBuffer);
    kSendEOIToPIC(iVectorNumber-32);
    kAddTickCount(1);
    kDecreaseProcessorTime();
    if(kIsProcessorTimeExpired()){
        kScheduleInInterupt(qwStackStartAddress);
    }
}

void kKeyboardInterruptHandler(int iVectorNumber){
    BYTE scanCode;
    if(kIsOutputBufferFull()){
        scanCode=kGetKeyBoardScanCode();
        kUpdateKeyBoardManagerAndPutKeyDatatToQueue(scanCode);
    }

    kSendEOIToPIC(iVectorNumber-32);
}

void kHDDInterruptHandler(int iVectorNumber){
    static int g_iCommonInterruptCount=0;
    char vcBuffer[100];
    ksprintf(vcBuffer, "[INT:%d,%d]", iVectorNumber, g_iCommonInterruptCount%10);
    kTerminalPrintString(70, 0, vcBuffer);
    g_iCommonInterruptCount++;

    if(iVectorNumber-32==14){
        kSetHDDInterruptFlag(TRUE, TRUE);
    }
    else{
        kSetHDDInterruptFlag(FALSE, TRUE);
    }

    kSendEOIToPIC(iVectorNumber-32);
}