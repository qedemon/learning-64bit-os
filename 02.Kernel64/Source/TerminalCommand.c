#include "TextModeTerminal.h"
#include "TerminalCommand.h"
#include "Utility.h"
#include "string.h"
#include "PIT.h"
#include "RTC.h"

void kTerminalCommandHelp(const char* pcArgument){
    TERMINALCOMMANDENTRY* pTerminalCmd;
    int i;
    for(i=0; (pTerminalCmd=kTerminalGetCMDEntry(i))!=NULL; i++){
        kprintf("%s\t", pTerminalCmd->pcCommand);
        if(kstrlen(pTerminalCmd->pcCommand)<8)
            kprintf("%c", '\t');
        kprintf("%s\n", pTerminalCmd->pcHelp);
    }
}
void kTerminalCommandClear(const char* pcArgument){
    kTerminalClear();
    kTerminalSetCursorOffset(0);
}
void kTerminalCommandShowTotalRamSize(const char* pcArgumnet){
    kprintf("Total RAM size = %d MB\n", kGetTotalRamSize());
}
void kTerminalCommandStringToDeciHexConvert(const char* pcArgument){
    char vcArgument[100];
    int i=0;
    ARGUMENTLIST aList;
    kInitializeArgumentList(&aList, pcArgument);
    while(1){
        if(kGetNextArgumnet(&aList, vcArgument)==0){
            break;
        }
        kprintf("Argument%d = %s, ", i+1, vcArgument);
        i++;
        if(kMemCmp(vcArgument, "0x", 2)==0){
            long lValue=katoi(vcArgument+2, 16);
            kprintf("HEX value = 0x%q, Decimal value = %d\n", lValue, lValue);
        }
        else{
            long lValue=katoi(vcArgument, 10);
            kprintf("Decimal value = %d, Hex value = 0x%q\n", lValue, lValue);
        }
    }
}

void kTerminalCommandShutdown(const char* pcArgument){
    kprintf("System Shutdown Start\n");
    kprintf("Press Any Key To Reboot PC");
    while(1){
        KEYDATA keyData;
        if(kGetKeyFromKeyQueue(&keyData)){
            break;
        }
    }
    kReboot();
}

void kTerminalCommandWait(const char* pcArgument){
    ARGUMENTLIST argumentList;
    char pcBuffer[100];
    kInitializeArgumentList(&argumentList, pcArgument);
    if(kGetNextArgumnet(&argumentList, pcBuffer)!=0){
        int iWait=katoi(pcBuffer, 10);
        kprintf("wait %d ms.\n", iWait);
        kWaitms(iWait);
    }
    else{
        kprintf("input the time for wait\n");
    }

}


void kTerminalShowDateAndTime(const char* pcArgument){
    BYTE bHour, bMinute, bSecond;
    BYTE bMonth, bDayOfMonth, bDayOfWeek;
    WORD wYear;
    kReadRTCTime(&bHour, &bMinute, &bSecond);
    kReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);
    kprintf("Date : %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, kConvertDayOfWeekToString(bDayOfWeek));
    kprintf("Time : %d:%d:%d\n", bHour, bMinute, bSecond);
}


void kTermianlMeasureCPUSpeed(const char* pcArgument){
    QWORD qwCPUCount;
    kprintf("Measureing CPU Speed......\n");
    qwCPUCount=kMeasureCPUSpeed();
    kprintf("CPU Frequency : %dMHz\n", qwCPUCount/1000/1000);
}

void kTerminalStartTimer(const char* pcArgument){
    ARGUMENTLIST argumentList;
    char pcBuffer[100];
    int millisecond;
    BOOL bPeriodic;
    kInitializeArgumentList(&argumentList, pcArgument);
    if(kGetNextArgumnet(&argumentList, pcBuffer)!=0){
        millisecond=katoi(pcBuffer, 10);
        bPeriodic=FALSE;
        if(kGetNextArgumnet(&argumentList, pcBuffer)!=0)
            bPeriodic=katoi(pcBuffer, 10);
        kInitializePIT(MSTOCOUNT(millisecond), bPeriodic);
    }
}