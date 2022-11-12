#include "InterruptHandler.h"
#include "Utility.h"
#include "PIC.h"
#include "Keyboard.h"
#include "TextModeTerminal.h"
#include "string.h"
#include "Task.h"

void kCommonExceptionHandler(int iVectorNumber, QWORD qwErrorCode){
    char vcBuffer[]="[VEC:  ]";
    vcBuffer[5]=iVectorNumber/10+'0';
    vcBuffer[6]=iVectorNumber%10+'0';
    kprintf("=======================================\n");
    kprintf("Common Exception Handler Execute!!\n");
    kprintf("%d\n", iVectorNumber);
    kprintf("=======================================\n");
    while(1);
}

void kCommonInterruptHandler(int iVectorNumber){
    static int g_iCommonInterruptCount=0;
    char vcBuffer[100];
    ksprintf(vcBuffer, "[INT:%d,%d]", iVectorNumber, g_iCommonInterruptCount%10);
    kTerminalPrintString(70, 0, vcBuffer);
    g_iCommonInterruptCount++;
    kSendEOIToPIC(iVectorNumber-32);
}

void kTimerInterruptHandler(int iVectorNumber, QWORD qwStackBaseAddress){
    static int g_iTimerInterruptCount=0;
    char vcBuffer[100];
    g_iTimerInterruptCount++;
    kSendEOIToPIC(iVectorNumber-32);

    kprintf("stack bp : 0x%q", qwStackBaseAddress);

    /*kDecreaseProcessorTime();
    if(kIsProcessorTimeExpired()){
        if(kScheduleInInterupt(qwStackBaseAddress)==FALSE){
            ksprintf(vcBuffer, "NULL %d", g_iTimerInterruptCount%10);
            kTerminalPrintString(70, 0, vcBuffer);
        }
    }*/
}

void kKeyboardInterruptHandler(int iVectorNumber){
    BYTE scanCode;
    if(kIsOutputBufferFull()){
        scanCode=kGetKeyBoardScanCode();
        kUpdateKeyBoardManagerAndPutKeyDatatToQueue(scanCode);
    }

    kSendEOIToPIC(iVectorNumber-32);
}